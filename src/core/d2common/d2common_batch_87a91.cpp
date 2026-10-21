// D2Common 1.13c batch: CRT double -> decimal digits record (_fltout2-style wrapper).
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) void d2_fn_d2common_879d7(uint32_t out[3], const uint32_t in[2]);  // ___dtold
__attribute__((weak)) intptr_t d2_fn_d2common_88273(uint32_t lo, uint32_t hi, uint32_t se, int32_t ndig, int32_t p5, void* rec);  // FUN_6fdd8273
__attribute__((weak)) uint32_t* D2Common_6960_Strcpy(uint32_t* pDest, const uint32_t* pSrc);

// 1.13c D2Common+0x87A91: strflt[0]=sign char, [1]=decpt, [2]=result of 88273, [3]=buf6 (digits copied in).
EMSCRIPTEN_KEEPALIVE intptr_t d2_fn_d2common_87a91(uint32_t flt, uint32_t p2, int32_t* strflt, uint32_t* buf6) {
 uint32_t in[2] = {flt, p2};
 uint32_t ld[3];
 struct { int16_t decpt; int8_t sign; int8_t pad; uint32_t digits[6]; } rec;
 d2_fn_d2common_879d7(ld, in);
 const intptr_t r = d2_fn_d2common_88273(ld[0], ld[1], ld[2] & 0xffff, 0x11, 0, &rec);
 strflt[2] = static_cast<int32_t>(r);
 strflt[0] = rec.sign;
 strflt[1] = rec.decpt;
 D2Common_6960_Strcpy(buf6, rec.digits);
 strflt[3] = static_cast<int32_t>(reinterpret_cast<intptr_t>(buf6));
 return reinterpret_cast<intptr_t>(strflt);
}
}
