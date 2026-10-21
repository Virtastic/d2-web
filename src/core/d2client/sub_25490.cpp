// Derived from cached 1.13c D2Client decompile (0x25490). Unrecovered callees are host seams on one env,
// called in retail's order. Serialized 32-bit handles at unit+0x10/+0x14 go through resolve/alloc seams.
#include <cstdint>
#include <cstring>
namespace d2client_25490 {
using U=uint32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
static uint8_t rb(P p,U n){return static_cast<uint8_t*>(p)[n];}
struct Env {
 P (*resolve)(U); U (*alloc)(U,U);                 // Ordinal_10042 returns a handle
 U (*o10688)(U); void (*o10574)(P,U,U,U); void (*o10032)(P,U,U,U);
 void (*o10857)(P,U); void (*o10595)(P); U (*o10826)(U); uint8_t (*o10459)(U);
 U (*o10585)(uint8_t); U (*o10265)(U); void (*o10024)(U,U); void (*fatal)(); // _exit(-1)
 uint8_t (*glob128)(U); // byte at absolute 0x128 + index
 U (*f25280)(); void (*f24920)(P);
};
// 1.13c D2Client+0x25490
void sub_25490(P unit,U ecx,U arg,U eax,U ebx,Env& e){
 wr(unit,100,0); wr(unit,0x10,eax&0xff);
 U h=e.alloc(0x76,0); wr(unit,0x14,h); P rec=e.resolve(h);
 std::memset(rec,0,0x38); wr(rec,0x10,0); wr(rec,0x14,0);
 U idx=e.o10688(arg); P owner=e.resolve(rd(unit,0x10));
 U f=rd(unit,0xc4); f=rb(owner,0xc4+idx)?f|2:f&~2u; wr(unit,0xc4,f);
 wr(rec,0,idx);
 e.o10574(unit,arg,ebx,ecx);
 if(rb(owner,0x120+idx)) e.o10032(unit,arg,0,ecx);
 f=rd(unit,0xc4); f=e.glob128(idx)?f|4:f&~4u; wr(unit,0xc4,f);
 e.o10857(unit,arg);
 wr(unit,0xc4,rd(unit,0xc4)&~8u);
 e.o10595(unit);
 f=rd(unit,0xc4); f=rb(owner,0x118+idx)?f&~0x20u:f|0x20; wr(unit,0xc4,f);
 if(rb(owner,0x131+idx)==1) wr(unit,0xc4,rd(unit,0xc4)|0x100000);
 U t=rd(unit,4);
 if(t==0x3b){
  U v=e.o10826(arg); U n=e.o10585(e.o10459(v));
  if(n>0xff){e.o10024(0x6fb8359f,e.o10265(0x95));e.fatal();return;}
  static_cast<uint8_t*>(rec)[4]=uint8_t(n);
 } else if(t==0x3c){
  U v=e.o10826(arg);
  if(v==0x26) static_cast<uint8_t*>(rec)[4]=4; else if(v==4) static_cast<uint8_t*>(rec)[4]=0x26;
 }
 if(e.f25280()==0) e.f24920(unit);
}
}
