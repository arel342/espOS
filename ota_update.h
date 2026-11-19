// ota_update.h - Update OTA espOS langsung dari GitHub Releases (tanpa HP / PC)
// ============================================================================
//  Alur:  Wi-Fi + NTP siap -> GET api.github.com/.../releases/latest -> bandingkan versi
//         -> (user menekan Pasang, atau otomatis) unduh .bin lewat HTTPS -> hitung SHA-256 sambil menulis
//         ke slot OTA yang tidak aktif -> cocokkan SHA-256 (+ tanda tangan + versi di dalam .bin)
//         -> baru Update.end() dan reboot -> firmware baru wajib "sehat" 15 detik, kalau tidak ROLLBACK.
//
//  KEAMANAN:
//   * HTTPS saja (tidak ada setInsecure). Setiap redirect divalidasi: harus https & host GitHub.
//   * Sertifikat: bundel root CA terpercaya Arduino-ESP32, atau root CA tertanam (OTA_PIN_ROOT_CA).
//   * SHA-256 diverifikasi SEBELUM Update.end() (sebelum slot baru diaktifkan & sebelum reboot).
//   * Anti-downgrade: versi harus > versi berjalan DAN >= versi sehat tertinggi yang pernah berjalan
//     (disimpan di NVS), dan penanda versi di dalam .bin harus sama dengan tag rilis.
//   * Tanda tangan (opsional, OTA_REQUIRE_SIGNATURE): ECDSA/RSA atas SHA-256 firmware, kunci publik tertanam.
//   * Rollback: firmware baru harus lolos jendela validasi; crash-loop -> kembali ke firmware lama.
//
//  Perlu: Partition Scheme dengan DUA slot OTA (pakai partitions.csv di folder sketch).
// ============================================================================
#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include "sdkconfig.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_heap_caps.h"
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "ota_config.h"
#include "ota_util.h"

extern Preferences prefs;

// Penanda versi yang ditanam di dalam firmware (dipindai saat unduh untuk mencegah file lama berlabel versi baru)
static const char OTA_VERSION_MARKER[] __attribute__((used)) = OTA_MARK ESPOS_VERSION "|";

// Validasi rollback ditunda sampai firmware terbukti sehat (core Arduino akan memanggil ini bila rollback aktif)
extern "C" bool verifyRollbackLater() { return true; }

// bundel root CA bawaan core (Mozilla)
extern const uint8_t otaCaBundleStart[] asm("_binary_x509_crt_bundle_start");
extern const uint8_t otaCaBundleEnd[]   asm("_binary_x509_crt_bundle_end");

enum : uint8_t { OTA_IDLE = 0, OTA_CHECKING, OTA_AVAILABLE, OTA_UPTODATE, OTA_ERROR, OTA_DOWNLOADING, OTA_VERIFYING, OTA_READY };

struct OtaInfo {
  volatile uint8_t  state;
  volatile uint8_t  pct;
  volatile uint32_t got, total;
  volatile bool     busy, changed, silent, avail;
  char     tag[24];            // tag rilis mentah, mis. "v1.9.0"
  char     latest[16];         // "1.9.0"
  uint32_t latestV;
  char     notes[200];
  char     err[96];
  char     boot[64];           // pesan sekali tampil setelah boot (berhasil diperbarui / rollback)
  uint32_t nextCheck; bool sched;
};
static OtaInfo  otaInfo;
static uint32_t otaCurV = 0, otaMinV = 0, otaBadV = 0;
static bool     otaPending = false, otaValidated = false, otaAutoCheck = true, otaAutoInst = false;
static uint32_t otaAutoTriedV = 0;

static inline bool otaInstalling() { return otaInfo.state >= OTA_DOWNLOADING; }
static inline bool otaBannerOn()   { return otaInfo.avail || otaInstalling(); }

