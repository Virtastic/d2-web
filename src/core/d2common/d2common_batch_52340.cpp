// D2Common 1.13c batch. Callees and process globals are weak retail-named seams so the file links.
#include <cstdint>
#include <emscripten.h>

#define W __attribute__((weak))
extern "C" {
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W char DAT_6fdda728[1];
W uint32_t gdwBitMasks_exref[32];
W int32_t* DAT_6fdf0b50;
W int32_t sub_4FCB0(intptr_t unit, intptr_t skill);             // 0x4FCB0 bonus skill level
W bool sub_4F590();                                              // 0x4F590
W int32_t sub_4FDC0(intptr_t unit, intptr_t skill);             // 0x4FDC0
W uint32_t sub_50930(intptr_t skill, intptr_t unit);            // 0x50930
W bool sub_50AF0(intptr_t unit);                                 // 0x50AF0
W bool sub_50870(intptr_t unit);                                 // 0x50870
W bool sub_50F00(intptr_t unit);                                 // 0x50F00
W bool sub_52210(intptr_t skill);                                // 0x52210

// 1.13c D2Common+0x52340: skill usability check. param_1 = unit, param_2 = skill (dwords:
// +0 record ptr (32-bit), +0x28 level, +0x34 -1 = add bonus level). Returns a reason code, 0 = ok-ish.
EMSCRIPTEN_KEEPALIVE int32_t sub_52340(int32_t* param_1, int32_t* param_2) {
 // Weak seams: trap rather than jump to 0 / read a null table. (gdwBitMasks_exref is a zero
 // placeholder when unlinked, its values are not settled here.)
 if (!sub_4FCB0 || !sub_4F590 || !sub_4FDC0 || !sub_50930 || !sub_50AF0 || !sub_50870 || !sub_50F00 ||
     !sub_52210 || !DAT_6fdf0b50) __builtin_trap();
 if (!param_2) {
  if (Ordinal_10265 && Ordinal_10024) Ordinal_10024(DAT_6fdda728, Ordinal_10265(0x8ea));
  __builtin_trap(); // retail: _exit(-1)
 }
 const uint8_t* rec = reinterpret_cast<const uint8_t*>(static_cast<intptr_t>(static_cast<uint32_t>(param_2[0])));
 if (!rec || (gdwBitMasks_exref[8] & rec[5] & 0xff) == 0) return 3;
 if (!param_1) return 7;
 int32_t lvl = param_2[10];
 if (param_2[0xd] == -1) lvl += sub_4FCB0(reinterpret_cast<intptr_t>(param_1), reinterpret_cast<intptr_t>(param_2));
 if (lvl < 0) lvl = 0;
 int32_t cap = *DAT_6fdf0b50;
 if (lvl < cap) cap = lvl;
 if (cap == 0) return 7;
 if (gdwBitMasks_exref[0x14] & rec[4]) return 6;
 if (gdwBitMasks_exref[0x10] & rec[4]) return 5;
 const intptr_t u = reinterpret_cast<intptr_t>(param_1), s = reinterpret_cast<intptr_t>(param_2);
 if (sub_50930(s, u) == 0 || !sub_50F00(u)) return 2;
 if (sub_50AF0(u)) {
  if (!sub_4F590()) return 4;
  if (sub_4FDC0(u, s) != 0) {
   if (!sub_50870(u)) return 2;
   return sub_52210(s) ? 0 : 8;
  }
 }
 return 1;
}
}
