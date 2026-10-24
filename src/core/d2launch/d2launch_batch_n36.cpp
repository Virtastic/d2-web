// D2Launch batch n36 (rva 0x141F0..0x176C0). Retail 32-bit addresses/ordinals are weak seams.
#include <cstdint>
#include <cstdlib>
#define W __attribute__((weak))
typedef uint32_t u32;

extern "C" {
W u32 q_d2launch_batch_n36_Ordinal_10098(u32 dat); W void Ordinal_10050(u32* slot); W void Ordinal_10196(u32 h, u32 v);
W void Ordinal_10001(u32 h, u32 v); W int32_t Ordinal_10003(void); W intptr_t Ordinal_10020(intptr_t);
W void* q_d2launch_batch_n36_Ordinal_10265(u32 id); W void Ordinal_10024(const char* s, void* str); W u32 Ordinal_10227(void);
W u32 Ordinal_10151(void); W u32 Ordinal_10134(u32 a); W void Ordinal_10019(void); W void Ordinal_10070(void);
W void Ordinal_10131(void); W void Ordinal_10053(u32 a); W void Ordinal_10097(void); W void Ordinal_10004(void);
W int Ordinal_268(u32 a); W void Ordinal_253(const char* s); W u32 Ordinal_10009(void);
W void n36_Ordinal_10014(void* cb); W u32 q_d2launch_batch_n36_Ordinal_10042(u32 line, u32 z); W void Ordinal_501(void* d, const char* s, u32 n);
W void Ordinal_10091(u32 h, u32 v); W void Ordinal_10147(void); W void Ordinal_10192(void* s, u32 v);
W void FUN_6fa521b0(u32 a, u32 b); W int FUN_6fa541b0(u32 a); W void FUN_6fa51aa0(char c); W void FUN_6fa52a70(u32 h, u32 a, u32 b);
W u32 FUN_6fa52ba0(void); W void FUN_6fa52bf0(const char* s); W u32 FUN_6fa52070(void); W void FUN_6fa52140(u32* e);
W int FUN_6fa52020(const void* a, const void* b); W void FUN_6fa55d30(void); W void FUN_6fa54c60(void);
W void n36_sprintf(char* d, const char* fmt, const char* a, const char* b);
W int32_t DAT_6fa65e34, DAT_6fa65e30, DAT_6fa64d94, DAT_6fa65ad4, DAT_6fa65ad8, DAT_6fa65eb0, DAT_6fa656c4;
W u32 DAT_6fa653c0[0x40], DAT_6fa652b8[0x40], DAT_6fa654dc[7], DAT_6fa653b8, DAT_6fa65abc, DAT_6fa65530;
W u32 DAT_6fa65e40, DAT_6fa65e60, DAT_6fa6552c, DAT_6fa65e78; W u32* DAT_6fa65e74;
W u32 DAT_6fa638c0, DAT_6fa638f0, DAT_6fa63920, DAT_6fa63d70, DAT_6fa63e00, DAT_6fa63dd0, DAT_6fa63e60, DAT_6fa63e30;
W u32 DAT_6fa63c50, DAT_6fa63c80, DAT_6fa63cb0, DAT_6fa63ce0, DAT_6fa63d10, DAT_6fa63d40;
W u32 DAT_6fa63b30, DAT_6fa63b60, DAT_6fa63b90, DAT_6fa63bc0, DAT_6fa63a70, DAT_6fa63aa0, DAT_6fa63ad0, DAT_6fa63b00;
W u32 DAT_6fa61bb0, DAT_6fa61be0, DAT_6fa61c10, DAT_6fa61c40, DAT_6fa61040, DAT_6fa61070, DAT_6fa610a0;
W u32 DAT_6fa5e0f8, DAT_6fa5e1b8;
W u32 n36_g2n_u32(u32 guest); // guest ptr -> value read helper unused
}
#define A(x) ((u32)(uintptr_t)&(x))