// ---------------------------------------------------------------- util kecil
static void otaFail(const char* m) {
  strncpy(otaInfo.err, m, sizeof(otaInfo.err) - 1); otaInfo.err[sizeof(otaInfo.err) - 1] = 0;
  Serial.printf("[OTA] GAGAL: %s\n", m);
  if (!otaInfo.silent) otaInfo.state = OTA_ERROR;
  otaInfo.changed = true;
}
static char otaLastHost[56] = "";
static int  otaLastTls = 0;
static void otaHttpErr(int code, char* out, size_t cap) {
  if (code == 404) snprintf(out, cap, "Berkas rilis tidak ditemukan (404)");
  else if (code == 403 || code == 429) snprintf(out, cap, "GitHub membatasi permintaan, coba nanti");
  else if (code > 0) snprintf(out, cap, "Server membalas HTTP %d", code);
  else if (code <= -100) snprintf(out, cap, "Alamat unduhan ditolak (bukan HTTPS GitHub)");
  else snprintf(out, cap, "HTTPS gagal %s (%d, tls 0x%X)", otaLastHost, code, (unsigned)(otaLastTls < 0 ? -otaLastTls : otaLastTls));
}
static void otaTrust(WiFiClientSecure& c) {
#if OTA_PIN_ROOT_CA
  c.setCACert(OTA_ROOT_CA_PEM);
#else
  #if defined(ESP_ARDUINO_VERSION_VAL)
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 1, 0)
      #define OTA_BUNDLE_2ARG 1
    #endif
  #endif
  #ifdef OTA_BUNDLE_2ARG
    c.setCACertBundle(otaCaBundleStart, (size_t)(otaCaBundleEnd - otaCaBundleStart));   // core 3.1+
  #else
    c.setCACertBundle(otaCaBundleStart);                                                // core 2.x / 3.0
  #endif
#endif
}
static bool otaHaveSlot() { return esp_ota_get_next_update_partition(NULL) != NULL; }

// Satu koneksi = satu klien TLS BARU (tiap lompatan redirect juga baru) supaya tidak ada sisa state / memori TLS
struct OtaConn {
  WiFiClientSecure* cl = nullptr;
  HTTPClient http;
  ~OtaConn() { http.end(); delete cl; }
};

