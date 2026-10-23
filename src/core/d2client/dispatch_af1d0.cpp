// Derived from cached 1.13c code; ECX=param_1, EDX=param_2 (byte blob). Callee is an env seam.
#include <cstdint>
#include <cstring>
namespace d2client_dispatch_af1d0 {
using P=void*;
struct Calls { uint32_t (*c2a750)(P,int32_t*,int32_t*); }; // RVA 0x2A750
// 1.13c D2Client+0xAF1D0
void sub_AF1D0(int32_t* p1,const uint8_t* p2,Calls& c){
 if(!p1) return;
 int32_t m[7]; // stack frame order: local_1c[0..2], local_10, local_c, local_8, local_4
 uint16_t w; int32_t d; memcpy(&w,p2+6,2); memcpy(&d,p2+10,4);
 m[0]=w; m[1]=-1; m[2]=p2[9]; m[3]=d; m[4]=p2[8]; m[5]=0; m[6]=0;
 c.c2a750(p1,reinterpret_cast<int32_t*>(static_cast<intptr_t>(0x16)),m); // ponytail: msg id 0x16 passed as the int* msg slot per decompile; seam decides
}
}
