// Implementations derived from 1.13c D2Client disassembly (objdump). Register/caller-frame inputs are explicit
// parameters; ordinal callees and screen globals are function-pointer / value seams. No game data in this file.
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace d2client_n126 {
using U=uint32_t; using I=int32_t; using P=void*; using W=uint16_t;
static U rd(const void*p,U o){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4);return v;}

// Panel row table at D2Client+0xE1210: 12 rows of {x0,x1,y,thresh,statId}.
struct Fn8EA20{
 I (*setFont)(I);                       // D2Win#10184
 I (*stat)(P,I,I);                      // D2Common#10973 GetStat(unit,id,layer)
 I (*base)(P,I,I);                      // D2Common#10587 base stat
 I (*c196)(P);                          // D2Common#10913
 I (*listFind)(U);                      // D2Client+0x20580 (list at g_6fbcc4d4, node+8==key -> node+0x1c)
 bool (*isPlayerLike)(P);               // D2Client+0x55440
 P (*invItem)(P,I);                     // D2Common#11139 (inv, bodyloc)
 I (*q1)(U thunk,P a);                  // single-arg D2Common queries, thunk = import-thunk RVA
 I (*c922)(P,P,I,I);                    // D2Common#10804
 I (*c454)(P,I,I);                      // D2Common#10910
 I (*diffVal)(U diff);                  // *(int*)D2Common#10237(diff)
 void (*numToWide)(W*,I,I);             // D2Lang#10002
 void (*toWide)(W*,const char*,I);      // D2Lang toUnicode
 void (*textSize)(const W*,I*,I*);      // D2Win#10177
 I (*textWidth)(const W*);              // D2Win#10028
 void (*draw)(const W*,I,I,I,I);        // D2Win#10150
};
struct G8EA20{ I xoff,yoff; U diff; I expansion; };
static I damageText(P u,I cur,const Fn8EA20&f,int&col,I&hi){
 I a19=f.stat(u,0x19,0),a12=f.stat(u,0x12,0),a11=f.stat(u,0x11,0);
 I edi=a19,ebp=0,v16;
 if(a12+a19>0)col=3; else if(a12+a19<0)col=1;
 P inv=reinterpret_cast<P>(static_cast<uintptr_t>(rd(u,0x60)));
 P w=inv?f.invItem(inv,4):nullptr;
 v16=f.stat(u,0x16,0);
 if(w){
  if(f.q1(0xC87A,w)){ cur=cur-f.stat(w,0x15,0)+f.stat(u,0x17,0); v16=v16-f.stat(w,0x16,0)+f.stat(u,0x18,0);}
  I p=int16_t(f.q1(0xC940,w)); if(p) edi+=f.stat(u,0,0)*p/100;
  p=int16_t(f.q1(0xC8CE,w)); if(p) edi+=f.stat(u,2,0)*p/100;
  edi+=f.c922(u,w,0,1);
  ebp=f.c454(w,0x6f,0);
 } else edi+=f.stat(u,rd(u,4)==0x10f?2:0,0);
 if(edi<-90)edi=-90;
 I lo=(a12+edi+100)*cur/100+ebp, h=(a11+edi+100)*v16/100+ebp;
 static const I tr[4][3]={{0x30,0x31,1},{0x32,0x33,9},{0x34,0x35,3},{0x36,0x37,3}};
 for(auto&t:tr){ I a=f.stat(u,t[0],0),b=f.stat(u,t[1],0); if(a||b)col=t[2]; lo+=a; h+=b; }
 I a=f.stat(u,0x39,0),b=f.stat(u,0x3a,0);
 if(a||b){
  I wv=f.stat(u,0x65,0);
  if(wv<=0){ I d=f.stat(u,0x146,0); if(d<1)d=1; wv=f.stat(u,0x3b,0)/d; }
  lo+=(wv*a)>>8; h+=(wv*b)>>8; col=2;
 }
 hi=h; return lo;
}
// 1.13c D2CLIENT+0x8EA20
void sub_8EA20(P unit,const I rows[12][5],const G8EA20&g,const Fn8EA20&f){
 if(!unit) return;
 I oldFont=f.setFont(0);
 for(int i=0;i<12;++i){
  W wb[128]; std::memset(wb,0,sizeof wb); char nb[128];
  const I*r=rows[i]; I id=r[4];
  I cur=f.stat(unit,id,0), bs=f.base(unit,id,0);
  int col=cur>bs?3:(cur<bs?1:0);
  I x0=r[0]+g.xoff, x1=r[1]+g.xoff, y=r[2]-g.yoff-2;
  if(id==6){
   I edi=f.c196(unit)>>8;
   I esi=f.listFind(rd(unit,0xC))*edi/100;
   if(!f.isPlayerLike(unit)&&esi<0)esi=1;
   std::snprintf(nb,0x80,"%d / %d",esi,edi); f.toWide(wb,nb,0x80);
   if((f.base(unit,7,0)>>8)<edi)col=3;
  } else if(id==0x15){
   I hi; I lo=damageText(unit,cur,f,col,hi);
   std::snprintf(nb,0x80,"%d-%d",lo,hi); f.toWide(wb,nb,0x80);
  } else if(id==0x1f){
   I v=f.q1(0xC2A4,unit); if(f.q1(0xC280,unit))col=3;
   f.numToWide(wb,v,0x80);
  } else if(id==0x27||id==0x29||id==0x2b||id==0x2d){
   I esi=cur, ex=0; I kk=id-0x27;
   static const U up[4]={0xC2B0,0xC268,0xC256,0xC29E}, dn[4]={0xC298,0xC238,0xC27A,0xC25C};
   static const I sid[4]={0x28,0x2a,0x2c,0x2e};
   int s=kk/2;
   if(f.q1(up[s],unit))col=3;
   if(f.q1(dn[s],unit))col=1;
   ex=f.stat(unit,sid[s],0);
   if(g.expansion) esi+=f.diffVal(g.diff);
   else if(g.diff==1)esi-=0x14; else if(g.diff==2)esi-=0x32;
   I cap=ex+0x4b; if(cap>=0x5f)cap=0x5f;
   I val=esi<-100?-100:esi;
   if(val>=cap){col=4;f.numToWide(wb,cap,0x80);}
   else { if(val<0)col=1; f.numToWide(wb,val,0x80);}
  } else f.numToWide(wb,cur,0x80);
  f.setFont(0);
  I w=0,h=0; f.textSize(wb,&w,&h);
  if(r[3]&&w>r[3]){ f.setFont(6); ++y; }
  I x;
  if(r[0]==0) x=x1-f.textWidth(wb);
  else if(r[1]==0) x=x0;
  else { I span=x1-x0+1, tw=f.textWidth(wb); x=tw<span?x0+((span-tw)>>1):x0; }
  f.draw(wb,x,y,col,0);
 }
 f.setFont(oldFont);
}

