// D2Launch batch 11BE0. Host seams (tick, cursor, message queue) are arguments.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define W __attribute__((weak)) extern
W int32_t Ordinal_10003(void);
W void Ordinal_10000(void);
W intptr_t Ordinal_10020(intptr_t);
W void Ordinal_10022(int);
W void q_d2launch_batch_11be0_Ordinal_10024(const char*, void*);
W void Ordinal_10043(int, int);
W void* q_d2launch_batch_11be0_Ordinal_10013(void);
W void Ordinal_10106(void);
W int Ordinal_10112(uint32_t);
W void Ordinal_10147(void);
W void Ordinal_10161(uint32_t);
W void* Ordinal_10166(void);
W void Ordinal_10167(void);
W void* q_d2launch_batch_11be0_Ordinal_10265(int);
W int q_d2launch_batch_11be0_Ordinal_422(const char*, const char*, int, char*, int);
W void Ordinal_425(const char*, const char*, int, const char*);
W uint32_t DAT_6fa65e68, DAT_6fa65e64, DAT_6fa65504, DAT_6fa64d94, DAT_6fa64d90;
W int32_t DAT_6fa65e78;
W int32_t** DAT_6fa65e74;  // table of slot ptrs (host-width)
#undef W

static int32_t g_lastX, g_lastY;
static uint32_t g_lastTick;

// 1.13c D2Launch+0x11BE0  (msg in EAX)
bool FUN_6fa51be0(uint32_t msg, uint32_t tick, int32_t x, int32_t y) {
  int32_t dt = (int32_t)(tick - g_lastTick);
  int32_t dx = x - g_lastX, dy = y - g_lastY;
  g_lastX = x; g_lastY = y; g_lastTick = tick;
  return (std::abs(dx) + std::abs(dy)) < 5 && dt < 0x12d && msg == 0x201;
}

// 1.13c D2Launch+0x11CB0  peek/dispatch: mouse buttons 0x204/5/7/8 dropped, 0x12/0x102/0x201/0x202 not dispatched.
// next() pops a message (returns false when empty); dispatch() is TranslateMessage+DispatchMessage.
void FUN_6fa51cb0(bool (*next)(uint32_t* msg), void (*dispatch)()) {
  uint32_t m;
  while (next(&m)) {
    if (m > 0x202) {
      if (m == 0x204 || m == 0x205 || m == 0x207 || m == 0x208) continue;
      dispatch();
    } else if (m < 0x201 && m != 0x12 && m != 0x102) dispatch();
  }
}

// 1.13c D2LAUNCH+0x12020  qsort compare on pointer-to-pointer strings
int FUN_6fa52020(const uint8_t* const* const* a, const uint8_t* const* const* b) {
  const uint8_t* p = **a;
  const uint8_t* q = **b;
  for (;; p++, q++) {
    if (*p != *q) return *p < *q ? -1 : 1;
    if (*p == 0) return 0;
  }
}

// 1.13c D2Launch+0x12070
uint32_t FUN_6fa52070(void) {
  if (DAT_6fa65e74) {
    for (int i = 0; i < DAT_6fa65e78; i++) {
      int32_t* e = DAT_6fa65e74[i];
      if (!e) continue;
      if (e[1] == 0) { q_d2launch_batch_11be0_Ordinal_10024("", q_d2launch_batch_11be0_Ordinal_10265(0x1120)); exit(-1); }
      Ordinal_10043(0x1121, 0);
      if (*DAT_6fa65e74[i] == 0) { q_d2launch_batch_11be0_Ordinal_10024("", q_d2launch_batch_11be0_Ordinal_10265(0x1122)); exit(-1); }
      Ordinal_10043(0x1123, 0);
      Ordinal_10043(0x1125, 0);
    }
    Ordinal_10043(0x1128, 0);
    DAT_6fa65e74 = nullptr;
  }
  return 1;
}

