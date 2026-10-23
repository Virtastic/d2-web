// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: globals, ordinals and callees come in through Env.
#include <cstdint>
namespace d2client_a7400 {
using U=uint32_t;
struct Env {
 U (*rd)(U addr); void (*wr)(U addr,U v); void (*wr8)(U addr,uint8_t v);
 U gFlag;   // DAT 0x6fbc9854
 U gVal;    // DAT 0x6fbcc3b8
 U (*a5a60)(U inEax);          // returns player/owner ptr
 U (*o10826)(U); uint8_t (*o10459)(U);
 void (*o10936)(); void (*o10874)(U,U); void (*o10679)(U); void (*o10614)(); void (*o10445)(U);
 void (*c91e0)(U u); void (*d5490)(U p4,U u,U owner); void (*b0170)();
 void (*b2820)(U inEax,U p4); void (*a5a30)(); void (*b6950)();
 void (*o10819)(U u,const char* file,U line);
 U (*a7360)(U ecx);            // sub_A7360
 void (*d5b70)(U r);
 void (*fatal)(U code);        // Ordinal_10265(0x202) -> Ordinal_10024 -> _exit(-1)
};
// 1.13c D2Client+0xA7400
// EAX=inEax, ECX=u, EDX=id; stack: p3, p4, type(p5). u is a 32-bit-layout struct.
U sub_A7400(const Env& e,U inEax,U u,U id,U p3,U p4,U type){
 U owner=0;
 if(u){ U f=e.rd(u+0xC8); e.wr(u+0xC8, e.gFlag? (f|0x2000000):(f&0xfdffffff)); }
 e.wr(u,type); e.wr(u+4,p3); e.wr(u+12,id); e.wr(u+0xD4,0); e.wr(u+8,0);
 if(inEax||p4){
  owner=e.a5a60(inEax);
  e.wr8(u+0x18,e.o10459(e.o10826(owner)));
  e.wr(u+0x1C,e.gVal);
 }
 e.o10936();
 if(type==4){ e.o10874(0,u); e.o10679(u); }
 if(owner){
  uint64_t v=(uint64_t)e.rd(owner+0x6c)*0x6ac690c5ull+e.rd(owner+0x70);
  e.wr(owner+0x6c,(U)v); e.wr(owner+0x70,(U)(v>>32));
  e.wr(u+0x28,(U)v); e.o10614();
 }
 U ecx=0;
 switch(type){
 case 0: e.c91e0(u); break;
 case 1: e.fatal(0x202); return 0;
 case 2: e.o10445(u); e.d5490(p4,u,owner); break;
 case 3: e.b0170(); break;
 case 4: e.o10445(u); e.b2820(inEax,p4); break;
 case 5: e.o10445(u); e.a5a30(); goto tail;
 }
 e.b6950();
 if(type==0||type==1) e.o10819(u,"..\\Source\\D2Client\\UNIT\\CUnit.cpp",0x21d);
tail:
 U r=e.a7360(ecx);
 if(r&&type==2) e.d5b70(r);
 return r;
}
}
