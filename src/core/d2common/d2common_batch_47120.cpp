// D2Common 1.13c batch. Unwired retail callees are weak so the file links.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W intptr_t sub_37F40(int32_t);                                            // 0x37F40
W int32_t sub_38B00(intptr_t);                                            // 0x38B00
W void sub_3A1D0(intptr_t, int32_t, uint32_t, uint32_t, intptr_t);        // 0x3A1D0
W uint8_t* sgptDataTables;

static inline uint32_t rd(intptr_t p, int off) { return *(const uint32_t*)(p + off); }

// 1.13c D2Common+0x47120
EMSCRIPTEN_KEEPALIVE int32_t sub_47120(intptr_t p1, int32_t p2, uint32_t p3, uint32_t p4) {
 uint8_t* t = sgptDataTables;
 if (!p1 || rd(p1, 0) != 4) return 0;
 uint32_t shift = rd((intptr_t)t, 0xc6c), mask = rd((intptr_t)t, 0xc70);
 uint32_t a = rd(p1, 0x5c);
 if (a && (int32_t)rd((intptr_t)a, 0x10) < 0) {
  intptr_t n = sub_37F40(0x40);
  while (n) {
   t = sgptDataTables;
   if (0xcc < (int32_t)rd((intptr_t)t, 0xbd4) && rd((intptr_t)t, 0xbcc) + 0x10230 != 0) {
    int32_t v = sub_38B00((intptr_t)(rd((intptr_t)t, 0xbcc) + 0x10230));
    t = sgptDataTables;
    if (v) {
     int32_t hi = v >> 8;
     if (hi < 1 || hi >= 0x100) return 0;
     uint32_t lo = p4 & (((int32_t)p4 < 1) - 1u);
     if (hi <= (int32_t)lo) lo = hi;
     sub_3A1D0(n, 0xcc, hi * 0x100 + (lo & 0xff), (p2 << (shift & 0x1f)) + (mask & p3), p1);
     return 1;
    }
   }
   n = (intptr_t)rd(n, 0x2c);
   for (;;) {
    if (!n) goto next;
    if (*(const uint8_t*)(n + 0x10) & 0x40) break;
    n = (intptr_t)rd(n, 0x2c);
   }
  }
 }
next:
 intptr_t c = (intptr_t)rd(p1, 0x60);
 if (c && rd(c, 0) == 0x1020304) {
  intptr_t q = (intptr_t)rd(c, 0xc);
  while (q) {
   if (sub_47120(rd(q, 0) == 4 ? q : 0, p2, p3, p4)) return 1;
   if (rd(q, 0) != 4) return 0;
   int32_t x = (int32_t)rd(q, 0x14);
   if (x == 0 || x == -0x5c) return 0;
   q = (intptr_t)rd((intptr_t)x, 100);
  }
 }
 return 0;
}
}
