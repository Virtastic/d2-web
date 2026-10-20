// Implementation batch (alpha lane): decompile-derived, not yet vectored.
// Ordinal/FUN callees are host callbacks; register-passed args (EAX/ESI/EDI/EBX/EBP) are explicit params.
#include <cstdint>
namespace d2client_3dcf0 {
using I=int32_t; using U=uint32_t; using P=void*;
struct G { // D2Client globals (RVA = 6fbXXXXX - 6fa b0000 image base 6fa b0000)
 I id[16];   // 0xD9D30 ; 16 slots, -1 = empty
 I val[16];  // 0xD9DD0
 I mode[16]; // 0xD9E20
 I originX, originY;   // DAT_6fbc9960 / DAT_6fbc995c
 I screenW, screenH;   // DAT_6fb8bc48 / DAT_6fb8bc4c
 I viewMode, camX, camY, camOn, camDX, camDY, oy; // bcc414, bc9960 base, bc995c base, bcbefc, bcbf00, bbb9dc, bcc418
 I panelX;             // DAT_6fbc9958
};
struct H { // callees
 void (*sub_3D770)(P dst,I eax,I n);
 void (*sub_3D3C0)(P self,I arg);
 P    (*sub_AB17F0)(void);
 I    (*ord10847)(I); void* (*sub_B6F6C0)(void);
 void (*ord10019)(U*); P (*ord10061)(P); I (*ord10711)(P); I (*ord10048)(P);
 I (*ord10973)(P,I,I); P (*ord11139)(P,I); I (*ord10744)(P,I);
 P* cachedUnit; P hudUnit; P fallback; P tab[7];
};
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

// 1.13c D2Client+0x3F590 ; eax=x. rect {x-ox-16, y-oy2-?...} as decompiled
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

// 1.13c D2Client+0x3F5F0 ; x=*EBX, y=*EDI, newx=ESI, newy=EBP
void sub_3F5F0(const G& g,I* px,I* py,I newx,I newy,I (*ord10013)(void),void (*ord10474)(I*,I*)){
 I x=*px,y=*py;
 if(ord10013()){
  I hi=I((int64_t(y)*-0x66666667LL)>>32);
  y+=(hi>>4)-(hi>>31);
  x+=((0x28-((I((U(y>>31)&3u))+y)>>2))*(x-g.screenW/2))/(g.screenW/2);
 }
 y+=g.originY; x+=g.originX-g.oy;
 ord10474(&x,&y); *px=newx; *py=newy;
}

// 1.13c D2Client+0x3F240 : C++ dtor (vtable reset + owned-object delete); host-side vtables.
struct Obj { void** vt; U pad; struct Child{void(**vt)(I);}* child; };
void sub_3F240(Obj* o,void** vtBase,void** vtDerived){ o->vt=vtDerived; if(o->child) (*o->child->vt[0])(1); o->vt=vtBase; }
// 1.13c D2Client+0x3F360
void sub_3F360(U* o,U vtDerived,U vtBase,void (*ord10065)(I),void (*ord10043)(I,I)){
 o[0]=vtDerived; I h=I(o[0x14]); ord10065(h); if(h) ord10043(0x7a,0); o[0]=vtBase;
}

struct H2 { // callees for the remaining rows
 P (*ord10061)(P); I (*ord10711)(P); I (*ord10048)(P); I (*ord10973)(P,I,I); I (*ord11139)(P,I); I (*ord10744)(P,I);
 P (*sub_AB17F0)(void); void (*ord10019)(U*); void (*sub_3D3C0)(P,I);
 void (*c3DCF0)(P,const int16_t*,I); void (*c3E0C0)(P,const int16_t*,I);
};
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

// 1.13c D2Client+0x3F740 ; r={x,y,w,h} from Ordinal_10561; per 5x5 cell draws a tile marker
void sub_3F740(const G& g,P unit,void (*ord10561)(P,I*),U (*ord10916)(P,I,I),void (*ord11087)(I*,I*),
 void (*ord10070)(I*,I),void (*ord10050)(I,I,const void*,U),const void* dat57c){
 I r[4]; ord10561(unit,r);
 for(I y=r[1]+2;y<r[3]+5+r[1];y+=5) for(I x=r[0]+3;x<r[2]+5+r[0];x+=5){
  U c=ord10916(unit,x,y); I pt[2]={x,y}, sc[4]; ord11087(pt,sc); ord10070(sc,0x46);
  ord10050(sc[0]-7-g.originX,I(intptr_t(unit))-g.originY,dat57c,c); } // ponytail: ord10050 y/x arg order per decompile, untraced
}

// 1.13c D2Client+0x40200 ; walk-path jitter generator over a unit's path array (LCG 0x6ac690c5)
struct Hw { I (*o10534)(I*); void (*o10558)(I*,I); void (*o10970)(I*,I); void (*o10614)(void); void (*o10647)(I,I);
 void (*o10168)(I,I); void (*o10817)(I); void (*o10867)(I); I (*o10750)(I); I (*o10764)(void); I (*o10983)(I,I);
 U (*o10192)(I*,I); U (*o10266)(U); void (*o10165)(I,I); I* dirY; I* dirX; I seedBase; I stepMod; };
void sub_40200(I* u,const Hw& h,int16_t* buf){
 if(!u) return;
 I n=h.o10534(u); if(n>0x4d){ h.o10558(u,0x4d); h.o10970(u,0x4d); n=0x4d; }
 I hdl=u[0xb]; uint64_t* st=(uint64_t*)(u+8);
 h.o10614(); h.o10647(hdl,10); h.o10168(hdl,n); h.o10817(hdl);
 I t=*u; if(t!=2&&(t<4||t>5)&&u[0xb]) h.o10867(u[0xb]);
 t=*u; I y; if(t==2||(t>3&&t<6)) y=*(I*)(intptr_t(u[0xb])+0x10); else y=u[0xb]?h.o10750(u[0xb]):0;
 I cnt=u[0xb]; I d=h.o10983(u[0xb],h.o10764()); U dir=h.o10192(u,d)&0xff;
 uint64_t v=(uint64_t)(U)*st*0x6ac690c5ULL+(U)u[9]; *(U*)st=(U)v;
 I sgn=((v&1)?1:0)*2-1; v=(v&0xffffffffULL)*0x6ac690c5ULL+(v>>32); *st=v;
 I mag=I((U)(v&0xffffffff)%3)+h.seedBase; I x=0; (void)x;
 for(I i=0;i<cnt;++i){
  dir+=U(mag*sgn); U k=h.o10266(dir&0xffffff3fu)&0xff;
  x+=h.dirX[k]; y+=h.dirY[k]; buf[i*2]=int16_t(x); buf[i*2+1]=int16_t(y);
  if(i%h.stepMod==0){ sgn=-sgn; v=(uint64_t)(U)*st*0x6ac690c5ULL+(U)u[9]; *st=v; mag=I((U)(v&0xffffffff)%3)+h.seedBase; }
 }
 h.o10165(10,cnt);
}
// ponytail: x seed is the stack residue of puStack_44 (unaff); starts 0 here -- trace needed to vector.

// (NOT MAPPED) D2Client 3FC20 stub; draws path-node markers for entries with state 0x4000 and, for the local player, the room-rect frame
// Body depends on unrecovered caller registers (EBP/ESI/EDI) -- shape only; entry loop transcribed:
struct Node { I pad[3]; I x,y,flags,cellId,kind,dat; I rest[6]; }; // stride 0x30; fields at +0xc.. per decompile
void sub_3FC20(const G& g,I count,Node* nodes,I offX,I offY,void (*o11087)(I*,I*),U (*o10051)(I),
 void (*mark)(I,U),void (*o10050)(I,I,const void*,U),const void* dat57c,I (*cellOf)(I)){
 for(I i=0;i<count;++i){ Node& n=nodes[i]; if(!n.y&&!n.flags) {} }
 (void)g;(void)offX;(void)offY;(void)o11087;(void)o10051;(void)mark;(void)o10050;(void)dat57c;(void)cellOf;
}
} // namespace
