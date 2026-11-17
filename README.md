espOS

espOS adalah proyek sistem operasi bergaya mini smartphone yang dibuat menggunakan ESP32-S3 dan layar sentuh TFT. Antarmukanya dirancang menyerupai ponsel modern, dengan home screen, aplikasi bawaan, control panel, lock screen, konektivitas Wi-Fi, dan beberapa mini game.

«Versi saat ini: v1.6
Dibuat oleh Farel.»

---

Tampilan dan konsep

espOS menggunakan layar portrait 240 × 320 piksel. Interaksi utama dilakukan melalui touchscreen, termasuk swipe untuk berpindah halaman, membuka control panel, dan membuka lock screen.

Fitur

Antarmuka

- Boot animation dan home screen.
- Dua halaman aplikasi yang dapat digeser.
- Tiga tema: Aurora, Senja, dan Mono.
- Animasi transisi antarhalaman dan aplikasi.
- Navigasi Back, Home, dan Recent Apps.
- Control panel dengan slider kecerahan.

Aplikasi bawaan

- Berkas — pengelolaan file LittleFS.
- Kalkulator — operasi matematika dasar.
- Catatan — empat slot catatan dengan keyboard QWERTY.
- Stopwatch — stopwatch dengan kontrol mulai dan berhenti.
- Setelan — tema, waktu, kunci layar, kalibrasi touchscreen, dan informasi sistem.

Sistem

- Lock screen dengan gesture swipe.
- PIN 4–8 digit atau pola minimal 4 titik.
- Wi-Fi, pemindaian jaringan, dan auto-connect.
- Sinkronisasi waktu melalui NTP (WIB).
- Senter LED RGB.
- Light sleep dan deep sleep.
- Informasi CPU, RAM, PSRAM, flash, dan uptime.
- Kalibrasi touchscreen otomatis.

Mini game

- Dino — permainan berlari dan menghindari rintangan.
- Ping Pong — permainan melawan komputer.
- Flappy — permainan melewati pipa.

Optimasi

- Framebuffer RGB565.
- Alokasi framebuffer menggunakan PSRAM.
- Animasi dengan buffer tambahan.
- Pengiriman frame melalui task FreeRTOS.
- Manajer alokasi PSRAM.

Hardware

Komponen| Keterangan
Mikrokontroler| ESP32-S3
Board rekomendasi| ESP32-S3 N16R8
Flash| 16 MB
PSRAM| 8 MB OPI
Display| ILI9341 2.8"
Resolusi| 240 × 320
Touch| XPT2046

Wiring GPIO

TFT ILI9341

TFT| ESP32-S3
CS| GPIO 10
RESET| GPIO 8
DC| GPIO 9
MOSI| GPIO 11
SCK| GPIO 12
MISO| GPIO 13
BL| GPIO 6

Touchscreen XPT2046

XPT2046| ESP32-S3
T_CS| GPIO 14
T_CLK| GPIO 12
T_DIN| GPIO 11
T_DO| GPIO 13
T_IRQ| Tidak digunakan

TFT dan touchscreen berbagi jalur SPI.

Pin tambahan

Fungsi| GPIO
LED RGB| GPIO 48
LED RGB alternatif| GPIO 38
Wake / BOOT| GPIO 0

Periksa pemetaan GPIO sesuai varian board.

Software dan library

- Arduino IDE
- ESP32 Arduino Core
- Adafruit GFX Library
- Adafruit ILI9341

Library bawaan ESP32 yang digunakan:

- SPI
- Preferences
- LittleFS
- WiFi
- ESP-IDF heap, GPIO, dan sleep API.

Struktur proyek

espOS/
├── README.md
└── espOS_S3/
    ├── espOS_S3.ino
    ├── espos_types.h
    ├── games.h
    └── psram_heap.h

File| Fungsi
espOS_S3.ino| Program utama dan sistem
espos_types.h| Struktur data dan tipe UI
games.h| Implementasi mini game
psram_heap.h| Manajemen PSRAM

Konfigurasi Arduino IDE

Pengaturan| Nilai
Board| ESP32S3 Dev Module
Flash Size| 16MB
PSRAM| OPI PSRAM
CPU Frequency| 240MHz
Upload Speed| 921600
Partition Scheme| Aplikasi dan partisi SPIFFS/LittleFS

Pastikan partisi filesystem tersedia dan ukuran aplikasi mencukupi.

Instalasi dan upload

1. Install Arduino IDE.
2. Install ESP32 Arduino Core dari Espressif.
3. Install Adafruit GFX Library.
4. Install Adafruit ILI9341.
5. Buat folder sketch "espOS_S3".
6. Masukkan empat file source ke folder tersebut.
7. Hubungkan ESP32-S3 melalui USB.
8. Pilih konfigurasi board yang sesuai.
9. Klik Verify untuk compile.
10. Klik Upload untuk flashing.

Serial Monitor menggunakan baud rate 115200.

Pada penggunaan pertama, espOS meminta kalibrasi touchscreen. Sentuh empat titik yang ditampilkan.

Penggunaan awal

- Swipe kiri/kanan untuk berpindah halaman.
- Swipe dari atas ke bawah untuk membuka control panel.
- Swipe ke atas untuk membuka lock screen.
- Buka Setelan untuk mengatur tema, waktu, dan keamanan.
- Tahan tile Wi-Fi untuk membuka pengaturan jaringan.
- Pilih jaringan dan masukkan password.

Zona waktu bawaan adalah WIB (UTC+7).

Penyimpanan data

espOS menggunakan:

- LittleFS: file dan catatan.
- Preferences: tema, kecerahan, kunci layar, kalibrasi, skor game, dan konfigurasi Wi-Fi.

"LittleFS.begin(true)" memungkinkan filesystem diformat otomatis jika mount gagal. Backup data sebelum eksperimen dengan partisi atau filesystem.

Catatan dan batasan

- PSRAM 8 MB direkomendasikan.
- Board lain mungkin membutuhkan penyesuaian GPIO.
- Persentase baterai masih menggunakan konstanta, bukan sensor baterai aktual.
- PIN/pola merupakan penguncian antarmuka lokal, bukan sistem keamanan kriptografis tingkat tinggi.
- Kredensial Wi-Fi disimpan melalui Preferences pada flash.
- Jangan memasukkan password pribadi ke source code publik.
- Lisensi proyek belum ditentukan.

Kontribusi

Bug report, ide, dan pengembangan fitur dapat diajukan melalui GitHub Issues atau Pull Request.

Sertakan informasi board, versi ESP32 Arduino Core, konfigurasi Arduino IDE, dan log Serial Monitor saat melaporkan masalah.

Author

Farel — Creator of espOS.

---

espOS is an experimental mini-smartphone interface project built with Arduino and ESP32-S3.
