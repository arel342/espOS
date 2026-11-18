// psram_heap.h  -  Manajer Memori PSRAM untuk espOS
// ESP32-S3 N16R8 : 16 MB Flash  +  8 MB OPI PSRAM
// ============================================================
//
//  SETUP ARDUINO IDE  (WAJIB sebelum compile):
//  ─────────────────────────────────────────────────────────
//  Board             : ESP32S3 Dev Module
//  PSRAM             : OPI PSRAM          ← INI PALING PENTING
//  Flash Size        : 16 MB (128Mb)
//  Partition Scheme  : pakai partitions.csv di folder sketch (2 slot OTA @4MB + LittleFS ~7.8MB)
//                      JANGAN pakai skema 1 slot ("Huge APP", "2MB APP/12.5MB FATFS") -> OTA tidak bisa
//  Upload Speed      : 921600
//  CPU Frequency     : 240 MHz (opsional, untuk performa)
//
//  CARA PAKAI DI espOS_S3.ino :
//  ─────────────────────────────────────────────────────────
//  1.  #define FB_PREFER_PSRAM   1    // ubah dari 0 ke 1
//  2.  #define BG_CACHE_INTERNAL 0    // ubah dari 1 ke 0
//  3.  #include "psram_heap.h"        // tambah SETELAH espos_types.h
//  4.  Di setup(), baris PERTAMA: psramInit();
//  5.  Di sysinfo, tambah baris PSRAM bebas (lihat contoh di bawah)
//
// ============================================================
#pragma once
#include <Arduino.h>
#include "esp_heap_caps.h"

// ============================================================
//  KONFIGURASI UKURAN BUFFER DI PSRAM
// ============================================================

// Catatan: 4 catatan × 16 KB = 64 KB (jauh lebih besar dari
// buffer stack/internal sebelumnya yang hanya ~512 B per catatan)
#define PSRAM_NOTE_SIZE       (16 * 1024)   // 16 KB / catatan
#define PSRAM_NOTE_COUNT      4

// Buffer untuk membaca isi file di SC_FILEVIEW
// 64 KB → bisa tampung file teks lumayan besar
#define PSRAM_FILEVIEW_SIZE   (64 * 1024)

// Work-buffer serbaguna; bisa dipakai app apa pun
// (kalkulator, string sementara, dll.) — 32 KB
#define PSRAM_WORKBUF_SIZE    (32 * 1024)

// Cache string pendek/widget — pool blok kecil
#define PSRAM_POOL_BLOCK      512           // ukuran per blok
#define PSRAM_POOL_COUNT      64            // 64 × 512 B = 32 KB

// Ruang bebas yang sengaja TIDAK dialokasikan di sini,
// supaya WiFi-stack, NTP, dll. tetap bisa malloc dari PSRAM.
// Sisa ≈ 8192 KB − (semua di bawah) ≈ 7+ MB masih bebas untuk
// heap_caps_malloc(MALLOC_CAP_SPIRAM) kapan saja.

// ============================================================
//  MAKRO ALOKASI PSRAM
// ============================================================

// Malloc/calloc/free eksplisit ke PSRAM
#define PSRAM_MALLOC(sz)         heap_caps_malloc((sz), MALLOC_CAP_SPIRAM)
#define PSRAM_CALLOC(n, sz)      heap_caps_calloc((n), (sz), MALLOC_CAP_SPIRAM)
#define PSRAM_REALLOC(p, sz)     heap_caps_realloc((p), (sz), MALLOC_CAP_SPIRAM)
#define PSRAM_FREE(p)            heap_caps_free(p)

// Fallback ke internal RAM jika PSRAM penuh (jarang terjadi di N16R8)
#define PSRAM_MALLOC_OR_INTERNAL(sz) \
  ([](size_t _s) -> void* { \
    void* _p = heap_caps_malloc(_s, MALLOC_CAP_SPIRAM); \
    return _p ? _p : heap_caps_malloc(_s, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT); \
  }(sz))

// new-operator yang langsung menggunakan PSRAM
//   contoh:  uint16_t* buf = PSRAM_NEW_ARRAY(uint16_t, 76800);
#define PSRAM_NEW_ARRAY(T, n)    ((T*)PSRAM_CALLOC((n), sizeof(T)))
#define PSRAM_DEL_ARRAY(p)       PSRAM_FREE(p)

