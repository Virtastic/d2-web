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

// 1.13c D2Game+0x9B570 ; game = unaff_EBX
int D2Game_9B570(Env98& e, char* unit, char* game) {
  if (unit) {
    uint32_t f = at<uint32_t>(unit, 200);
    if ((f >> 0x16 & 1) || (f >> 0x17 & 1)) return 1;
  }
  void* n = e.ord10170(at<uint32_t>(unit, 0x60));
  while (n) {
    uint32_t id = e.ord10727(n);
    if (!game) e.fatal(0x2d3);
    char* u = (char*)at<uintptr_t>(game, 0x1720 + (id & 0x7f) * sizeof(uintptr_t));
    while (u && at<uint32_t>(u, 0xc) != id) u = (char*)at<uintptr_t>(u, 0xe4);
    n = e.ord10691(n);
    if (u) return 1;
  }
  return 0;
}

// 1.13c D2Game+0x9BC70 ; in_EAX = amount, unaff_EDI = out1
void D2Game_9BC70(Env98& e, void* unit, int* out2, int amount, int* out1) {
  int a = e.ord10049(unit), b = e.ord10973(unit, 0, 0);
  *out2 = 0;
  *out1 = amount;
  if (a < b + amount) { *out1 = a - b; *out2 = amount - (a - b); }
}

// 1.13c D2Game+0x9BD40 ; in_EAX = game (party list at +0x1d2c); returns nothing, bumps *list
void D2Game_9BD40(char* game) {
  uint16_t* hd = (uint16_t*)at<uintptr_t>(game, 0x1d2c);
  uint16_t v = *hd; if (v < 3) v = 3;
  for (uint8_t* p; (p = (uint8_t*)at<uintptr_t>(hd, 4)) != nullptr;) {   // ponytail: next slot offsets guessed
    while (*(uint16_t*)p != v) { p = (uint8_t*)at<uintptr_t>(p, 8); if (!p) goto done; }
    if (++v < 3) v = 3;
  }
done:
  *hd = v + 1;
}

// 1.13c D2Game+0x9BF70 ; unaff_EBX = game
uint16_t D2Game_9BF70(Env98& e, char* game) {
  char* hd = (char*)at<uintptr_t>(game, 0x1d2c);
  if (!hd) e.fatal(0x7f);
  char* n = (char*)e.alloc10045("..\\Source\\D2Game\\UNIT\\Party.cpp", 0x80, 0);
  if (!n) e.fatal(0x81);
  memset(n, 0, 12);
  *(uint16_t*)n = 0;
  uint16_t id = 0; { // FUN_6fcbbd40 on this game
    uint16_t* h = (uint16_t*)hd; D2Game_9BD40(game); id = (uint16_t)(*h - 1); }
  *(uint16_t*)n = id;
  at<uintptr_t>(n, 8) = at<uintptr_t>(hd, 4);
  at<uintptr_t>(hd, 4) = (uintptr_t)n;
  return id;
}

// 1.13c D2Game+0x9C1F0
int D2Game_9C1F0(Env98& e, void* cb) {
  int r = 0;
  e.fn_9c070(cb, &r);
  return r;
}

// 1.13c D2Game+0x9C270 ; iStack_4 = amount (guessed: = param_3), unaff_EBX = base offset, game = iStack_4 table owner
int* D2Game_9C270(Env98& e, int* unit, int amount, char* game, int ebx) {
  int cnt = 0;
  e.fn_9c070(nullptr, &cnt);
  auto clamp = [&](int amt, int** rem) {
    int a = e.ord10049(unit), b = e.ord10973(unit, 0xe, 0);
    int* r = nullptr; int use = amt;
    if (a < b + amt) { use = a - b; r = (int*)(intptr_t)(amt - use); }
    *rem = r;
    if (unit) {
      int c = e.ord10973(unit, 0xe, 0);
      if (c + use < 0 || (*unit == 0 && e.ord10049(unit) < c + use)) { e.ord10887(unit, 0xe, 0, 0); return false; }
      e.ord10551(unit, 0xe, use, 0);
    }
    return true;
  };
  int* rem;
  if (cnt > 1) {
    short pid = (!unit || unit[0x20] == 0 || *unit != 0) ? (short)-1 : e.fn_9beb0(unit);
    char* party = (char*)e.fn_9bd00(pid);
    int total = amount;
    if (party) {
      int own = e.ord10826(e.ord10331(unit));
      for (uintptr_t* m = (uintptr_t*)at<uintptr_t>(party, 4); ; m = (uintptr_t*)m[1]) {
        if (!m) { clamp(total, &rem); return rem; }
        if (!game) e.fatal(0x2d3);
        for (int* u = (int*)at<uintptr_t>(game, 0x1120 + (*(uint32_t*)m & 0x7f) * sizeof(uintptr_t)); u; u = (int*)at<uintptr_t>(u, 0xe4)) {
          if ((uint32_t)u[3] == *(uint32_t*)m) {
            int pu = e.ord10331(u);
            if (u != unit && pu && e.ord10826(pu) == own && unit && unit[4] && unit[4] != 0x11) {
              int hi = 0, lo = 0;
              D2Game_9BC70(e, unit, &hi, amount, &lo);
              e.fn_1670(0);
              total += hi - ebx;
            }
            break;
          }
        }
      }
    }
  }
  clamp(amount, &rem);
  return rem;
}
