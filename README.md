# espOS

**espOS** adalah proyek antarmuka mini-smartphone berbasis **ESP32-S3**, layar sentuh TFT ILI9341, dan touchscreen XPT2046. Proyek ini menyediakan home screen, aplikasi bawaan, control panel, lock screen, konektivitas Wi-Fi, mini game, manajemen PSRAM, serta pembaruan firmware melalui Wi-Fi (OTA) dari GitHub Releases.

> **Firmware pada source ZIP:** v1.8.0  
> **Platform utama:** ESP32-S3  
> **Pengembang:** Farel

---

## Daftar fitur

### Antarmuka mini-smartphone
- Boot animation dan home screen bergaya smartphone.
- Dua halaman aplikasi yang dapat digeser ke kiri dan kanan.
- Tema Aurora, Senja, dan Mono.
- Animasi transisi layar dan aplikasi.
- Navigasi Back, Home, dan Recent Apps.
- Control panel dengan gesture swipe dan slider kecerahan.
- Lock screen dengan jam dan tanggal; swipe ke atas untuk membuka.
- Wizard setup awal untuk perangkat yang belum memiliki konfigurasi espOS.
- Reset pabrik melalui Settings dengan konfirmasi.

### Aplikasi bawaan
- **Berkas:** melihat daftar file LittleFS, membaca file teks, menghapus file, dan melihat kapasitas penyimpanan.
- **Kalkulator:** operasi dasar, persentase, perubahan tanda, dan clear.
- **Catatan:** empat slot catatan dengan keyboard QWERTY di layar.
- **Stopwatch:** pengukur waktu dengan kontrol mulai dan berhenti.
- **Setelan:** tema, waktu dan tanggal, kunci layar, kalibrasi touchscreen, informasi sistem, update espOS, dan reset pabrik.

### Sistem dan konektivitas
- Kunci layar menggunakan PIN 4–8 digit atau pola minimal 4 titik.
- Penyimpanan kredensial kunci layar dalam bentuk hash.
- Pembatasan percobaan PIN/pola yang salah.
- Wi-Fi Station: pemindaian jaringan, input password melalui keyboard layar, penyimpanan jaringan, dan auto-connect.
- Sinkronisasi waktu melalui NTP dengan zona waktu WIB (UTC+7).
- Senter menggunakan LED RGB onboard yang kompatibel.
- Light sleep dan deep sleep.
- Informasi CPU, RAM internal, PSRAM, flash, dan uptime.
- Kalibrasi touchscreen otomatis pada penggunaan pertama dan penyimpanan hasil kalibrasi.

### Mini game
- **Dino:** berlari, melompat, dan menghindari rintangan.
- **Ping Pong:** permainan melawan komputer dengan kontrol sentuh.
- **Flappy:** melewati pipa dan menyimpan skor terbaik.

### Optimasi tampilan dan memori
- Framebuffer RGB565.
- Alokasi framebuffer mengutamakan PSRAM.
- Buffer tambahan untuk animasi/transisi jika memori tersedia.
- Pipeline render dan transfer frame TFT menggunakan task FreeRTOS.
- Manajer alokasi PSRAM untuk buffer catatan, file viewer, work buffer, pool, dan framebuffer.

---

## Sistem OTA v1.8.0

espOS dapat memeriksa dan memasang firmware baru melalui Wi-Fi menggunakan GitHub Releases. Firmware OTA menggunakan modul terpisah agar konfigurasi, proses update, dan fungsi utilitas lebih mudah dirawat.

### Fitur OTA
- Memeriksa rilis terbaru dari GitHub Releases.
- Pemeriksaan pertama setelah Wi-Fi dan waktu NTP siap.
- Pemeriksaan berkala setiap 6 jam.
- Pemeriksaan manual dari **Setelan → Update espOS**.
- Opsi pemeriksaan otomatis.
- Opsi pemasangan otomatis ketika perangkat terkunci (default nonaktif).
- Menampilkan versi terbaru, status unduhan, progres, dan pesan kesalahan.
- Mengunduh firmware ke slot aplikasi OTA yang tidak sedang digunakan.
- Verifikasi SHA-256 sebelum firmware diaktifkan.
- Pemeriksaan penanda versi di dalam binary agar cocok dengan tag rilis.
- Proteksi downgrade berdasarkan versi firmware dan versi sehat tertinggi yang tersimpan.
- Menolak kembali menawarkan versi yang sebelumnya gagal dan di-rollback.
- Verifikasi tanda tangan firmware ECDSA/RSA secara opsional.
- Mekanisme rollback jika firmware baru gagal melewati tahap validasi boot.
- HTTPS dengan validasi sertifikat dan pembatasan host tujuan/redirect.