static void freeAll() { for (int i = 0; i < DAT_6fa65e34; i++) Ordinal_10050(&DAT_6fa653c0[i]); DAT_6fa65e34 = 0; }
[[noreturn]] static void fatal(u32 id) { Ordinal_10024("", q_d2launch_batch_n36_Ordinal_10265(id)); _Exit(-1); }
// push a control from a template; fatal on a full 0x40 table (retail checks before each push after the first)
static u32 pushC(u32 dat, u32 id = 0x60d) {
  if (DAT_6fa65e34 >= 0x40) fatal(id);
  u32 h = q_d2launch_batch_n36_Ordinal_10098(dat); DAT_6fa653c0[DAT_6fa65e34++] = h; return h;
}
static u32 pushS(u32 dat) {
  if (DAT_6fa65e30 >= 0x40) fatal(0x624);
  u32 h = q_d2launch_batch_n36_Ordinal_10098(dat); DAT_6fa652b8[DAT_6fa65e30++] = h; return h;
}

extern "C" {
// 1.13c D2Launch+0x141F0
u32 FUN_6fa541f0(u32* p) { if (!Ordinal_10227()) return 0; FUN_6fa521b0(p[3], 0); return 1; }

// 1.13c D2Launch+0x14290
u32 FUN_6fa54290(u32* p) {
  int32_t mode = (int32_t)Ordinal_10151();
  int n = Ordinal_10227() ? 7 : 5; // slots 0x6fa654dc..<0x6fa654f8 / <0x6fa654f0
  for (int i = 0; i < n; i++) if (!Ordinal_10134(0)) return 0;
  int32_t prev = DAT_6fa64d94; DAT_6fa64d94 = -1;
  if (prev != -1) { Ordinal_10019(); if (prev >= 0 && prev <= 6) Ordinal_10070(); }
  int32_t cur = -1; u32 id = p[0];
  static const int8_t map[7] = {4, 2, 0, 1, 3, 5, 6};
  for (int i = 0; i < 7; i++) if (id == DAT_6fa654dc[i]) {
    cur = map[i];
    if (i < 5) FUN_6fa521b0(p[3], 0); else if (i == 5) FUN_6fa541b0((u32)(uintptr_t)p); else FUN_6fa541f0(p);
    break;
  }
  if (cur == prev || mode != 1) { if (mode == 2 || mode == 3 || mode == 4) goto set; }
  else { if (cur >= 0 && cur <= 6) Ordinal_10070(); set: DAT_6fa64d94 = cur; }
  FUN_6fa51aa0((char)DAT_6fa64d94);
  if (DAT_6fa64d94 == -1 && mode != 1) {
    for (int i = 0; i < 6; i++) Ordinal_10131();
    if (DAT_6fa65ad4 == 1) { Ordinal_10131(); Ordinal_10131(); }
    if (Ordinal_10227()) { Ordinal_10131(); Ordinal_10131(); Ordinal_10131(); }
    Ordinal_10053(0);
  } else {
    Ordinal_10131(); Ordinal_10131();
    if (DAT_6fa65eb0) { Ordinal_10131(); Ordinal_10131(); }
    if (DAT_6fa65ad4 == 1) { Ordinal_10131(); Ordinal_10131(); }
    Ordinal_10131(); Ordinal_10053(DAT_6fa653b8); Ordinal_10131();
  }
  if (DAT_6fa65ad4 == 1) Ordinal_10131();
  if (Ordinal_10227()) {
    uint8_t* fl = (uint8_t*)(uintptr_t)(DAT_6fa65abc + 0x1eb);
    if (DAT_6fa64d94 == 6 || DAT_6fa64d94 == 5) { Ordinal_10131(); Ordinal_10131(); Ordinal_10131(); *fl |= 0x20; }
    else if (DAT_6fa64d94 != -1) { Ordinal_10131(); Ordinal_10131(); Ordinal_10131(); Ordinal_10097(); *fl |= 0x20; }
  }
  if (DAT_6fa65ad4 == 1) { Ordinal_10097(); *(uint8_t*)(uintptr_t)(DAT_6fa65abc + 0x1eb) |= 0x40; }
  return 1;
}

// 1.13c D2Launch+0x14920
void FUN_6fa54920(char* buf, const char* fmt) {
  Ordinal_10004();
  n36_sprintf(buf, fmt, 0, 0);
  if (Ordinal_268(0)) { Ordinal_253(buf); return; }
  if (!Ordinal_10009()) { n36_sprintf(buf, fmt, "DATA\\LOCAL", (const char*)A(DAT_6fa5e0f8)); if (Ordinal_268(0)) Ordinal_253(buf); }
  else { n36_sprintf(buf, fmt, "DATA\\LOCAL", (const char*)A(DAT_6fa5e1b8)); if (Ordinal_268(0)) Ordinal_253(buf); }
}

// 1.13c D2Launch+0x149E0
void FUN_6fa549e0(void) { freeAll(); }

// 1.13c D2Launch+0x14A20
void FUN_6fa54a20(void) {
  freeAll(); DAT_6fa65e40 = 1; DAT_6fa65e60 = 0;
  u32 h = pushC(A(DAT_6fa638c0)); Ordinal_10196(h, 0); Ordinal_10001(h, 1);
  Ordinal_10001(pushC(A(DAT_6fa63e60)), 1);
  Ordinal_10001(pushC(A(DAT_6fa63920)), 1);
  pushC(A(DAT_6fa63e30)); Ordinal_10003(); Ordinal_10020(0);
}

// 1.13c D2Launch+0x14C60
void FUN_6fa54c60(void) {
  freeAll(); DAT_6fa65e40 = 1; DAT_6fa65e60 = 0;
  bool alt = DAT_6fa65ad8 != 0;
  u32 a = pushC(A(alt ? DAT_6fa63d70 : DAT_6fa638c0)); Ordinal_10196(a, 0);
  u32 b = pushC(A(alt ? DAT_6fa63e00 : DAT_6fa638f0));
  u32 c = pushC(A(alt ? DAT_6fa63dd0 : DAT_6fa63920));
  Ordinal_10001(a, 1); Ordinal_10001(b, 1); Ordinal_10001(c, 1);
  Ordinal_10003(); Ordinal_10020(0); DAT_6fa6552c = 0x6fa52a60;
}

// 1.13c D2Launch+0x15230
void FUN_6fa55230(void) {
  freeAll();
  u32 a = pushC(A(DAT_6fa63c50)); Ordinal_10196(a, 1); Ordinal_10001(a, 1);
  Ordinal_10001(pushC(A(DAT_6fa63c80)), 1);
  u32 h = pushC(A(DAT_6fa63cb0)); Ordinal_10001(h, 1); FUN_6fa52a70(h, a, 0);
  h = pushC(A(DAT_6fa63ce0)); Ordinal_10001(h, 1); FUN_6fa52a70(h, 1, 0);
  h = pushC(A(DAT_6fa63d10)); Ordinal_10001(h, 1); FUN_6fa52a70(h, a, 0);
  h = pushC(A(DAT_6fa63d40)); Ordinal_10001(h, 1); FUN_6fa52a70(h, a, 4);
  DAT_6fa6552c = 1;
}

// 1.13c D2Launch+0x15400
void FUN_6fa55400(u32 p1, u32 p2, const int16_t* text) {
  freeAll();
  u32 a = pushC(A(DAT_6fa63b30)); Ordinal_10196(a, 1); Ordinal_10001(a, 1);
  Ordinal_10001(pushC(A(DAT_6fa63b60)), 1);
  u32 h = pushC(A(DAT_6fa63b90)); Ordinal_10001(h, 1); FUN_6fa52a70(h, 1, 0);
  u32 arg, mode;
  if (!text || !*text) {
    h = pushC(A(DAT_6fa63bc0)); Ordinal_10001(h, 1); arg = A(DAT_6fa63b90); mode = 0;
  } else {
    h = FUN_6fa52ba0(); Ordinal_10001(h, 1); FUN_6fa52a70(h, a, 0);
    h = FUN_6fa52ba0(); Ordinal_10001(h, 1); arg = (u32)(uintptr_t)text; mode = 4;
  }
  FUN_6fa52a70(h, arg, mode);
  DAT_6fa6552c = a;
}

// 1.13c D2Launch+0x155B0
void FUN_6fa555b0(u32 ebx) {
  freeAll();
  u32 a = pushC(A(DAT_6fa63a70)); Ordinal_10196(a, 1); Ordinal_10001(a, 1);
  Ordinal_10001(pushC(A(DAT_6fa63aa0)), 1);
  u32 h = pushC(A(DAT_6fa63ad0)); Ordinal_10001(h, 1); FUN_6fa52a70(h, a, 0);
  h = pushC(A(DAT_6fa63b00)); Ordinal_10001(h, 1); FUN_6fa52a70(h, A(DAT_6fa63ad0), 0);
  DAT_6fa6552c = ebx;
}

// 1.13c D2Launch+0x15B60
void FUN_6fa55b60(void) {
  FUN_6fa54c60(); n36_Ordinal_10014((void*)(uintptr_t)0x6fa51b70);
  if (DAT_6fa656c4) DAT_6fa65530 = ((u32(*)(void))(uintptr_t)*(u32*)(uintptr_t)(DAT_6fa656c4 + 0x40))();
  freeAll();
  if (!DAT_6fa65530) { DAT_6fa65e78 = 0xffffffff; return; }
  const char* p = (const char*)(uintptr_t)(DAT_6fa65530 + 8);
  DAT_6fa65e78 = *(u32*)(uintptr_t)(DAT_6fa65530 + 4);
  if ((int32_t)DAT_6fa65e78 >= 0x200) fatal(0x1168);
  FUN_6fa52070();
  u32 n = DAT_6fa65e78;
  if (!n) return;
  u32* tab = (u32*)(uintptr_t)q_d2launch_batch_n36_Ordinal_10042(0x1173, 0);
  for (u32 i = 0; i < n; i++) tab[i] = 0;
  DAT_6fa65e74 = tab;
  for (u32 k = n; k; k--) {
    u32* e = (u32*)(uintptr_t)q_d2launch_batch_n36_Ordinal_10042(0x1179, 0);
    const char* s = p + 4; u32 l = 0; while (s[l]) l++; l++;
    e[0] = q_d2launch_batch_n36_Ordinal_10042(0x1186, 0); Ordinal_501((void*)(uintptr_t)e[0], s, l);
    s += l; u32 m = 0; while (s[m]) m++; m++;
    e[1] = q_d2launch_batch_n36_Ordinal_10042(0x118b, 0); Ordinal_501((void*)(uintptr_t)e[1], s, m);
    p = s + m;
    FUN_6fa52140(e);
  }
  qsort(DAT_6fa65e74, DAT_6fa65e78, 4, FUN_6fa52020);
}

// 1.13c D2Launch+0x167B0
void FUN_6fa567b0(u32 mode) {
  FUN_6fa55d30(); freeAll();
  FUN_6fa52bf0("palette\\sky\\pal.pl2");
  u32 h = 0;
  for (int i = 0; i < 3; i++) h = pushS(0);
  Ordinal_10003(); Ordinal_10020(0);
  for (int i = 0; i < 4; i++) h = pushS(0);
  pushS(A(DAT_6fa61bb0)); pushS(A(DAT_6fa61be0)); pushS(A(DAT_6fa61c10)); pushS(A(DAT_6fa61c40));
  (void)h;
  // retail switch on mode falls through 2..6 (default included), each step: Ordinal_10147, Ordinal_10192(&text,0)
  int32_t start = (mode >= 2 && mode <= 6) ? (int32_t)mode : 1;
  uint8_t text[16] = {0};
  for (int32_t c = start; c <= 6; c++) { Ordinal_10147(); Ordinal_10192(text, 0); }
}

// 1.13c D2Launch+0x176C0
void FUN_6fa576c0(void) {
  FUN_6fa55d30();
  Ordinal_10091(pushS(A(DAT_6fa61040)), 0);
  Ordinal_10091(pushS(A(DAT_6fa61070)), 0);
  pushS(A(DAT_6fa610a0));
  FUN_6fa52bf0("palette\\sky\\pal.pl2");
}
}
