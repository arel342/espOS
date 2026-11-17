**espOS** adalah proyek sistem operasi bergaya mini smartphone yang dibuat menggunakan **ESP32-S3** dan layar sentuh TFT. Antarmukanya dirancang menyerupai ponsel modern, dengan home screen, aplikasi bawaan, control panel, lock screen, konektivitas Wi-Fi, dan beberapa mini game.

> **Versi saat ini: v1.6**  
> Dibuat oleh **Farel**.

---

## Tampilan dan konsep

espOS menggunakan layar portrait 240 × 320 piksel. Interaksi utama dilakukan melalui touchscreen, termasuk swipe untuk berpindah halaman, membuka control panel, dan membuka lock screen.

## Fitur

### Antarmuka
- Boot animation dan home screen.
- Dua halaman aplikasi yang dapat digeser ke kiri dan kanan.
- Tiga tema: **Aurora**, **Senja**, dan **Mono**.
- Animasi transisi antarhalaman dan aplikasi.
- Tombol navigasi Back, Home, dan Recent Apps.
- Control panel dengan animasi swipe dan slider kecerahan.

### Aplikasi bawaan
- **Berkas** — melihat daftar file di LittleFS, membuka file teks, menghapus file, dan melihat informasi kapasitas penyimpanan.
- **Kalkulator** — operasi dasar, persentase, perubahan tanda, dan clear.
- **Catatan** — empat slot catatan dengan keyboard QWERTY di layar; data disimpan di flash.
- **Stopwatch** — stopwatch dengan kontrol mulai dan berhenti.
- **Setelan** — tema, waktu dan tanggal, kunci layar, kalibrasi touchscreen, informasi sistem, dan menu Tentang.

### Sistem
- Lock screen dengan gesture swipe ke atas.
- Pilihan kunci PIN 4–8 digit atau pola minimal 4 titik.
- Wi-Fi mode station, pemindaian jaringan, penyimpanan jaringan, dan auto-connect.
- Sinkronisasi waktu melalui NTP dengan zona waktu WIB (UTC+7).
- Senter menggunakan LED RGB bawaan board yang didukung.
- Light sleep dan deep sleep melalui control panel.
- Informasi CPU, RAM internal, PSRAM, flash, dan uptime.
- Kalibrasi touchscreen otomatis saat penggunaan pertama; hasilnya disimpan di Preferences.

### Mini game
- **Dino** — permainan berlari, melompat, dan menghindari rintangan.
- **Ping Pong** — permainan melawan komputer dengan kontrol sentuh.
- **Flappy** — permainan melewati pipa dan mencatat skor terbaik.

### Optimasi tampilan dan memori
- Framebuffer RGB565.
- Alokasi framebuffer mengutamakan PSRAM.
- Animasi transisi menggunakan buffer tambahan jika memori tersedia.
- Pengiriman frame ke TFT dapat berjalan pada task FreeRTOS terpisah agar proses render dan transfer layar dapat bertumpang tindih.
- Manajer alokasi PSRAM tersedia melalui `psram_heap.h`.

---

## Hardware

Konfigurasi utama yang dituju:

| Komponen | Keterangan |
|---|---|
| Mikrokontroler | ESP32-S3 |
| Board rekomendasi | ESP32-S3 N16R8 (16 MB Flash + 8 MB OPI PSRAM) |
| Display | TFT ILI9341, 2.8 inci, 240 × 320 |
| Touch controller | XPT2046 |
| LED | LED RGB onboard yang kompatibel dengan `neopixelWrite()` |

PSRAM sangat direkomendasikan, terutama untuk framebuffer dan animasi. Kode memiliki fallback untuk framebuffer utama ketika alokasi PSRAM gagal, tetapi fitur yang membutuhkan buffer tambahan dapat menjadi terbatas.

## Wiring GPIO

Pin berikut adalah konfigurasi default di `espOS_S3.ino`.

### TFT ILI9341

| TFT | ESP32-S3 |
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

| XPT2046 | ESP32-S3 |
|---|---:|
| T_CS | GPIO 14 |
| T_CLK | GPIO 12 |
| T_DIN | GPIO 11 |
| T_DO | GPIO 13 |
| T_IRQ | Tidak digunakan |

TFT dan XPT2046 berbagi jalur SPI. Pastikan setiap modul menggunakan pin CS masing-masing.

### Pin tambahan

| Fungsi | GPIO | Keterangan |
|---|---:|---|
| LED RGB onboard | GPIO 48 | Default pada konfigurasi ini |
| LED RGB alternatif | GPIO 38 | Beberapa varian DevKitC-1 v1.1 |
| Wake dari deep sleep | GPIO 0 | Tombol BOOT, aktif LOW |

**Penting:** pin LED RGB dapat berbeda antarvarian board. Periksa skematik board sebelum mengubah atau menyambungkan LED. Jangan menganggap seluruh board ESP32-S3 memiliki pemetaan LED yang sama.

---

## Software dan library

Gunakan:
- Arduino IDE.
- ESP32 Arduino Core dari Espressif.
- `Adafruit GFX Library`.
- `Adafruit ILI9341`.

Library berikut sudah disediakan oleh ESP32 Arduino Core dan digunakan oleh proyek:
- `SPI`
- `Preferences`
- `LittleFS`
- `WiFi`
- API ESP-IDF untuk heap, GPIO, dan sleep.

Touchscreen XPT2046 dibaca langsung melalui SPI oleh kode proyek; library touchscreen terpisah tidak diperlukan.