// Buka URL (HTTPS saja). Redirect diikuti MANUAL dan tiap lompatan divalidasi (https + host GitHub).
static int otaOpen(OtaConn& c, const char* url0, bool api) {
  String url = url0;
  for (int hop = 0; hop < 5; hop++) {
    if (!otaUrlOk(url.c_str())) return -100;
    { const char* h = url.c_str() + 8; size_t n = 0;
      while (h[n] && h[n] != '/' && h[n] != '?' && h[n] != ':' && n < sizeof(otaLastHost) - 1) n++;
      memcpy(otaLastHost, h, n); otaLastHost[n] = 0; }
    c.http.end(); delete c.cl;
    c.cl = new WiFiClientSecure();
    otaTrust(*c.cl);
    if (!c.http.begin(*c.cl, url)) return -101;
    c.http.setConnectTimeout(10000);
    c.http.setTimeout(15000);
    c.http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
    c.http.setUserAgent("espOS-OTA/" ESPOS_VERSION);
    if (api) { c.http.addHeader("Accept", "application/vnd.github+json"); c.http.addHeader("X-GitHub-Api-Version", "2022-11-28"); }
    static const char* hk[] = {"Location"};
    c.http.collectHeaders(hk, 1);
    Serial.printf("[OTA] GET hop%d %s | heap internal %u, blok terbesar %u\n", hop, otaLastHost,
                  (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL), (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    int code = c.http.GET();
    if (code < 0) {
      char eb[100] = ""; otaLastTls = c.cl->lastError(eb, sizeof(eb));
      Serial.printf("[OTA] %s -> kode %d, TLS error %d (%s)\n", otaLastHost, code, otaLastTls, eb);
    }
    if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308) {
      String loc = c.http.header("Location");
      c.http.end();
      if (!loc.length()) return -102;
      url = loc;
      continue;
    }
    return code;
  }
  return -103;
}

// ambil teks kecil (JSON rilis)
static bool otaFetchText(const char* url, bool api, String& out, int maxLen) {
  OtaConn c;
  int code = otaOpen(c, url, api);
  if (code != 200) { char e[96]; otaHttpErr(code, e, sizeof(e)); otaFail(e); return false; }
  int sz = c.http.getSize();
  if (sz > maxLen) { otaFail("Respons GitHub terlalu besar"); return false; }
  out = c.http.getString();
  if (out.length() == 0) { otaFail("Respons GitHub kosong"); return false; }
  return true;
}
// ambil berkas kecil biner (.sha256 / .sig) dengan batas ukuran
static bool otaFetchBin(const char* url, uint8_t* buf, size_t cap, size_t* outLen) {
  OtaConn c;
  int code = otaOpen(c, url, false);
  if (code != 200) { char e[96]; otaHttpErr(code, e, sizeof(e)); otaFail(e); return false; }
  int sz = c.http.getSize();
  if (sz <= 0 || (size_t)sz > cap) { otaFail("Ukuran berkas verifikasi tidak wajar"); return false; }
  WiFiClient* s = c.http.getStreamPtr();
  size_t got = 0; uint32_t t0 = millis();
  while (got < (size_t)sz && millis() - t0 < 20000) {
    int av = s->available();
    if (av > 0) { size_t want = (size_t)av < (size_t)sz - got ? (size_t)av : (size_t)sz - got; got += s->readBytes(buf + got, want); t0 = millis(); }
    else if (!s->connected()) break;
    else delay(5);
  }
  if (got != (size_t)sz) { otaFail("Unduhan berkas verifikasi terputus"); return false; }
  *outLen = got;
  return true;
}

// ---------------------------------------------------------------- versi & anti-downgrade
static void otaEvaluate(bool setState) {
  uint32_t v = otaInfo.latestV;
  bool newer = v > otaCurV && v >= otaMinV;       // anti-downgrade: harus lebih baru DAN tidak di bawah versi sehat tertinggi
#if OTA_ALLOW_DOWNGRADE
  newer = (v != otaCurV);                         // keputusan sadar pemilik perangkat (lihat ota_config.h)
#endif
  if (v == otaBadV) newer = false;                // versi yang pernah gagal & di-rollback tidak ditawarkan lagi
  otaInfo.avail = newer;
  if (setState) otaInfo.state = newer ? OTA_AVAILABLE : OTA_UPTODATE;
  otaInfo.changed = true;
}

// ---------------------------------------------------------------- cek rilis terbaru
static bool otaDoCheck(bool setState) {
  String js;
  if (!otaFetchText("https://api.github.com/repos/" OTA_GH_OWNER "/" OTA_GH_REPO "/releases/latest", true, js, 200000)) return false;
  char tag[24];
  if (!otaJsonStr(js.c_str(), "tag_name", tag, sizeof(tag)) || !otaTagOk(tag)) { otaFail("Data rilis tidak valid"); return false; }
  uint32_t v = otaParseVer(tag);
  if (!v) { otaFail("Format versi rilis tidak dikenal"); return false; }
  const char* bp = strstr(js.c_str(), "\"zipball_url\""); if (!bp) bp = js.c_str();
  char notes[sizeof(otaInfo.notes)];
  if (!otaJsonStr(bp, "body", notes, sizeof(notes))) notes[0] = 0;
  strcpy(otaInfo.tag, tag);
  otaVerStr(v, otaInfo.latest, sizeof(otaInfo.latest));
  otaInfo.latestV = v;
  strcpy(otaInfo.notes, notes);
  otaEvaluate(setState);
  Serial.printf("[OTA] terbaru %s, terpasang %s -> %s\n", tag, ESPOS_VERSION, otaInfo.avail ? "ADA UPDATE" : "sudah terbaru");
  return true;
}

// ---------------------------------------------------------------- tanda tangan (ECDSA / RSA atas SHA-256)
static bool otaVerifySig(const uint8_t dig[32], const uint8_t* sig, size_t sl) {
  if (!strstr(OTA_SIGN_PUBKEY_PEM, "BEGIN PUBLIC KEY") || strstr(OTA_SIGN_PUBKEY_PEM, "ISI_DENGAN")) return false;
  mbedtls_pk_context pk; mbedtls_pk_init(&pk);
  bool ok = false;
  if (mbedtls_pk_parse_public_key(&pk, (const unsigned char*)OTA_SIGN_PUBKEY_PEM, strlen(OTA_SIGN_PUBKEY_PEM) + 1) == 0)
    ok = (mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, dig, 32, sig, sl) == 0);
  mbedtls_pk_free(&pk);
  return ok;
}

