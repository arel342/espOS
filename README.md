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

espOS dapat memeriksa dan memasang firmware baru melalui Wi-Fi menggunakan GitHub Releases.

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
OTA memerlukan konfigurasi partisi yang mendukung dua slot aplikasi, `otadata`, dan LittleFS.

1. Gunakan board ESP32-S3 dengan kapasitas flash yang sesuai.
2. Gunakan tabel partisi proyek (`partitions.csv`).
3. Pilih Flash Size **16 MB** untuk board N16R8.
4. Aktifkan PSRAM **OPI PSRAM** untuk ESP32-S3 N16R8.
5. Flash firmware melalui USB untuk pertama kali.

**Penting:** mengganti tabel partisi dapat menghapus data LittleFS yang sudah tersimpan. Backup data penting sebelum melakukan perubahan partisi atau erase flash.

### Membuat rilis OTA dari Arduino IDE
Folder `tools/` tidak diperlukan untuk proses compile dan upload biasa melalui Arduino IDE. Untuk menerbitkan firmware OTA:

1. Naikkan nilai `ESPOS_VERSION` di `ota_config.h`, misalnya dari `1.8.0` menjadi `1.9.0`.
2. Compile firmware menggunakan konfigurasi board dan tabel partisi yang sama dengan perangkat.
3. Pilih **Sketch → Export Compiled Binary**.
4. Ambil file aplikasi `espOS_S3.ino.bin` dari folder sketch/build. Jangan gunakan merged image, bootloader, atau filesystem image.
5. Buat file checksum SHA-256 dengan PowerShell Windows:

   ```powershell
   Get-FileHash .\espOS_S3.ino.bin -Algorithm SHA256 |
     ForEach-Object { $_.Hash.ToLower() + '  espOS_S3.ino.bin' } |
     Set-Content -Encoding ascii .\espOS_S3.ino.bin.sha256
   ```

6. Buat GitHub Release dengan tag yang sama dengan versi, misalnya `v1.9.0`.
7. Upload `espOS_S3.ino.bin` dan `espOS_S3.ino.bin.sha256` ke release tersebut.

Jangan mengganti isi rilis lama. Gunakan tag versi baru untuk setiap rilis.

### Verifikasi tanda tangan firmware
Verifikasi tanda tangan bersifat opsional dan bukan bagian dari proses flash USB biasa. Jika `OTA_REQUIRE_SIGNATURE` diaktifkan, setiap rilis harus menyediakan file tanda tangan `.sig` yang sesuai dengan public key di firmware. Pembuatan pasangan kunci dan tanda tangan harus dilakukan menggunakan perangkat lunak kriptografi eksternal yang tepercaya. **Jangan pernah menyimpan atau mengunggah private key ke repository maupun GitHub Releases.**

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

## Persiapan software