**Catatan keamanan:** SHA-256 dari file checksum yang diunggah bersama binary membantu mendeteksi file rusak atau tidak utuh, tetapi tidak membuktikan siapa penerbit firmware. Untuk perlindungan terhadap rilis berbahaya atau akun GitHub yang diambil alih, aktifkan verifikasi tanda tangan dan simpan private key di luar repository.

### Persiapan OTA pertama kali
OTA memerlukan konfigurasi partisi yang mendukung dua slot aplikasi, `otadata`, dan LittleFS. Karena itu:

1. Gunakan board ESP32-S3 dengan flash yang sesuai.
2. Salin `partitions.csv` ke folder sketch.
3. Pilih Flash Size **16 MB** untuk board N16R8.
4. Aktifkan PSRAM **OPI PSRAM** untuk ESP32-S3 N16R8.
5. Pilih partition scheme yang menggunakan tabel partisi proyek.
6. Flash firmware melalui USB untuk pertama kali.

**Penting:** mengganti tabel partisi dapat menghapus data LittleFS yang sudah tersimpan. Backup data penting sebelum melakukan perubahan partisi atau erase flash.

### Membuat rilis OTA
1. Naikkan `ESPOS_VERSION` di `ota_config.h`, misalnya dari `1.8.0` menjadi `1.9.0`.
2. Compile firmware dengan konfigurasi board dan partisi yang sama.
3. Arduino IDE: **Sketch → Export Compiled Binary**.
4. Gunakan file aplikasi `espOS_S3.ino.bin`, bukan merged image, bootloader, atau filesystem image.
5. Jalankan script pembuat checksum:

   ```sh
   ./tools/make_release.sh espOS_S3.ino.bin
   ```

6. Buat GitHub Release dengan tag yang sama dengan versi, misalnya `v1.9.0`.
7. Upload:
   - `espOS_S3.ino.bin`
   - `espOS_S3.ino.bin.sha256`
   - `espOS_S3.ino.bin.sig` jika verifikasi tanda tangan diaktifkan.

Jangan mengganti isi rilis lama. Gunakan tag versi baru untuk setiap rilis.

### Mengaktifkan tanda tangan firmware
Secara default, `OTA_REQUIRE_SIGNATURE` bernilai `0`. Untuk mewajibkan tanda tangan:

1. Jalankan `tools/gen_signing_key.sh`.
2. Simpan `signing_private.pem` di tempat aman dan **jangan commit ke GitHub**.
3. Salin public key ke `OTA_SIGN_PUBKEY_PEM` di `ota_config.h`.
4. Ubah `OTA_REQUIRE_SIGNATURE` menjadi `1`.
5. Saat membuat rilis, berikan private key ke script:

   ```sh
   ./tools/make_release.sh espOS_S3.ino.bin signing_private.pem
   ```

6. Upload file `.sig` bersama binary dan `.sha256`.

Jangan pernah mengunggah private key ke repository atau GitHub Releases.

### Pengujian utilitas OTA di komputer
Sebagian fungsi utilitas OTA dapat diuji tanpa board ESP32:

```sh
cd tools
g++ -std=c++11 -I.. test_ota_util.cpp -o test_ota_util
./test_ota_util
```

Pengujian ini mencakup parsing versi, validasi tag dan URL, pembacaan JSON, parsing SHA-256, serta pemindaian penanda versi firmware. Lulusnya unit test tersebut **bukan** pengganti pengujian OTA pada perangkat ESP32-S3 fisik.

Dokumentasi tambahan: [`espOS_S3/README_OTA.md`](espOS_S3/README_OTA.md).

---

## Hardware