// ---------------------------------------------------------------- unduh + verifikasi + tulis
#define OTA_CHUNK 4096
static void otaDoInstall() {
  otaInfo.silent = false; otaInfo.pct = 0; otaInfo.got = 0; otaInfo.total = 0; otaInfo.err[0] = 0;
  // 1) segarkan info rilis (pastikan masih ada & masih lebih baru)
  if (!otaDoCheck(false)) return;
  if (!otaInfo.avail) { otaInfo.state = OTA_UPTODATE; otaInfo.changed = true; return; }
  const esp_partition_t* part = esp_ota_get_next_update_partition(NULL);
  if (!part) { otaFail("Partisi OTA tidak ada. Pakai partitions.csv 2 slot"); return; }
  otaInfo.state = OTA_DOWNLOADING; otaInfo.changed = true;

  char base[128];
  snprintf(base, sizeof(base), "https://github.com/%s/%s/releases/download/%s/", OTA_GH_OWNER, OTA_GH_REPO, otaInfo.tag);

  // 2) SHA-256 yang diharapkan (+ tanda tangan)
  uint8_t expect[32]; uint8_t tmp[160]; size_t tl = 0;
  { String u = String(base) + OTA_ASSET_SHA;
    if (!otaFetchBin(u.c_str(), tmp, sizeof(tmp), &tl)) return;
    if (!otaParseHex256((const char*)tmp, tl, expect)) { otaFail("Berkas .sha256 tidak valid"); return; } }
#if OTA_REQUIRE_SIGNATURE
  uint8_t sig[520]; size_t sigLen = 0;
  { String u = String(base) + OTA_ASSET_SIG;
    if (!otaFetchBin(u.c_str(), sig, sizeof(sig), &sigLen)) return;
    if (sigLen < 8) { otaFail("Berkas tanda tangan tidak valid"); return; } }
#endif

  // 3) buka unduhan firmware
  OtaConn dl; HTTPClient& http = dl.http;
  String bu = String(base) + OTA_ASSET_BIN;
  int code = otaOpen(dl, bu.c_str(), false);
  if (code != 200) { char e[96]; otaHttpErr(code, e, sizeof(e)); http.end(); otaFail(e); return; }
  int total = http.getSize();
  if (total < 65536 || (uint32_t)total > part->size) { http.end(); otaFail("Ukuran firmware tidak cocok dengan partisi"); return; }
  otaInfo.total = (uint32_t)total;
  if (!Update.begin((size_t)total, U_FLASH)) { http.end(); otaFail("Gagal membuka partisi OTA"); Serial.println(Update.errorString()); return; }

  uint8_t* w = (uint8_t*)malloc(OTA_CHUNK + OTA_CARRY + 1);
  if (!w) { Update.abort(); http.end(); otaFail("Memori tidak cukup"); return; }
  mbedtls_sha256_context sha; mbedtls_sha256_init(&sha); mbedtls_sha256_starts(&sha, 0);
  WiFiClient* s = http.getStreamPtr();
  size_t got = 0, carry = 0; bool markFound = false; char fwVer[16] = ""; const char* why = nullptr;
  uint32_t lastData = millis();

  // 4) tulis ke slot yang TIDAK aktif sambil menghitung SHA-256 dan memindai penanda versi
  while (got < (size_t)total) {
    int av = s->available();
    if (av <= 0) {
      if (!s->connected()) { why = "Unduhan terputus"; break; }
      if (millis() - lastData > 20000) { why = "Waktu unduh habis"; break; }
      delay(2); continue;
    }
    size_t want = (size_t)av < (size_t)OTA_CHUNK ? (size_t)av : (size_t)OTA_CHUNK;
    if (want > (size_t)total - got) want = (size_t)total - got;
    size_t n = s->readBytes(w + carry, want);
    if (n == 0) { delay(2); continue; }
    lastData = millis();
    if (got == 0 && w[carry] != 0xE9) { why = "Bukan berkas firmware ESP32"; break; }
    if (!markFound && otaScanMarker(w, carry + n, fwVer, sizeof(fwVer))) markFound = true;
    mbedtls_sha256_update(&sha, w + carry, n);
    if (Update.write(w + carry, n) != n) { why = "Gagal menulis flash"; break; }
    got += n;
    size_t tot = carry + n; carry = tot < OTA_CARRY ? tot : OTA_CARRY;
    memmove(w, w + tot - carry, carry);
    otaInfo.got = (uint32_t)got; otaInfo.pct = (uint8_t)((uint64_t)got * 100 / (uint64_t)total);
    delay(1);                                   // beri waktu ke task lain (WDT, Wi-Fi, TFT)
  }
  free(w);
  http.end();
  if (!why && got != (size_t)total) why = "Unduhan tidak lengkap";
  if (why) { mbedtls_sha256_free(&sha); Update.abort(); otaFail(why); return; }

  // 5) VERIFIKASI sebelum slot baru diaktifkan
  otaInfo.state = OTA_VERIFYING; otaInfo.changed = true;
  uint8_t dig[32]; mbedtls_sha256_finish(&sha, dig); mbedtls_sha256_free(&sha);
  if (memcmp(dig, expect, 32) != 0) { Update.abort(); otaFail("SHA-256 tidak cocok. Update dibatalkan"); return; }
#if OTA_REQUIRE_SIGNATURE
  if (!otaVerifySig(dig, sig, sigLen)) { Update.abort(); otaFail("Tanda tangan firmware tidak sah. Dibatalkan"); return; }
#endif
#if OTA_REQUIRE_VERSION_MARKER
  if (!markFound) { Update.abort(); otaFail("Penanda versi tidak ada di firmware"); return; }
  if (otaParseVer(fwVer) != otaInfo.latestV) { Update.abort(); otaFail("Versi di dalam firmware tidak sama dengan rilis"); return; }
#if !OTA_ALLOW_DOWNGRADE
  if (otaParseVer(fwVer) <= otaCurV) { Update.abort(); otaFail("Firmware bukan versi yang lebih baru"); return; }
#endif
#endif
  if (!Update.end(true)) { otaFail("Gagal menyelesaikan update"); Serial.println(Update.errorString()); return; }
  Serial.printf("[OTA] firmware %s terverifikasi (SHA-256%s) dan siap boot\n", otaInfo.latest, OTA_REQUIRE_SIGNATURE ? " + tanda tangan" : "");
  otaInfo.pct = 100; otaInfo.state = OTA_READY; otaInfo.changed = true;
}

