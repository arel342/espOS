// ota_config.h - pengaturan update OTA espOS (EDIT FILE INI, bukan ota_update.h)
// ============================================================================
//  ALUR RILIS (ringkas, detail ada di README_OTA.md):
//    1. Naikkan ESPOS_VERSION di bawah (mis. "1.8.0" -> "1.9.0")
//    2. Arduino IDE: Sketch > Export Compiled Binary  -> ambil  espOS_S3.ino.bin
//       (BUKAN yang .merged.bin / .bootloader.bin / .partitions.bin)
//    3. Jalankan  tools/make_release.sh  -> membuat .sha256 (dan .sig bila memakai tanda tangan)
//    4. GitHub > Releases > Draft new release:  tag "v1.9.0"  (harus SAMA dengan ESPOS_VERSION),
//       unggah:  espOS_S3.ino.bin, espOS_S3.ino.bin.sha256, (opsional) espOS_S3.ino.bin.sig
// ============================================================================
#pragma once

// ---- versi firmware ini (format  MAJOR.MINOR.PATCH, tiap angka 0..1023) ----
#define ESPOS_VERSION        "1.8.0"

// ---- sumber rilis di GitHub: https://github.com/<OWNER>/<REPO>/releases ----
#define OTA_GH_OWNER         "arel342"
#define OTA_GH_REPO          "espOS"

// ---- nama berkas di rilis ----
#define OTA_ASSET_BIN        "espOS_S3.ino.bin"
#define OTA_ASSET_SHA        OTA_ASSET_BIN ".sha256"     // hasil: sha256sum espOS_S3.ino.bin
#define OTA_ASSET_SIG        OTA_ASSET_BIN ".sig"        // hasil: openssl dgst -sha256 -sign (hanya bila OTA_REQUIRE_SIGNATURE 1)

// ---- jadwal cek otomatis (saat Wi-Fi terhubung & jam sudah sinkron NTP) ----
#define OTA_FIRST_CHECK_DELAY_MS   15000UL               // cek pertama, setelah Wi-Fi + NTP siap
#define OTA_CHECK_INTERVAL_MS      (6UL * 3600UL * 1000UL)   // cek ulang tiap 6 jam
#define OTA_RETRY_INTERVAL_MS      (10UL * 60UL * 1000UL)    // bila gagal, coba lagi 10 menit

// ---- KEAMANAN ----
// Tanda tangan firmware (disarankan untuk perangkat sensitif). 1 = wajib: firmware TANPA tanda tangan
// yang sah akan ditolak. Isi OTA_SIGN_PUBKEY_PEM di bawah dengan kunci publik milik Anda.
#define OTA_REQUIRE_SIGNATURE      0

// 0 = tolak versi yang sama / lebih lama (DEFAULT, sangat disarankan)
// 1 = izinkan versi berbeda yang lebih lama (hanya untuk debugging / rollback manual yang disengaja)
#define OTA_ALLOW_DOWNGRADE        0

// 1 = firmware yang diunduh wajib memuat penanda versi yang sama dengan tag rilis
//     (mencegah file lama yang dilabeli versi baru). Penanda ditanam otomatis oleh ota_update.h.
#define OTA_REQUIRE_VERSION_MARKER 1

// Sertifikat HTTPS:
//   0 = pakai bundel root CA terpercaya bawaan Arduino-ESP32 (Mozilla) -> otomatis, tahan pergantian CA GitHub
//   1 = pakai SATU root CA yang Anda tanam sendiri di OTA_ROOT_CA_PEM (pinning; harus Anda perbarui bila GitHub ganti CA)
#define OTA_PIN_ROOT_CA            0

// ---- ROLLBACK ----
#define OTA_VALIDATE_AFTER_MS      15000UL   // firmware baru dianggap sehat bila hidup normal selama ini
#define OTA_MAX_BOOT_TRIES         3         // boot ke-4 sebelum dinyatakan sehat -> kembali ke firmware lama

// ---- kunci publik tanda tangan (PEM). Buat dengan tools/gen_signing_key.sh ----
static const char OTA_SIGN_PUBKEY_PEM[] __attribute__((unused)) =
  "-----BEGIN PUBLIC KEY-----\n"
  "ISI_DENGAN_KUNCI_PUBLIK_ANDA\n"
  "-----END PUBLIC KEY-----\n";

// ---- (opsional) root CA GitHub yang ditanam, hanya dipakai bila OTA_PIN_ROOT_CA 1 ----
static const char OTA_ROOT_CA_PEM[] __attribute__((unused)) =
  "-----BEGIN CERTIFICATE-----\n"
  "ISI_DENGAN_ROOT_CA_PEM\n"
  "-----END CERTIFICATE-----\n";
