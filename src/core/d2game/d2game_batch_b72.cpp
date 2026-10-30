// Batch b72: plain transcriptions of cached 1.13c D2Game functions. Hidden register inputs are explicit
// leading parameters. Skipped (stack-local/register artifacts in the decompile): 0x75A40 0x7E070 0x7EA10.
#include <cstdint>
#include <initializer_list>
#include <cstdlib>
typedef int32_t i32;
typedef uint32_t u32;
#define W(a, o) (*(i32*)((intptr_t)(a) + (o)))
#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
extern "C" __attribute__((weak)) void Ordinal_10024(const void*, u32);
EXT(Ordinal_10331); EXT(Ordinal_10482); EXT(Ordinal_10362); EXT(Ordinal_10237);
EXT(Ordinal_10973); EXT(Ordinal_10045); EXT(Ordinal_10158);
EXT(Ordinal_10498); EXT(Ordinal_10494); EXT(Ordinal_11103); EXT(FUN_6fc21560); extern "C" __attribute__((weak, import_module("env"), import_name("FUN_6fcaea80"))) void* b72_FUN_6fcaea80(void*, void*, int, int, int, int, int); EXT(FUN_6fc95e10);
EXT(FUN_6fc21130); EXT(FUN_6fc21100); EXT(FUN_6fc21220); EXT(FUN_6fc95680); EXT(FUN_6fcc28f0); EXT(FUN_6fc95a40);

// 1.13c D2Game+0x75900 (edi = struct whose +4 is the unit; param_1 = the one stack argument)
// FUN_6fc21220 takes the unit's class id in EAX; FUN_6fc95680 takes (param_1, unit, 0, edi).
extern "C" void b72_sub_75900(i32 edi, i32 param_1) {
  i32 unit = W(edi, 4), err;
  if (!unit) err = 0x1ed;
  else {
    if ((W(unit, 0xc4) >> 0x11) & 1) return;
    if (W(unit, 4) == 0x158) err = 0x1f8;
    else {
      i32 y = (i32)FUN_6fc21130(unit), x = (i32)FUN_6fc21100(unit);
      i32 room = (i32)Ordinal_10331(unit);
      if ((int16_t)Ordinal_10482(room, x, y, 0x801) != 0) return;
      if ((i32)Ordinal_10331(unit) == 0) err = 0x1ff;
      else if ((i32)FUN_6fc21220(W(unit, 4)) != 0) { FUN_6fc95680(param_1, unit, 0, edi); return; }
      else err = 0x204;
    }
  }
  Ordinal_10024((const void*)0 /* DAT_6fd19ff9: the assert file string, no address in this seam */, Ordinal_10265(err));
  std::abort();
}

// 1.13c D2Game+0x75C60 (eax = unit, ebx = game)
extern "C" u32 b72_sub_75C60(i32 unit, i32 game) {
  if (!unit) return 0;
  i32 r = (i32)Ordinal_10362();
  if (r < 0) return 0;
  i32 v = (i32)Ordinal_10237((uint8_t)*(uint8_t*)((intptr_t)game + 0x6d));
  v = W(v, 0x10) + 1;
  if ((W(unit, 0xc4) >> 9) & 1) v = (i32)Ordinal_10973();
  if (FUN_6fcc28f0(game, r, unit, 0, v, 0, 0, 0, 0, 1)) FUN_6fc95a40(game);
  return 1;
}

static void b72_enqueue(i32 owner, i32 msg) {
  if (!W(owner, 0xec)) W(owner, 0xec) = msg;
  if (W(owner, 0xf0)) W(W(owner, 0xf0), 0) = msg;
  W(owner, 0xf0) = msg;
  Ordinal_10158();
}

// 1.13c D2Game+0x7A090 (esi = owner)
extern "C" void b72_sub_7A090(i32 esi) {
  i32 p = (i32)Ordinal_10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp", 799, 0);
  *(uint8_t*)(p + 4) = 0xa4; W(p, 0) = 0; *(uint16_t*)(p + 8) = 799;
  b72_enqueue(esi, p);
}

// 1.13c D2Game+0x7A440 (esi = owner, retaddr = stack word)
extern "C" void b72_sub_7A440(i32 esi, uint16_t retaddr, uint16_t param_1, uint8_t param_2) {
  i32 p = (i32)Ordinal_10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp", 0x298, 0);
  *(uint16_t*)(p + 8) = 0x298; *(uint8_t*)(p + 10) = 0; *(uint16_t*)(p + 12) = retaddr;
  *(uint8_t*)(p + 4) = 0x9a; W(p, 0) = 0; *(uint16_t*)(p + 0xe) = param_1; *(uint8_t*)(p + 0x10) = param_2;
  b72_enqueue(esi, p);
}

// 1.13c D2Game+0x765E0 (ecx = game, edx = unit). Disassembly-resolved: FUN_6fc21560 takes (unit+4, 3);
// FUN_6fc95e10 gets (game, &local 0x20-byte event {3, unit, 0,0,0, byte+0x15=0x64, 0}, 1).
extern i32** sgptDataTables_exref;
extern "C" void b72_sub_765E0(i32 game, i32* unit) {
  Ordinal_10498(unit[0xb]);
  i32 cls = unit[1];
  if ((u32)FUN_6fc21560(cls, 3) == 0) { b72_FUN_6fcaea80((void*)(intptr_t)game, (void*)(intptr_t)unit, 2, W(game, 0xa8) + 1, 0, 0, 0); return; }
  if (unit[0] == 1 && cls >= 0) {
    i32 t = *(i32*)sgptDataTables_exref;  // monstats: +0xa78 rows (0x1a8), +0xa80 count; short at +2 = base class
    if (cls < W(t, 0xa80) && W(t, 0xa78) + cls * 0x1a8 != 0) cls = *(int16_t*)((intptr_t)W(t, 0xa78) + cls * 0x1a8 + 2);
    if (cls >= 0 && cls < W(t, 0xa80) && cls == 0x4e) {
      i32 v = W(game, 0xa8) + ((i32)Ordinal_10494(unit, 0x15) == 0 ? 0xf : 0x2d);
      b72_FUN_6fcaea80((void*)(intptr_t)game, (void*)(intptr_t)unit, 2, v, 0, 0, 0);
      return;
    }
  }
  Ordinal_11103(unit, 0);
  unit[0x2c] = 0xa0;
  i32 ev[8] = {3, (i32)(intptr_t)unit, 0, 0, 0, 0, 0, 0};
  ((uint8_t*)ev)[0x15] = 0x64;
  FUN_6fc95e10(game, ev, 1);
}