// ---------------------------------------------------------------- tugas FreeRTOS
static void otaCheckTask(void*) {
  otaDoCheck(true);
  otaInfo.changed = true; otaInfo.busy = false;
  vTaskDelete(NULL);
}
static void otaInstallTask(void*) {
  otaDoInstall();
  otaInfo.changed = true; otaInfo.busy = false;
  vTaskDelete(NULL);
}
static bool otaSpawn(TaskFunction_t fn, const char* name) {
  if (heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) < 45000) { otaFail("Memori internal kurang untuk HTTPS"); return false; }
  otaInfo.busy = true;
  if (xTaskCreatePinnedToCore(fn, name, 16384, NULL, 1, NULL, 0) != pdPASS) { otaInfo.busy = false; otaFail("Gagal membuat tugas update"); return false; }
  return true;
}
// netErr = nullptr bila jaringan siap; selain itu teks alasan
static void otaStartCheck(bool user, const char* netErr) {
  if (otaInfo.busy || otaInstalling()) return;
  if (netErr) { if (user) { otaInfo.silent = false; otaFail(netErr); } return; }
  if (!otaHaveSlot()) { if (user) { otaInfo.silent = false; otaFail("Partisi OTA tidak ada. Pakai partitions.csv 2 slot"); } return; }
  otaInfo.silent = !user;
  if (user) { otaInfo.state = OTA_CHECKING; otaInfo.err[0] = 0; }
  otaInfo.changed = true;
  otaSpawn(otaCheckTask, "otachk");
}
static void otaStartInstall(const char* netErr) {
  if (otaInfo.busy || otaInstalling() || !otaInfo.avail) return;
  otaInfo.silent = false;
  if (netErr) { otaFail(netErr); return; }
  if (!otaHaveSlot()) { otaFail("Partisi OTA tidak ada. Pakai partitions.csv 2 slot"); return; }
  otaInfo.state = OTA_DOWNLOADING; otaInfo.pct = 0; otaInfo.got = 0; otaInfo.err[0] = 0; otaInfo.changed = true;
  if (!otaSpawn(otaInstallTask, "otainst")) return;
}

