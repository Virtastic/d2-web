// D2Net CRT batch: malloc, __local_unwind2, strcpy, strcat. Imports injected; pure functions of inputs.
#include <cstdint>
#include <cstddef>
// 1.13c D2Net+0x2C28: _malloc(size) = __nh_malloc(size, newmode); newmode is the global at 0xb0d8.
extern "C" void* d2net_malloc_2c28(size_t n, int newmode, void* (*nh_malloc)(size_t, int)) { return nh_malloc(n, newmode); }
// 1.13c D2Net+0x2C7E: __local_unwind2. Scope table entries are {prev, filter, handler}; walk from the frame's
// current level (+0xc) to stopLevel (or -1), storing prev as the new level and calling handler when filter==0.
struct UnwindEnt { int32_t prev; int32_t filter; void (*handler)(); };
struct UnwindFrame { uint32_t pad[2]; UnwindEnt* table; int32_t level; };
extern "C" void d2net_local_unwind2_2c7e(UnwindFrame* f, int32_t stopLevel, void (*enter)()) {
  while (f->level != -1 && f->level != stopLevel) {
    UnwindEnt* e = &f->table[f->level];
    f->level = e->prev;
    if (e->filter == 0) { enter(); e->handler(); }
  }
}
// 1.13c D2Net+0x43E0: strcpy (word-at-a-time in retail; result identical to the byte loop). Returns dst.
extern "C" char* d2net_strcpy_43e0(char* d, const char* s) { char* r = d; while ((*d++ = *s++)) {} return r; }
// 1.13c D2Net+0x43F0: strcat. Returns dst.
extern "C" char* d2net_strcat_43f0(char* d, const char* s) { char* r = d; while (*d) d++; while ((*d++ = *s++)) {} return r; }
// 1.13c D2Net+0x2770: __chkstk touches stack pages and rewrites the return slot; register-only asm, not expressible in C.
