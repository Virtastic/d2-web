// Batch ac630: SCmd packet handlers + ring buffer. Derived from cached 1.13c code.
// Env-seam: unrecovered callees are Env function pointers; register args are explicit params.
// Guesses: Node/Block struct layouts for wasm64 (original offsets are 32-bit), Ordinal roles.
#include <cstdint>
#include <cstring>
namespace d2client_ac630 {
using U = uint32_t; using I = int32_t; using P = uintptr_t;
static U rd32(const uint8_t* p, U o) { U v; std::memcpy(&v, p + o, 4); return v; }
static uint16_t rd16(const uint8_t* p, U o) { uint16_t v; std::memcpy(&v, p + o, 2); return v; }
struct Block { I cap, count, f2; uint8_t* data; };            // ring block; data at orig +0xC
struct Node { I kind; I f1, f2; U id; I f4; uint8_t* data; P f6; };
struct Env {
  void (*fatal)(U msgId);                      // Ordinal_10265+10024+_exit
  Block* (*newBlock)(void* ring);                               // 0xABDC0
  I elemSize;                                                   // DAT_6fbcbbd8
  Node* (*find)(I table, U id);                                 // hash lookup (id&0x7f chain); table 0=bba608,1=bbaa08
  Node* (*findPlayerNode)(U id, I tableBase, U idx);            // 0xA4E20
  void* (*ord10708)(I);  void (*ord10252)(void*, uint8_t);
  void (*ord10302)(Node*, uint16_t, uint8_t, uint8_t, const char*, I);
  I* g_bfb8;                                                    // DAT_6fbcbfb8
  void (*ord501)(uint8_t* dst, const uint8_t* src, I n);        // memcpy-like
  void* (*fn4d630)(void*, U);                               // 0x4D630
  void (*fn5dd0)(I, const uint16_t*);                           // 0x45DD0
  void (*fn31aa0)(Node*);                                       // 0x81AA0
  I g_bbfc; void (*fn47a0)(const uint8_t*);                     // DAT_6fbcbbfc, 0x247A0
  void (*ord10422)(I, uint8_t*);
  void (*fn3550)(I);                                            // 0x43550
  void (*fnC530)();                                             // 0xAC530
  uint8_t* (*ord10042)(I, I); void (*ord10043)(I, I);
  uint8_t* g_bbcc; I g_bbd0;                                    // DAT_6fbcbbcc/bbd0
  I g_c39c;                                                     // DAT_6fbcc39c
  // AC970 unit-list walk
  P (*first)(P); P (*next)(P); P (*inner)(P); I (*test)(P, void*); void (*a)(); void (*b)();
  void (*fin1)(); void (*fin2)();
};

// 1.13c D2Client+0xAC630  (thiscall; EAX = byte count)
void sub_AC630(Env& e, uint8_t* self, const uint8_t* src, U n) {
  if (!self) { e.fatal(0x8a5); return; }
  Block* b; std::memcpy(&b, self + 0xd8, sizeof b);
  if (!b || b->count + 1 == b->cap) b = e.newBlock(self);
  if (I(n) > e.elemSize) { e.fatal(0x127f); return; }
  std::memcpy(b->data + size_t(b->count) * e.elemSize, src, n);
  b->count++;
}
// 1.13c D2Client+0xAC750  (copies 26 bytes: 13 wchar, then call with 0)
void sub_AC750(Env& e, const uint16_t* s) { uint16_t t[14]; std::memcpy(t, s, 26); e.fn5dd0(0, t); }
// 1.13c D2Client+0xAC800
void sub_AC800(Env& e, const uint8_t* pk) {
  if (pk[0xb]) return;
  Node* n = e.find(0, rd32(pk, 3)); if (!n) return;
  if (n->kind != 0) { e.fatal(0x5a); return; }
  void* h = e.ord10708(-1); if (!h) { e.fatal(0xad6); return; }
  e.ord10252(h, pk[9]);
}
// 1.13c D2Client+0xAC910
void sub_AC910(Env& e, const uint8_t* pk) {
  Node* n = e.findPlayerNode(rd32(pk, 3), 0, pk[1]);
  if (n) { e.ord10302(n, rd16(pk, 7), pk[9], pk[2], "..\\Source\\D2Client\\GAME\\SCmd.cpp", 0xa8c); *e.g_bfb8 = 0; }
}
// 1.13c D2Client+0xAC970  (EBX = unit; walks list at +0x60)
void sub_AC970(Env& e, uint8_t* unit) {
  if (!unit) return;
  P lst; std::memcpy(&lst, unit + 0x60, sizeof lst); if (!lst) return;
  P it = e.first(lst);
  while (it) { P nx = e.next(it); P d = e.inner(it); it = nx; if (d && *(I*)d == 4 && e.test(d, nullptr)) { e.fin1(); e.fin2(); } }
  e.a(); e.b();
}
// 1.13c D2Client+0xACBF0  (switch on I32 at pkt+1; every case 1..0x1a calls the same 0x43550)
void sub_ACBF0(Env& e, const uint8_t* pk) { e.fn3550(I(rd32(pk, 1))); }
// 1.13c D2Client+0xACD70  (ESI = pkt)
void sub_ACD70(Env& e, const uint8_t* pk) {
  bool alloc = true;
  if (e.g_bbcc) { if (!pk[2]) alloc = false; else { e.ord10043(0x5b, 0); e.g_bbcc = nullptr; e.g_bbd0 = 0; } }
  if (alloc) e.g_bbcc = e.ord10042(0x62, 0);
  uint8_t* d = e.g_bbcc + e.g_bbd0;
  for (U i = 0; i < pk[1]; ++i) d[i] = pk[7 + i];
  e.g_bbd0 += pk[1];
  if (I(rd32(pk, 3)) == e.g_bbd0) { e.fnC530(); e.ord10043(0x71, 0); e.g_bbcc = nullptr; e.g_bbd0 = 0; }
}
// 1.13c D2Client+0xAD160
void sub_AD160(Env& e, uint8_t* pk) {
  U arg = rd32(pk, 1);  // guess: plain 32-bit value
  U a = rd32(pk, 0x15), b = rd32(pk, 0x19);
  Node* n = e.find(1, a);
  if (n) { if (n->kind != 2) { e.fatal(0x5a); return; } e.ord501(n->data + 0x28, pk + 5, 0x10); }
  if (b == 0xffffffffu) return;
  n = e.find(1, b);
  if (n) { if (n->kind != 2) { e.fatal(0x5a); return; } e.ord501(n->data + 0x28, pk + 5, 0x10); }
  uint8_t* r = (uint8_t*)e.fn4d630(pk, arg);
  if (r) { std::memcpy(r + 0x3c, &a, 4); std::memcpy(r + 0x40, &b, 4); }
}
// 1.13c D2Client+0xAD2A0
void sub_AD2A0(Env& e, const uint8_t* pk) {
  U w = rd32(pk, 0); Node* n = e.findPlayerNode(rd32(pk, 4) << 16 | (w >> 16), 0, (w >> 8) & 0xff);
  if (n) e.fn31aa0(n);
}
// 1.13c D2Client+0xAD2F0
void sub_AD2F0(Env& e, const uint8_t* pk) {
  if (!e.g_bbfc) { e.fatal(0xd21); return; }
  e.fn47a0(pk);
}
// 1.13c D2Client+0xAD370
void sub_AD370(Env& e, const uint8_t* pk) {
  U w = rd32(pk, 0); Node* n = e.findPlayerNode(rd32(pk, 4) << 16 | (w >> 16), 0, (w >> 8) & 0xff);
  if (!n) return;
  U* h = reinterpret_cast<U*>(n->data + 0xa4);  // piVar1[0x29]; guess offset kept 32-bit
  if (*h) e.ord10422(0, reinterpret_cast<uint8_t*>(uintptr_t(*h)));
  *h = 0;
}
// 1.13c D2Client+0xAD450  (EAX = pkt, param_1 = ECX)
void sub_AD450(Env& e, const uint8_t* pk, I ecx) {
  if (ecx == -1) return;
  switch (pk[0]) {
    case 0xaf: e.g_c39c = 1; break;
    case 0xb0: e.g_c39c = 0; break;
    case 0xb3: sub_ACD70(e, pk); break;
    case 0xb4: sub_ACBF0(e, pk); break;
  }
}
}
