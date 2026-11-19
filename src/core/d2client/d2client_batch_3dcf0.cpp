// Implementation batch (alpha lane): decompile-derived, not yet vectored.
// Ordinal/FUN callees are host callbacks; register-passed args (EAX/ESI/EDI/EBX/EBP) are explicit params.
#include <cstdint>
#include "d2client_batch_3dcf0.h"
namespace d2client_3dcf0 {
static void match(const G& g,P self,const int16_t* p1,I p2,I eax,I want,const H& h){
 for(int i=0;i<16;++i)
  if(g.id[i]==*p1&&g.val[i]==p2&&g.id[i]!=-1&&g.mode[i]==want) h.sub_3D770((uint8_t*)self+0x22,eax-0x23,4);
}
// 1.13c D2Client+0x3DCF0
void sub_3DCF0(const G& g,P self,const int16_t* p1,I p2,I eax,const H& h){ match(g,self,p1,p2,eax,0,h); }
// 1.13c D2Client+0x3E0C0
void sub_3E0C0(const G& g,P self,const int16_t* p1,I p2,I eax,const H& h){ match(g,self,p1,p2,eax,1,h); }

// dup of D2Client+0x3F590 ; eax=x. rect {x-ox-16, y-oy2-?...} as decompiled
void sub_3F590(const G& g,I y,I x,void (*ord10070)(I*,I),I arg){
 I r[4]; r[0]=(x-g.originX)-0x10; r[2]=(x-g.originX)+0x10; r[1]=y-g.originY; r[3]=r[1]+0x10; ord10070(r,arg);
}

// 1.13c D2Client+0x3F860
void sub_3F860(G& g,I (*ord10651)(P),I (*ord11142)(P),P unit){
 g.panelX=-0xa0;
 if(g.viewMode==2) g.panelX=g.screenW/2-0xa0; else if(g.viewMode==3) g.panelX=0;
 I a=ord10651(unit), b=ord11142(unit);
 g.originX=a-g.screenW/2; g.originY=(b-g.screenH/2)+0x10;
 if(g.camOn){ g.originX+=g.camDX; g.originY+=g.camDY; }
}

// 1.13c D2Client+0x3F5F0 -- EBX = &x, EDI = &y, plain ret: a screen point to a world point. In perspective mode (the
// no-argument mode test) y loses a fortieth of itself and x spreads about the half screen width by (40 - y/4) parts in that
// half width; then the view origin is added (x less the panel shift) and the pair goes through the two-pointer transform.
// Both cells take the transformed pair; EAX is the new x.
I sub_3F5F0(const G& g,I* px,I* py,I (*ord10013)(void),void (*ord10474)(I*,I*)){
 I x=*px,y=*py;
 if(ord10013()){
  I hi=I((int64_t(y)*-0x66666667LL)>>32);
  y=I(U(y)+U((hi>>4)-(hi>>31)));
  I half=g.screenW/2;
  x=I(U(x)+U(I((0x28u-U(I((U(y>>31)&3u)+U(y))>>2))*(U(x)-U(half)))/half));
 }
 y=I(U(y)+U(g.originY)); x=I(U(x)+U(g.originX)-U(g.oy));
 ord10474(&x,&y); *px=x; *py=y;
 return x;
}

// 1.13c D2Client+0x3F240 : C++ dtor (vtable reset + owned-object delete); host-side vtables.
struct Obj { void** vt; U pad; struct Child{void(**vt)(I);}* child; };
void sub_3F240(Obj* o,void** vtBase,void** vtDerived){ o->vt=vtDerived; if(o->child) (*o->child->vt[0])(1); o->vt=vtBase; }
// 1.13c D2Client+0x3F360
void sub_3F360(U* o,U vtDerived,U vtBase,void (*ord10065)(I),void (*ord10043)(I,I)){
 o[0]=vtDerived; I h=I(o[0x14]); ord10065(h); if(h) ord10043(0x7a,0); o[0]=vtBase;
}

// 1.13c D2Client+0x3F740 ; r={x,y,w,h} from Ordinal_10561(unit,&r); per 5x5 cell: c=10916(unit,x,y); (X,Y)=11087(&x,&y) in place;
// 10070(rect{X-ox-16,Y-oy,X-ox+16,Y-oy+16},0x46); 10050(X-7-ox,Y-oy,&DAT_6fb8057c,c) (cdecl). Bounds re-read from r each step (disasm).
void sub_3F740(const G& g,P unit,void (*ord10561)(P,I*),U (*ord10916)(P,I,I),void (*ord11087)(I*,I*),
 void (*ord10070)(I*,I),void (*ord10050)(I,I,const void*,U),const void* dat57c){
 I r[4]; ord10561(unit,r);
 for(I y=r[1]+2;y<r[3]+r[1]+5;y+=5) for(I x=r[0]+3;x<r[2]+r[0]+5;x+=5){
  U c=ord10916(unit,x,y); I X=x,Y=y; ord11087(&X,&Y);
  I rc[4]={X-g.originX-0x10,Y-g.originY,X-g.originX+0x10,Y-g.originY+0x10}; ord10070(rc,0x46);
  ord10050(X-7-g.originX,Y-g.originY,dat57c,c); }
}
} // namespace