Konfigurasi utama yang dituju:

| Komponen | Keterangan |
|---|---|
| Mikrokontroler | ESP32-S3 |
| Board rekomendasi | ESP32-S3 N16R8 (16 MB Flash + 8 MB OPI PSRAM) |
| Display | TFT ILI9341 2.8 inci, 240 × 320 |
| Touch controller | XPT2046 |
| LED | LED RGB onboard yang kompatibel dengan `neopixelWrite()` |

PSRAM 8 MB direkomendasikan untuk framebuffer dan animasi. Beberapa fitur dapat menjadi terbatas jika PSRAM tidak tersedia atau alokasinya gagal.

## Wiring GPIO

Pin berikut adalah konfigurasi default firmware.

### TFT ILI9341

| Pin TFT | ESP32-S3 |
|---|---:|
| CS | GPIO 10 |
| RESET | GPIO 8 |
| DC | GPIO 9 |
| MOSI | GPIO 11 |
| SCK | GPIO 12 |
| MISO | GPIO 13 |
| BL / Backlight | GPIO 6 |
| VCC | Sesuai spesifikasi modul |
| GND | GND |

### Touchscreen XPT2046

| Pin XPT2046 | ESP32-S3 |
|---|---:|
| T_CS | GPIO 14 |
| T_CLK | GPIO 12 |
| T_DIN | GPIO 11 |
| T_DO | GPIO 13 |
| T_IRQ | Tidak digunakan |

TFT dan XPT2046 berbagi jalur SPI. Pastikan masing-masing modul menggunakan pin CS yang berbeda.

### Pin tambahan

| Fungsi | GPIO | Keterangan |
|---|---:|---|
| LED RGB onboard | GPIO 48 | Default konfigurasi |
| LED RGB alternatif | GPIO 38 | Beberapa varian DevKitC-1 v1.1 |
| Wake dari deep sleep | GPIO 0 | Tombol BOOT, aktif LOW |

Pemetaan LED dapat berbeda antarvarian board. Periksa skematik board sebelum mengubah pin atau menyambungkan LED eksternal.

---

## Software dan library

- Arduino IDE.
- ESP32 Arduino Core dari Espressif.
- Adafruit GFX Library.
- Adafruit ILI9341.

Library berikut tersedia dari ESP32 Arduino Core:
- `SPI`
- `Preferences`
- `LittleFS`
- `WiFi`
- API ESP-IDF untuk heap, OTA, GPIO, sleep, dan fungsi terkait.

Touchscreen XPT2046 dibaca langsung melalui SPI oleh firmware; library touchscreen terpisah tidak diperlukan.

## Konfigurasi Arduino IDE

| Pengaturan | Rekomendasi |
|---|---|
| Board | `ESP32S3 Dev Module` |
| Flash Size | `16MB` untuk N16R8 |
| PSRAM | `OPI PSRAM` untuk N16R8 |
| CPU Frequency | `240MHz` |
| Upload Speed | `921600` (turunkan jika upload tidak stabil) |
| Partition Scheme | Gunakan tabel partisi proyek (`partitions.csv`) |

Nama opsi dapat berbeda tergantung versi ESP32 Arduino Core. Pastikan tabel partisi yang dipilih menyediakan dua slot OTA, `otadata`, dan partisi `spiffs` untuk LittleFS.

## Struktur repository

Struktur yang disarankan untuk GitHub:

```text
espOS/
├── README.md
└── espOS_S3/
    ├── espOS_S3.ino
    ├── espos_types.h
    ├── games.h
    ├── psram_heap.h
    ├── ota_config.h
    ├── ota_update.h
    ├── ota_util.h
    ├── partitions.csv
    ├── README_OTA.md
    └── tools/
        ├── gen_signing_key.sh
        ├── make_release.sh
        └── test_ota_util.cpp
```

### Fungsi file

