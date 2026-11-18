// src/core/d2game/d2game_batch_98e30.cpp -- eight 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Batch lane (OBJECTIVE v8.3): marker +
// implementation only, no spec, no vector.
//
// GUESSED INPUTS: Ghidra lost register-only arguments (unaff_EBX/EDI, in_EAX); they are explicit
// parameters here, named at each function. Retail structs are 32-bit; here pointer fields are host
// pointers (uintptr_t) at the retail offsets -- offsets are retail's, widths of pointer slots are not.
// Out-of-batch callees (Ordinal_*, FUN_*) are function pointers on Env98 in retail argument order.
// Fatal asserts (_exit(-1) after Ordinal_10024) go through env.fatal(line).
#include <cstdint>
#include <cstring>

struct Env98 {
  void (*fatal)(int line);                                   // Ordinal_10265+10024+_exit
  void (*fn_a0c70)(void*, void*, int, int, int, uintptr_t);  // FUN_6fcc0c70
  void* (*tables)();                                         // *sgptDataTables
  int (*fn_91340)(void*);                                    // FUN_6fcb1340
  void (*fn_e07a0)(uint32_t);                                // FUN_6fd007a0
  void* (*ord10392)(uint32_t);                               // unit lookup
  uint32_t (*fn_1100)(void*);                                // FUN_6fc21100
  uint32_t (*fn_1130)(void*);                                // FUN_6fc21130
  void* (*fn_6f930)(int, void*);                             // FUN_6fc8f930
  void (*ord10637)(void*, uint32_t);
  void (*ord10019)(void*, uint32_t);
  void (*ord10689)(void*, int, int);
  void* (*ord10170)(uint32_t);                               // list head
  uint32_t (*ord10727)(void*);                               // node -> unit id
  void* (*ord10691)(void*);                                  // node next
  void* (*alloc10045)(const char*, int, int);
  void (*fn_9c070)(void*, int*);                             // FUN_6fcbc070
  int (*ord10049)(void*);
  int (*ord10973)(void*, int, int);
  int (*ord10331)(void*);
  int (*ord10826)(int);
  void (*ord10887)(void*, int, int, int);
  void (*ord10551)(void*, int, int, int);
  void* (*fn_9bd00)(short);                                  // FUN_6fcbbd00
  short (*fn_9beb0)(void*);                                  // FUN_6fcbbeb0
  void (*fn_1670)(int);                                      // FUN_6fc21670
};

template <class T> static T& at(void* b, size_t o) { return *(T*)((char*)b + o); }

// 1.13c D2Game+0x98E30
int D2Game_98E30(Env98& e, int* p1) {
  e.fn_a0c70(p1, nullptr, 0, 3, 0xa783, 0x6fcbed70);
  return 0;
}

// 1.13c D2Game+0x993A0
int D2Game_993A0(Env98& e, uint32_t p1, int* p2, int p3) {
  char* dt = (char*)e.tables();
  if (p3 < 0 || p3 >= at<int>(dt, 0xba0)) return 0;
  char* row = (char*)at<uintptr_t>(dt, 0xb98) + (size_t)p3 * 0x23c;
  if (!row) return 0;
  int h = e.fn_91340(p2);
  if (h <= 0) return 0;
  e.fn_e07a0(p1);
  int* u = (int*)e.ord10392(at<uint32_t>(p2, 0x2c));
  if (u == p2 || !u) return 0;
  uint32_t a[0x17] = {};            // local_60[0x17], zeroed
  a[0] = 1;
  *(uintptr_t*)&a[1] = (uintptr_t)p2;  // piStack_5c (slot index guessed under 64-bit)
  a[0x0b] = e.fn_1100(u);           // uStack_4c
  a[0x0c] = e.fn_1130(u);           // uStack_48
  a[0x12] = (uint32_t)p3;           // iStack_30
  void* r = e.fn_6f930(h, a);
  if (!r) return 0;
  e.ord10637(r, (uint32_t)u[0]);
  e.ord10019(r, (uint32_t)u[3]);
  short s = at<short>(row, 0x4e);
  if (s >= 0 && s < at<int>(dt, 0xbc0)) e.ord10689(u, s, 0);
  return 1;
}

// The unit-list ownership test: d2game_9b570.cpp is its own file.

// 0x9BC70 and 0x9BD40 live in the game tree (D2Game/UNIT/PartySub9B.cpp); the unreferenced stand-alone
// 0x9BF70 and 0x9C270 bodies that called them are gone (their probes are self-contained).

// 1.13c D2Game+0x9C1F0
int D2Game_9C1F0(Env98& e, void* cb) {
  int r = 0;
  e.fn_9c070(cb, &r);
  return r;
}

