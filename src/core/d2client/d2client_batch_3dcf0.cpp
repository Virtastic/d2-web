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

// 1.13c D2Client+0x3E490 ; eax=self, stk=in_stack_00000014, rec = Ordinal_10019 out (word[4]=flag, [?]=iStack_14)
void sub_3E490(const G& g,P self,const int16_t* p1,I stk,I eax,const H& h,I* tabIdx,P* slots){
 I k=h.ord10847(*p1); const int16_t* sel;
 if(k<0||k>6) sel=(const int16_t*)h.fallback;
 else { if(!slots[k*0x22/4]) slots[k*0x22/4]=h.sub_B6F6C0(); sel=(const int16_t*)slots[k*0x22/4]; }
 (void)tabIdx;
 if(!h.sub_AB17F0()) return;
 U rec[18]={0}; h.ord10019(rec);
 // rec[17]=iStack_4 (flag), rec[13]=iStack_14 (arg2)
 if(rec[17]) sub_3E0C0(g,self,sel,I(rec[13]),eax,h); else sub_3DCF0(g,self,sel,I(rec[13]),eax,h);
 h.sub_3D3C0(self,stk);
}

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

// 1.13c D2Client+0x3E5E0 ; flag=unaff_retaddr, rec[] = Ordinal_10019 out (iStack_c=rec[15], iStack_4=rec[17])
void sub_3E5E0(P self,I p1,const int16_t* p2,I flag,I p5,P hud,const H2& h){
 P u=h.sub_AB17F0(); if(!u) return;
 U rec[18]={0}; rec[0]=((uint8_t*)u)[7]; rec[13]=U(p1); rec[16]=0; h.ord10019(rec);
 if(!flag) h.c3DCF0(self,p2,I(rec[15])); else h.c3E0C0(self,p2,I(rec[15]));
 if(hud){ P v=*(P*)((uint8_t*)hud+0x60);
  if(v){
   if(*p2==2){ P w=h.ord10061(v);
    if(w&&h.ord10711(w)&&h.ord10048(w)){ rec[13]=U(h.ord10973(w,0x46,0)); h.sub_3D3C0(self,p5); return; } }
   else if(*p2==4){ P a=h.ord10061(v); I b=h.ord11139(v,4), c=h.ord11139(v,5);
    if(I(intptr_t(a))==b) b=c; p5=I(rec[17]);
    if(b){ P bp=(P)(intptr_t)b; if(h.ord10744(bp,0x2d)&&h.ord10711(bp)&&h.ord10048(bp)){ rec[16]=U(h.ord10973(bp,0x46,0)); p5=I(rec[17]); } } } } }
 h.sub_3D3C0(self,p5);
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

// 1.13c D2Client+0x3FC20 (stdcall, 1 arg: room/unit). Disassembly-derived; rect = 8 ints copied by D2Common#10561 (room+0x4c).
struct H3FC20 {
 const I* (*o10388)(I room,U* count);              // list of 0x30-byte nodes (12 words); EAX=list, *count out
 void (*fatal)(void);                              // 10265(0x2d5), 10024(&DAT_6fb8359f), exit(-1) -- does not return
 void (*o10561)(I room,I* rect8);
 void (*o11087)(I* x,I* y);                        // in place, both pointers
 U (*o10051)(I);
 void (*f3F590)(I eaxX,I y,I arg);                 // EAX = x, stack (y, arg), ret 8
 void (*o10050)(I x,I y,const void* dat,U c);      // cdecl
 I (*o10331)(I);
 const I* (*o10310)(I room);                       // first node; node[0..3]={x0,y0,x1,y1}, next = word 0x2c/4
 const I* (*nextNode)(const I*);
 void (*o10375)(I* x,I* y);                        // in place
 void (*o10010)(I,I,I,I,I,I);
 U (*deref28)(I p);                                // *(int*)(p+0x28)
};
void sub_3FC20(const G& g,I room,I listBbfc,const void* dat57c,const H3FC20& h){
 U cnt=0; const I* list=h.o10388(room,&cnt);
 if(!list&&cnt){ h.fatal(); return; }
 I rect[8]; h.o10561(room,rect);
 for(U i=0;i<cnt;++i){ const I* n=list+i*12;
  if(!n[4]) continue;                               // [esi+4], esi=n+0xc
  I A=(n[2]+rect[4])*5+3, B=(n[3]+rect[5])*5+3;
  h.o11087(&A,&B);
  if(n[7]==0xf) continue;
  if((n[5]&0x1c000)!=0x4000) continue;
  U f=h.o10051(n[6]);
  if(f&4) h.f3F590(A+0x20,B,0x78);
  else if(n[5]&0x800) h.f3F590(A+0x10,B+8,0x78);
  else h.f3F590(A,B,0xff);
  I X=A+(-7-g.originX), Y=B-g.originY;
  h.o10050(X,Y,dat57c,h.deref28((U)n[4]));
 }
 I own=listBbfc?h.o10331(listBbfc):0;
 if(room!=own) return;
 for(const I* p=h.o10310(room);p;p=h.nextNode(p)){
  I X[4]={p[0],p[0],p[2],p[2]}, Y[4]={p[1],p[3],p[1],p[3]};
  for(int i=0;i<4;i++){ h.o10375(&X[i],&Y[i]); X[i]-=g.originX; Y[i]-=g.originY; }
  if(room==(listBbfc?h.o10331(listBbfc):0)){
   h.o10010(X[0],Y[0],X[0],Y[0]+0x10,0x78,0xff);
   h.o10010(X[1],Y[1],X[1]+0x10,Y[1],0x78,0xff);
   h.o10010(X[2],Y[2],X[2]-0x10,Y[2],0x78,0xff);
   h.o10010(X[3],Y[3],X[3],Y[3]-0x10,0x78,0xff);
  }
  h.o10010(X[0],Y[0],X[1],Y[1],0x78,0xff);
  h.o10010(X[0],Y[0],X[2],Y[2],0x78,0xff);
  h.o10010(X[1],Y[1],X[3],Y[3],0x78,0xff);
  h.o10010(X[2],Y[2],X[3],Y[3],0x78,0xff);
 }
}
} // namespace
