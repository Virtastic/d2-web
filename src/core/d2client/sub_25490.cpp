// 1.13c D2Client+0x25490 -- unit-record init (caller 0xA7400), corrected against the disassembly.
// Registers: AL = unit+0x10 byte (the id), ECX = c0 (relayed to two callees), EDX = unit; stack w0..w2 (ret 0xC).
// Ordinal_10688(w2) returns a pointer to a class record T; id indexes byte arrays inside T. Pointers are 32-bit handles
// (resolve maps a handle to a host pointer). Unrecovered callees are host seams called in retail's order.
#include <cstdint>
#include <cstring>
namespace d2client_25490 {
using U=uint32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
static uint8_t rb(P p,U n){return static_cast<uint8_t*>(p)[n];}
struct Env {
 P (*resolve)(U); U (*alloc)(U lineNo, U zero);              // 0xBF7A: ECX=0x38 EDX=file, stack (0x76,0)
 U (*o10688)(U); void (*o10574)(P,U,U,U); void (*o10032)(P,U,U,U);
 void (*o10857)(P,U); void (*o10595)(P); U (*o10826)(U); U (*o10459)(U);
 U (*o10585)(U); U (*o10265)(U); void (*o10024)(U,U); void (*fatal)(U code);
 U (*f25280)(); void (*f24920)(P unit,U ax,U b164,U b165,U b166);
};
// 1.13c D2Client+0x25490
void sub_25490(P unit,U ecx,U w0,U w1,U w2,U eax,Env& e){
 (void)w0;
 wr(unit,100,0); wr(unit,0x10,eax&0xff);
 U h=e.alloc(0x76,0); wr(unit,0x14,h); P rec=e.resolve(h);
 std::memset(rec,0,0x38); wr(rec,0x10,0); wr(rec,0x14,0);
 U th=e.o10688(w2); P t=e.resolve(th); U id=rd(unit,0x10);
 U f=rd(unit,0xc4); f=rb(t,0xc4+id)?f|2:f&~2u; wr(unit,0xc4,f);
 wr(rec,0,th);
 e.o10574(unit,w2,w1,ecx);
 if(rb(t,0x120+id)) e.o10032(unit,w2,w1,ecx);
 f=rd(unit,0xc4); f=rb(t,0x128)?f|4:f&~4u; wr(unit,0xc4,f);
 e.o10857(unit,w2);
 wr(unit,0xc4,rd(unit,0xc4)&~8u);
 e.o10595(unit);
 f=rd(unit,0xc4); f=rb(t,0x118+id)?f&~0x20u:f|0x20; wr(unit,0xc4,f);
 if(rb(t,0x131+id)==1) wr(unit,0xc4,rd(unit,0xc4)|0x100000);
 U ty=rd(unit,4);
 if(ty==0x3c){
  U v=e.o10826(w2);
  if(v==0x26) static_cast<uint8_t*>(rec)[4]=4; else if(v==4) static_cast<uint8_t*>(rec)[4]=0x26;
 } else if(ty==0x3b){
  U v=e.o10826(w2); U a=e.o10459(v);
  U n=e.o10585((w1&~0xffu)|(a&0xff));   // retail pushes the dword whose low byte it just overwrote (w1's slot)
  if((int32_t)n>0xff){e.o10024(0x8359f,e.o10265(0x95));e.fatal(0xffffffffu);}   // _exit(-1) never returns in retail; the capture hooks it, so the store below still runs
  static_cast<uint8_t*>(rec)[4]=uint8_t(n);
 }
 if(e.f25280()==0) e.f24920(unit,rb(t,0x110+id),rb(t,0x164),rb(t,0x165),rb(t,0x166));
}
}
