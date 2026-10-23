// src/core/storm/storm_1b1c0.cpp -- 1.13c Storm+0x1b1c0
#include <cstdio>
#include <cstdint>
#include <cstring>
// Heap-page teardown. Page layout is retail's 32-bit one: +0 next page, +0x1C first block, +0x20 end,
// +0x5C source line, +0x60 source name. Block header: u16 size at +0, flags byte at +3.
struct StormPageEnv {
  bool trackingOff;                                   // DAT_6fc43134 == 0
  void (*setLastError)(uint32_t);                     // SetLastError + DAT_6fc45388 = code
  void (*errDisplay)(uint32_t code, const char* name, int32_t line); // 2F590, only if DAT_6fc44d58
  void (*debugOut)(const char* msg);                  // OutputDebugStringA
  void (*blockFree)();                                // 1ABF0
  void (*virtualFree)(void* p);                       // VirtualFree(p,0,MEM_RELEASE)
};
// 1.13c Storm+0x1b1c0: walk blocks; leaked ones (flags&0x82==0) are reported and freed; if any live
// block has 0x80 the page is kept (returns it), else unlinked and released (returns list head).
extern "C" uint8_t** Storm_Sub1B1C0(StormPageEnv& e, uint8_t** head, bool errHook) {
  uint8_t* pg = *head; uint8_t flagAcc = 0;
  uint16_t* p = *(uint16_t**)(pg + 0x1C); uint16_t* end = *(uint16_t**)(pg + 0x20);
  if (p < end) {
    do {
      uint8_t f = ((uint8_t*)p)[3]; p = (uint16_t*)((uint8_t*)p + *p);
      if ((f & 0x82) == 0) {
        if ((f & 0x40) == 0) {
          if (e.trackingOff) { e.setLastError(0x8510007e); if (errHook) e.errDisplay(0x8510007e, (char*)pg + 0x60, *(int32_t*)(pg + 0x5C)); }
          else { char b[200]; snprintf(b, sizeof b, "Storm Error : memory never released -- %s, %i\n", (char*)pg + 0x60, *(int32_t*)(pg + 0x5C)); e.debugOut(b); }
        }
        e.blockFree();
      } else flagAcc |= f & 0x80;
    } while (p < end);
    if (flagAcc) return (uint8_t**)pg;
  }
  *head = *(uint8_t**)pg; e.virtualFree(pg); return head;
}
