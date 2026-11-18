// espos_types.h - tipe data espOS (dipisah agar prototipe otomatis Arduino IDE tidak error)
#pragma once
#include <Arduino.h>

typedef uint16_t u16;

struct Theme {
  const char* name;
  u16 bgTop, bgBot, glowA, glowB, arc, accent, discBase, discHi;
  uint8_t gaA, gaB, arcAlpha;
};

struct AGlyph { uint16_t off; uint8_t w, h; uint16_t adv; int8_t xo, yo; };   // adv dalam 1/16 px
struct AFont  { uint8_t first, last; const AGlyph* g; const uint8_t* bmp; };
struct AIcon  { uint8_t w, h; const uint8_t* d; };
struct App    { const char* name; u16 tile, edge, ink; const AIcon* ic; const AIcon* big; };

enum Screen : uint8_t {
  SC_HOME, SC_CALC, SC_NOTES, SC_NOTEEDIT, SC_FILES, SC_FILEVIEW, SC_TOOLS, SC_STOPWATCH,
  SC_FLASH, SC_SYSINFO, SC_SETTINGS, SC_BRIGHT, SC_SETTIME, SC_ABOUT, SC_RECENTS,
  SC_WIFI, SC_WIFIPASS, SC_LOCKSET, SC_PINPAD, SC_PATTERN, SC_DINO, SC_PONG, SC_FLAPPY, SC_OTA
};

// gaya tombol
enum BtnStyle : uint8_t { B_KEY, B_KEY2, B_OP, B_EQ, B_ROW, B_APP, B_HIDDEN, B_KB, B_KBSP };

struct Btn {
  int16_t x, y, w, h;
  int16_t code;
  uint8_t style;
  char label[26];
  char sub[30];
  const AIcon* icon;
  u16 ink;
};

// jaringan Wi-Fi hasil pindai
struct WfNet { char ssid[33]; int8_t rssi; uint8_t secure; };

// riak sentuhan (efek tekan pada tombol)
struct Ripple { bool on; int bi; float x, y; uint32_t t0; };
