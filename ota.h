#pragma once
#include <algorithm>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <atomic>
#include "esp_ota_ops.h"
#include "mbedtls/md.h"
#include "ota_ca.h"

#define ESPOS_BUILD 1600
// Strong override of the Arduino core hook; setup confirms after local self-test.
extern "C" bool verifyRollbackLater() { return true; }
#if defined(ESPOS_REQUIRE_SIGNATURE) && !CONFIG_SECURE_SIGNED_APPS
#error "Sensitive build requires IDF signed-app verification"
#endif
namespace ota {
static const char releases[] = "https://github.com/arel342/espOS/releases/";
enum State { Idle, Checking, Current, Available, Writing, Failed, Unsupported, Blocked, Clock };
static const char* const labels[] = {"Belum diperiksa", "Memeriksa...", "Sudah terbaru", "Update tersedia", "Memasang update...", "Gagal, coba lagi", "Perlu slot OTA + rollback", "Build pernah dipasang", "Menunggu jam NTP"};
static std::atomic<State> state{Idle};
static std::atomic<bool> busy{false};
static uint32_t last;
static bool scheduled;
static bool pending() {
  esp_ota_img_states_t s;
  return esp_ota_get_state_partition(esp_ota_get_running_partition(), &s)==ESP_OK && s==ESP_OTA_IMG_PENDING_VERIFY;
}
// No automatic redirects: validate scheme and authority at EVERY hop.
static bool open(HTTPClient& h, WiFiClientSecure& tls, String url) {
  for (int hop=0; hop<5; ++hop) {
    if (!url.startsWith(releases) && !url.startsWith("https://release-assets.githubusercontent.com/") && !url.startsWith("https://objects.githubusercontent.com/")) return false;
    if (!h.begin(tls,url)) return false;
    h.useHTTP10(true);
    const char* headers[]={"Location"}; h.collectHeaders(headers,1);
    int code=h.GET();
    if (code==200) return true;
    url=h.header("Location"); h.end();
    if (code!=301 && code!=302 && code!=307 && code!=308) return false;
  }
  return false;
}
static State run() {
#if !CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
  return Unsupported;
#else
  const esp_partition_t* slot=esp_ota_get_next_update_partition(nullptr);
  if (!slot || slot->address==esp_ota_get_running_partition()->address || pending()) return Unsupported;
  if (time(nullptr)<1704067200) return Clock;
  WiFiClientSecure tls; tls.setCACert(otaRoots);
  HTTPClient h;
  if (!open(h,tls,String(releases)+"latest/download/espos-s3.txt")) return Failed;
  int len=h.getSize(); char meta[96]={};
  if (len<=0 || len>=sizeof(meta) || h.getStream().readBytes(meta,len)!=len) return Failed;
  h.end();
  // Canonical manifest: decimal build, one space, lowercase SHA-256, LF.
  char* end; unsigned long build=strtoul(meta,&end,10);
  if (!build || build>=UINT32_MAX || String(build)!=String(meta).substring(0,end-meta) || *end!=' ' || (meta+len-end)!=66 || meta[len-1]!='\n') return Failed;
  char* hash=end+1;
  if (strspn(hash,"0123456789abcdef")!=64) return Failed;
  if (build<=ESPOS_BUILD) return Current;
  Preferences p; if (!p.begin("espos-ota",false)) return Failed;
  if (build<=p.getUInt("attempt",0)) return Blocked;
  state=Available; vTaskDelay(pdMS_TO_TICKS(15000)); // time to open the notification
  state=Writing;
  if (!open(h,tls,String(releases)+"download/ota-"+String(build)+"/espos-s3.bin")) return Failed;
  int remaining=h.getSize();
  esp_ota_handle_t handle;
  if (remaining<=0 || remaining>slot->size || esp_ota_begin(slot,remaining,&handle)!=ESP_OK) return Failed;
  mbedtls_md_context_t md; mbedtls_md_init(&md);
  bool ok=mbedtls_md_setup(&md,mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),0)==0 && mbedtls_md_starts(&md)==0;
  uint8_t buf[1024], digest[32];
  while (ok && remaining>0) {
    size_t n=std::min((size_t)remaining,sizeof(buf));
    ok=h.getStream().readBytes((char*)buf,n)==n && mbedtls_md_update(&md,buf,n)==0 && esp_ota_write(handle,buf,n)==ESP_OK;
    remaining-=n;
  }
  ok=ok && mbedtls_md_finish(&md,digest)==0; mbedtls_md_free(&md);
  for (int i=0; ok && i<32; ++i) { char hex[3]; snprintf(hex,sizeof(hex),"%02x",digest[i]); ok=!memcmp(hex,hash+2*i,2); }
  if (!ok) { esp_ota_abort(handle); return Failed; }
  // IDF also checks image format/chip and signature when signed apps are enforced.
  if (esp_ota_end(handle)!=ESP_OK || p.putUInt("attempt",build)!=sizeof(uint32_t) || esp_ota_set_boot_partition(slot)!=ESP_OK) return Failed;
  ESP.restart(); return Writing;
#endif
}
static void worker(void*) { state=run(); busy=false; vTaskDelete(nullptr); }
static void check() {
  if (WiFi.status()!=WL_CONNECTED || busy.exchange(true)) return;
  last=millis(); scheduled=true; state=Checking;
  if (xTaskCreate(worker,"ota",12288,nullptr,1,nullptr)!=pdPASS) { state=Failed; busy=false; }
}
static void tick() {
  if (WiFi.status()!=WL_CONNECTED) { scheduled=false; return; }
  State s=state.load();
  if (!scheduled || millis()-last >= (s==Clock ? 30000UL : s==Failed ? 900000UL : 21600000UL)) check();
}
}