// ============================================================
//  STRUKTUR DATA MANAJER PSRAM
// ============================================================
struct PsramMgr {
  // ---- buffer tetap (dialokasikan 1× saat boot) ----
  char*    note[PSRAM_NOTE_COUNT];   // konten 4 catatan
  char*    fileView;                 // isi file yang sedang dilihat
  char*    workBuf;                  // work-buffer serbaguna

  // ---- pool blok kecil ----
  uint8_t* pool;                     // memori pool
  bool     poolUsed[PSRAM_POOL_COUNT];

  // ---- status ----
  bool     ok;                       // true jika PSRAM berhasil init
  uint32_t totalKB;
  uint32_t reservedKB;               // yang dialokasikan oleh psramInit()
};

// Instance global — akses via PSRAM (alias psramMgr)
static PsramMgr psramMgr = {};
#define PSRAM psramMgr               // alias singkat

// ============================================================
//  INISIALISASI  (panggil di setup() SEBELUM hal lain)
// ============================================================
static bool psramManagerInit() {
  if (!psramFound()) {
    Serial.println(F("[PSRAM] !! TIDAK DITEMUKAN !!"));
    Serial.println(F("[PSRAM]    Cek Arduino IDE: Tools > PSRAM > OPI PSRAM"));
    psramMgr.ok = false;
    return false;
  }

  psramMgr.totalKB = (uint32_t)(ESP.getPsramSize() / 1024);
  size_t freeBefore = ESP.getFreePsram();

  Serial.printf("[PSRAM] Ditemukan  : %u KB (%.1f MB)\n",
    psramMgr.totalKB, psramMgr.totalKB / 1024.0f);

  // ---- alokasi catatan ----
  for (int i = 0; i < PSRAM_NOTE_COUNT; i++) {
    psramMgr.note[i] = (char*)PSRAM_CALLOC(1, PSRAM_NOTE_SIZE);
    if (!psramMgr.note[i]) {
      Serial.printf("[PSRAM] GAGAL: catatan[%d]\n", i);
      goto fail;
    }
  }

  // ---- alokasi file viewer ----
  psramMgr.fileView = (char*)PSRAM_CALLOC(1, PSRAM_FILEVIEW_SIZE);
  if (!psramMgr.fileView) { Serial.println(F("[PSRAM] GAGAL: fileView")); goto fail; }

  // ---- alokasi work-buffer ----
  psramMgr.workBuf = (char*)PSRAM_CALLOC(1, PSRAM_WORKBUF_SIZE);
  if (!psramMgr.workBuf) { Serial.println(F("[PSRAM] GAGAL: workBuf")); goto fail; }

  // ---- alokasi pool ----
  psramMgr.pool = (uint8_t*)PSRAM_CALLOC(PSRAM_POOL_COUNT, PSRAM_POOL_BLOCK);
  if (!psramMgr.pool) { Serial.println(F("[PSRAM] GAGAL: pool")); goto fail; }
  memset(psramMgr.poolUsed, false, sizeof(psramMgr.poolUsed));

  psramMgr.reservedKB = (uint32_t)((freeBefore - ESP.getFreePsram()) / 1024);
  psramMgr.ok = true;

  Serial.printf("[PSRAM] Dipesan    : %u KB\n", psramMgr.reservedKB);
  Serial.printf("[PSRAM] Bebas sisa : %u KB\n", (unsigned)(ESP.getFreePsram() / 1024));
  Serial.printf("[PSRAM] Siap       : ya\n");
  return true;

fail:
  psramMgr.ok = false;
  return false;
}

// ============================================================
//  POOL ALLOCATOR  (blok 512-byte dari PSRAM, sangat cepat)
// ============================================================

// Ambil satu blok dari pool PSRAM.
// Jika pool penuh → fallback ke PSRAM_MALLOC biasa.
static void* psramPoolAlloc() {
  if (!psramMgr.ok || !psramMgr.pool) return PSRAM_MALLOC(PSRAM_POOL_BLOCK);
  for (int i = 0; i < PSRAM_POOL_COUNT; i++) {
    if (!psramMgr.poolUsed[i]) {
      psramMgr.poolUsed[i] = true;
      memset(psramMgr.pool + (size_t)i * PSRAM_POOL_BLOCK, 0, PSRAM_POOL_BLOCK);
      return psramMgr.pool + (size_t)i * PSRAM_POOL_BLOCK;
    }
  }
  // Pool penuh — alokasi langsung
  return PSRAM_MALLOC(PSRAM_POOL_BLOCK);
}

