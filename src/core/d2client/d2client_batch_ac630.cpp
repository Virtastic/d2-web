// Batch ac630: SCmd packet handlers + ring buffer. Derived from cached 1.13c code.
// Env-seam: unrecovered callees are Env function pointers; register args are explicit params.
// Guesses: Node/Block struct layouts for wasm64 (original offsets are 32-bit), Ordinal roles.
#include <cstdint>
#include <cstring>
namespace d2client_ac630 {
using U = uint32_t; using I = int32_t; using P = uintptr_t;
static U rd32(const uint8_t* p, U o) { U v; std::memcpy(&v, p + o, 4); return v; }
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

// 1.13c D2Client+0xAD2A0
void sub_AD2A0(Env& e, const uint8_t* pk) {
  U w = rd32(pk, 0); Node* n = e.findPlayerNode(rd32(pk, 4) << 16 | (w >> 16), 0, (w >> 8) & 0xff);
  if (n) e.fn31aa0(n);
}
// 1.13c D2Client+0xAD370
void sub_AD370(Env& e, const uint8_t* pk) {
  U w = rd32(pk, 0); Node* n = e.findPlayerNode(rd32(pk, 4) << 16 | (w >> 16), 0, (w >> 8) & 0xff);
  if (!n) return;
  U* h = reinterpret_cast<U*>(n->data + 0xa4);  // piVar1[0x29]; guess offset kept 32-bit
  if (*h) e.ord10422(0, reinterpret_cast<uint8_t*>(uintptr_t(*h)));
  *h = 0;
}
}