### 1. Install Arduino IDE
1. Unduh Arduino IDE dari [arduino.cc/en/software](https://www.arduino.cc/en/software).
2. Install dan jalankan Arduino IDE.

### 2. Install ESP32 Arduino Core
1. Buka **File → Preferences**.
2. Pada kolom **Additional boards manager URLs**, tambahkan URL berikut:

   ```text
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```

3. Klik **OK**.
4. Buka **Tools → Board → Boards Manager**.
5. Cari `esp32` dari **Espressif Systems**, lalu klik **Install**.
6. Tunggu sampai instalasi selesai.

Panduan resmi: [Installing Arduino ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).

### 3. Install library
Buka **Sketch → Include Library → Manage Libraries**, lalu cari dan install:
- `Adafruit GFX Library`
- `Adafruit ILI9341`

Library berikut tersedia dari ESP32 Arduino Core:
- `SPI`
- `Preferences`
- `LittleFS`
- `WiFi`
- API ESP-IDF untuk heap, OTA, GPIO, sleep, dan fungsi terkait.

Touchscreen XPT2046 dibaca langsung melalui SPI oleh firmware; library touchscreen terpisah tidak diperlukan.

---

## Struktur repository

Struktur file firmware yang diperlukan untuk Arduino IDE:

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
    └── README_OTA.md
```

Folder `tools/` dan skrip di dalamnya tidak diperlukan untuk compile maupun flash firmware melalui Arduino IDE. README ini juga tidak lagi menggunakan skrip dari folder tersebut.

### Fungsi file

| File | Fungsi |
|---|---|
| `espOS_S3.ino` | Firmware utama, UI, input touchscreen, Wi-Fi, penyimpanan, dan integrasi OTA |
| `espos_types.h` | Tipe data, struktur UI, dan enum layar |
| `games.h` | Implementasi game Dino, Ping Pong, dan Flappy |
| `psram_heap.h` | Helper dan manajer alokasi PSRAM |
| `ota_config.h` | Versi firmware, sumber GitHub, jadwal OTA, dan opsi keamanan |
| `ota_update.h` | Proses pemeriksaan, unduhan, verifikasi, pemasangan, dan rollback OTA |
| `ota_util.h` | Fungsi utilitas OTA |
| `partitions.csv` | Tabel partisi ESP32-S3 untuk dua slot OTA dan LittleFS |
| `README_OTA.md` | Dokumentasi khusus alur OTA dan penerbitan rilis |

**Catatan Arduino IDE:** nama folder sketch harus sama dengan nama file utama, yaitu `espOS_S3`. Simpan seluruh file firmware di dalam folder tersebut.

---

## Instalasi dan upload awal melalui Arduino IDE

### 1. Siapkan source code
1. Unduh repository sebagai ZIP dari GitHub, lalu ekstrak.
2. Buka folder `espOS_S3`.
3. Pastikan file `espOS_S3.ino`, seluruh file `.h`, dan `partitions.csv` berada di folder sketch yang sama.
4. Nama folder harus `espOS_S3`, sama dengan nama file utama `espOS_S3.ino`.

### 2. Buka project
1. Jalankan Arduino IDE.
2. Pilih **File → Open**.
3. Buka file `espOS_S3.ino` dari folder `espOS_S3`.

### 3. Pilih board dan konfigurasi
Hubungkan ESP32-S3 ke komputer menggunakan kabel USB yang mendukung transfer data. Kemudian atur:

| Pengaturan | Rekomendasi |
|---|---|
| Board | `ESP32S3 Dev Module` |
| Port | Port COM/serial ESP32-S3 yang terdeteksi |
| Flash Size | `16MB` untuk board N16R8 |
| PSRAM | `OPI PSRAM` untuk board N16R8 |
| CPU Frequency | `240MHz` |
| Upload Speed | `921600`; turunkan jika upload tidak stabil |
| Partition Scheme | Tabel partisi proyek yang mendukung dua slot OTA dan LittleFS |

Nama atau lokasi opsi dapat berbeda tergantung versi ESP32 Arduino Core. Pastikan konfigurasi partisi yang dipakai benar-benar menggunakan `partitions.csv` milik proyek. Jika versi Arduino-ESP32 yang digunakan tidak menyediakan pilihan untuk memuat tabel partisi kustom, ikuti metode pemasangan custom partition table yang sesuai dengan versi core tersebut; jangan memilih skema partisi secara asal.

### 4. Compile dan flash
1. Klik **Verify** (ikon centang) untuk memeriksa kompilasi.
2. Jika kompilasi berhasil, klik **Upload** (ikon panah).
3. Jika board tidak masuk mode upload otomatis, tahan tombol **BOOT**, tekan lalu lepaskan **RESET**, kemudian lepaskan **BOOT** saat proses upload dimulai.
4. Setelah upload selesai, buka **Tools → Serial Monitor**.
5. Atur baud rate ke **115200** untuk melihat log inisialisasi.

### 5. Setup pertama
Setelah flash kosong, perangkat menjalankan wizard setup awal. Ikuti kalibrasi touchscreen dan langkah setup yang ditampilkan. Konfigurasi kunci layar dan Wi-Fi dapat dilewati.

---

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
