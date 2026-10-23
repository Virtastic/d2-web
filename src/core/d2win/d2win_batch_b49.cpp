// D2Win batch b49. Seams: fatal = Ordinal_10265(code)+Ordinal_10024+_exit(-1); alloc = Fog #10042; cmpFree = D2CMP #10065.
#include <cstdint>
#include <cstring>
struct D2WinB49Env {
  void (*fatal)(uint32_t code);                 // noreturn
  void* (*alloc)(uint32_t line, uint32_t);      // Fog #10042
  void (*fogFree)(uint32_t line, uint32_t);     // Fog #10043
  void (*cmpFree)(uint32_t cel);                // D2CMP #10065
  uint32_t (*lineCount)(void* ctl);             // sub_9200
  void (*a050)(void* ctl);                      // sub_A050
  void (*f7fc0)(void* ctl);                     // sub_17FC0
  uint8_t* (*glyphInfo)();                      // *DAT_6f901508 call
  uint32_t* g149c;                              // DAT_6f90149c
  void (*o10104)(uint32_t); void (*o10005)(uint32_t); void (*o10068)(uint32_t, uint32_t, uint32_t, uint32_t);
};

// 1.13c D2Win+0x09290
uint32_t d2win_sub_9290(D2WinB49Env& e, int32_t* c, const uint16_t* s, int32_t n, uint32_t p4, uint8_t p5) {
  if (!c) e.fatal(0x1a7);
  if (*c != 4) e.fatal(0x1a8);
  for (; n > 0; n--) {
    uint32_t* it = (uint32_t*)e.alloc(0x1af, 0);
    size_t len = 0; while (s[len]) len++;
    uint16_t* cp = (uint16_t*)e.alloc(0x1b0, 0);
    *(uintptr_t*)it = (uintptr_t)cp;
    memcpy(cp, s, (len + 1) * 2);
    it[1] = it[2] = it[3] = it[4] = 0; it[5] = p4; *(uint8_t*)&it[6] = p5; it[7] = 0;
    if (!c[0x12]) c[0x12] = (int32_t)(intptr_t)it; else ((uint32_t*)(intptr_t)c[0x13])[7] = (uint32_t)(uintptr_t)it;
    c[0x13] = (int32_t)(intptr_t)it; c[0x15]++;
  }
  if (c[0x24]) {
    int32_t k = (int32_t)e.lineCount(c);
    int32_t* o = (int32_t*)(intptr_t)c[0x24];
    if (*o != 5) e.fatal(0x56);
    int32_t d = c[0x15] - k;
    o[0x12] = d < 1 ? 0 : d;
  }
  return 1;
}

// 1.13c D2Win+0xA0A0
void d2win_sub_a0a0(D2WinB49Env& e, int32_t* c) {
  if (*c != 0xc) e.fatal(0x50);
  e.a050(c); e.f7fc0(c);
}

// 1.13c D2Win+0xA6F0
void d2win_sub_a6f0(D2WinB49Env& e, uint32_t cel) {
  e.cmpFree(cel);
  if (cel) e.fogFree(0x5c, 0);
}

// 1.13c D2Win+0xc720
void d2win_sub_c720(D2WinB49Env& e, int32_t* p, int32_t v) {
  if (!p) e.fatal(0xd6b);
  int32_t mx = p[3] * 0x100, x = v < 1 ? 0 : v;
  p[2] = x < mx ? x : mx;
}

// 1.13c D2Win+0x13970
void d2win_sub_13970(D2WinB49Env& e, int32_t* p, int32_t v) {
  if (!p) e.fatal(0x79);
  if (*p != 1) e.fatal(0x7b);
  p[0x9a] = v;
}

// 1.13c D2Win+0x17C00
void d2win_sub_17c00(D2WinB49Env& e, uint32_t m) {
  *e.g149c = m;
  if (m == 1) e.o10104(1);
  e.o10005(m);
  if (m == 1) e.o10068(0, 0x800000, 0x7d000, 0);
}

// 1.13c D2Win+0x120c0 (caller 0x12410): width of up to n chars of a UTF-16 string, skipping 0xff 'c' [0-6] colour codes.
int32_t d2win_sub_120c0(D2WinB49Env& e, const uint16_t* s, int32_t n) {
  int32_t i = 0, w = 0, lim = 3;
  while (*s) {
    if (n <= i) return w;
    uint16_t ch = *s;
    if (ch == 10) { s++; i++; lim++; continue; }
    if (!(n <= lim) && ch == 0xff && s[1] == 'c' && s[2] >= 0x30 && s[2] <= 0x36) { s += 3; i += 3; lim += 3; continue; }
    w += e.glyphInfo()[3]; s++; i++; lim++;
  }
  return w;
}
