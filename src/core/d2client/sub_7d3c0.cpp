#include <cstdint>
#include <cstddef>
// Prior art: none (D2MOO has no D2Client). Recovered from the 1.13c decompile.
// Host-driven: every global the routine touches lives in S, every callee is a hook.
// The no-return paths (_exit after Ordinal_10265/10024) are reported via fatal().
namespace d2client_7d3c0 {
struct S {
  int32_t g_bc48, g_cec38; int16_t g_3af6; int32_t g_91384, g_3af0, g_46c8, g_46c4;
  int32_t g_c3dc, g_c3e0, g_c3f8, g_c3fc, g_c2f8, g_c2f4;
  int32_t g_3a84, g_3a60; int16_t g_3aca; int32_t g_3ac2; int16_t g_3ad2;
  void* g_3a68; int32_t g_3a7c, g_3a80; uint8_t g_3b0b; int32_t g_bf9c, g_bbfc, g_d3448, g_3aec;
  int32_t tbl_f0d8; const int32_t* tbl_f398;           // pairs {value, key}, zero-terminated
  int32_t* hash[128];                                   // DAT_6fbba808, bucket = id & 0x7f
  uint32_t tick;                                        // GetTickCount result
  void (*c9f0)(S&); void (*b3d0)(S&); void (*o10043)(S&, int, int);
  void (*c43e0)(S&, const uint8_t*); void (*f6a60)(S&, int32_t);
  void (*ae00)(S&); void (*o10184)(S&); void* (*o10003)(S&, int); void* (*c690)(S&, void*);
  void (*b386f0)(S&); void (*d2eb0)(S&); uint8_t (*a8a70)(S&, int32_t, int, int, int);
  int (*o10048)(S&); void (*eccc0)(S&, int, int); void (*fatal)(S&, uint32_t code);
};
static inline int32_t* nx(int32_t* n) { return reinterpret_cast<int32_t**>(n)[0x39] ? reinterpret_cast<int32_t**>(n)[0x39] : nullptr; }
// 1.13c D2Client+0x7D3C0
void sub_7D3C0(S& s, int32_t* p1, int16_t p2) {
  s.g_cec38 = (s.g_bc48 - 0x145) / 2; s.g_3af6 = 0; s.g_91384 = 0xc; s.g_3af0 = 0;
  s.g_46c8 = 0x10; s.g_46c4 = 0x10; s.g_c3dc = 0; s.g_c3e0 = 0; s.g_c3f8 = 0; s.g_c3fc = 0;
  s.g_c2f8 = 0; s.g_c2f4 = 0; s.c9f0(s);
  if (!p1) {
    if (s.g_3a84) { if (s.g_3a60) { s.b3d0(s); s.o10043(s, 0x68a, 0); s.g_3a60 = 0; } s.g_3a84 = 0; }
    s.g_3aca = 0; s.g_3ac2 = 0;
  } else {
    if (s.g_3aca) {
      for (int32_t* n = s.hash[s.g_3ac2 & 0x7f]; n; n = nx(n)) {
        if (n[3] == s.g_3ac2) {
          if (n[0] != 1) { s.fatal(s, 0x5a); return; }
          if (n != p1) { uint8_t b[5] = {'0', 1, 0, 0, 0}; s.c43e0(s, b); s.f6a60(s, s.g_3ac2); }
          break;
        }
      }
    }
    s.g_3aca = 1; s.g_3ac2 = p1[3];
  }
  s.ae00(s); s.g_3ad2 = p2;
  if (!s.g_3a68) {
    s.g_3a7c = 0; s.o10184(s);
    s.g_3a68 = s.c690(s, s.o10003(s, 2));
    if (!s.g_3a68) { s.fatal(s, 0x1012); return; }
    s.o10184(s); s.b386f0(s); s.g_3b0b = 0;
    if (s.tbl_f0d8) {
      for (const int32_t* e = s.tbl_f398; *e; e += 2) {
        if ((int16_t)e[1] == p2) {
          int32_t v = *e;
          s.d2eb0(s); s.g_3b0b = s.a8a70(s, s.g_bbfc, 5, 0, 0); s.g_bf9c = v;
          break;
        }
      }
    }
    *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(s.g_3a68) + 0x16) = 0;
    s.g_3a80 = 1;
    if (!s.g_d3448) { s.g_d3448 = 1; s.g_3aec = (int32_t)s.tick; s.eccc0(s, s.o10048(s), 0); }
  }
}
}
