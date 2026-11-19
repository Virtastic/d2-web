// Batch D2Client+0x51390..0x527A0, derived from cached 1.13c decompile.
// Ordinals/callees are injected through Calls; globals are passed in State.
#include <cstdint>
#include <cstring>
namespace d2client_batch_51390 {
using U=uint32_t; using I=int32_t; using P=void*;
struct Calls {
 I (*pathX)(P); I (*pathY)(P);           // Ordinal_10867 / 10750 (path x/y)
 P (*resolve)(U);                        // Ordinal_10331 (1 arg)
 P (*resolve3)(U,U,U);                   // Ordinal_10331 (room,1,0x220) as 52570 pushes it
 P (*node630f0)(U*);                     // D2Client+0xB30F0 (msg dispatch, 23 words)
 void (*o10637)(P,I); void (*o10019)(P,I); P (*o10754)(P);
 void (*fn5ff10)(U,I,U,U,I,I);           // D2Client+0xB5FF10: EDX=unit, stack (id,x,y,0,1)
 I (*o10573)(P,I,I,I,I); P (*o10389)(P,I,I,P,I,I);
 U (*o11117)(); void (*o11129)(); I (*mask)(); P (*o10078)(...);
};
// 1.13c D2Client+0x51390 and +0x51500 live in their own files (d2client_51390.cpp, d2client_51500.cpp).
// 1.13c D2Client+0x51E30
void sub_51E30(P esi,I (*f568e0)(P,I,I,I),void (*f1860)(P),I& c2f4,I& c2f8){
 if(!esi){c2f4=0;c2f8=1;return;}
 I r=f568e0(esi,0,0,0); c2f8=0;
 if(r){f1860(esi);c2f8=1;return;}
 c2f4=0;
}
// 1.13c D2Client+0x524D0
// Returns packed pair; rgb outputs set when flag 5591 set.
uint64_t sub_524D0(U p2,I c2e0,I c0a0,uint8_t c5591,I a212c,uint8_t* o3,uint8_t* o4,uint8_t* o5,uint64_t (*tail)(I)){
 if(!c2e0){
  if(!c0a0) return ~0ull; // original exits the process (_exit(-1))
  if(c5591){*o3=0xff;*o4=0x40;*o5=0x30;
   if(a212c==-1)return (uint64_t(uintptr_t(o5))<<32)|0x50;
   return tail((a212c*0x80)/0x1e);}
 }
 return uint64_t(p2)<<32;
}
// Probe-facing form: EAX only, Fog's float cosine as the tail, truncated as _ftol does.
int32_t sub_524D0(I c2e0,I c0a0,uint8_t c5591,I a212c,uint8_t* o3,uint8_t* o4,uint8_t* o5,float (*f)(I)){
 static float (*s_f)(I); s_f=f;
 return I(sub_524D0(0,c2e0,c0a0,c5591,a212c,o3,o4,o5,[](I v)->uint64_t{return uint64_t(int64_t(double(s_f(v))*80.0));}));   // Fog 10083 result, fmul [D2Client+0xD753C] (80.0f), then _ftol
}
}
