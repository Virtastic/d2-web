// Prior art: none. Derived from cached 1.13c decompile. Env-seam style: every Ordinal_/FUN_ callee comes in through Env.
// Ordinal names UNVERIFIED. `self` is the register-passed unaff_EDI struct: self[0]=kind, self[0xb]=handle.
#include <cstdint>
namespace d2client_c5f20 {
using I=int32_t; using U=uint32_t;
struct Env {
 I (*o10980)(I p); I (*o10649)(I p); void (*o10137)(I p,I v);
 void (*o11164)(I h,I v); void (*o11056)(I h,I v); void (*o10286)(I h,I a,I p); void (*o10647)(I h,I v);
 I (*f1230)(); I (*f1300)();        // FUN_6fab1230 / FUN_6fab1300 -> record ptr (0 = none)
 I (*rd16)(I a); I (*rd8)(I a);     // read short at +0x34 / byte at +0x41 of the record
 void (*o10488)(I h,U v,const char* file,I line); void (*o10334)(I h);
};
// 1.13c D2Client+0xC5F20
I c5f20(const Env& e,const I* self,I p){
 I h=self[0xb]; if(!h) return 0;
 I a=e.o10980(p), b=e.o10649(p);
 if(!a||!b) return 0;
 e.o10137(p,0x1101); e.o11164(h,0); e.o11056(h,0); e.o10286(h,a,p); e.o10647(h,9);
 U v;
 if(self[0]==1){ I r=e.f1230(); if(!r) return 0; v=(U)(I)(int16_t)e.rd16(r+0x34); }
 else { I r=e.f1300(); v=(U)(uint8_t)e.rd8(r+0x41); }
 e.o10488(h,v<<8,"..\\Source\\D2Client\\SKILLS\\SkillsBar.cpp",0x237); e.o10334(h);
 return 1;
}
}
