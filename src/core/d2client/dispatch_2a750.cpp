// Derived from cached 1.13c code; ECX=param_1, EDX=msg (int*), stack=param_3 (int*). Callees are env seams.
#include <cstdint>
namespace d2client_dispatch_2a750 {
using P=void*;
struct Calls {
 uint32_t (*c9830)(P,int32_t*);          // case 0: result returned
 void (*cdc70)(P,int32_t*,int32_t*);     // case 1 (RVA 0x4DC70)
 void (*c5ce0)(P,int32_t*);              // case 2 (RVA 0x25CE0)
 void (*c81d30)(int32_t,int32_t);        // case 4 (RVA 0x81D30)
};
// 1.13c D2Client+0x2A750
uint32_t sub_2A750(P p1,int32_t* msg,int32_t* p3,Calls& c){
 switch(msg[0]){
 case 0: return c.c9830(p1,msg);
 case 1: c.cdc70(p1,msg,p3); return 1;
 case 2: c.c5ce0(p1,msg); return 1;
 case 4: c.c81d30(p3[0],p3[1]); return 1;
 }
 return 1;
}
}
