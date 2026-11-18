// ============================================================================
//  games.h  -  Dino (mirip Chrome), Ping Pong, Flappy Bird untuk espOS
//  Di-include dari espOS_S3.ino (setelah fb, px, rrect, drawText, variabel sentuh).
//  Tiap game: reset  -> gameStart(),  update per frame -> gameStep(),  gambar -> gameDraw()
// ============================================================================
#pragma once

// ikon beranda (24x24, 4-bit alpha) - dibuat otomatis dari sprite
static const uint8_t IC_G_DINO_D[] = {0,0,0,0,0,0,0,15,255,255,240,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,255,15,255,255,240,0,0,0,0,0,0,0,255,255,255,255,240,0,0,0,0,0,0,0,255,255,255,255,240,0,0,0,0,0,0,0,255,255,255,255,240,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,255,255,240,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,15,255,255,255,0,0,15,0,0,0,0,0,15,255,255,255,0,0,15,0,0,0,0,0,255,255,255,255,0,0,15,240,0,0,0,0,255,255,255,255,15,0,15,255,0,0,0,15,255,255,255,255,15,240,15,255,240,0,0,255,255,255,255,255,15,0,15,255,255,0,15,255,255,255,255,255,0,0,15,255,255,255,255,255,255,255,255,240,0,0,15,255,255,255,255,255,255,255,255,240,0,0,0,255,255,255,255,255,255,255,255,0,0,0,0,15,255,255,255,255,255,255,255,0,0,0,0,0,255,255,255,255,255,255,255,0,0,0,0,0,15,255,255,255,255,255,0,0,0,0,0,0,0,255,255,0,255,255,0,0,0,0,0,0,0,255,0,0,255,240,0,0,0,0};
static const uint8_t IC_G_PONG_D[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,240,0,0,0,0,0,255,255,255,255,255,255,240,0,0,0,0,0,136,136,136,136,136,136,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,130,0,0,0,0,0,0,0,0,0,2,239,254,32,0,0,0,0,0,0,0,0,8,255,255,128,0,0,0,0,0,0,0,0,8,255,255,128,0,0,0,0,0,0,0,0,2,239,254,32,0,0,0,0,0,0,0,0,0,40,130,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,136,136,136,136,136,136,0,0,0,0,0,15,255,255,255,255,255,255,0,0,0,0,0,15,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const uint8_t IC_G_FLAP_D[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,120,187,135,32,0,0,0,0,0,0,4,191,255,255,255,251,64,0,0,0,0,0,143,255,255,255,255,201,232,0,0,0,0,9,255,255,255,255,248,0,63,144,0,0,0,111,255,255,255,255,244,0,11,246,0,0,0,223,255,255,255,255,247,0,30,253,0,0,2,255,253,133,71,175,255,136,223,255,80,0,4,255,114,138,184,67,239,255,255,255,249,0,2,252,14,255,255,245,143,255,255,255,254,32,0,222,20,173,235,129,159,255,255,255,195,0,0,111,196,0,0,24,255,255,255,246,0,0,0,9,255,219,188,255,255,255,255,144,0,0,0,0,143,255,255,255,255,255,248,0,0,0,0,0,4,191,255,255,255,251,64,0,0,0,0,0,0,2,120,187,135,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static AIcon IC_G_DINO = {24, 24, IC_G_DINO_D};
static AIcon IC_G_PONG = {24, 24, IC_G_PONG_D};
static AIcon IC_G_FLAP = {24, 24, IC_G_FLAP_D};

// jendela permainan: kartu di bawah status bar, di atas tombol navigasi
#define GV_X 6
#define GV_Y 40
#define GV_W 228
#define GV_H 244
#define GV_B (GV_Y + GV_H)

static inline bool isGameScreen(Screen s) { return s == SC_DINO || s == SC_PONG || s == SC_FLAPPY; }

// ----------------------------------------------------------------------------
//  Primitif gambar (semua di-clip ke jendela permainan)
// ----------------------------------------------------------------------------
static inline int gmin(int a, int b) { return a < b ? a : b; }
static inline int gmax(int a, int b) { return a > b ? a : b; }
static inline float gclampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

static inline void gPix(int x, int y, u16 c) {
  if (x < GV_X || x >= GV_X + GV_W || y < GV_Y || y >= GV_Y + GV_H) return;
  fb[y * SCR_W + x] = c;
}
static inline void gPixA(int x, int y, u16 c, uint8_t a) {
  if (x < GV_X || x >= GV_X + GV_W || y < GV_Y || y >= GV_Y + GV_H) return;
  u16* p = fb + y * SCR_W + x; *p = mix(c, *p, a);
}
static void gRect(int x, int y, int w, int h, u16 c) {
  int x0 = gmax(x, GV_X), y0 = gmax(y, GV_Y), x1 = gmin(x + w, GV_X + GV_W), y1 = gmin(y + h, GV_Y + GV_H);
  for (int j = y0; j < y1; j++) { u16* p = fb + j * SCR_W; for (int i = x0; i < x1; i++) p[i] = c; }
}
static void gRectA(int x, int y, int w, int h, u16 c, uint8_t a) {
  int x0 = gmax(x, GV_X), y0 = gmax(y, GV_Y), x1 = gmin(x + w, GV_X + GV_W), y1 = gmin(y + h, GV_Y + GV_H);
  for (int j = y0; j < y1; j++) { u16* p = fb + j * SCR_W; for (int i = x0; i < x1; i++) p[i] = mix(c, p[i], a); }
}
static void gDisc(float cx, float cy, float r, u16 c, uint8_t a = 255) {      // lingkaran halus
  int x0 = (int)floorf(cx - r - 1), x1 = (int)ceilf(cx + r + 1), y0 = (int)floorf(cy - r - 1), y1 = (int)ceilf(cy + r + 1);
  for (int j = y0; j <= y1; j++) for (int i = x0; i <= x1; i++) {
    float dx = i + 0.5f - cx, dy = j + 0.5f - cy;
    float cv = r - sqrtf(dx * dx + dy * dy) + 0.5f; if (cv <= 0) continue; if (cv > 1) cv = 1;
    gPixA(i, j, c, (uint8_t)(a * cv));
  }
}
static void gSpr(const char* const* rows, int h, int x, int y, u16 c) {         // sprite pixel-art: '#' = warna
  for (int j = 0; j < h; j++) { const char* r = rows[j]; for (int i = 0; r[i]; i++) if (r[i] == '#') gPix(x + i, y + j, c); }
}

// font piksel 3x5 (angka + huruf untuk "GAME OVER" / "HI")
static const uint8_t PXF[19][5] = {
  {7,5,5,5,7}, {2,6,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7}, {5,5,7,1,1}, {7,4,7,1,7}, {7,4,7,5,7}, {7,1,1,1,1}, {7,5,7,5,7}, {7,5,7,1,7},   // 0-9
  {7,4,5,5,7}, {2,5,7,5,5}, {5,7,7,5,5}, {7,4,6,4,7}, {7,5,5,5,7}, {5,5,5,5,2}, {6,5,6,5,5}, {5,5,7,5,5}, {7,2,2,2,7}                    // G A M E O V R H I
};
static int pxIdx(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  switch (c) { case 'G': return 10; case 'A': return 11; case 'M': return 12; case 'E': return 13; case 'O': return 14;
               case 'V': return 15; case 'R': return 16; case 'H': return 17; case 'I': return 18; }
  return -1;
}
static int pxTextW(const char* s, int sc) { int n = (int)strlen(s); return n ? n * 4 * sc - sc : 0; }
static void pxText(int x, int y, int sc, const char* s, u16 c) {
  for (; *s; s++, x += 4 * sc) {
    int g = pxIdx(*s); if (g < 0) continue;
    for (int r = 0; r < 5; r++) for (int k = 0; k < 3; k++) if (PXF[g][r] & (4 >> k)) gRect(x + k * sc, y + r * sc, sc, sc, c);
  }
}
static void pxTextOutline(int x, int y, int sc, const char* s, u16 c, u16 oc) {
  for (int dy = -1; dy <= 1; dy++) for (int dx = -1; dx <= 1; dx++) if (dx || dy) pxText(x + dx * (sc > 2 ? 2 : 1), y + dy * (sc > 2 ? 2 : 1), sc, s, oc);
  pxText(x, y, sc, s, c);
}

// ----------------------------------------------------------------------------
//  Masukan sentuh khusus game (membaca status sentuh yang sudah disaring pollTouch)
// ----------------------------------------------------------------------------
static uint32_t gLastMs = 0;
static float gDt = 0.016f, gF = 1.0f;                      // gF = jumlah "frame 60 fps" sejak frame lalu
static bool  gTouch = false, gWas = false, gEdge = false, gInDuck = false;
static int   gTx = 0, gTy = 0;
#define D_GY_PX (GV_Y + 150)                               // garis tanah Dino (piksel layar)

static void gInput(uint32_t now) {
  uint32_t d = now - gLastMs; gLastMs = now; if (d > 50) d = 50; if (d < 1) d = 1;
  gDt = d / 1000.0f; gF = gDt * 60.0f;
  bool t = tDown && !tBlocked && t0y >= GV_Y - 6 && t0y < NAV_Y && !cpDrag;
  gEdge = t && !gWas; gWas = t; gTouch = t;
  gTx = tx; gTy = ty;
  gInDuck = (t0x < GV_X + 104 && t0y > D_GY_PX + 14);      // tombol "Tunduk" (kiri bawah) hanya dipakai Dino
}

static uint32_t hiDino = 0, hiFlap = 0, pongWins = 0;
static void gamesLoad() { hiDino = prefs.getUInt("hiDino", 0); hiFlap = prefs.getUInt("hiFlap", 0); pongWins = prefs.getUInt("pongW", 0); }

// ============================================================================
//  1) DINO  -  mengikuti mesin game Chrome (satuan dunia = piksel Chrome, 1 satuan = 0,5 px layar)
// ============================================================================
static const char* const SP_DINO_RUN1[] = {
  "..............######..",
  ".............########.",
  ".............##.######",
  ".............#########",
  ".............#########",
  ".............#########",
  ".............######...",
  ".............#####....",
  ".............######...",
  "............#######...",
  "#...........#######...",
  "#..........########...",
  "##.........########.#.",
  "###.......#########.##",
  "####.....##########.#.",
  "#####...###########...",
  "##################....",
  "##################....",
  ".################.....",
  "..###############.....",
  "...##############.....",
  "....###########.......",
  ".....####..###........",
  ".....##..............."
};
static const char* const SP_DINO_RUN2[] = {
  "..............######..",
  ".............########.",
  ".............##.######",
  ".............#########",
  ".............#########",
  ".............#########",
  ".............######...",
  ".............#####....",
  ".............######...",
  "............#######...",
  "#...........#######...",
  "#..........########...",
  "##.........########.#.",
  "###.......#########.##",
  "####.....##########.#.",
  "#####...###########...",
  "##################....",
  "##################....",
  ".################.....",
  "..###############.....",
  "...##############.....",
  "....###########.......",
  ".....###..####........",
  "..........###........."
};
static const char* const SP_DINO_JUMP[] = {
  "..............######..",
  ".............########.",
  ".............##.######",
  ".............#########",
  ".............#########",
  ".............#########",
  ".............######...",
  ".............#####....",
  ".............######...",
  "............#######...",
  "#...........#######...",
  "#..........########...",
  "##.........########.#.",
  "###.......#########.##",
  "####.....##########.#.",
  "#####...###########...",
  "##################....",
  "##################....",
  ".################.....",
  "..###############.....",
  "...##############.....",
  "....###########.......",
  ".....####..####.......",
  ".....##....###........"
};
static const char* const SP_DINO_DEAD[] = {
  "..............######..",
  ".............########.",
  ".............#.#.####.",
  ".............#########",
  ".............#########",
  ".............#########",
  ".............######...",
  ".............#####....",
  ".............######...",
  "............#######...",
  "#...........#######...",
  "#..........########...",
  "##.........########.#.",
  "###.......#########.##",
  "####.....##########.#.",
  "#####...###########...",
  "##################....",
  "##################....",
  ".################.....",
  "..###############.....",
  "...##############.....",
  "....###########.......",
  ".....####..####.......",
  ".....##....###........"
};
static const char* const SP_DINO_BLINK[] = {
  "..............######..",
  ".............########.",
  ".............#########",
  ".............#########",
  ".............#########",
  ".............#########",
  ".............######...",
  ".............#####....",
  ".............######...",
  "............#######...",
  "#...........#######...",
  "#..........########...",
  "##.........########.#.",
  "###.......#########.##",
  "####.....##########.#.",
  "#####...###########...",
  "##################....",
  "##################....",
  ".################.....",
  "..###############.....",
  "...##############.....",
  "....###########.......",
  ".....####..####.......",
  ".....##....###........"
};
static const char* const SP_DUCK_A[] = {
  "..................#########...",
  ".................###.########.",
  ".................############.",
  "#................############.",
  "##...............######.......",
  "####............########......",
  "###################.####......",
  "#####################.........",
  ".###################..........",
  "..#################...#.......",
  "...###############............",
  "....####..#####...............",
  "....##....###................."
};
static const char* const SP_DUCK_B[] = {
  "..................#########...",
  ".................###.########.",
  ".................############.",
  "#................############.",
  "##...............######.......",
  "####............########......",
  "###################.####......",
  "#####################.........",
  ".###################..........",
  "..#################...#.......",
  "...###############............",
  "....#####..###................",
  "..........###................."
};
static const char* const SP_CACT_S[] = {
  "...###...",
  "...###...",
  "...###...",
  "#..###...",
  "#..###..#",
  "#..###..#",
  "#..###..#",
  "#########",
  "#########",
  "...######",
  "...###...",
  "...###...",
  "...###...",
  "...###...",
  "...###...",
  "...###...",
  "...###...",
  "...###..."
};
static const char* const SP_CACT_L[] = {
  "....#####....",
  "....#####....",
  "....#####....",
  "##..#####....",
  "##..#####..##",
  "##..#####..##",
  "##..#####..##",
  "##..#####..##",
  "##..#####..##",
  "#########..##",
  "#########..##",
  "....#########",
  "....#########",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####....",
  "....#####...."
};
static const char* const SP_PTERA_A[] = {
  "............####.......",
  "............####.......",
  "............####.......",
  "...........#####..###..",
  "...........#####..###..",
  "...........#####.####..",
  "..........###########..",
  "..........##########...",
  "..........##########...",
  "....################..#",
  ".######################",
  "..##################...",
  "......###########......",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  "......................."
};
static const char* const SP_PTERA_B[] = {
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  ".......................",
  "............####.......",
  "....################..#",
  ".######################",
  "..##################...",
  "......###########......",
  "..........##########...",
  "..........###########..",
  "...........#####.####..",
  "...........#####..###..",
  "............####.......",
  "............####.......",
  "............####......."
};


struct DObs { bool on, followed; uint8_t type, size, flap; float x, y, w, h, gap, spOff; };

enum { DS_IDLE = 0, DS_RUN = 1, DS_DEAD = 2 };
static uint8_t dState = DS_IDLE;
static float dSpeed = 6.0f, dDist = 0, dTimeMs = 0, dY = 93.0f, dVel = 0, dGroundX = 0, dRunMs = 0, dDeadT = 0;
static bool  dJump = false, dDuck = false, dReachMin = false, dDrop = false, dHeldPrev = false;
static DObs  dObs[4];
static uint8_t dLastType[2] = {9, 9};
static float dCloudX[5], dCloudY[5]; static int dCloudN = 0; static float dCloudGap = 0;
static float dNightK = 0, dNightMs = 0; static bool dNight = false;
static int   dScore = 0, dLastHund = 0; static float dFlashMs = 0, dBlinkMs = 0;
static float dMoonX = 300;

#define D_WU       (GV_W * 2)                             // lebar dunia (satuan)
#define D_GROUND_Y 93.0f
#define D_CANVAS_TOP (D_GY_PX - 70)
static inline int dsx(float xu) { return GV_X + (int)floorf(xu * 0.5f); }
static inline int dsy(float yu) { return D_CANVAS_TOP + (int)floorf(yu * 0.5f); }

static const int16_t DBX_RUN[6][4]  = {{22,0,17,16},{1,18,30,9},{10,35,14,8},{1,24,29,5},{5,30,21,4},{9,34,15,4}};
static const int16_t DBX_DUCK[1][4] = {{1,18,55,25}};
static const int16_t OBX_S[3][4] = {{0,7,5,27},{4,0,6,34},{10,4,7,14}};
static const int16_t OBX_L[3][4] = {{0,12,7,38},{8,0,7,49},{13,10,10,38}};
static const int16_t OBX_P[5][4] = {{15,15,16,5},{18,21,24,6},{2,14,4,3},{6,10,4,7},{10,8,6,9}};

static void dinoReset() {
  dState = DS_IDLE; dSpeed = 6.0f; dDist = 0; dTimeMs = 0; dY = D_GROUND_Y; dVel = 0; dGroundX = 0; dRunMs = 0; dDeadT = 0;
  dJump = dDuck = dReachMin = dDrop = false; dHeldPrev = false;
  for (int i = 0; i < 4; i++) dObs[i].on = false;
  dLastType[0] = dLastType[1] = 9;
  dCloudN = 2; dCloudX[0] = 120; dCloudY[0] = 38; dCloudX[1] = 330; dCloudY[1] = 62; dCloudGap = 150;
  dNightK = 0; dNightMs = 0; dNight = false; dScore = 0; dLastHund = 0; dFlashMs = 0; dBlinkMs = 1500; dMoonX = 340;
}

static void dSpawn() {
  int t, guard = 0;
  for (;;) {
    t = random(0, 3);
    if (t == 2 && dSpeed < 8.5f) { if (++guard < 8) continue; t = 0; }
    if (dLastType[0] == t && dLastType[1] == t && ++guard < 12) continue;
    break;
  }
  int slot = -1; for (int i = 0; i < 4; i++) if (!dObs[i].on) { slot = i; break; }
  if (slot < 0) return;
  DObs& o = dObs[slot];
  o.on = true; o.followed = false; o.type = t; o.flap = 0; o.spOff = 0;
  float uw, uh, minGap, multi;
  if (t == 0)      { uw = 17; uh = 35; o.y = 105; minGap = 120; multi = 4; }
  else if (t == 1) { uw = 25; uh = 50; o.y = 90;  minGap = 120; multi = 7; }
  else             { uw = 46; uh = 40; static const float PY[3] = {100, 75, 50}; o.y = PY[random(0, 3)]; minGap = 150; multi = 0; o.spOff = random(0, 2) ? 0.8f : -0.8f; }
  o.size = 1;
  if (t < 2) { o.size = (uint8_t)random(1, 4); if (o.size > 1 && multi > dSpeed) o.size = 1; }
  o.w = uw * o.size; o.h = uh; o.x = (float)D_WU;
  float mg = roundf(o.w * dSpeed + minGap * 0.6f), xg = roundf(mg * 1.5f);
  o.gap = (float)random((long)mg, (long)xg + 1);
  dLastType[1] = dLastType[0]; dLastType[0] = (uint8_t)t;
}

static inline bool dBoxHit(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh) {
  return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}
static bool dCollide(const DObs& o) {
  float tx0 = 50 + 1, ty0 = dY + 1, tw = 44 - 2, th = 47 - 2;
  float ox0 = o.x + 1, oy0 = o.y + 1, ow = o.w - 2, oh = o.h - 2;
  if (!dBoxHit(tx0, ty0, tw, th, ox0, oy0, ow, oh)) return false;
  const int16_t (*tb)[4] = dDuck ? DBX_DUCK : DBX_RUN; int tn = dDuck ? 1 : 6;
  int16_t ob[5][4]; int on = 3;
  if (o.type == 0 || o.type == 1) {
    const int16_t (*src)[4] = (o.type == 0) ? OBX_S : OBX_L;
    for (int i = 0; i < 3; i++) for (int k = 0; k < 4; k++) ob[i][k] = src[i][k];
    if (o.size > 1) { ob[1][2] = (int16_t)(o.w - ob[0][2] - ob[2][2]); ob[2][0] = (int16_t)(o.w - ob[2][2]); }
  } else { on = 5; for (int i = 0; i < 5; i++) for (int k = 0; k < 4; k++) ob[i][k] = OBX_P[i][k]; }
  for (int a = 0; a < tn; a++) for (int b = 0; b < on; b++)
    if (dBoxHit(tb[a][0] + tx0, tb[a][1] + ty0, tb[a][2], tb[a][3], ob[b][0] + ox0, ob[b][1] + oy0, ob[b][2], ob[b][3])) return true;
  return false;
}

static void dStartJump() { dJump = true; dVel = -10.0f - dSpeed / 10.0f; dReachMin = false; dDrop = false; }
static void dEndJump()   { if (dReachMin && dVel < -5.0f) dVel = -5.0f; }

static void dinoUpdate() {
  float f = gF;
  bool jumpHeld = gTouch && !gInDuck, duckHeld = gTouch && gInDuck;
  dBlinkMs -= gDt * 1000.0f; if (dBlinkMs < -120) dBlinkMs = 1500 + random(0, 2500);
  if (dState == DS_IDLE) {
    if (gEdge && !gInDuck) { dState = DS_RUN; dStartJump(); }
    return;
  }
  if (dState == DS_DEAD) {
    dDeadT += gDt;
    if (gEdge && dDeadT > 0.55f) { dinoReset(); dState = DS_RUN; dTimeMs = 0; }
    return;
  }
  // ---- RUN ----
  dTimeMs += gDt * 1000.0f; dRunMs += gDt * 1000.0f;
  if (dSpeed < 13.0f) { dSpeed += 0.001f * f; if (dSpeed > 13.0f) dSpeed = 13.0f; }
  dDist += dSpeed * f; dGroundX += dSpeed * f;
  dScore = (int)(dDist * 0.025f + 0.5f);
  if (dScore / 100 > dLastHund) { dLastHund = dScore / 100; dFlashMs = 900; }
  if (dFlashMs > 0) dFlashMs -= gDt * 1000.0f;
  // lompat / merunduk
  if (gEdge && !gInDuck && !dJump) dStartJump();
  if (dHeldPrev && !jumpHeld && dJump) dEndJump();
  dHeldPrev = jumpHeld;
  if (duckHeld) { if (dJump) dDrop = true; else dDuck = true; } else { dDuck = false; dDrop = false; }
  if (dJump) {
    dDuck = false;
    dY += dVel * (dDrop ? 3.0f : 1.0f) * f; dVel += 0.6f * f;
    if (dY < D_GROUND_Y - 30.0f || dDrop) dReachMin = true;
    if (dY < 30.0f || dDrop) dEndJump();
    if (dY >= D_GROUND_Y) { dY = D_GROUND_Y; dJump = false; dVel = 0; dDrop = false; }
  }
  // awan
  for (int i = 0; i < dCloudN; i++) dCloudX[i] -= 1.0f * f;
  while (dCloudN > 0 && dCloudX[0] < -50) { for (int i = 1; i < dCloudN; i++) { dCloudX[i - 1] = dCloudX[i]; dCloudY[i - 1] = dCloudY[i]; } dCloudN--; }
  if (dCloudN < 5 && (dCloudN == 0 || (D_WU - dCloudX[dCloudN - 1]) > dCloudGap)) {
    dCloudX[dCloudN] = (float)D_WU; dCloudY[dCloudN] = (float)random(30, 72); dCloudN++; dCloudGap = (float)random(100, 400);
  }
  // rintangan
  int n = 0;
  for (int i = 0; i < 4; i++) if (dObs[i].on) {
    DObs& o = dObs[i]; n++;
    o.x -= (dSpeed + o.spOff) * f;
    if (o.type == 2) { o.flap = (uint8_t)(((int)(dTimeMs / 166.0f)) & 1); }
    if (o.x + o.w < 0) { o.on = false; n--; }
  }
  if (dTimeMs > 3000.0f) {
    if (n == 0) dSpawn();
    else {
      DObs* last = nullptr; float bx = -1e9f;
      for (int i = 0; i < 4; i++) if (dObs[i].on && dObs[i].x > bx) { bx = dObs[i].x; last = &dObs[i]; }
      if (last && !last->followed && last->x + last->w + last->gap < (float)D_WU) { last->followed = true; dSpawn(); }
    }
  }
  // tabrakan: rintangan terdepan
  if (dTimeMs > 3000.0f) {
    DObs* first = nullptr; float bx = 1e9f;
    for (int i = 0; i < 4; i++) if (dObs[i].on && dObs[i].x < bx) { bx = dObs[i].x; first = &dObs[i]; }
    if (first && dCollide(*first)) {
      dState = DS_DEAD; dDeadT = 0;
      if ((uint32_t)dScore > hiDino) { hiDino = (uint32_t)dScore; prefs.putUInt("hiDino", hiDino); }
    }
  }
  // siang / malam (tiap 700 poin, 12 detik)
  if (dNight) { dNightMs += gDt * 1000.0f; if (dNightMs > 12000.0f) { dNight = false; dNightMs = 0; } }
  else if (dScore > 0 && dScore % 700 == 0 && dNightMs == 0) { dNight = true; dNightMs = 1; }
  if (!dNight && dScore % 700 != 0) dNightMs = 0;
  float tk = dNight ? 1.0f : 0.0f;
  if (dNightK < tk) { dNightK += gDt * 1.0f; if (dNightK > tk) dNightK = tk; }
  else if (dNightK > tk) { dNightK -= gDt * 1.0f; if (dNightK < tk) dNightK = tk; }
  dMoonX -= 0.25f * f; if (dMoonX < -20) dMoonX = (float)D_WU + 40;
}

// restart: panah melingkar (seperti Chrome)
static void dRestartIcon(int cx, int cy, u16 c) {
  for (int j = -12; j <= 12; j++) for (int i = -12; i <= 12; i++) {
    float d = sqrtf((float)(i * i + j * j));
    if (d < 6.2f || d > 9.2f) continue;
    float ang = atan2f((float)j, (float)i);
    if (ang > -1.35f && ang < -0.15f) continue;        // celah di kanan-atas untuk kepala panah
    gPix(cx + i, cy + j, c);
  }
  for (int r = 0; r < 6; r++) for (int k = -r; k <= r; k++) gPix(cx + 8 + k, cy - 8 + r - 3, c);   // kepala panah
}

static void dinoDraw() {
  uint8_t k8 = (uint8_t)(255.0f * (1.0f - dNightK));
  u16 bg = mix(H(0xF7F7F7), H(0x0A0A0A), k8), ink = mix(H(0x535353), H(0xACACAC), k8), lite = mix(H(0xDADADA), H(0x2A2A2A), k8);
  u16 mid = mix(H(0x757575), H(0x8A8A8A), k8);
  rrect(GV_X, GV_Y, GV_W, GV_H, 14, bg);
  // bulan & bintang (malam)
  if (dNightK > 0.02f) {
    uint8_t na = (uint8_t)(255.0f * dNightK);
    int mx = dsx(dMoonX), my = GV_Y + 36;
    for (int j = -8; j <= 8; j++) for (int i = -8; i <= 8; i++) {
      float d1 = sqrtf((float)(i * i + j * j)), d2 = sqrtf((float)((i - 4) * (i - 4) + (j + 1) * (j + 1)));
      if (d1 <= 6.5f && d2 > 6.0f) gPixA(mx + i, my + j, H(0xE6E6E6), na);
    }
    static const int16_t ST[6][2] = {{30, 30}, {88, 52}, {150, 26}, {190, 64}, {60, 78}, {120, 70}};
    for (int s = 0; s < 6; s++) {
      uint8_t ta = (uint8_t)(na * (((millis() / 400 + s) & 3) ? 1.0f : 0.35f));
      int sx2 = GV_X + ST[s][0], sy2 = GV_Y + ST[s][1];
      gPixA(sx2, sy2, H(0xDDDDDD), ta); gPixA(sx2 - 1, sy2, H(0xDDDDDD), ta / 2); gPixA(sx2 + 1, sy2, H(0xDDDDDD), ta / 2);
      gPixA(sx2, sy2 - 1, H(0xDDDDDD), ta / 2); gPixA(sx2, sy2 + 1, H(0xDDDDDD), ta / 2);
    }
  }
  // awan
  for (int i = 0; i < dCloudN; i++) {
    int cx = dsx(dCloudX[i]), cy = dsy(dCloudY[i]);
    gRect(cx + 2, cy + 3, 19, 4, lite); gRect(cx + 6, cy, 7, 4, lite); gRect(cx + 12, cy + 1, 6, 3, lite); gRect(cx, cy + 5, 23, 2, lite);
  }
  // tanah: garis + tonjolan
  gRect(GV_X, D_GY_PX, GV_W, 1, ink);
  {
    int sc = (int)(dGroundX * 0.5f);
    for (int seg = -1; seg < GV_W / 14 + 2; seg++) {
      int idx = (sc / 14) + seg; uint32_t h = (uint32_t)idx * 2654435761u; h ^= h >> 15;
      int bx = GV_X + seg * 14 - (sc % 14) + (int)((h >> 4) & 7), by = D_GY_PX + 2 + (int)((h >> 8) & 3);
      if (h & 1) gRect(bx, by, 2 + (int)((h >> 12) & 3), 1, ink);
      if ((h & 7) == 2) gRect(bx + 5, D_GY_PX + 5, 2, 1, lite);
      if ((h & 15) == 6) gRect(bx + 3, D_GY_PX - 1, 2, 1, ink);
    }
  }
  // rintangan
  for (int i = 0; i < 4; i++) if (dObs[i].on) {
    const DObs& o = dObs[i];
    if (o.type == 0) for (int s = 0; s < o.size; s++) gSpr(SP_CACT_S, 18, dsx(o.x + s * 17.0f), dsy(o.y), ink);
    else if (o.type == 1) for (int s = 0; s < o.size; s++) gSpr(SP_CACT_L, 25, dsx(o.x + s * 25.0f), dsy(o.y), ink);
    else gSpr(o.flap ? SP_PTERA_B : SP_PTERA_A, 20, dsx(o.x), dsy(o.y), ink);
  }
  // dino
  {
    int dx = dsx(50.0f);
    if (dState == DS_DEAD)       gSpr(SP_DINO_DEAD, 24, dx, dsy(dY), ink);
    else if (dState == DS_IDLE)  gSpr(dBlinkMs < 0 ? SP_DINO_BLINK : SP_DINO_JUMP, 24, dx, dsy(dY), ink);
    else if (dJump)              gSpr(SP_DINO_JUMP, 24, dx, dsy(dY), ink);
    else if (dDuck)              gSpr((((int)(dRunMs / 125.0f)) & 1) ? SP_DUCK_B : SP_DUCK_A, 13, dx, D_GY_PX - 13, ink);
    else                         gSpr((((int)(dRunMs / 83.0f)) & 1) ? SP_DINO_RUN2 : SP_DINO_RUN1, 24, dx, dsy(dY), ink);
  }
  // skor: HI 00000  00000
  {
    char b[12]; int xr = GV_X + GV_W - 10, y0 = GV_Y + 10;
    bool show = !(dFlashMs > 0 && (((int)(dFlashMs / 120.0f)) & 1));
    if (show) { snprintf(b, sizeof(b), "%05d", dScore); pxText(xr - pxTextW(b, 2), y0, 2, b, ink); }
    snprintf(b, sizeof(b), "%05u", (unsigned)hiDino);
    int hx = xr - 38 - 14 - pxTextW(b, 2); pxText(hx, y0, 2, b, mid); pxText(hx - pxTextW("HI ", 2) - 2, y0, 2, "HI", mid);
  }
  // petunjuk kontrol (kiri bawah = tunduk)
  {
    rrect(GV_X + 10, GV_B - 54, 94, 40, 12, ink, 22);
    rrect(GV_X + 10.5f, GV_B - 53.5f, 93, 39, 11.5f, ink, 70, 1.0f);
    drawText(F10, GV_X + 57, GV_B - 30, "Tunduk", ink, 255, 1);
    drawText(F9, GV_X + GV_W - 12, GV_B - 30, "Ketuk = lompat", mid, 255, 2);
  }
  if (dState == DS_IDLE) drawText(B13, GV_X + GV_W / 2, D_GY_PX - 62, "Ketuk untuk bermain", ink, 255, 1);
  if (dState == DS_DEAD) {
    int cx = GV_X + GV_W / 2;
    pxText(cx - pxTextW("GAME OVER", 2) / 2, D_GY_PX - 70, 2, "GAME OVER", ink);
    dRestartIcon(cx, D_GY_PX - 38, ink);
  }
}

// ============================================================================
//  2) PING PONG  -  pemain di bawah (geser jari), komputer di atas, 7 poin menang
// ============================================================================
#define P_PW 48
#define P_PH 7
#define P_BS 7
enum { PS_READY = 0, PS_PLAY = 1, PS_OVER = 2 };
static uint8_t pState = PS_READY;
static float pBx, pBy, pVx, pVy, pSpeed, pPad, pAi, pAiErr;
static int pScoreP = 0, pScoreA = 0, pServe = 1;          // pServe: +1 = bola menuju pemain
static float pTrailX[7], pTrailY[7]; static float pFlash = 0; static bool pWon = false;
#define P_PLAYER_Y (GV_B - 26)
#define P_AI_Y     (GV_Y + 19)

static void pongCenter() { pBx = GV_X + GV_W * 0.5f - P_BS * 0.5f; pBy = GV_Y + GV_H * 0.5f - P_BS * 0.5f; pVx = pVy = 0; pSpeed = 170.0f;
  for (int i = 0; i < 7; i++) { pTrailX[i] = pBx; pTrailY[i] = pBy; } }
static void pongReset() { pScoreP = pScoreA = 0; pServe = 1; pPad = pAi = GV_X + GV_W * 0.5f - P_PW * 0.5f; pAiErr = 0; pFlash = 0; pWon = false; pState = PS_READY; pongCenter(); }
static void pongLaunch() {
  float a = ((float)random(-35, 36)) * 0.0174533f;
  pVx = pSpeed * sinf(a); pVy = pServe * pSpeed * cosf(a); pState = PS_PLAY;
}
static void pongHit(bool player, float cx) {
  float pad = player ? pPad : pAi;
  float off = gclampf((cx - (pad + P_PW * 0.5f)) / (P_PW * 0.5f + 3), -1.0f, 1.0f);
  pSpeed = pSpeed * 1.055f; if (pSpeed > 400.0f) pSpeed = 400.0f;
  float a = off * 1.05f;
  pVx = pSpeed * sinf(a); pVy = (player ? -1.0f : 1.0f) * pSpeed * cosf(a);
  pAiErr = (float)random(-16, 17);
}
static void pongUpdate() {
  float dt = gDt;
  // paddle pemain mengikuti jari
  if (gTouch) { float tgt = gTx - P_PW * 0.5f; float dx = tgt - pPad; float mx = 900.0f * dt; pPad += gclampf(dx, -mx, mx); }
  pPad = gclampf(pPad, (float)GV_X, (float)(GV_X + GV_W - P_PW));
  if (pState == PS_READY) {
    float tgt = GV_X + GV_W * 0.5f - P_PW * 0.5f; pAi += gclampf(tgt - pAi, -120 * dt, 120 * dt);
    if (gEdge) pongLaunch();
    return;
  }
  if (pState == PS_OVER) { pFlash += dt; if (gEdge && pFlash > 0.6f) pongReset(); return; }
  // komputer
  float aiSp = 120.0f + 16.0f * pScoreP + (pSpeed - 170.0f) * 0.25f; if (aiSp > 260) aiSp = 260;
  float tgt = (pVy < 0) ? (pBx + P_BS * 0.5f + pAiErr - P_PW * 0.5f) : (GV_X + GV_W * 0.5f - P_PW * 0.5f);
  float dx = tgt - pAi; if (fabsf(dx) > 3) pAi += gclampf(dx, -aiSp * dt, aiSp * dt);
  pAi = gclampf(pAi, (float)GV_X, (float)(GV_X + GV_W - P_PW));
  // bola
  for (int i = 6; i > 0; i--) { pTrailX[i] = pTrailX[i - 1]; pTrailY[i] = pTrailY[i - 1]; }
  pTrailX[0] = pBx; pTrailY[0] = pBy;
  float px0 = pBx, py0 = pBy;
  pBx += pVx * dt; pBy += pVy * dt;
  if (pBx < GV_X) { pBx = (float)GV_X; pVx = fabsf(pVx); }
  if (pBx > GV_X + GV_W - P_BS) { pBx = (float)(GV_X + GV_W - P_BS); pVx = -fabsf(pVx); }
  // paddle pemain (tepi atas)
  {
    float yb0 = py0 + P_BS, yb1 = pBy + P_BS;
    if (pVy > 0 && yb0 <= P_PLAYER_Y + 1 && yb1 >= P_PLAYER_Y) {
      float t = (yb1 - yb0) != 0 ? (P_PLAYER_Y - yb0) / (yb1 - yb0) : 0; t = gclampf(t, 0, 1);
      float cx = px0 + (pBx - px0) * t + P_BS * 0.5f;
      if (cx >= pPad - 3 && cx <= pPad + P_PW + 3) { pBy = P_PLAYER_Y - P_BS; pongHit(true, cx); }
    }
  }
  // paddle komputer (tepi bawah)
  {
    float yt0 = py0, yt1 = pBy;
    if (pVy < 0 && yt0 >= P_AI_Y + P_PH - 1 && yt1 <= P_AI_Y + P_PH) {
      float t = (yt1 - yt0) != 0 ? ((P_AI_Y + P_PH) - yt0) / (yt1 - yt0) : 0; t = gclampf(t, 0, 1);
      float cx = px0 + (pBx - px0) * t + P_BS * 0.5f;
      if (cx >= pAi - 3 && cx <= pAi + P_PW + 3) { pBy = (float)(P_AI_Y + P_PH); pongHit(false, cx); }
    }
  }
  // poin
  if (pBy > GV_B) { pScoreA++; pServe = 1; pState = PS_READY; pongCenter(); }
  else if (pBy + P_BS < GV_Y) { pScoreP++; pServe = -1; pState = PS_READY; pongCenter(); }
  if (pScoreP >= 7 || pScoreA >= 7) {
    pState = PS_OVER; pFlash = 0; pWon = pScoreP > pScoreA;
    if (pWon) { pongWins++; prefs.putUInt("pongW", pongWins); }
  }
}
static void pongDraw() {
  u16 acc = H(0x4EE3B5), red = H(0xFF7A7A);
  rrect(GV_X, GV_Y, GV_W, GV_H, 14, H(0x0C1424));
  rrect(GV_X + 0.5f, GV_Y + 0.5f, GV_W - 1, GV_H - 1, 13.5f, H(0xFFFFFF), 26, 1.0f);
  int my = GV_Y + GV_H / 2;
  for (int x = GV_X + 8; x < GV_X + GV_W - 8; x += 14) gRectA(x, my - 1, 8, 2, H(0xFFFFFF), 70);       // garis tengah putus-putus
  char b[4];
  snprintf(b, sizeof(b), "%d", pScoreA); pxText(GV_X + GV_W / 2 - pxTextW(b, 6) / 2, my - 56, 6, b, H(0x44506A));
  snprintf(b, sizeof(b), "%d", pScoreP); pxText(GV_X + GV_W / 2 - pxTextW(b, 6) / 2, my + 26, 6, b, H(0x44506A));
  // jejak bola
  for (int i = 6; i >= 1; i--) gRectA((int)pTrailX[i], (int)pTrailY[i], P_BS, P_BS, H(0xFFFFFF), (uint8_t)(10 + (7 - i) * 6));
  // paddle & bola
  gRect((int)pAi, P_AI_Y, P_PW, P_PH, red);   gRectA((int)pAi, P_AI_Y + P_PH, P_PW, 2, red, 70);
  gRect((int)pPad, P_PLAYER_Y, P_PW, P_PH, acc); gRectA((int)pPad, P_PLAYER_Y - 2, P_PW, 2, acc, 70);
  gRect((int)pBx, (int)pBy, P_BS, P_BS, H(0xFFFFFF));
  drawText(F8, GV_X + 10, GV_Y + 12, "KOMPUTER", red, 255, 0);
  drawText(F8, GV_X + 10, GV_B - 8, "KAMU", acc, 255, 0);
  snprintf(b, sizeof(b), "%u", (unsigned)pongWins); char c[28]; snprintf(c, sizeof(c), "Menang: %s", b);
  drawText(F8, GV_X + GV_W - 10, GV_B - 8, c, COL_MUTED, 255, 2);
  int cx = GV_X + GV_W / 2;
  if (pState == PS_READY) {
    rrect(cx - 78, my - 14, 156, 28, 14, H(0x000000), 150);
    drawText(F10, cx, my + 4, pScoreP + pScoreA == 0 ? "Geser jari - ketuk mulai" : "Ketuk untuk lanjut", COL_WHITE, 255, 1);
  }
  if (pState == PS_OVER) {
    rrect(cx - 84, my - 34, 168, 68, 16, H(0x000000), 190);
    drawText(B13, cx, my - 6, pWon ? "Kamu menang!" : "Komputer menang", pWon ? acc : red, 255, 1);
    drawText(F9, cx, my + 16, "Ketuk untuk main lagi", COL_WHITE, 255, 1);
  }
}

// ============================================================================
//  3) FLAPPY BIRD
// ============================================================================
#define F_GROUND 30
#define F_GTOP   (GV_B - F_GROUND)
#define F_PW     34
#define F_GAP    68
#define F_SPACE  116.0f
#define F_BX     (GV_X + 66)
struct FPipe { bool on, passed; float x, gy; };
enum { FS_READY = 0, FS_PLAY = 1, FS_FALL = 2, FS_OVER = 3 };
static uint8_t fState = FS_READY;
static float fBy, fVy, fScroll, fSince, fT, fFlash, fDeadT;
static int fScore = 0; static bool fNew = false;
static FPipe fP[4];
static const u16 FC_OUT = H(0x543847);

static void flappyReset() {
  fState = FS_READY; fBy = GV_Y + 110.0f; fVy = 0; fScroll = 0; fSince = F_SPACE - 70; fT = 0; fFlash = 0; fDeadT = 0; fScore = 0; fNew = false;
  for (int i = 0; i < 4; i++) fP[i].on = false;
}
static void fSpawnPipe() {
  for (int i = 0; i < 4; i++) if (!fP[i].on) {
    fP[i].on = true; fP[i].passed = false; fP[i].x = (float)(GV_X + GV_W + 6);
    float lo = GV_Y + 28 + F_GAP * 0.5f, hi = F_GTOP - 28 - F_GAP * 0.5f;
    fP[i].gy = lo + (float)random(0, 1000) * (hi - lo) / 1000.0f; return;
  }
}
static bool fCircleRect(float cx, float cy, float r, float rx, float ry, float rw, float rh) {
  float nx = gclampf(cx, rx, rx + rw), ny = gclampf(cy, ry, ry + rh), dx = cx - nx, dy = cy - ny; return dx * dx + dy * dy < r * r;
}
static void fDie() {
  if (fState >= FS_FALL) return;
  fState = FS_FALL; fFlash = 0.22f; fDeadT = 0; if (fVy < 0) fVy = 0;
  fNew = (uint32_t)fScore > hiFlap;
  if (fNew) { hiFlap = (uint32_t)fScore; prefs.putUInt("hiFlap", hiFlap); }
}
static void flappyUpdate() {
  float dt = gDt; fT += dt;
  if (fFlash > 0) fFlash -= dt;
  if (fState == FS_READY) {
    fBy = GV_Y + 110.0f + sinf(fT * 5.0f) * 6.0f; fScroll += 85.0f * dt;
    if (gEdge) { fState = FS_PLAY; fVy = -285.0f; }
    return;
  }
  if (fState == FS_PLAY) {
    fScroll += 85.0f * dt;
    if (gEdge) fVy = -285.0f;
    fVy += 980.0f * dt; if (fVy > 430) fVy = 430;
    fBy += fVy * dt;
    if (fBy < GV_Y + 8) { fBy = GV_Y + 8.0f; if (fVy < 0) fVy = 0; }
    fSince += 85.0f * dt; if (fSince >= F_SPACE) { fSince -= F_SPACE; fSpawnPipe(); }
    for (int i = 0; i < 4; i++) if (fP[i].on) {
      FPipe& p = fP[i]; p.x -= 85.0f * dt;
      if (p.x + F_PW < GV_X - 4) { p.on = false; continue; }
      if (!p.passed && p.x + F_PW < F_BX) { p.passed = true; fScore++; }
      float top = p.gy - F_GAP * 0.5f, bot = p.gy + F_GAP * 0.5f;
      if (fCircleRect(F_BX, fBy, 7.5f, p.x, (float)GV_Y, (float)F_PW, top - GV_Y) || fCircleRect(F_BX, fBy, 7.5f, p.x, bot, (float)F_PW, (float)(F_GTOP - bot)) ||
          fCircleRect(F_BX, fBy, 7.5f, p.x - 2, top - 14, F_PW + 4.0f, 14.0f) || fCircleRect(F_BX, fBy, 7.5f, p.x - 2, bot, F_PW + 4.0f, 14.0f)) fDie();
    }
    if (fBy + 7 >= F_GTOP) { fBy = F_GTOP - 7.0f; fDie(); fState = FS_OVER; fDeadT = 0; }
    return;
  }
  if (fState == FS_FALL) {
    fVy += 1300.0f * dt; if (fVy > 600) fVy = 600; fBy += fVy * dt;
    if (fBy + 7 >= F_GTOP) { fBy = F_GTOP - 7.0f; fState = FS_OVER; fDeadT = 0; }
    return;
  }
  fDeadT += dt;
  if (gEdge && fDeadT > 0.7f) flappyReset();
}

static void fBird(float cx, float cy, float ang, int wing) {
  float ca = cosf(-ang), sa = sinf(-ang);
  int ix = (int)floorf(cx), iy = (int)floorf(cy);
  float wy = wing == 0 ? 0.0f : (wing == 1 ? 2.2f : -2.0f);
  for (int j = -16; j <= 16; j++) for (int i = -16; i <= 16; i++) {
    float rx = i + 0.5f, ry = j + 0.5f;
    float X = rx * ca - ry * sa, Y = rx * sa + ry * ca;
    float e = (X / 11.0f) * (X / 11.0f) + (Y / 8.0f) * (Y / 8.0f);
    u16 col = 0; bool on = false;
    float ed = (X - 5.6f) * (X - 5.6f) + (Y + 3.0f) * (Y + 3.0f);
    float pd = (X - 6.8f) * (X - 6.8f) + (Y + 3.0f) * (Y + 3.0f);
    float wd = ((X + 4.5f) / 5.8f) * ((X + 4.5f) / 5.8f) + ((Y - 2.0f + wy * 0.5f) / 3.6f) * ((Y - 2.0f + wy * 0.5f) / 3.6f);
    bool beak = X >= 8.0f && X <= 15.5f && fabsf(Y - 2.0f) <= 3.6f - (X - 8.0f) * 0.22f;
    bool beakIn = X >= 8.6f && X <= 14.4f && fabsf(Y - 2.0f) <= 2.6f - (X - 8.6f) * 0.2f;
    if (pd <= 2.0f) { col = H(0x222222); on = true; }
    else if (ed <= 14.0f) { col = H(0xFFFFFF); on = true; }
    else if (ed <= 19.0f && e <= 1.3f) { col = FC_OUT; on = true; }
    else if (wd <= 0.72f) { col = H(0xFFF3B0); on = true; }
    else if (wd <= 1.1f) { col = FC_OUT; on = true; }
    else if (beakIn) { col = Y > 2.0f ? H(0xE8501A) : H(0xFB7B2A); on = true; }
    else if (beak) { col = FC_OUT; on = true; }
    else if (e <= 1.0f) { col = (Y > 3.2f) ? H(0xF4E07A) : H(0xF7D23E); on = true; }
    else if (e <= 1.3f) { col = FC_OUT; on = true; }
    if (on) gPix(ix + i, iy + j, col);
  }
}

static void flappyDraw() {
  // langit bertingkat
  rrect(GV_X, GV_Y, GV_W, GV_H, 14, H(0x4EC0CA));
  gRect(GV_X, GV_Y + 120, GV_W, GV_H - 120 - F_GROUND, H(0x6ECDD4));
  gRect(GV_X, GV_Y + 160, GV_W, GV_H - 160 - F_GROUND, H(0x8EDAE0));
  // awan
  for (int c = 0; c < 3; c++) {
    float cx = GV_X + fmodf(c * 96.0f - fScroll * 0.18f + 600.0f * 10, 300.0f) - 30.0f, cy = GV_Y + 120.0f + c * 11.0f;
    gDisc(cx, cy, 9, H(0xFFFFFF), 200); gDisc(cx + 11, cy + 3, 11, H(0xFFFFFF), 200); gDisc(cx + 24, cy + 4, 8, H(0xFFFFFF), 200); gRectA((int)cx - 6, (int)cy + 5, 38, 7, H(0xFFFFFF), 200);
  }
  // bukit
  for (int x = 0; x < GV_W; x++) {
    float hh = 14.0f + 7.0f * sinf((x + fScroll * 0.35f) * 0.045f) + 4.0f * sinf((x + fScroll * 0.35f) * 0.11f);
    gRect(GV_X + x, F_GTOP - (int)hh, 1, (int)hh, H(0x9BE470));
  }
  // pipa
  for (int i = 0; i < 4; i++) if (fP[i].on) {
    const FPipe& p = fP[i]; int x = (int)p.x; int top = (int)(p.gy - F_GAP * 0.5f), bot = (int)(p.gy + F_GAP * 0.5f);
    for (int part = 0; part < 2; part++) {
      int y0 = part ? bot : GV_Y, y1 = part ? F_GTOP : top;
      gRect(x, y0, F_PW, y1 - y0, FC_OUT);
      gRect(x + 1, y0, F_PW - 2, y1 - y0, H(0x73BF2E));
      gRect(x + 3, y0, 5, y1 - y0, H(0x9CE659));
      gRect(x + F_PW - 8, y0, 5, y1 - y0, H(0x558022));
      int cy0 = part ? bot : top - 14;
      gRect(x - 2, cy0, F_PW + 4, 14, FC_OUT);
      gRect(x - 1, cy0 + 1, F_PW + 2, 12, H(0x73BF2E));
      gRect(x + 1, cy0 + 1, 5, 12, H(0x9CE659)); gRect(x + F_PW - 6, cy0 + 1, 5, 12, H(0x558022));
    }
  }
  // tanah
  gRect(GV_X, F_GTOP, GV_W, F_GROUND, H(0xDED895));
  gRect(GV_X, F_GTOP, GV_W, 2, FC_OUT);
  gRect(GV_X, F_GTOP + 2, GV_W, 6, H(0x73BF2E));
  { int off = (int)fScroll % 12; for (int x = -12; x < GV_W + 12; x += 12) for (int r = 0; r < 6; r++) gRect(GV_X + x - off + r, F_GTOP + 2 + r, 6, 1, H(0x9CE659)); }
  gRect(GV_X, F_GTOP + 8, GV_W, 1, H(0x558022));
  gRect(GV_X, F_GTOP + 9, GV_W, 3, H(0xD2B04C));
  // burung
  float ang = (fState == FS_READY) ? 0.0f : gclampf(fVy * 0.0030f, -0.45f, 1.25f);
  int wing = (fState == FS_FALL || fState == FS_OVER) ? 0 : (((int)(fT * 11.0f)) % 3);
  fBird(F_BX, fBy, ang, wing);
  // skor
  char b[8];
  if (fState == FS_PLAY || fState == FS_FALL) { snprintf(b, sizeof(b), "%d", fScore); pxTextOutline(GV_X + GV_W / 2 - pxTextW(b, 4) / 2, GV_Y + 22, 4, b, H(0xFFFFFF), FC_OUT); }
  int cx = GV_X + GV_W / 2;
  if (fState == FS_READY) {
    pxTextOutline(cx - pxTextW("0", 4) / 2, GV_Y + 22, 4, "0", H(0xFFFFFF), FC_OUT);
    rrect(cx - 70, GV_Y + 176, 140, 26, 13, H(0x000000), 120);
    drawText(F10, cx, GV_Y + 193, "Ketuk untuk terbang", COL_WHITE, 255, 1);
  }
  if (fState == FS_OVER) {
    pxTextOutline(cx - pxTextW("GAME OVER", 3) / 2, GV_Y + 30, 3, "GAME OVER", H(0xFC7B2A), FC_OUT);
    rrect(cx - 72, GV_Y + 62, 144, 76, 10, FC_OUT);
    rrect(cx - 70, GV_Y + 64, 140, 72, 8, H(0xDED895));
    drawText(F8, cx - 60, GV_Y + 82, "SKOR", H(0xE8601C), 255, 0);
    drawText(F8, cx - 60, GV_Y + 112, "TERBAIK", H(0xE8601C), 255, 0);
    snprintf(b, sizeof(b), "%d", fScore); pxTextOutline(cx + 60 - pxTextW(b, 2), GV_Y + 86, 2, b, H(0xFFFFFF), FC_OUT);
    snprintf(b, sizeof(b), "%u", (unsigned)hiFlap); pxTextOutline(cx + 60 - pxTextW(b, 2), GV_Y + 116, 2, b, H(0xFFFFFF), FC_OUT);
    if (fNew) drawText(F8, cx + 60, GV_Y + 100, "BARU!", H(0xE8601C), 255, 2);
    u16 med = fScore >= 30 ? H(0xF3D13B) : (fScore >= 20 ? H(0xC8CED6) : (fScore >= 10 ? H(0xC77B3E) : 0));
    if (med) { gDisc(cx - 42, GV_Y + 100, 12, FC_OUT); gDisc(cx - 42, GV_Y + 100, 10, med); }
    if (fDeadT > 0.7f) drawText(F9, cx, GV_Y + 158, "Ketuk untuk main lagi", COL_WHITE, 255, 1);
  }
  if (fFlash > 0) gRectA(GV_X, GV_Y, GV_W, GV_H, H(0xFFFFFF), (uint8_t)(fFlash * 1100.0f > 220 ? 220 : fFlash * 1100.0f));
}

// ============================================================================
//  API untuk espOS_S3.ino
// ============================================================================
static void gameStart(Screen s) {
  gWas = false; gEdge = false; gLastMs = millis();
  if (s == SC_DINO) dinoReset(); else if (s == SC_PONG) pongReset(); else if (s == SC_FLAPPY) flappyReset();
}
static void gameStep(uint32_t now) {
  gInput(now);
  switch (screen) {
    case SC_DINO: dinoUpdate(); break;
    case SC_PONG: pongUpdate(); break;
    case SC_FLAPPY: flappyUpdate(); break;
    default: break;
  }
}
static void gameDraw() {
  switch (screen) {
    case SC_DINO: dinoDraw(); break;
    case SC_PONG: pongDraw(); break;
    case SC_FLAPPY: flappyDraw(); break;
    default: break;
  }
}
