// D2Common 1.13c batch: MSVC CRT __ld12cvt. Unwired callees are weak.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) int32_t D2Common_87631_RoundMan(uint32_t* pMan, int32_t nbit);
__attribute__((weak)) void D2Common_876D7_ShrMan(uint32_t* pMan, int32_t n);
uint32_t D2Common_876BE_IsZeroMan(const uint32_t* pMan);
void D2Common_876A3_CopyMan(uint32_t* pDst, const uint32_t* pSrc);

// 1.13c D2Common+0x87752 (__ld12cvt): _LDBL12 at p1 -> float/double bits at p2 per format table p3.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_87752_ld12cvt(const uint16_t* p1, uint32_t* p2, const int32_t* p3)
{
 uint32_t m[3];  // m[0]=local_10, m[1]=local_c, m[2]=local_8
 uint32_t l1c[3];
 memcpy(&m[0], p1 + 3, 4);
 memcpy(&m[1], p1 + 1, 4);
 m[2] = (uint32_t)p1[0] << 16;
 const uint16_t u1 = p1[5];
 const uint32_t e = u1 & 0x7fff;
 int32_t iVar5 = (int32_t)e - 0x3fff;
 uint32_t ret;
 if (iVar5 == -0x3fff) {
  iVar5 = 0;
  if (D2Common_876BE_IsZeroMan(&m[0]) != 0) { ret = 0; goto fin; }
  m[0] = 0; m[1] = 0;
 } else {
  D2Common_876A3_CopyMan(l1c, &m[0]);
  if (D2Common_87631_RoundMan(&m[0], p3[2]) != 0) iVar5 = (int32_t)e - 0x3ffe;
  const int32_t iVar2 = p3[1];
  if (iVar5 < iVar2 - p3[2]) {
   m[0] = 0; m[1] = 0;
  } else {
   if (iVar2 < iVar5) {
    if (p3[0] <= iVar5) {
     m[1] = 0; m[2] = 0; m[0] = 0x80000000u;
     D2Common_876D7_ShrMan(&m[0], p3[3]);
     iVar5 = p3[5] + p3[0];
     ret = 1;
     goto fin;
    }
    m[0] &= 0x7fffffffu;
    iVar5 = p3[5] + iVar5;
    D2Common_876D7_ShrMan(&m[0], p3[3]);
    ret = 0;
    goto fin;
   }
   D2Common_876A3_CopyMan(&m[0], l1c);
   D2Common_876D7_ShrMan(&m[0], iVar2 - iVar5);
   D2Common_87631_RoundMan(&m[0], p3[2]);
   D2Common_876D7_ShrMan(&m[0], p3[3] + 1);
  }
 }
 iVar5 = 0;
 ret = 2;
fin:
 m[0] = ((uint32_t)iVar5 << ((0x1fu - (uint32_t)p3[3]) & 0x1f)) | ((u1 & 0x8000) ? 0x80000000u : 0u) | m[0];
 if (p3[4] == 0x40) { p2[1] = m[0]; p2[0] = m[1]; }
 else if (p3[4] == 0x20) { p2[0] = m[0]; }
 return ret;
}
}