// ---------------------------------------------------------------- rollback
static void otaRollback() {
  Serial.println("[OTA] firmware baru tidak sehat -> ROLLBACK ke firmware lama");
  prefs.putUInt("oBad", otaCurV); prefs.putUChar("oRb", 1);
  prefs.remove("oPend"); prefs.remove("oJust"); prefs.putUChar("oBoot", 0);
#ifdef CONFIG_APP_ROLLBACK_ENABLE
  esp_ota_mark_app_invalid_rollback_and_reboot();           // kembali ke sini hanya bila gagal
#endif
  const esp_partition_t* other = esp_ota_get_next_update_partition(NULL);   // slot satunya = firmware sebelumnya
  if (other) {
    uint8_t magic = 0; esp_ota_img_states_t st = ESP_OTA_IMG_UNDEFINED;
    bool bad = esp_ota_get_state_partition(other, &st) == ESP_OK && (st == ESP_OTA_IMG_INVALID || st == ESP_OTA_IMG_ABORTED);
    if (esp_partition_read(other, 0, &magic, 1) == ESP_OK && magic == 0xE9 && !bad && esp_ota_set_boot_partition(other) == ESP_OK) {
      delay(100); esp_restart();
    }
  }
  Serial.println("[OTA] rollback tidak bisa dilakukan, lanjut boot");
}

