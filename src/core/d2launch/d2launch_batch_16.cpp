// D2Launch batch 16. 9E40 skipped: per-case string ids are lost in the decompile (needs disasm).
#include <cstdint>
#include <cstring>

__attribute__((weak)) extern void* Ordinal_401(uint32_t, const char*, int, int);  // Fog alloc
__attribute__((weak)) extern void Ordinal_403(void*, const char*, int, int);      // Fog free
__attribute__((weak)) extern int32_t Ordinal_502(void);                           // key (caller id)
__attribute__((weak)) extern uint32_t FUN_GetTickCount(void);
__attribute__((weak)) extern void Ordinal_10054(uint32_t, int);
__attribute__((weak)) extern void Ordinal_10043(int, int);
__attribute__((weak)) extern void Ordinal_10106(void);
__attribute__((weak)) extern void Ordinal_10131(void);
__attribute__((weak)) extern void Ordinal_10147(void);
__attribute__((weak)) extern int Ordinal_10227(void);
__attribute__((weak)) extern uint8_t* DAT_6fa65ec8;
__attribute__((weak)) extern uint8_t* DAT_6fa65ecc;
__attribute__((weak)) extern uint32_t DAT_6fa65ed0, DAT_6fa65ed4, DAT_6fa65ee4;
__attribute__((weak)) extern uint32_t* DAT_6fa65ef4;  // cache list head (32-bit nodes in retail)

// 1.13c D2Launch+0x9E10  (index in EAX)
uint32_t FUN_6fa49e10(uint32_t idx) { return (idx == 0 || idx == 1 || idx == 6 || idx == 9) ? 0 : 1; }

static void freeCur() {
  if (!DAT_6fa65ecc) return;
  if (*(int32_t*)(DAT_6fa65ecc + 0x334) == 0) Ordinal_10054(*(uint32_t*)(DAT_6fa65ecc + 0x338), 1);
  Ordinal_10043(0x2a1, 0);
  DAT_6fa65ecc = nullptr;
}

// 1.13c D2Launch+0xA150
void FUN_6fa4a150(void) {
  freeCur();
  Ordinal_10106(); Ordinal_10106(); Ordinal_10106();
  for (int i = 0; i < 5; i++) Ordinal_10147();
  Ordinal_10131();
  if (Ordinal_10227() != 0) { Ordinal_10147(); Ordinal_10147(); return; }
  Ordinal_10131();
}

// 1.13c D2Launch+0xAE60
void FUN_6fa4ae60(void) {
  uint8_t* n = DAT_6fa65ec8;
  while (n) {
    uint32_t h = *(uint32_t*)(n + 0x338);
    n = *(uint8_t**)(n + 0x34c);
    Ordinal_10054(h, 1);
    Ordinal_10043(0x52d, 0);
  }
  DAT_6fa65ec8 = nullptr;
  DAT_6fa65ed0 = DAT_6fa65ed4 = DAT_6fa65ee4 = 0;
  freeCur();
}

struct CacheNode { int32_t key; void* data; uint32_t expiry; CacheNode* next; };
// 1.13c D2Launch+0xA230  (this = src buffer, size = byte count); 300000 ms lifetime
void FUN_6fa4a230(const void* src, uint32_t size) {
  static const char* F = "..\\Source\\D2Launch\\Src\\CharSel.cpp";
  int32_t key = Ordinal_502();
  CacheNode* n = (CacheNode*)DAT_6fa65ef4;
  while (n && n->key != key) n = n->next;
  if (n) {
    Ordinal_403(n->data, F, 0xf1, 0);
    n->data = Ordinal_401(size, F, 0xf2, 0);
  } else {
    n = (CacheNode*)Ordinal_401(0x10, F, 0xfa, 0);
    n->key = key;
    n->data = Ordinal_401(size, F, 0xfc, 0);
    n->next = (CacheNode*)DAT_6fa65ef4;
    DAT_6fa65ef4 = (uint32_t*)n;
  }
  memcpy(n->data, src, size);
  n->expiry = FUN_GetTickCount() + 300000;
}
