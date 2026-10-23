// 1.13c D2Game+0x86900: monster AI melee/ranged attack-choice step (ecx = game, edx = unit-ish param_2, stack = AI arg block).
// Plain transcription of the cached decompile; callees by address.
#include <cstdint>
#include <cstdlib>
typedef int32_t i32;
typedef uint32_t u32;
#define W(a, o) (*(i32*)((intptr_t)(a) + (o)))
#define S(a, o) (*(int16_t*)((intptr_t)(a) + (o)))
#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
extern "C" __attribute__((weak)) void Ordinal_10024(const void*, u32);
EXT(Ordinal_11103); EXT(Ordinal_11027); EXT(FUN_6fcd1bc0); EXT(FUN_6fc21180); EXT(FUN_6fc21320); EXT(FUN_6fc95e10);
EXT(FUN_6fd12380); EXT(FUN_6fd126b0); EXT(FUN_6fd12d80); EXT(FUN_6fd12220); EXT(FUN_6fd122c0); EXT(FUN_6fd12540);
extern u32* gdwBitMasks_exref;

// 1.13c D2Game+0x86900
extern "C" void b86900_sub_86900(i32 p1, i32* p2, i32* p3) {
  i32 st = p3[7], dist = p3[5], flag = p3[6], target = p3[2], ai = p3[0];
  i32 m = S(st, 0x6e + *(uint8_t*)((intptr_t)p1 + 0x6d) * 2);
  u32 b1 = m & 1, b4 = m & 4, b2 = m & 2;
  i32 lvl = *(uint8_t*)((intptr_t)p1 + 0x6d) * 2, k;
  if (W(ai, 0x1c) > 0) W(ai, 0x1c)--;
  i32 d38; i32 has = (i32)FUN_6fcd1bc0(p1, &d38);
  u32 *seed = (u32*)(p2 + 8);
#define ROLL() do { uint64_t v = (uint64_t)(u32)p2[8] * 0x6ac690c5ull + (u32)p2[9]; *(uint64_t*)(p2 + 8) = v; k = (i32)((u32)v % 100); } while (0)
#define F2380() do { FUN_6fd12380(p2); return; } while (0)
#define ATK() do { FUN_6fd12540(p1, p2, target, 2, 0, 0, 1); return; } while (0)
#define CHASE() do { FUN_6fd126b0(p2, p1, target, 4); return; } while (0)
#define FALL() do { FUN_6fd122c0(0xf); return; } while (0)
#define R() ((i32)(u32)FUN_6fc21180(seed))
  if (p2 && *p2 == 1 && p2[5] && (W(p2[5], 0x54) == 3 || W(p2[5], 0x54) == 0x13)) {
    if (W(ai, 0x14) == 0) W(ai, 0x14) = 1;
    if (dist < 0x1e && W(ai, 0x18) < dist) W(ai, 0x18) = dist;
    if (flag) {
      ROLL();
      if ((u32)k > 0x1e || b1 == 0) {
        Ordinal_11103(p2, 0);
        i32 ev[8] = {4, (i32)(intptr_t)p2, 0, 0, 0, 0, 0, 0};
        uint8_t v = 100;
        i32 t = (i32)FUN_6fc21320();
        if (t && (*(uint8_t*)((intptr_t)t + 0x104) & gdwBitMasks_exref[4])) v = 0x65;
        ((uint8_t*)ev)[0x15] = v;
        FUN_6fc95e10(p1, ev);
        return;
      }
      if (R() < 0x32) F2380();
      F2380();
    }
  }
  i32 d = (i32)Ordinal_11027(p2);
  if (W(ai, 0x14) != 2) {
    if (d < 0x21) { W(ai, 0x14) = 2; if (FUN_6fd12d80(p1, p2, 8)) return; }
    if (flag) {
      W(ai, 0x14) = 1; ROLL();
      if (k < S(st, 0x56 + lvl)) {
        if (b1 == 0 || has == 0 || R() > 0x1e) { FUN_6fd12220(p1, target); return; }
        if (d38 < 0x15) F2380();
      }
      if (R() > 0x20) { FUN_6fd122c0(10); return; }
      CHASE();
    }
    if (dist < S(st, 0x62 + lvl)) {
      W(ai, 0x14) = 1;
      if (R() >= S(st, 0x5c + lvl)) {
        if (dist > 0x14) ATK();
        if (dist < 9 && R() < 0x32) { FUN_6fd12d80(p1, p2, 8); return; }
        if (R() > 0x31) { FUN_6fd122c0(10); return; }
        CHASE();
      }
      if (((b2 && W(ai, 0x1c) < 1 && R() < S(st, 0x68 + lvl)) || (b4 && W(ai, 0x1c) < 1 && R() < S(st, 0x68 + lvl)))) {
        FUN_6fd12380(p2); W(ai, 0x1c) = 0xb; return;
      }
      if (b1 == 0 || has == 0 || d38 > 0x14) ATK();
      if (R() > 0x4a) CHASE();
      if (R() < 0x32) F2380();
      F2380();
    }
    if (W(ai, 0x14) == 1) ATK();
    FALL();
  }
  if (d > 0x4a) { W(ai, 0x14) = 1; ATK(); }
  if (dist < 0xe || dist <= W(ai, 0x18)) {
    i32 r;
    if (S(st, 0x32) < 1 || (r = S(st, 0x34) * 100 / S(st, 0x32) - 100) < 1) r = 0;
    else if (r > 0x77) r = 0x78;
    if (r < -0x7e || r > 0x7e) {
      Ordinal_10024((const void*)0, Ordinal_10265(0x110));
      std::abort();
    }
    i32 o = (p2 && *p2 == 1 && p2[5]) ? W(p2[5], 0x2c) : 0;
    if (r) W(o, 0x1c) = r;
    if (FUN_6fd12d80(p1, p2, 8)) return;
  }
  if (dist < S(st, 0x62 + lvl) && R() < S(st, 0x5c + lvl)) {
    if ((b2 && W(ai, 0x1c) < 1 && R() < S(st, 0x68 + lvl)) || (b4 && W(ai, 0x1c) < 1 && R() < S(st, 0x68 + lvl))) {
      FUN_6fd12380(p2); W(ai, 0x1c) = 0xb; return;
    }
    if (b1 && has && d38 < 0x15) F2380();
    CHASE();
  }
  FALL();
}