// 1.13c D2LAUNCH+0x12140
uint32_t FUN_6fa52140(int32_t* p) {
  if (!DAT_6fa65e74) { q_d2launch_batch_11be0_Ordinal_10024("", q_d2launch_batch_11be0_Ordinal_10265(0x1106)); exit(-1); }
  for (int i = 0; i < DAT_6fa65e78; i++)
    if (!DAT_6fa65e74[i]) { DAT_6fa65e74[i] = p; return 1; }
  return 0;
}

// 1.13c D2Launch+0x121B0  (ESI = unaff)
uint32_t FUN_6fa521b0(int param_1, uint32_t esi) {
  if (DAT_6fa64d94 == (uint32_t)-1) {
    if (param_1) {
      Ordinal_10106(); Ordinal_10003(); Ordinal_10000();
      Ordinal_10106(); Ordinal_10003(); Ordinal_10000();
      DAT_6fa64d90 = esi;
      return 1;
    }
    if (DAT_6fa64d90 == esi) { Ordinal_10106(); Ordinal_10106(); }
  }
  return 1;
}

// 1.13c D2Launch+0x122C0
void FUN_6fa522c0(void) {
  // retail computes two Unicode::strlen results and discards them
  Ordinal_10166(); Ordinal_10166();
  if (DAT_6fa65504) Ordinal_10147();
}

// 1.13c D2Launch+0x12340  (ESI = buf, EDI = len): count '-' and '_'
int FUN_6fa52340(const char* buf, int len) {
  int n = 0;
  for (int i = 0; i < len; i++) if (buf[i] == '-' || buf[i] == '_') n++;
  return n;
}

// 1.13c D2Launch+0x12560
void FUN_6fa52560(void) {
  const char* s = (const char*)q_d2launch_batch_11be0_Ordinal_10013();
  Ordinal_10106();
  if (memcmp(s, "127.0.0.1", 10) == 0) {
    Ordinal_10003(); Ordinal_10020(0);
    Ordinal_10147(); Ordinal_10147(); Ordinal_10147();
    return;
  }
  Ordinal_10167();
  Ordinal_10147(); Ordinal_10147(); Ordinal_10147();
}

// 1.13c D2Launch+0x12770  intro movie: path from lang/locale; registry-ish "Aux Battle.net" ip tweak (|0x80 on 4th octet)
void FUN_6fa52770(bool (*next)(uint32_t*), void (*dispatch)(), const char* lang, const char* loc, int hasExpansion) {
  FUN_6fa51cb0(next, dispatch);
  char path[268];
  snprintf(path, sizeof path, "%s\\video\\%s\\D2x_Intro_%s.bik", "DATA\\LOCAL", lang, loc);
  (void)hasExpansion;
  char ip[16] = {0};
  int a = 0, b = 0, c = 0, d = 0;
  if (q_d2launch_batch_11be0_Ordinal_422("Diablo II", "Aux Battle.net", 0, ip, sizeof ip)) sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);
  d |= 0x80;
  snprintf(ip, sizeof ip, "%d.%d.%d.%d", 0xd8, 0x94, 0xf6, d);
  Ordinal_425("Diablo II", "Aux Battle.net", 0, ip);
}

// 1.13c D2Launch+0x128D0
uint8_t FUN_6fa528d0(void) {
  char ip[100] = "d";
  if (q_d2launch_batch_11be0_Ordinal_422("Diablo II", "Aux Battle.net", 0, ip, sizeof ip)) {
    int a, b, c, d = 0x22;
    sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);
    if (d & 0x10) return (uint8_t)(~(uint8_t)d) >> 7;
  }
  return 0;
}

// 1.13c D2Launch+0x129D0  progress bar; getProgress is [DAT_6fa656c4+0x4c]
void FUN_6fa529d0(int (*getProgress)(void)) {
  Ordinal_10106();
  int p = getProgress();
  if (p > 100) p = 100;
  int w = Ordinal_10112(DAT_6fa65e68);
  (void)w;  // retail passes (w*p)/100 on the stack to 10161
  Ordinal_10161(DAT_6fa65e68);
  char buf[16];
  snprintf(buf, sizeof buf, "%d%%", p);
  if (DAT_6fa65e64) Ordinal_10022(0);
}
