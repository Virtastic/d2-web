// D2Client sub_45B40: item-needs-repair predicate (esi = unit); imports are the D2Common thunks.
#include <cstdint>
namespace d2client_45b40 {
using U=uint32_t;
struct Services {
 virtual ~Services()=default;
 virtual U o11088(U u)=0; // D2Client+0xC220
 virtual U o10087(U u)=0; // +0xCE14
 virtual U o10865(U u)=0; // +0xC8BC
 virtual U o11116(U u)=0; // +0xC916
 virtual U stat(U u,U id,U layer)=0; // #10973 via +0xC1C6
 virtual U o10048(U u)=0; // +0xC73C
 virtual U o10463(U u)=0; // +0xC91C
 virtual U o10607(U u,U* out)=0; // +0xCE20
};
// 1.13c D2Client+0x45B40
U sub_45B40(U u,Services& s){
 if(s.o11088(u)==4||!s.o10087(u)) return 0;
 if(s.o10865(u)){ U m=s.o11116(u); if(m&&(int32_t)s.stat(u,0x48,0)<(int32_t)m) return 1; }
 if(s.o10048(u)&&(int32_t)s.stat(u,0x46,0)<(int32_t)s.o10463(u)) return 1;
 U q=0;
 if(s.o10607(u,&q)&&!q) return 1;
 return 0;
}
}
