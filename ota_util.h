// ota_util.h - fungsi murni (tanpa Arduino) untuk OTA: versi, JSON mini, hex SHA-256,
// pemindai penanda versi di dalam .bin, dan daftar host yang diizinkan.
// Dipisah supaya bisa diuji di komputer (lihat tools/test_ota_util.cpp).
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

// "v1.9.0" / "1.9" -> (major<<20)|(minor<<10)|patch ; 0 = tidak valid. Sufiks (mis. -beta) = tidak valid.
static inline uint32_t otaParseVer(const char* s) {
  if (!s) return 0;
  if (*s == 'v' || *s == 'V') s++;
  if (*s < '0' || *s > '9') return 0;
  uint32_t part[3] = {0, 0, 0}; int n = 0;
  while (*s) {
    uint32_t v = 0; int digits = 0;
    while (*s >= '0' && *s <= '9') { v = v * 10 + (uint32_t)(*s - '0'); if (++digits > 4) return 0; s++; }
    if (digits == 0 || v > 1023) return 0;
    part[n++] = v;
    if (*s == '.') { s++; if (n >= 3 || *s < '0' || *s > '9') return 0; }
    else if (*s) return 0;
  }
  return (part[0] << 20) | (part[1] << 10) | part[2];
}
static inline void otaVerStr(uint32_t v, char* out, size_t cap) {
  snprintf(out, cap, "%u.%u.%u", (unsigned)(v >> 20), (unsigned)((v >> 10) & 1023), (unsigned)(v & 1023));
}

// tag rilis aman dipakai di URL: 1..23 karakter [A-Za-z0-9._-]
static inline bool otaTagOk(const char* t) {
  size_t n = t ? strlen(t) : 0;
  if (n == 0 || n > 23) return false;
  for (size_t i = 0; i < n; i++) {
    char c = t[i];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '.' || c == '_' || c == '-')) return false;
  }
  return true;
}

// URL harus HTTPS, tanpa user-info, port kosong/443, dan host = github.com | api.github.com | *.githubusercontent.com
static inline bool otaUrlOk(const char* url) {
  if (!url || strncmp(url, "https://", 8) != 0) return false;
  const char* h = url + 8; const char* e = h;
  while (*e && *e != '/' && *e != '?' && *e != '#') e++;
  size_t n = (size_t)(e - h);
  if (n == 0 || n > 80) return false;
  char host[84]; memcpy(host, h, n); host[n] = 0;
  if (strchr(host, '@') || strchr(host, '\\')) return false;
  char* c = strchr(host, ':');
  if (c) { if (strcmp(c, ":443") != 0) return false; *c = 0; }
  for (char* p = host; *p; p++) if (*p >= 'A' && *p <= 'Z') *p += 32;
  if (!strcmp(host, "github.com") || !strcmp(host, "api.github.com")) return true;
  const char* suf = ".githubusercontent.com"; size_t sl = strlen(suf), hl = strlen(host);
  return hl > sl && !strcmp(host + hl - sl, suf);
}

// ---- JSON mini: cari "key":  lalu kembalikan pointer ke awal nilai ----
static inline const char* otaJsonFind(const char* js, const char* key) {
  if (!js || !key) return nullptr;
  char needle[40]; int nl = snprintf(needle, sizeof(needle), "\"%s\"", key);
  if (nl <= 0 || nl >= (int)sizeof(needle)) return nullptr;
  const char* p = js;
  while ((p = strstr(p, needle)) != nullptr) {
    const char* q = p + nl;
    while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') q++;
    if (*q == ':') { q++; while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') q++; return q; }
    p += nl;
  }
  return nullptr;
}
// nilai string -> out (escape disederhanakan: \n \r \t jadi spasi, \" \\ \/ , \uXXXX jadi '?', UTF-8 jadi '?')
static inline bool otaJsonStr(const char* from, const char* key, char* out, size_t cap) {
  if (!out || cap == 0) return false;
  out[0] = 0;
  const char* p = otaJsonFind(from, key);
  if (!p || *p != '"') return false;
  p++;
  size_t n = 0;
  while (*p && *p != '"') {
    unsigned char c = (unsigned char)*p;
    char emit = 0;
    if (c == '\\') {
      p++; if (!*p) break;
      switch (*p) {
        case 'n': case 't': emit = ' '; break;
        case 'r': emit = 0; break;
        case 'u': emit = '?'; for (int i = 0; i < 4 && p[1]; i++) p++; break;
        default:  emit = *p; break;                    // \" \\ \/
      }
    } else if (c >= 0xC0) emit = '?';                  // awal karakter UTF-8 multi-byte
    else if (c >= 0x80) emit = 0;                      // lanjutan UTF-8: lewati
    else if (c < 0x20) emit = ' ';
    else emit = (char)c;
    if (emit && n + 1 < cap) out[n++] = emit;
    p++;
  }
  out[n] = 0;
  return *p == '"';
}

// 64 hex (boleh diikuti spasi/"*namafile", format keluaran sha256sum) -> 32 byte
static inline int otaHexV(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}
static inline bool otaParseHex256(const char* s, size_t len, uint8_t out[32]) {
  size_t i = 0;
  while (i < len && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) i++;
  if (len - i < 64) return false;
  for (int k = 0; k < 32; k++) {
    int a = otaHexV(s[i + 2 * k]), b = otaHexV(s[i + 2 * k + 1]);
    if (a < 0 || b < 0) return false;
    out[k] = (uint8_t)((a << 4) | b);
  }
  if (len - i > 64) { char c = s[i + 64]; if (c != ' ' && c != '\t' && c != '\r' && c != '\n' && c != '*') return false; }
  return true;
}

// ---- penanda versi di dalam firmware:  "ESPOS|FWV|1.8.0|" ----
#define OTA_MARK     "ESPOS|FWV|"
#define OTA_MARK_LEN 10
#define OTA_CARRY    31          // sisa ujung potongan sebelumnya (>= panjang penanda penuh - 1)
// cari penanda di buf[0..len). Penanda sah bila diawali digit dan ditutup '|'.
static inline bool otaScanMarker(const uint8_t* buf, size_t len, char* out, size_t cap) {
  if (len <= OTA_MARK_LEN) return false;
  for (size_t i = 0; i + OTA_MARK_LEN < len; i++) {
    if (buf[i] != 'E' || memcmp(buf + i, OTA_MARK, OTA_MARK_LEN) != 0) continue;
    size_t j = i + OTA_MARK_LEN, n = 0;
    if (!(buf[j] >= '0' && buf[j] <= '9')) continue;   // mis. literal pencari itu sendiri (diikuti NUL)
    while (j < len && ((buf[j] >= '0' && buf[j] <= '9') || buf[j] == '.') && n + 1 < cap) out[n++] = (char)buf[j++];
    if (j < len && buf[j] == '|') { out[n] = 0; return true; }
  }
  return false;
}