// ---- sub_93450: item description / tooltip builder (stdcall, two stack args) -------------------------------
static void wcpy(W*d,const W*s){while((*d++=*s++)!=0){}}
static void wcat(W*d,const W*s){while(*d)++d;wcpy(d,s);}
static U wlen(const W*s){U n=0;while(s[n])++n;return n;}
// Screen/UI globals read by the function (D2Client data VMAs in the trailing comments).
struct G93450{
 U hideAll;          // 6fbcc474
 U invOpen;          // 6fbaae10
 I mx,my;            // 6fbcb828, 6fbcb824
 I sw,sh;            // 6fb8bc48, 6fb8bc4c
 I xoff,yoff;        // 6fbcb9a0, 6fbcb9a4
 U panel[6];         // 6fbaad84 6fbaaddc 6fbaade4 6fbaadb0 6fbaadb8 6fbaade8
 U g2c,g28;          // 6fbcbc2c, 6fbcbc28
 P item;             // 6fbcbc38
 U mode;             // 6fbcbc34
 P override_;        // 6fbcbbfc
 U g97a8,g90ea4;     // 6fbc97a8, 6fb90ea4
 I a8,ac,b0,b4;      // 6fb90ea8 6fb90eac 6fb90eb0 6fb90eb4
 U g864;             // 6fbcb864
 U expansion;        // 6fbc9854
};
struct Fn93450{
 const W* (*str)(U id);                          // D2Lang#10003
 void (*draw)(const W*,I x,I y,I a,I b);        // D2Win#10085
 void (*textSize)(const W*,I*w,I*h);            // D2Win#10177
 I (*isExpansion)();                             // Fog BF8C
 I (*s5980)();                                   // D2Client+0x45980 (hit test)
 I (*s3c150)(I my,I mx);                         // D2Client+0x3c150
 I (*s8e50)(I flag,I key);                       // D2Client+0x38e50 -> ax
 const W* (*s9f30)(I key,I flag);                // D2Client+0x39f30
 I (*c202)(P);                                   // D2Common#11017
 I (*c8b6)(P,P);                                 // D2Common#10391
 I (*c19c)(P);                                   // D2Common#10106 quality
 I (*c856)(P);                                   // D2Common#10661
 I (*flag)(P item,U mask,U line);                // D2Common#10202 ITEMS_CheckFlag
 I (*c220)(P);                                   // D2Common#11088 item type
 I (*c9a0)(P);                                   // D2Common#10822
 P (*c1ba)(I classId);                           // D2Common#10695 -> ItemsTxt*
 void (*fatal)(U line);                          // Fog log + exit(-1)
 I (*c928)(P);                                   // D2Common#11144
 void (*c898)(P item,P pl,I z,I*a,I*b,I*c);      // D2Common#10244
 I (*c8aa)(P,P);                                 // D2Common#11015
 I (*c1cc)(P,I);                                 // D2Common#10744
 I (*c90a)(P);                                   // D2Common#10757
 I (*c454)(P,I,I);                               // D2Common#10910
 I (*c84a)(P,I);                                 // D2Common#10823
 I (*c8ec)(P,I);                                 // D2Common#10925
 I (*c88c)(P);                                   // D2Common#10089
 I (*c93a)(P);                                   // D2Common#10935
 I (*scale)(I a,I b,I d);                        // D2Client+0x14a0
 // local helpers; register inputs are explicit parameters
 W* (*s6f5f0)(W*buf,I col);                      // D2Client+0x16f5f0 (edi=buf, stack col)
 void (*s414f0)(P item,W*buf,I n);               // +0x414f0
 void (*s41360)(W*dst,const W*src);              // +0x41360 (esi=dst, stack src)
 void (*s3dee0)(W*buf,P item,P txt);             // +0x3dee0
 void (*s40770)(W*buf,P item);                   // +0x40770
 void (*s3de40)(W*buf,I n);                      // +0x3de40
 void (*s433e0)(W*buf);                          // +0x433e0 (eax=buf)
 void (*s3d4f0)(P item,W*buf);                   // +0x3d4f0 (eax=item)
 void (*s3c1e0)(W*buf);                          // +0x3c1e0 (esi=buf)
 void (*s3e180)(W*buf,P item);                   // +0x3e180
 void (*s3dcc0)(W*buf,P txt,I v);                // +0x3dcc0 (edi=buf)
 void (*s3dd80)(W*buf,P txt,I v);                // +0x3dd80 (edi=buf)
 void (*s3ff20)(P item,W*buf);                   // +0x3ff20
 void (*s3f9e0)(P item,W*buf);                   // +0x3f9e0 (eax=item)
 void (*s3d940)(W*buf,P item,P txt);             // +0x3d940 (edi=buf)
 void (*s3fdc0)(P item,W*buf,P txt);             // +0x3fdc0 (eax=item)
 void (*s3fb80)(P pl,P item,W*buf);              // +0x3fb80
 void (*s3d880)(W*buf,P item);                   // +0x3d880 (esi=buf)
 void (*s060b0)(W*buf,P item,I a,I b);           // +0x060b0 (edi=buf)
 void (*s3d600)(P txt,W*buf,P item);             // +0x3d600 (eax=txt, esi=buf)
 void (*s428c0)(P pl);                           // +0x428c0
 void (*s42650)();                               // +0x42650
 void (*s42440)();                               // +0x42440
};
static P ptr(U v){return reinterpret_cast<P>(static_cast<uintptr_t>(v));}
// 1.13c D2CLIENT+0x93450
void sub_93450(P arg1,I arg2,const G93450&g,const Fn93450&f){
 static W b60[0x400],ba60[0x400],b860[0x100],b1460[0x100],b2060[0x100],b1260[0x100],b3e60[0x400],b3060[0x400],
  b4660[0x1000],b3860[0x200],b2260[0x100],b1e60[0x100],b2660[0x100],b2a60[0x100],b2e60[0x100],b2860[0x100],
  b2460[0x100],b2c60[0x100],b1660[0x100],b1c60[0x100],b1860[0x200];
 W*const all[]={b60,ba60,b860,b1460,b2060,b1260,b3e60,b3060,b4660,b3860,b2260,b1e60,b2660,b2a60,b2e60,b2860,b2460,b2c60,b1660,b1c60,b1860};
 const U sz[]={0x400,0x400,0x100,0x100,0x100,0x100,0x400,0x400,0x1000,0x200,0x100,0x100,0x100,0x100,0x100,0x100,0x100,0x100,0x100,0x100,0x200};
 for(int i=0;i<21;++i) std::memset(all[i],0,sz[i]*2);
 I F20=0,F24=0,F28=0,F2c=0,F34=0,F48=0; F48=g.item?I(int8_t(rd(g.item,4))):-1; // decompile: (char)item[+4], -1 if no item
 const W*F18=f.str(0xf9e);
 P pl=arg1; if(arg2&&g.override_)pl=g.override_;
 P item=g.item;
 auto cat=[&](W*d,const W*s){wcat(d,s);};
 if(!g.hideAll){
  I mx=g.mx,my=g.my,sh=g.sh;
  if(g.invOpen){
   I e=g.xoff+0x110, edi=g.yoff+sh-0x3f;
   if(mx>=e&&mx<=e+0x20&&my>=edi-0x20&&my<=edi) f.draw(f.str(0x1030),g.xoff+0x11f,g.yoff+sh-0x62,0,1);
  }
  bool any=false; for(U p:g.panel) if(p) any=true;
  if(any){
   I e=g.sw-g.xoff, lo=e-0x12e;
   if(mx>=lo&&mx<=lo+0x20&&my>=g.yoff+sh-0x60&&my<=g.yoff+sh-0x40) f.draw(f.str(0x1030),e-0x11f,g.yoff+sh-0x63,0,1);
   I ebp=g.yoff+sh-0x45, c=e-0xed;
   if(mx>=c&&mx<=c+0x14&&my>=ebp-0x12&&my<=ebp&&!f.s5980()){
    static const uint8_t m[15]={0,3,3,3,3,3,3,3,3,3,1,2,2,3,2};
    static const U ids[4]={0x1003,0x1021,0x101e,0x101d};
    U idx=g.mode-1, id=0x101d; if(idx<=0xe) id=ids[m[idx]];
    f.draw(f.str(id),e-0xde,g.yoff+sh-0x59,0,1);
   }
   if(f.isExpansion()&&g.expansion&&f.s3c150(my,mx)){
    wcpy(b1860,f.str(0x58c6));
    if((int16_t)f.s8e50(1,0x2c)!=-1){ cat(b1860,f.str(0xf9d)); cat(b1860,f.s9f30(0x2c,1)); }
    else if((int16_t)f.s8e50(0,0x2c)!=-1){ cat(b1860,f.str(0xf9d)); cat(b1860,f.s9f30(0x2c,0)); }
    I x=g.xoff+(mx>g.xoff+0x1f4?0x258:0x170);
    f.draw(b1860,x,0x15-g.yoff,0,1);
   }
  }
 }
 // 3a27
 if(!(g.g2c||g.g28)||!item) return;
 P inv=ptr(rd(arg1,0x60)); if(!inv) return;
 if(f.c202(inv)) return;
 if(!g.panel[1]&&inv&&!f.c8b6(inv,item)) return;
 I w=0,h=0;
 auto fin=[&](I a){ // 3bc2: position and draw the assembled text in ba60
  if(g.ac-h>0) f.draw(ba60,g.a8,g.ac,a,1);
  else f.draw(ba60,g.b0,h+g.b4,a,1);
 };
 if(g.g28&&!g.g90ea4&&g.g97a8&&g.mode>=1&&g.mode<=9){
  f.s414f0(item,b3060,0x80);
  wcpy(b860,f.str(0xd7f)); cat(b860,F18); cat(b60,b860);
  I t=f.c220(item)-0x3c;
  if(t>=0&&t<=0x1c){
   static const uint8_t m[29]={0,1,2,3,4,5,6,5,4,2,3,1,6,6,7,7,7,7,7,7,7,7,7,7,7,0,0,0,5};
   static const U id[7]={0x2aa5,0x2aa9,0x2aa7,0x2aa8,0x2aa6,0x2aab,0x2aaa};
   if(m[t]!=7){ cat(b60,f.str(id[m[t]])); cat(b60,F18); }
  }
  cat(b60,b3060);
  f.s6f5f0(b60,0);
  f.s41360(ba60,b60);
  f.textSize(ba60,&w,&h);
  fin(0); return;
 }
 I q=f.c19c(item);
 if(g.g864==8||f.c856(item)){ f.s42440(); return; }
 if(q==5&&f.flag(item,0x10,0xf9f)){ f.s428c0(arg1); return; }
 if(f.c220(item)==0x12){ f.s42650(); return; }
 I k=f.c9a0(item);
 if(k!=7){
  wcpy(b1460,f.str(k+0x2aa5)); cat(b1460,F18);
  I c=pl?I(rd(pl,4)):-1; if(c!=k)F2c=1;
 }
 P txt=f.c1ba(I(rd(item,4)));
 if(!txt){ f.fatal(0xfc2); return; }
 if(f.c928(item)) f.s3dee0(b60,item,txt); else f.s40770(b2660,item);
 I o3c=0,o30=0,o44=0;
 f.c898(item,pl,0,&o3c,&o30,&o44);
 if(f.flag(item,0x10,0xfd6)){
  I n=f.c8aa(item,pl);
  if(n>1){ if(!o44)F28=1; f.s3de40(b2a60,n); }
 }
 if(f.c1cc(item,0x35)) f.s433e0(b3860);
 if(f.c90a(item)) f.s3d4f0(item,b1e60);
 if(f.c1cc(item,0xd)) f.s3c1e0(b2260);
 if(f.c1cc(item,0x2d)||f.c1cc(item,0x32)){
  f.s3e180(b2060,item);
  I ebp=0,edi=0;
  I s5b=f.c454(item,0x5b,0);
  U t10a=rd(txt,0x10a)&0xffff, t10c=rd(txt,0x10c)&0xffff;
  if(s5b){ ebp=f.scale(s5b,I(t10a),100); edi=f.scale(s5b,I(t10c),100); }
  if(f.flag(item,0x400000,0xffa)){ ebp-=10; edi-=10; }
  if(t10c){ if(!o30)F24=1; f.s3dcc0(b2860,txt,edi); }
  if(t10a){ if(!o3c)F20=1; f.s3dd80(b2e60,txt,ebp); }
  if(f.c1cc(item,0x2d)){
   if(f.c84a(item,0)>=0&&f.c8ec(item,0)>=0) f.s3ff20(item,b2460);
   f.s3f9e0(item,b3e60);
  }
  if(f.c1cc(item,0x33)){
   if(pl&&rd(pl,4)==3&&(!f.c88c(item)||f.c9a0(item)==3)) f.s3d940(b1660,item,txt);
   f.s3fdc0(item,b2c60,txt);
  } else if(f.c1cc(item,0xf)&&pl&&rd(pl,4)==6) f.s3d940(b1660,item,txt);
  if(f.c1cc(item,0x32)&&f.c93a(item)>0) f.s3fb80(pl,item,b1c60);
 }
 I col;
 switch(q){
  case 4: col=3; break; case 5: col=2; break; case 6: col=9; break;
  case 7: col=4; break; case 8: col=8; break; case 9: col=10; break;
  default:
   if(f.flag(item,0x800,0x1050)) col=5;
   else if(f.flag(item,0x400000,0x1054)) col=5;
   else col=F48;
 }
 if(!f.flag(item,0x10,0x105d)&&g.mode>=1&&g.mode<=9&&!g.g90ea4) col=0;
 if(f.flag(item,0x10,0x1069)){
  if(!f.flag(item,0x800,0x106c)) f.s3d880(b1260,item);
  f.s060b0(b4660,item,1,0);
 } else { wcpy(b860,f.str(0xd7f)); cat(b860,F18); }
 f.s3d600(txt,b1260,item);
 f.s414f0(item,b3060,0x100);
 wcpy(b60,f.s6f5f0(b2060,3));
 U code=rd(txt,0x80);
 static const U codes[]={0x20686563,0x20746562,0x20736574,0x20646566,0x20616f74,0x206e6864,0x20796562,0x2072626d,0x20316b70,0x20326b70,0x20336b70};
 bool hit=false; for(U c:codes) if(code==c) hit=true;
 if(hit||f.c1cc(item,0x4a)) col=8;
 cat(b60,f.s6f5f0(b4660,3));
 cat(b60,f.s6f5f0(b860,1));
 cat(b60,f.s6f5f0(b3e60,0));
 cat(b60,f.s6f5f0(b2a60,F28));
 cat(b60,f.s6f5f0(b2e60,F20));
 cat(b60,f.s6f5f0(b2860,F24));
 cat(b60,f.s6f5f0(b1460,F2c));
 W*const z[]={b2660,b3860,b2260,b1260,b2460,b1660,b2c60,b1c60};
 for(W*b:z) cat(b60,f.s6f5f0(b,0));
 cat(b60,f.s6f5f0(b1e60,4));
 if(f.flag(item,0x100,0x10a7)) col=1;
 cat(b60,f.s6f5f0(b3060,col));
 if(g.mode>=1&&g.mode<=9) f.s41360(ba60,b60); else wcpy(ba60,b60);
 I esi=F34;
 bool plain=!(reinterpret_cast<const uint8_t*>(txt)[0x12a])||code==0x2067656c;
 if(!plain){
  bool lab=false;
  if(g.mode==0&&(!item||rd(item,0x10)==0)){
   U id=0;
   if(code==0x20646b62)id=0x89d; else if(code==0x20786f62)id=0x89c;
   if(id){ wcpy(b60,ba60); wcpy(ba60,f.str(id)); cat(ba60,F18); cat(ba60,b60); lab=true; }
  }
  (void)lab;
  esi=4; f.s6f5f0(ba60,4);
 }
 if(wlen(ba60)>=0x3ff) ba60[0x3ff]=0;
 f.textSize(ba60,&w,&h);
 fin(esi);
}
}