// Kembalikan blok ke pool (atau free jika bukan dari pool).
static void psramPoolFree(void* p) {
  if (!p) return;
  if (psramMgr.ok && psramMgr.pool) {
    uint8_t* base = psramMgr.pool;
    uint8_t* ptr  = (uint8_t*)p;
    size_t   span = (size_t)PSRAM_POOL_COUNT * PSRAM_POOL_BLOCK;
    if (ptr >= base && ptr < base + span) {
      size_t idx = (ptr - base) / PSRAM_POOL_BLOCK;
      if (idx < (size_t)PSRAM_POOL_COUNT) {
        psramMgr.poolUsed[idx] = false;
        return;
      }
    }
  }
  PSRAM_FREE(p);
}

// ============================================================
//  HELPER UMUM
// ============================================================

// Duplikat string ke PSRAM (pengganti strdup yang ke internal)
static char* psramStrdup(const char* src) {
  if (!src) return nullptr;
  size_t n = strlen(src) + 1;
  char*  d = (char*)PSRAM_MALLOC(n);
  if (d) memcpy(d, src, n);
  return d;
}

// Alokasi buffer pixel (RGB565) di PSRAM
// contoh: u16* canvas = psramAllocPixels(240, 320);
static uint16_t* psramAllocPixels(int w, int h) {
  return (uint16_t*)PSRAM_CALLOC((size_t)w * h, sizeof(uint16_t));
}

// ============================================================
//  STATISTIK  (untuk SC_SYSINFO)
// ============================================================

// Format "bebas / total KB" ke buffer
static void psramStatStr(char* buf, size_t bufLen) {
  if (!psramFound()) {
    snprintf(buf, bufLen, "Tidak ada");
    return;
  }
  uint32_t freeKB  = (uint32_t)(ESP.getFreePsram()  / 1024);
  uint32_t totalKB = (uint32_t)(ESP.getPsramSize()   / 1024);
  snprintf(buf, bufLen, "%u / %u KB", freeKB, totalKB);
}

// Cetak ringkasan ke Serial
static void psramPrintStats() {
  if (!psramFound()) { Serial.println(F("[PSRAM] Tidak ada")); return; }
  uint32_t total = (uint32_t)(ESP.getPsramSize()  / 1024);
  uint32_t free_ = (uint32_t)(ESP.getFreePsram()  / 1024);
  uint32_t used  = total - free_;
  Serial.printf("[PSRAM] Total %u KB | Terpakai %u KB (%.0f%%) | Bebas %u KB\n",
    total, used, 100.0f * used / total, free_);
}

// ============================================================
//  FRAMEBUFFER HELPER
//  Gunakan fungsi ini sebagai pengganti allocFrame() di .ino
// ============================================================

// Alokasi satu framebuffer (240×320 × 2 byte = 150 KB) di PSRAM.
// mustPsram = true  → crash jika PSRAM tidak ada (fb utama)
// mustPsram = false → coba PSRAM, fallback ke internal (bg/snap)
static uint16_t* psramAllocFramebuf(bool mustPsram) {
  static const size_t FB = (size_t)240 * 320 * 2;   // 150 KB
  uint16_t* p = (uint16_t*)PSRAM_MALLOC(FB);
  if (p) { memset(p, 0, FB); return p; }
  if (mustPsram) return nullptr;   // caller akan error-handle
  // fallback ke internal RAM
  p = (uint16_t*)heap_caps_malloc(FB, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (p) memset(p, 0, FB);
  return p;
}

// ============================================================
//  CATATAN TENTANG SISA PSRAM
// ============================================================
//
//  N16R8 total PSRAM : 8 192 KB
//  Dipesan psramInit:
//    catatan 4×16 KB :   64 KB
//    fileView        :   64 KB
//    workBuf         :   32 KB
//    pool 64×512 B   :   32 KB
//    ─────────────────  ───────
//    Sub-total        :  192 KB
//
//  3 framebuffer (fb+bgBuf+snapBuf) × 150 KB = 450 KB
//  ─────────────────────────────────────────────────
//  Total dipesan      :  642 KB
//  Sisa bebas untuk   : ~7 550 KB   ← WiFi stack, NTP,
//  heap_caps_malloc()               dynamic alloc app lain
//
// ============================================================