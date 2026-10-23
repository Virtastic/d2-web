// D2Common 1.13c batch. Unwired callees are weak seams so the file links.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) void d2_sub_66A00(uint32_t a, uint32_t b, int32_t* out2);  // out[0]=local_8, out[1]=local_4
__attribute__((weak)) void* d2_sub_3E110(int32_t* p);
__attribute__((weak)) uint32_t d2_sub_66FB0(const uint8_t* eax, void* self, uint32_t a, uint32_t* p);
__attribute__((weak)) uint32_t d2_ord_10265(uint32_t);
__attribute__((weak)) void d2_ord_10024(const void* fmt, uint32_t v);

// 1.13c D2Common+0x67300
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_67300(const uint8_t* p1, uint32_t p2, uint32_t p3) {
 if (*reinterpret_cast<const int32_t*>(p1 + 0x7c) == 0) return 0;
 int32_t l[2];  // l[0]=local_8, l[1]=local_4
 d2_sub_66A00(p2, p3, l);
 if (l[0] < 0) l[0] = -l[0];
 if (l[0] <= 0) {  // zero, or INT_MIN stays negative
  if (l[1] < 0) l[1] = -l[1];
  if (l[1] <= 0) {
   d2_ord_10024(nullptr, d2_ord_10265(0x342));  // retail: format string DAT_6fdda728
   __builtin_trap();                            // _exit(-1)
  }
 }
 void* self = d2_sub_3E110(&l[1]);
 return d2_sub_66FB0(p1, self, p2, &p3);
}
}
