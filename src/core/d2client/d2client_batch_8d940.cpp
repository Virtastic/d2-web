// Implementation batch D2Client+0x8D940..0x8E770 (item-tooltip text builders and small HUD helpers).
// Decompile-derived; imported DLL calls (string table, D2Common item/stat queries, D2Gfx draws) go
// through Env. Register-passed args the decompiler lost are named explicitly in each signature.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <cstdio>
namespace d2client_batch_8d940 {
typedef uint16_t wc;
struct Env {
 virtual ~Env()=default;
 virtual const wc* str(int32_t slot)=0;                 // Ordinal_10003: localized string / colour code, call order per function
 virtual void fmt(char* out,int32_t n,int32_t v)=0;     // Ordinal_578 (_snprintf with DAT_6fb834c4 / 6fb835a0 "%d")
 virtual int32_t q(int32_t what,int32_t a,int32_t b)=0; // generic D2Common/D2Client query (what = ordinal)
 virtual int32_t g(int32_t what)=0;                     // D2Client global read (what = RVA of the global)
 virtual void gset(int32_t what,int32_t v)=0;
 virtual void draw(int32_t a,int32_t b,int32_t c,int32_t d,int32_t e,int32_t f)=0; // Ordinal_10041/10150/10184
 virtual bool personalize(wc* out,const wc* in,int32_t n)=0; // Unicode::Personalize(.., lang)
};
static void ucpy(wc* d,const wc* s){ while((*d++=*s++)); }
static void ucat(wc* d,const wc* s){ while(*d) ++d; ucpy(d,s); }
static void w2u(wc* d,const char* s,int32_t n){ while(n-- >0 && *s) *d++=(uint8_t)*s++; *d=0; }
// Builders 0x8D940..0x8DEE0 share one shape: fetch the two colour codes (D2Lang strings 0xf9b, 0xf9e), Unicode::strcpy
// a label string, Storm#578 _snprintf(10, "%i") the number, Unicode::win2Unicode(n, 9), Unicode::strcat colour/number/colour.
// e.str(id) is D2Lang#10003 with the real string id; e.q(10973, unit, stat) is D2Common#10973 (layer 0).
static void w2u9(wc* d,const char* s){ int i=0; for(;i<9&&s[i];i++) d[i]=(uint8_t)s[i]; if(s[i]) d[i-1]=0; else d[i]=0; }
static void num(char* b,const char* f,int32_t v,wc* n){ (void)f; std::snprintf(b,10,"%d",(int)v); w2u9(n,b); } // "%i" / "%ld" print alike
static void labelNum(Env& e,wc* out,int32_t label,int32_t v,const wc* c0,const wc* c1){
 char b[12]; wc n[10]; ucpy(out,e.str(label)); num(b,"%i",v,n); ucat(out,c0); ucat(out,n); ucat(out,c1);
}
static int32_t rd16(const void* p,unsigned o){ uint16_t v; std::memcpy(&v,(const uint8_t*)p+o,2); return v; }

// 1.13c D2Client+0x8D940  (EDI = out; stdcall unit, data; ret 8). "<label> <c0>lo<c0><0xd88><c0>hi<c1>":
// label 0xd8c when D2Common#10744(unit,0x33), else 0x5516 when #10744(unit,0xf), else nothing is written.
// lo/hi start at data[0xfe]/data[0xff]; for 0x33 the player's (DAT_6fbcbbfc) #10630(player,0x75) skill, if present and
// #10494(player,0x65), adds #10567(player,0x75,t,1)>>8 and #10297(player,0x75,t,1)>>8 with t = #10306(player,skill,1).
struct Imp8d940 { int32_t(*o10630)(int32_t,int32_t); int32_t(*o10306)(int32_t,int32_t,int32_t);
 int32_t(*o10567)(int32_t,int32_t,int32_t,int32_t); int32_t(*o10297)(int32_t,int32_t,int32_t,int32_t); };
void tooltipRequirement(Env& e,const Imp8d940& f,int32_t player,int32_t unit,const uint8_t* data,wc* out){
 const wc* c0=e.str(0xf9b); const wc* c1=e.str(0xf9e);
 int32_t lo=data[0xfe], hi=data[0xff], label;
 if(e.q(10744,unit,0x33)){
  label=0xd8c;
  int32_t sk=f.o10630(player,0x75);
  if(sk && e.q(10494,player,0x65)){
   int32_t t=f.o10306(player,sk,1);
   lo+=f.o10567(player,0x75,t,1)>>8; hi+=f.o10297(player,0x75,t,1)>>8;
  }
 } else if(e.q(10744,unit,0xf)) label=0x5516; else return;
 char b[12]; wc n[10];
 ucpy(out,e.str(label)); num(b,"%i",lo,n); ucat(out,c0); ucat(out,n); ucat(out,c0);
 ucat(out,e.str(0xd88)); num(b,"%i",hi,n); ucat(out,c0); ucat(out,n); ucat(out,c1);
}

// 1.13c D2Client+0x14A0  ( EAX = a, EDX = b, ECX = c): a*b/c with the D2 overflow guards.
static int32_t ratio(int32_t a,int32_t b,int32_t c){
 if(!c) return 0;
 if(b>0x100000){ if(c<=(b>>4)) return (b/c)*a; return (int32_t)(((int64_t)b*a)/c); }
 if(a>0x10000){ if(c<=(a>>4)) return (a/c)*b; return (int32_t)(((int64_t)b*a)/c); }
 return (b*a)/c;
}
// 1.13c D2Client+0x8DAD0  (EAX = item; stdcall unit, statMin, statMax, outMin, outMax, outFlag; ret 0x18).
// *outMin = 1 and *outMax = 2 up front; nothing else when unit or item is null. The item is re-parented for the read:
// r = #11160(DAT_6fbcbc38, &slot); owner = r if r and r != unit; #10164(owner,item,1); read min/max by #10973; then
// #11127(owner,item) when r was null, else #10164(owner,item,slot). *outFlag = 1 when the #10587 base values exceed
// the current ones; max = max(max,min). Stat 0x110 (when the stat table has > 0x110 rows) adds
// #10449(stat, #11044(owner[0x1c],&k), k, 0,0,0) to max; stat 0x111 (> 0x111 rows) adds ratio(v, max, 100).
struct Imp8dad0 {
 int32_t(*o11160)(int32_t,int32_t*); void(*o10164)(int32_t,int32_t,int32_t); void(*o11127)(int32_t,int32_t);
 int32_t(*o10587)(int32_t,int32_t,int32_t); int32_t(*o11044)(int32_t,int32_t*);
 int32_t(*o10449)(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t);
 int32_t(*statRows)(); int32_t(*statBase)(); int32_t(*rd32)(int32_t);   // sgptDataTables +0xbd4 / +0xbcc
};
void damageRange(Env& e,const Imp8dad0& f,int32_t gBC38,int32_t item,int32_t unit,int32_t sMin,int32_t sMax,
                 int32_t* outMin,int32_t* outMax,int32_t* outFlag){
 *outMin=1; *outMax=2;
 if(!unit||!item) return;
 int32_t slot=0;
 int32_t r=f.o11160(gBC38,&slot);
 int32_t owner=(r && r!=unit)?r:unit;
 f.o10164(owner,item,1);
 int32_t lo=e.q(10973,item,sMin), hi=e.q(10973,item,sMax);
 if(!r) f.o11127(owner,item); else f.o10164(owner,item,slot);
 int32_t bLo=f.o10587(item,sMin,0), bHi=f.o10587(item,sMax,0);
 if(bLo<lo || bHi<hi) *outFlag=1;
 if(hi<=lo) hi=lo;
 int32_t st=e.q(10973,item,0x110);
 if(st && f.statRows()>0x110 && f.statBase()+0x15840 && f.rd32(owner+0x1c)){
  int32_t k=0; int32_t t=f.o11044(f.rd32(owner+0x1c),&k);
  int32_t v=f.o10449(st,t,0,0,0,0); hi+=v; if(v) *outFlag=1;
 }
 st=e.q(10973,item,0x111);
 if(st && f.statRows()>0x111 && f.statBase()+0x15984 && f.rd32(owner+0x1c)){
  int32_t k=0; int32_t t=f.o11044(f.rd32(owner+0x1c),&k);
  int32_t v=f.o10449(st,t,0,0,0,0); hi+=ratio(v,hi,100); if(v) *outFlag=1;
 }
 *outMin=lo; *outMax=hi;
}

// 1.13c D2Client+0x8DCC0  (EDI = out; stdcall data, add; ret 8): label 0xd83, value data u16 +0x10c + add; empty if < 1.
void tooltipField10c(Env& e,const uint8_t* data,int32_t add,wc* out){
 const wc* c0=e.str(0xf9b); const wc* c1=e.str(0xf9e);
 int32_t v=rd16(data,0x10c)+add; if(v<1){ out[0]=0; return; } labelNum(e,out,0xd83,v,c0,c1);
}
// 1.13c D2Client+0x8DD80  (EDI = out; stdcall data, add; ret 8): label 0xd82, value data u16 +0x10a + add.
void tooltipField10a(Env& e,const uint8_t* data,int32_t add,wc* out){
 const wc* c0=e.str(0xf9b); const wc* c1=e.str(0xf9e);
 int32_t v=rd16(data,0x10a)+add; if(v<1){ out[0]=0; return; } labelNum(e,out,0xd82,v,c0,c1);
}
// 1.13c D2Client+0x8DE40  (ESI = out; stdcall v; ret 4): label 0xd8d.
void tooltipNumber(Env& e,int32_t v,wc* out){ const wc* c0=e.str(0xf9b); const wc* c1=e.str(0xf9e); labelNum(e,out,0xd8d,v,c0,c1); }

// 1.13c D2Client+0x8DEE0  (ESI = out; stdcall unit, rec; ret 8): when stat 0x46 > 0, label 0xd86, the stat as "%ld",
// then the string whose id is rec u16 +0xf4. Otherwise out is untouched.
void tooltipStat46(Env& e,int32_t unit,const uint8_t* rec,wc* out){
 const wc* c0=e.str(0xf9b); const wc* c1=e.str(0xf9e);
 if(e.q(10973,unit,0x46)>0){
  ucpy(out,e.str(0xd86)); char b[12]; wc n[10]; num(b,"%ld",e.q(10973,unit,0x46),n);
  ucat(out,c0); ucat(out,n); ucat(out,c1); ucat(out,e.str(rd16(rec,0xf4)));
 }
}

// 1.13c D2Client+0x8DFB0
int32_t personalizedName(Env& e,wc* out){
 if(!e.q(10202,0,0)) return 0;
 wc tmp[32]; w2u(tmp,(const char*)(intptr_t)e.q(10083,0,0),32);
 e.personalize(out,tmp,0x200); return 1;
}
// 1.13c D2Client+0x8E080
int32_t personalizedNameInPlace(Env& e,wc* out){
 if(!e.q(10202,0,0)) return 0;
 int32_t cls=e.q(10106,0,0); if(cls>=5 && cls<=9) return 0;
 wc tmp[32],res[0x200]; w2u(tmp,(const char*)(intptr_t)e.q(10083,0,0),32);
 e.personalize(res,tmp,0x200); ucpy(out,res); return 1;
}

// 1.13c D2Client+0x8E180
void itemFlagsText(Env& e,int32_t unit,wc* out){
 wc t[16]; (void)e.str(0); w2u(t,"",15); ucpy(out,t);
 bool eth=e.q(10202,unit,0x400000)!=0;
 if(eth){ w2u(t,"Ethereal",15); ucat(out,e.str(1)); }
 if(e.q(10202,unit,0x800)){
  if(eth){ w2u(t,", ",15); ucat(out,t); }
  ucat(out,e.str(2));
  char b[12]; wc n[20]; e.fmt(b,10,(uint8_t)e.q(10366,unit,0)); w2u(n,b,9);
  ucat(out,e.str(3)); ucat(out,n); ucat(out,e.str(4));
 }
}

// 1.13c D2Client+0x8E2F0
void hoverItemPickup(Env& e){
 if((e.g(0xBCBC34)!=1 || e.g(0x8BC48)/2<=e.g(0xBCB828)) && e.g(0xBCB824)<e.g(0x8BC4C)-0x27){
  if(e.g(0xBCB828)<e.g(0x8BC48)/2){ e.gset(0xBCBC68,0); return; }
  int32_t it=e.q(11017,0,0);
  if(it && e.g(0xBCBC2C)){
   if(!e.q(10217,it,e.g(0xBCBC68)) || !e.q(10244,it,e.g(0xBCBBFC))){ e.q(11139,0,0); e.q(0x8C2B0,it,0); }
   e.q(0x8C5B0,0xBCBCA0+e.g(0xBCBC68)*5,0);
  }
 }
}

// 1.13c D2Client+0x8E440
void cursorItemPlace(Env& e,int8_t mode,int32_t unit){
 if(mode==-1||!unit||!e.g(0xBCBC28)) return;
 e.q(10114,unit,0); int32_t x=e.g(0xB90EB8),y=e.g(0xB90EBC);
 if(x<0||y<0) return;
 int32_t pm=e.g(0xBCBC34);
 if((pm==1&&e.g(0x8BC48)/2>e.g(0xBCB828))||e.g(0xBCB824)>=e.g(0x8BC4C)-0x27) return;
 if(pm==0x13&&e.g(0x8BC48)/2>e.g(0xBCB828)) return;
 if(e.q(11148,0,0)){ e.q(0x8C600,x,y*4+1); return; }
 if(!e.q(10967,0,0) && (!y || e.q(10695,y,0)!=0x20786f62)){ e.q(0x8C600,x,y*4+0); return; }
 e.q(10114,y,0); e.q(0x8C600,e.q(0x1660,0,0),e.q(0x1630,0,0)*4+3);
}

// 1.13c D2Client+0x8E770
void drawBeltPanelCaps(Env& e){
 if(!e.g(0xBC9854)||!e.g(0xBAAE14)) return;
 if(!e.g(0xBCBED8)){ e.gset(0xBCBED8,e.q(0xBF6C0,0,0)); if(!e.g(0xBCBED8)) return; }
 int32_t x=e.g(0xBCB9A0), y=e.g(0xBCB9A4)-0xe0+e.g(0x8BC4C), y2=e.g(0xBCB9A4)-0x30+e.g(0x8BC4C);
 e.draw(0,x,y,-1,5,0); e.draw(1,x+0x100,y,-1,5,0);
 e.draw(2,x,y2,-1,5,0); e.draw(3,x+0x100,y2,-1,5,0);
 e.q(10184,0,0); e.q(10150,0x82-e.g(0xBCB9A4),e.g(0xBCBEF4)); e.q(10184,0,0);
}
}
