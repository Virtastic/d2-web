// D2Common 1.13c batch: CRT float-to-text front end (VS2003 _cftoe/_cftof family).
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) intptr_t d2_fn_d2common_87a91(uint32_t flt, uint32_t p2, int32_t* strflt, uint32_t* buf6);  // FUN_6fdd7a91
__attribute__((weak)) intptr_t d2_fn_d2common_87960(char* buf, uint32_t size, int32_t ndig, uint32_t strflt);      // __fptostr
__attribute__((weak)) intptr_t d2_fn_d2common_87344(const int32_t* strflt, char* buf, uint32_t ndec, char g);        // __cftof2

// 1.13c D2Common+0x873E0: convert *(p1) to text in p2 (size p3); st[0]=sign, st[1]=decpt.
EMSCRIPTEN_KEEPALIVE char* d2_fn_d2common_873e0(const uint32_t* p1, char* p2, uint32_t p3) {
 int32_t st[2];
 uint32_t tmp[6];
 const uint32_t flt = p1[0];
 d2_fn_d2common_87a91(flt, p1[1], st, tmp);
 d2_fn_d2common_87960(p2 + (st[0] == 0x2d), static_cast<uint32_t>(st[1]) + p3, reinterpret_cast<intptr_t>(st), flt);
 d2_fn_d2common_87344(reinterpret_cast<const int32_t*>(static_cast<uintptr_t>(flt)), p2, p3, 0);  // strflt arrives in a register (still _PtFlt)
 return p2;
}
}
