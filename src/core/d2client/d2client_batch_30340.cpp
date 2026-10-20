// Prior art used: UI/input entries; D2MOO has no D2Client.
// Derived from cached 1.13c code, with register/stack arguments checked in x86.
// The skill-description damage-range builders (min/max, mode word, then sink 2F0A0).
// Callees stay behind Ops; Ops names follow the Ordinal numbers / RVAs in the decompile.
// sub_316C0 (5107 bytes) is deliberately not here.
#include <cstdint>
#include <cstring>
namespace d2client_batch_30340 {
using U=uint32_t; using I=int32_t; using P=void*; using C16=char16_t;
struct Ops {
 I (*stat)(P,I,I);                      // Ordinal_10973(unit,stat,layer)
 I (*dmgMin)(P,I,I,I); I (*dmgMax)(P,I,I,I); // Ordinal_10567 / 10297 (unit,skill,lvl,flag)
 I (*skMin)(P,I,I,I);  I (*skMax)(P,I,I,I);  // Ordinal_10121 / 11091
 I (*bonusTotal)();                     // D2Client+0x2FB50
 I (*rec2CD0)(I,I); I (*rec15E0)(I,I);  // eax=skill, edx=level
 void (*calc10323)(P,I*,I*,I*);
 void (*bonus2F980)(P,I*,I*,I*);
 void (*emit)(I eax,I ecx,I s0,I s1,I s2,I s3); // D2Client+0x2F0A0
 void (*fade190)(P rec,P unit,I*,I*,I*);        // D2Client+0x2E190
 I (*o10037)(P,I);
 I (*o10854)(P,P*); const uint8_t* (*o10695)(I);
 P (*o10708)(P,I,I); I (*hasSkill)(P,I); /*10494*/ I (*o10306)(P,P,I);
 int16_t (*o10213)(P); int16_t (*o10120)(P);
 const uint8_t* (*o17F0)(I);            // skill-desc record, +0x18/+0x1c/+0x20.. fields
 I (*o2C830)(uint8_t); I (*o10601)(I,const char*,I,I); I (*o10435)(P,I,I);
 P (*o10061)(P); I (*o10711)(P); I (*o10804)(P,P,I,I);
 void (*o2D6E0)(uint8_t,I*,I*);
 void (*o2D500)(P eax,I,I,I,I,I,I,I,I,I,I);     // ten stack args, first-listed = first param
 void (*o2E140)(I eax,I ecx,C16* buf,I s);
 void (*o2D3C0)(C16*,C16*,I,I,I,I,I);
 I (*o10747)(P); void (*o2C4F0)(P sk,I*,I*,I,I);
 I (*o10263)(P); I (*o10975)(P);
 I (*o10051)(P,P); I (*o10597)(P,P,I); I (*o10910)(P,I,I);
 I (*o10744)(P,I); I (*o11006)(P); I (*o11160)(P,I);
 void (*o10164)(P,P,I); P (*o11139)(P,I); I (*o10417)(P); I (*o10786)(P,I,I,I);
 I (*o2D810)(P eax,I edi);
 I (*o2F7D0)(I ebx,I,I,I,I); I (*o2F870)(I ebx,I,I,I,I,I); I (*o2DCA0)(I ebx,I,I,I);
 P (*owner)(P unit);                    // unit+0x60
};
struct Tables { I colors[4]; I rows[4][3]; uint8_t mask0c; bool modeFlag9854; };
static U r8(const void* p,U n){return static_cast<const uint8_t*>(p)[n];}
static I r16s(const void* p,U n){int16_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,2);return v;}
static I mulDiv(I pct,I v,I d){ // D2Client+0x14A0 (eax=pct, edx=v, ecx=d)
 if(!d)return 0;
 if(v<=0x100000){ if(pct<=0x10000)return (v*pct)/d; if(d<=(pct>>4))return (pct/d)*v; }
 else if(d<=(v>>4))return (v/d)*pct;
 return I((int64_t(v)*pct)/d);
}
// inline pct term of 30E30: v*pct/100 with the same three-way shape
static I pctOf(I v,I pct){
 if(v<=0x100000){ if(pct<=0x10000)return (pct*v)/100; if((pct&~0xf)<0x640)return I(int64_t(pct)*v/100); return (pct/100)*v; }
 if((v&~0xf)<0x640)return I(int64_t(pct)*v/100); return (v/100)*pct;
}
// 1.13c D2Client+0x30340
void sub_30340(const Ops& o,P unit,I flag,const int16_t* sk,I lvl,I a2,I a3,I a4){
 if(!flag||!sk)return; I skill=*sk, mode=1;
 I A=o.dmgMin(unit,skill,lvl,1)>>8, B=o.dmgMax(unit,skill,lvl,1)>>8;
 I bonus=o.bonusTotal()+o.rec2CD0(skill,lvl); I pct=bonus; if(bonus)mode=3;
 I x=0,y=0; o.calc10323(unit,&x,&y,&pct);
 x=x+A+mulDiv(pct,x,100)+mulDiv(bonus,A,100);
 y=y+B+mulDiv(pct,y,100)+mulDiv(bonus,B,100);
 I pmin=0,pmax=0; o.bonus2F980(unit,&pmin,&pmax,&mode);
 o.emit(y+pmax,x+pmin,mode,a2,a3,a4);
}
// 1.13c D2Client+0x304A0
void sub_304A0(const Ops& o,const Tables& t,P unit,I flag,const int16_t* sk,I a1,I a2,I a3,I a4){
 (void)a1; if(!flag||!sk)return; I mn=0,mx=0,mode=0;
 if(t.modeFlag9854){ o.fade190(P(sk),unit,&mn,&mx,&mode); I p=o.o10037(unit,*sk); mn+=(p*mn)/100; mx+=(p*mx)/100; }
 else{ o.fade190(P(sk),unit,&mn,&mode,&mx); // pointer order differs in this arm (L14 is max, L10 is mode)
  I p=o.stat(unit,0x19,0); if(p<=-0x5a)p=-0x5a; mn+=(p*mn)/100; mx+=(p*mx)/100; }
 o.emit(mx,mn,mode,a2,a3,a4);
}
// 1.13c D2Client+0x305D0
void sub_305D0(const Ops& o,P unit,I flag,const int16_t* sk,I lvl,I a2,I a3,I a4){
 if(!flag||!sk)return; P own=o.owner(unit); P hit=nullptr; if(!o.o10854(own,&hit))return;
 const uint8_t* rec=o.o10695(hit?I(*static_cast<const int32_t*>(static_cast<const void*>(static_cast<const uint8_t*>(hit)+4))):-1);
 I mn=I(r8(rec,0xfe)), mx=I(r8(rec,0xff));
 P s=o.o10708(unit,0x75,-1);
 if(s&&o.hasSkill(unit,0x65)){ I h=o.o10306(unit,s,1);
  mn+=o.dmgMin(unit,0x75,I(reinterpret_cast<intptr_t>(s)),1)>>8; (void)h;
  mx+=o.dmgMax(unit,0x75,I(reinterpret_cast<intptr_t>(s)),1)>>8; }
 I base=o.rec15E0(*sk,lvl); I mode=0;
 if(I p=o.o10213(P(sk)))base+=o.stat(unit,0,0)*p/100;
 if(I p=o.o10120(P(sk)))base+=o.stat(unit,2,0)*p/100;
 base+=o.stat(unit,0x19,0); I s12=o.stat(unit,0x12,0), s11=o.stat(unit,0x11,0);
 if(o.o10263(unit))mode=3; if(o.o10975(unit))mode=1;
 if(base<-0x5a)base=-0x5a;
 o.emit((base*mx+s11)/100+mx,(base*mn+s12)/100+mn,mode,a2,a3,a4);
}
// 1.13c D2Client+0x307B0
void sub_307B0(const Ops& o,P unit,I flag,const uint8_t* sk,I lvl,I a2,I a3,I a4){
 if(!flag||!sk)return; const uint8_t* rec=o.o17F0(r16s(sk,0)); if(!rec)return;
 I A=o.skMin(unit,r16s(sk,0),lvl,1), B=o.skMax(unit,r16s(sk,0),lvl,1);
 I mode=o.o2C830(uint8_t(r8(sk,0x1dc)));
 const char* f="..\\Source\\D2Client\\UI\\SkillDesc.cpp";
 I v1=o.o10601(flag,f,0xc2a,lvl); I d1=o.o10435(unit,I(*reinterpret_cast<const int32_t*>(rec+0x18)),v1); if(!d1)d1=1;
 I v2=o.o10601(flag,f,0xc2f,lvl); I d2=o.o10435(unit,I(*reinterpret_cast<const int32_t*>(rec+0x1c)),v2); if(!d2)d2=1;
 o.emit(((d1*B*0x19)/d2)>>8,((d1*A*0x19)/d2)>>8,mode,a2,a3,a4);
}
// 1.13c D2Client+0x308C0
void sub_308C0(const Ops& o,P unit,I flag,const uint8_t* rec,I a1,I a2,I a3,I a4){
 (void)a1; P own=unit?o.owner(unit):nullptr; if(!flag||!rec||!own)return;
 P sk=o.o10061(own); if(!sk||!o.o10711(sk))return;
 I edi=0;
 if(I p=o.o10213(sk))edi=(o.stat(unit,0,0)*p)/100;
 if(I p=o.o10120(sk))edi+=(o.stat(unit,2,0)*p)/100;
 I mn=o.stat(unit,0x9f,0), mx=o.stat(unit,0xa0,0); edi+=o.stat(unit,0x19,0);
 I s12=o.stat(unit,0x12,0), s11=o.stat(unit,0x11,0);
 if(edi<-0x5a)edi=-0x5a;
 mn+=((s12+edi)*mn)/100; mx+=((s11+edi)*mx)/100;
 I b=0; if(o.hasSkill(unit,0x4e))b=o.o10804(unit,sk,flag,1);
 mn+=mulDiv(b,mn,100); mx+=mulDiv(b,mx,100);
 o.o2D6E0(uint8_t(r8(rec,0x1dc)),&mx,&mn);
 if(mx<mn)mx=mn;
 I sc=I(r8(rec,0x1a5)); mn=mulDiv(sc,mn,0x80); mx=mulDiv(sc,mx,0x80);
 I mode=0; o.fade190(P(const_cast<uint8_t*>(rec)),unit,&mn,&mx,&mode);
 o.emit(mx,mn,mode,a2,a3,a4);
}
// 1.13c D2Client+0x30B40  (eax=rec, ecx=this/unit-ish; stack a0..a8)
void sub_30B40(const Ops& o,const uint8_t* rec,P self,P a0,I a1,I a2,I a3,I a4,I a5,I a6,I a7,I a8,I flag){
 (void)a5; (void)a1; if(!self||!rec||!a2)return;
 I mn=0,mx=0,mode=0; (void)mn;
 I L28=0,L2c=0,L18=0,L24=0;
 if(a8) o.fade190(P(const_cast<uint8_t*>(rec)),a0,&L28,&L2c,&L18);
 (void)flag; I sc=I(r8(rec,0x1a5));
 L24=L18; I L20=0,L10=0,L1c=0,L14=0;
 o.o2D500(a0,I(reinterpret_cast<intptr_t>(&L20)),I(reinterpret_cast<intptr_t>(&L10)),I(reinterpret_cast<intptr_t>(&L18)),
  I(reinterpret_cast<intptr_t>(&L1c)),I(reinterpret_cast<intptr_t>(&L14)),I(reinterpret_cast<intptr_t>(&L24)),
  I(reinterpret_cast<intptr_t>(self)),a2,a3,a4);
 auto sar7=[](I v){return (v+((v>>31)&0x7f))>>7;};
 if(L20&&L10){ L20=sar7(L20*sc)+L28; L10=sar7(L10*sc)+L2c; }
 if(L1c&&L14){ L1c=L28+sar7(L1c*sc); L14=sar7(L14*sc)+L2c; }
 if(!L20&&!L10){ o.emit(L14,L1c,L24,a6,a7,a8); return; }
 if(!L1c&&!L14){ o.emit(L10,L20,L18,a6,a7,a8); return; }
 C16 A[64]{},B[64]{};
 o.o2E140(L10,0,A,L20); o.o2E140(L14,0,B,L1c);
 o.o2D3C0(A,B,a6,a7,a8,L18,L24);
}
// 1.13c D2Client+0x30DB0
void sub_30DB0(const Ops& o,P unit,I flag,I a0,I* a1,I* a2,I a3,I a4,uint32_t cb){
 if(o.o10747(unit)){
  o.o2D500(unit,I(reinterpret_cast<intptr_t>(a1)),I(reinterpret_cast<intptr_t>(a2)),0,a3,a4,0,flag,I(cb),0,a0); return; }
 P own=o.owner(unit); P sk=own?o.o10061(own):nullptr;
 *a1=0; *a2=0; o.o2C4F0(sk,a1,a2,a0,flag);
}
// 1.13c D2Client+0x30E30
void sub_30E30(const Ops& o,const Tables& t,P unit,I* mn,I* mx,I* mode,P flag){
 for(int i=0;i<4;++i){
  I a=o.stat(unit,t.rows[i][0],0), b=o.stat(unit,t.rows[i][1],0);
  if(t.rows[i][2]!=-1){ if(I p=o.stat(unit,t.rows[i][2],0)){ a+=pctOf(a,p); b+=pctOf(b,p); } }
  if(b<=a)a=b;
  *mn+=a; *mx+=b; if(a||b)*mode=t.colors[i];
 }
 I lo=o.stat(unit,0x39,0), hi=o.stat(unit,0x3a,0);
 if(hi){
  if(I p=o.stat(unit,0x14c,0)){ lo+=pctOf(lo,p); hi+=pctOf(hi,p); }
  I k=o.stat(unit,0x65,0);
  if(k<1){ I d=o.stat(unit,0x146,0); if(d<2)d=1; k=o.stat(unit,0x3b,0)/d; }
  *mode=2; *mn+=(k*lo)>>8; *mx+=(k*hi)>>8;
 }
 if(*mx>0){ if(*mn<1)*mn=1; if(*mx<*mn+1)*mx=*mn+1; }
 if(flag){ const uint8_t* r=nullptr; (void)r; }
}
// Tail of 30E30 needs 10858(flag): kept separate so Ops stays small.
void sub_30E30_tail(const Ops& o,const Tables& t,P unit,I* mn,I* mx,I* mode,const uint8_t* flagRec,
 I (*f2980)(P,I*,I*,I*)){
 if(flagRec&&(t.mask0c&flagRec[4]))f2980(unit,mn,mx,mode);
 if(o.o10263(unit))*mode=3; if(o.o10975(unit))*mode=1;
}
// 1.13c D2Client+0x31220  (eax=base, ecx=unit; returns 1 on emit-ready, 0 otherwise)
I sub_31220(const Ops& o,const Tables& t,I base,P unit,I* mn,I* mx,I* mode,I a3,P flag,const uint8_t* flagRec,
 I scale,P sk,I useSkill){
 (void)flag; if(!flagRec)return 0; P own=o.owner(unit); if(!own)return 0;
 I lo,hi;
 if(!sk&&!(sk=o.o10061(own))){ lo=o.stat(unit,0x15,0)+1; hi=o.stat(unit,0x16,0)+2; }
 else if(useSkill){ lo=o.o10597(unit,sk,0x15); hi=o.o10597(unit,sk,0x16); }
 else if(o.o10051(unit,own)==2){ lo=o.stat(unit,0x17,0); hi=o.stat(unit,0x18,0); }
 else{ lo=o.stat(unit,0x15,0); hi=o.stat(unit,0x16,0); }
 if(!scale)scale=I(r8(flagRec,0x1a5));
 if(scale!=0x80){ lo=(lo*scale+(((lo*scale)>>31)&0x7f))>>7; hi=(hi*scale+(((hi*scale)>>31)&0x7f))>>7; }
 if(scale){ if(lo<1)lo=1; *mn+=lo; if(hi<2)hi=2; *mx+=hi; }
 base+=o.stat(unit,0x19,0); base+=o.o10804(unit,sk,I(reinterpret_cast<intptr_t>(flag)),1);
 I s12=o.stat(unit,0x12,0), s11=o.stat(unit,0x11,0); I edi=0;
 if(t.mask0c&flagRec[4]){ if(I f=o.bonusTotal()){ base+=f; *mode=3; } }
 if(sk){
  if(I p=o.o10213(sk))base+=o.stat(unit,0,0)*p/100;
  if(I p=o.o10120(sk))base+=o.stat(unit,2,0)*p/100;
  edi=o.o10910(sk,0x6f,0);
 } else base+=o.stat(unit,0,0);
 if(o.o10263(unit))*mode=3; if(o.o10975(unit))*mode=1;
 if(base<-0x5a)base=-0x5a;
 *mn=((s12+base+100)* *mn)/100+edi+a3;
 *mx=((s11+base+100)* *mx)/100+edi+a3;
 return 1;
}
// 1.13c D2Client+0x314D0  (eax=extra, ecx=this: shorts at +0x186/188/18a/18c)
I sub_314D0(const Ops& o,const uint8_t* self,P extra,I a0,I a1,I a2,I a3){
 if(!r16s(self,0x18a)&&!r16s(self,0x18c))return 0;
 if(extra)return o.o2D810(extra,a0);
 const uint8_t* rec=o.o17F0(a1); if(!rec)return 0;
 I cl=r16s(self,0x188)&0x1f, w=r16s(self,0x18c);
 I sh=w<<cl, e=((a2-1)*w+r16s(self,0x18a))<<cl; I fl=r16s(self,0x186)<<8; if(fl>e)e=fl;
 I rec16=r16s(rec,0x10)&0xffff;
 I hiBits=I(U(reinterpret_cast<uintptr_t>(rec))&0xffff0000u); I id=hiBits|rec16;
 if(r16s(self,0x188)!=8&&sh<=0x100&&e<=0x1900){
  if(a3<=10){ I rem=e%256; I b=(rem*10)/256; return o.o2F7D0(b,a0,id,e/256,0); }
  return o.o2F870(e,a0,id,0x1506,a3,0);
 }
 return o.o2DCA0(a0,id,e>>8,0);
}
// 1.13c D2Client+0x32BF0
void sub_32BF0(const Ops& o,const Tables& t,P unit,I flag,const uint8_t* sk,I lvl,I a2,I a3,I a4,
 I (*f30e30)(const Ops&,const Tables&,P,I*,I*,I*,I)){
 (void)f30e30; if(!flag||!sk)return; I skill=r16s(sk,0);
 I mode=o.o2C830(uint8_t(r8(sk,0x1dc)));
 I A=o.dmgMin(unit,skill,lvl,0)>>8, B=o.dmgMax(unit,skill,lvl,0)>>8;
 I bon=o.bonusTotal(), pct=bon, x=0,y=0; o.calc10323(unit,&x,&y,&pct);
 x=x+A+mulDiv(pct,x,100)+mulDiv(bon,A,100);
 y=y+B+mulDiv(pct,y,100)+mulDiv(bon,B,100);
 I p=o.o10786(unit,I(*reinterpret_cast<const int32_t*>(sk+0x138)),skill,lvl);
 x+=mulDiv(p,x,100); y+=mulDiv(p,y,100);
 if(P own=o.owner(unit)){
  P s=o.o10061(own); P w=nullptr;
  if(s) w=o.o11139(own,(o.o10417(s)==4)+4);
  auto ok=[&](P q){return q&&o.o10744(q,0x2d)&&o.o11006(q)&&o.o11160(q,0);};
  if(ok(s))o.o10164(unit,s,0); else s=nullptr;
  if(w&&w!=s&&ok(w))o.o10164(unit,w,0);
  sub_30E30(o,t,unit,&x,&y,&mode,reinterpret_cast<P>(intptr_t(flag)));
  if(s)o.o10164(unit,s,1);
 }
 o.emit(y,x,mode,a2,a3,a4);
}
}