// Panggil di awal setup() SETELAH prefs.begin()
static void otaBootInit() {
  memset((void*)&otaInfo, 0, sizeof(otaInfo));
  otaInfo.state = OTA_IDLE;
  otaCurV = otaParseVer(ESPOS_VERSION);
  otaMinV = prefs.getUInt("oMin", 0);
  otaBadV = prefs.getUInt("oBad", 0);
  otaAutoCheck = prefs.getUChar("oAuto", 1) != 0;
  otaAutoInst  = prefs.getUChar("oAInst", 0) != 0;
  const esp_partition_t* run = esp_ota_get_running_partition();
  esp_ota_img_states_t st = ESP_OTA_IMG_UNDEFINED;
  bool pendState = run && esp_ota_get_state_partition(run, &st) == ESP_OK && st == ESP_OTA_IMG_PENDING_VERIFY;
  otaPending = pendState || prefs.getUChar("oPend", 0) != 0;
  Serial.printf("[OTA] espOS %s | min %u | pending %d | slot %s\n", ESPOS_VERSION, (unsigned)otaMinV, (int)otaPending, run ? run->label : "?");
  if (prefs.getUChar("oRb", 0)) {
    char bv[16]; otaVerStr(otaBadV, bv, sizeof(bv));
    snprintf(otaInfo.boot, sizeof(otaInfo.boot), "Update v%s gagal, kembali ke v%s", bv, ESPOS_VERSION);
    prefs.remove("oRb");
  } else if (prefs.getUChar("oJust", 0)) {
    snprintf(otaInfo.boot, sizeof(otaInfo.boot), "Berhasil diperbarui ke v%s", ESPOS_VERSION);
    prefs.remove("oJust");
  }
  if (otaPending) {
    uint8_t n = prefs.getUChar("oBoot", 0) + 1;
    prefs.putUChar("oBoot", n);
    Serial.printf("[OTA] boot percobaan ke-%u dari maks %u\n", (unsigned)n, (unsigned)OTA_MAX_BOOT_TRIES);
    if (n > OTA_MAX_BOOT_TRIES) otaRollback();
  }
  // update yang sudah diketahui sebelum reboot -> notifikasi langsung muncul
  String t = prefs.getString("uTag", "");
  if (t.length() && otaTagOk(t.c_str()) && otaParseVer(t.c_str())) {
    strcpy(otaInfo.tag, t.c_str()); otaInfo.latestV = otaParseVer(t.c_str());
    otaVerStr(otaInfo.latestV, otaInfo.latest, sizeof(otaInfo.latest));
    otaEvaluate(true);
    if (!otaInfo.avail) { otaInfo.state = OTA_IDLE; prefs.remove("uTag"); }
  }
}

// Panggil tiap loop: tandai firmware SEHAT setelah berjalan normal OTA_VALIDATE_AFTER_MS
static void otaHealthTick(uint32_t now) {
  if (otaValidated || now < OTA_VALIDATE_AFTER_MS) return;
  otaValidated = true;
  if (otaPending) {
    esp_ota_mark_app_valid_cancel_rollback();
    prefs.remove("oPend"); prefs.putUChar("oBoot", 0);
    otaPending = false;
    Serial.println("[OTA] firmware baru dinyatakan SEHAT (rollback dibatalkan)");
  }
  if (otaCurV > otaMinV) { otaMinV = otaCurV; prefs.putUInt("oMin", otaMinV); }   // versi sehat tertinggi (anti-downgrade)
}

// Panggil tiap loop. net = Wi-Fi terhubung & jam sudah sinkron (sertifikat butuh jam yang benar)
static void otaTick(uint32_t now, bool net) {
  static bool wasBusy = false;
  if (wasBusy && !otaInfo.busy) {
    wasBusy = false;
    bool err = otaInfo.state == OTA_ERROR || otaInfo.err[0];
    otaInfo.nextCheck = now + (err ? OTA_RETRY_INTERVAL_MS : OTA_CHECK_INTERVAL_MS);
    if (otaInfo.avail && otaInfo.tag[0]) prefs.putString("uTag", otaInfo.tag);
    else if (otaInfo.state == OTA_UPTODATE) prefs.remove("uTag");
    otaInfo.changed = true;
  }
  if (otaInfo.busy) { wasBusy = true; return; }
  if (!otaAutoCheck || !net || otaInstalling()) return;
  if (!otaInfo.sched) { otaInfo.sched = true; otaInfo.nextCheck = now + OTA_FIRST_CHECK_DELAY_MS; return; }
  if ((int32_t)(now - otaInfo.nextCheck) >= 0) { otaInfo.err[0] = 0; otaStartCheck(false, nullptr); otaInfo.nextCheck = now + OTA_RETRY_INTERVAL_MS; }
}

// Dipanggil sekali saat state READY: simpan penanda lalu caller me-restart
static void otaPrepareReboot() {
  prefs.putUChar("oBoot", 0); prefs.putUChar("oPend", 1); prefs.putUChar("oJust", 1);
  prefs.remove("uTag");
}
