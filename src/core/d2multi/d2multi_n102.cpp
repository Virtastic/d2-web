// D2Multi CRT unlock/thunk tails. Seams are function pointers; unlock arg is retail's EBX (file handle).
#include <cstdint>
void (*d2multi_fn_136C)(void) = nullptr;
void (*d2multi_unlock_fhandle)(int) = nullptr;
int d2multi_ebx_fhandle = 0;

// 1.13c D2MULTI+0x033B1
void d2multi_sub_33B1(void) { if (d2multi_fn_136C) d2multi_fn_136C(); }
// 1.13c D2MULTI+0x04738
void d2multi_sub_4738(void) { if (d2multi_unlock_fhandle) d2multi_unlock_fhandle(d2multi_ebx_fhandle); }
// 1.13c D2MULTI+0x0499F
void d2multi_sub_499F(void) { if (d2multi_unlock_fhandle) d2multi_unlock_fhandle(d2multi_ebx_fhandle); }
// 1.13c D2MULTI+0x07B3B
void d2multi_sub_7B3B(void) { if (d2multi_unlock_fhandle) d2multi_unlock_fhandle(d2multi_ebx_fhandle); }
