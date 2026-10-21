// 1.13c D2Client+0x44230 transcribed from the cached decompile and the x86 (the decompile misread the
// locals; the x86 is authoritative). Callees are host callbacks. in_ECX = skipFlag (REG: __fastcall-like).
#include <cstdint>
namespace d2client_batch_44230 {
using U=uint32_t; using I=int32_t; using P=void*;
struct View { U flags; I r1,r2,r3,r4; U pad[3]; U px,py; }; // *DAT_6fbcc3c4: +0 flags,+4..+0x10 rect,+0x24/+0x28 pos
struct Calls {
 P player; View* view; U prevX,prevY,dx,dy; I g_cc3b8; I g_cc414; I g_46cc;   // bbfc cc3c4 9f60 9f5c cc408 cc40c cc3b8 cc414 a46cc
 void (*f714f0)(); I (*d0a2)(); I (*c1d2)(P); I (*c1f6)(P); void (*f3afd0)(I*,I*);
 void (*f3b0c0)(I); void (*f31340)(); void (*c5b6)(I,uint8_t*,uint8_t*,uint8_t*); void (*d192)(U,U,U);
 U (*f37e0)(); I (*d0a8)(U,U,U,U); void (*d3a2)(); void (*fef860)(); void (*f3b110)(View*); void (*f739e0)(View*);
 void (*d40e)(); void (*f6a90)(); void (*fa9a0)(); void (*d090)(I); void (*d08a)();
};
// 1.13c D2Client+0x44230
void sub_44230(Calls&c,I skipFlag){
 c.f714f0();
 if(c.d0a2()) return;
 const View*v=c.view; I a=v->r1,b=v->r2,cc=v->r3,d=v->r4;
 I x=c.c1d2(c.player)-(cc-a)/2, y=c.c1f6(c.player)-(d-b)/2;
 c.f3afd0(&x,&y);
 if(c.prevX!=0xffffffffu||c.prevY!=0xffffffffu){c.dx=(U)x-c.prevX;c.dy=(U)y-c.prevY;}
 View*w=c.view; c.prevX=(U)x; c.prevY=(U)y;
 if(w->px!=(U)x||w->py!=(U)y){w->flags|=1;w->px=(U)x;w->py=(U)y;}
 c.f3b0c0(0); c.f31340();
 uint8_t r,g,bl; c.c5b6(c.g_cc3b8,&r,&g,&bl); c.d192(r,g,bl);
 if(!c.d0a8(c.f37e0(),0,0,0)) return;
 c.d3a2(); c.fef860();
 if(c.g_cc414!=3) c.f3b110(c.view);
 c.f739e0(c.view); c.d40e();
 if(!skipFlag) c.f6a90();
 c.fa9a0();
 if(c.g_46cc){c.d090(0); if(--c.g_46cc<0)c.g_46cc=0;}
 c.d08a();
}
}