## Struktur proyek

Pastikan nama folder sketch sama dengan nama file `.ino`.

```text
espOS/
├── README.md
└── espOS_S3/
    ├── espOS_S3.ino
    ├── espos_types.h
    ├── games.h
    └── psram_heap.h
```

Kegunaan file:

| File | Fungsi |
|---|---|
| `espOS_S3.ino` | Program utama, UI, input touchscreen, Wi-Fi, penyimpanan, dan sistem |
| `espos_types.h` | Tipe data, struktur UI, dan enum layar |
| `games.h` | Implementasi game Dino, Ping Pong, dan Flappy |
| `psram_heap.h` | Helper dan manajer alokasi PSRAM |

Simpan semua file tersebut di dalam satu folder sketch `espOS_S3`.

## Konfigurasi Arduino IDE

Pilih board dan pengaturan yang sesuai dengan modul ESP32-S3 yang digunakan.

| Pengaturan | Nilai rekomendasi |
|---|---|
| Board | `ESP32S3 Dev Module` |
| Flash Size | `16MB` untuk board N16R8 |
| PSRAM | `OPI PSRAM` untuk board N16R8 |
| CPU Frequency | `240MHz` |
| Upload Speed | `921600` (turunkan jika upload tidak stabil) |
| Partition Scheme | Partisi dengan ruang aplikasi yang cukup dan partisi SPIFFS/LittleFS |

Nama opsi pada menu Arduino IDE dapat berbeda tergantung versi ESP32 Arduino Core. Pastikan skema partisi menyediakan ruang aplikasi yang cukup sekaligus partisi filesystem yang digunakan LittleFS.

## Instalasi dan upload

1. Install Arduino IDE.
2. Install ESP32 Arduino Core dari Espressif melalui Boards Manager.
3. Install `Adafruit GFX Library` dan `Adafruit ILI9341` melalui Library Manager.
4. Buat folder sketch bernama `espOS_S3`.
5. Masukkan empat file source ke folder tersebut.
6. Hubungkan ESP32-S3 ke komputer melalui USB.
7. Pilih board, port, PSRAM, Flash Size, dan Partition Scheme yang sesuai.
8. Compile menggunakan **Verify**.
9. Jika tidak ada error, pilih **Upload**.
10. Buka Serial Monitor pada baud rate **115200** untuk melihat log inisialisasi.

Pada penggunaan pertama, espOS akan meminta kalibrasi touchscreen. Sentuh empat titik kalibrasi yang muncul di layar dan tunggu sampai proses selesai.

## Penggunaan awal

- Geser layar ke kiri atau kanan untuk berpindah halaman aplikasi.
- Geser dari bagian atas layar ke bawah untuk membuka control panel.
- Geser lock screen ke atas untuk membuka perangkat.
- Buka **Setelan** untuk mengatur tema, waktu, dan kunci layar.
- Tahan tile Wi-Fi di control panel untuk membuka pengaturan jaringan.
- Pilih jaringan Wi-Fi dan masukkan password melalui keyboard di layar.
- Setelah berhasil terhubung, espOS dapat menyinkronkan waktu melalui NTP.

Zona waktu bawaan adalah **WIB (UTC+7)**. Konfigurasi NTP dan pin dapat diubah di bagian konfigurasi `espOS_S3.ino`.

---

## Penyimpanan data

espOS menggunakan dua mekanisme penyimpanan:
- **LittleFS** untuk file dan data catatan.
- **Preferences** untuk pengaturan seperti tema, kecerahan, kredensial kunci layar, kalibrasi touchscreen, skor game, dan konfigurasi Wi-Fi.

`LittleFS.begin(true)` digunakan saat inisialisasi filesystem. Parameter tersebut memungkinkan filesystem diformat otomatis jika proses mount gagal. Karena itu, kegagalan mount dapat menyebabkan data filesystem yang lama terhapus saat proses pemulihan.

**Saran:** backup data penting sebelum mengubah partisi, melakukan erase flash, atau melakukan eksperimen pada filesystem.

## Catatan dan batasan

- Proyek ini ditujukan untuk ESP32-S3 dengan TFT ILI9341 dan touchscreen XPT2046 sesuai konfigurasi pin di atas. Board atau modul lain mungkin memerlukan penyesuaian.
- PSRAM 8 MB direkomendasikan untuk pengalaman animasi yang lebih lengkap.
- LED RGB onboard harus sesuai dengan pin dan jenis LED pada board.
- Persentase baterai pada UI saat ini berasal dari konstanta `BATT_PERCENT`, bukan pembacaan sensor baterai secara langsung.
- Kunci PIN/pola merupakan fitur penguncian antarmuka lokal, bukan pengganti sistem keamanan perangkat dengan proteksi kriptografis yang kuat.
- Kredensial Wi-Fi disimpan melalui Preferences pada flash. Jangan memasukkan password pribadi ke source code yang akan dipublikasikan.
- Belum ada file lisensi di source yang disertakan. Tentukan dan tambahkan lisensi sebelum mendistribusikan proyek dengan ketentuan open-source tertentu.

## Kontribusi

Ide, laporan bug, dan pengembangan fitur dapat diajukan melalui GitHub Issues atau Pull Request. Sertakan informasi board, versi ESP32 Arduino Core, konfigurasi Arduino IDE, dan log Serial Monitor jika melaporkan masalah.

## Author

**Farel** — Creator of espOS.

---

*espOS is an experimental mini-smartphone interface project built with Arduino and ESP32-S3.*
'''