| File | Fungsi |
|---|---|
| `espOS_S3.ino` | Firmware utama, UI, input touchscreen, Wi-Fi, penyimpanan, dan integrasi OTA |
| `espos_types.h` | Tipe data, struktur UI, dan enum layar |
| `games.h` | Implementasi game Dino, Ping Pong, dan Flappy |
| `psram_heap.h` | Helper dan manajer alokasi PSRAM |
| `ota_config.h` | Versi firmware, sumber GitHub, jadwal OTA, dan opsi keamanan |
| `ota_update.h` | Proses pemeriksaan, unduhan, verifikasi, pemasangan, dan rollback OTA |
| `ota_util.h` | Fungsi utilitas OTA yang dapat diuji di komputer |
| `partitions.csv` | Tabel partisi ESP32-S3 untuk dua slot OTA dan LittleFS |
| `README_OTA.md` | Dokumentasi khusus alur OTA dan penerbitan rilis |
| `tools/gen_signing_key.sh` | Membuat pasangan kunci tanda tangan firmware |
| `tools/make_release.sh` | Membuat checksum dan tanda tangan file firmware |
| `tools/test_ota_util.cpp` | Unit test fungsi utilitas OTA |

**Catatan Arduino IDE:** nama folder sketch harus sama dengan nama file utama, yaitu `espOS_S3`. Simpan seluruh file firmware di dalam folder tersebut.

## Instalasi dan upload awal

1. Install Arduino IDE.
2. Install ESP32 Arduino Core melalui Boards Manager.
3. Install `Adafruit GFX Library` dan `Adafruit ILI9341` melalui Library Manager.
4. Siapkan folder sketch bernama `espOS_S3`.
5. Masukkan semua file firmware yang tercantum pada struktur proyek.
6. Salin `partitions.csv` ke folder sketch.
7. Hubungkan ESP32-S3 ke komputer melalui USB.
8. Pilih board, port, Flash Size, PSRAM, dan tabel partisi yang sesuai.
9. Compile menggunakan **Verify**.
10. Jika berhasil, pilih **Upload**.
11. Buka Serial Monitor pada baud rate **115200** untuk melihat log inisialisasi.

Setelah flash kosong, perangkat menjalankan wizard setup awal. Ikuti kalibrasi touchscreen dan langkah setup yang ditampilkan. Konfigurasi kunci layar dan Wi-Fi dapat dilewati.

## Penyimpanan data

espOS menggunakan:
- **LittleFS** untuk file dan data catatan.
- **Preferences/NVS** untuk pengaturan, tema, kecerahan, kredensial kunci layar, kalibrasi touchscreen, skor game, konfigurasi Wi-Fi, serta status proteksi OTA.

Backup data penting sebelum mengubah tabel partisi atau melakukan erase flash. Proses reset pabrik dan proses format filesystem memiliki dampak berbeda; baca konfirmasi yang ditampilkan perangkat sebelum melanjutkan.

## Batasan dan catatan keamanan

- Proyek ditujukan untuk ESP32-S3 dengan TFT ILI9341 dan touchscreen XPT2046 sesuai konfigurasi pin di atas.
- PSRAM 8 MB direkomendasikan untuk pengalaman animasi yang lebih lengkap.
- Persentase baterai pada UI merupakan nilai konfigurasi, bukan hasil pembacaan sensor baterai secara langsung.
- PIN/pola adalah penguncian antarmuka lokal, bukan pengganti proteksi kriptografis perangkat.
- Kredensial Wi-Fi disimpan di flash melalui Preferences. Jangan mengisi SSID/password pribadi di source code publik.
- `OTA_REQUIRE_SIGNATURE` default-nya nonaktif; aktifkan dan isi public key sebelum mengandalkan autentikasi penerbit firmware.
- Perubahan tabel partisi dapat menghapus data filesystem.
- Lakukan pengujian OTA dan rollback pada board cadangan sebelum memasang firmware ke perangkat utama.
- Belum ada file lisensi yang disertakan. Tambahkan lisensi jika proyek akan didistribusikan dengan ketentuan open-source tertentu.

## Kontribusi

Laporan bug, ide, dan pengembangan fitur dapat diajukan melalui GitHub Issues atau Pull Request. Sertakan jenis board, versi ESP32 Arduino Core, konfigurasi Arduino IDE, dan log Serial Monitor jika melaporkan masalah.

## Author

**Farel** — Creator of espOS.

---

*espOS is an experimental mini-smartphone interface project built with Arduino and ESP32-S3.*
