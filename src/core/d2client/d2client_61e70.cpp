// 1.13c D2Client+0x61E70 automap section loader; every callee is a callback and every module global is a parameter.
#include <cstdint>
#include <cstring>
namespace d2client_61e70 {
struct Env {
 int32_t (*open)();                                                   // 0x5F3E0 (no args), -1 = fail
 void (*seek)(int32_t h, int32_t pos, int32_t whence);                // 0xC028: ECX = h, EDX = pos, stack (0, whence)
 void (*read)(int32_t h, void* buf, int32_t len, int32_t* got);       // 0xC034: ECX = h, EDX = buf, stack (len, &got, 0)
 void (*close)(int32_t h);                                            // 0xC03A: ECX = h
 void* (*alloc)(int32_t size, int32_t line);                          // 0xBF7A: ECX = size, EDX = file, stack (line, 0)
 void (*release)(void* p, int32_t line);                              // 0xBF86: ECX = p, EDX = file, stack (line, 0)
 void* (*newNode)();                                                  // 0x5F6B0
 void (*insert)(void* node, void* root);                              // 0x61320: ECX = node, EDX = root slot
 void (*unlink)(int32_t h, uint8_t* tab, int32_t prev);               // 0x5F290: ESI = h, EDI = table, stack prev
};
// P = DAT_6fbcc1c4 (P[0] = automap key, P[2..5] = the four tree roots); thr = DAT_6fbcc1d8 (int32 table); c3bc = *DAT_6fbcc3bc (c3bc[1] compared).
// Returns true when retail would reach its fatal path (a block read short of its length: Fog error + exit).
bool sub_61E70(const Env& e, int32_t* P, const int32_t* thr, const int32_t* c3bc) {
 if (!P) return false;
 const int32_t h = e.open();
 if (h == -1) return false;
 uint32_t tab[100] = {};
 int32_t got = 0;
 e.read(h, tab, 0x190, &got);
 if (got != 0x190) { e.close(h); return false; }
 int32_t prev = 0, next = (int32_t)tab[P[0]];
 if (next == 0) { e.close(h); return false; }
 static const int32_t lines[4][3] = { {0x575, 0x57a, 0x586}, {0x58a, 0x58f, 0x59b}, {0x5a2, 0x5a7, 0x5b3}, {0x5bd, 0x5c2, 0x5ce} };
 do {
  int32_t v[8] = {};   // v[0] next, v[1] key, v[2] sel, v[3] tag, v[4..7] block byte sizes
  e.seek(h, next, 0);
  for (int k = 0; k < 8; k++) e.read(h, &v[k], 4, &got);
  if (v[1] != P[0]) { e.unlink(h, (uint8_t*)tab, prev); e.close(h); return false; }
  for (int s = 0; s < 4; s++) {
   const int32_t len = v[4 + s];
   if (!len) continue;
   if (s == 2 && v[3] != c3bc[1]) { e.seek(h, len, 1); continue; }
   uint16_t* b = (uint16_t*)e.alloc(len, lines[s][0]);
   e.read(h, b, len, &got);
   if (got != len) return true;
   const int32_t t = s == 3 ? thr[v[2]] : thr[0];
   for (uint32_t i = 0; i < (uint32_t)len >> 1; i += 3) {
    if ((int16_t)b[i] >= t) { e.unlink(h, (uint8_t*)tab, prev); break; }
    uint8_t* n = (uint8_t*)e.newNode();
    const int32_t one = 1; memcpy(n, &one, 4);
    memcpy(n + 4, &b[i], 2); memcpy(n + 6, &b[i + 1], 2); memcpy(n + 8, &b[i + 2], 2);
    e.insert(n, P + 2 + s);
   }
   e.release(b, lines[s][2]);
  }
  prev = next = v[0];   // [esp+0x24] and [esp+0x28] both take the record's next link
 } while (next != 0);
 e.close(h);
 return false;
}
}
