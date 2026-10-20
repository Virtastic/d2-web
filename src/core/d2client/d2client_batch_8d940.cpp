// Implementation batch D2Client+0x8D940..0x8E770 (item-tooltip text builders and small HUD helpers).
// Decompile-derived; imported DLL calls (string table, D2Common item/stat queries, D2Gfx draws) go
// through Env. Register-passed args the decompiler lost are named explicitly in each signature.
#include <cstdint>
#include <cstring>
#include <initializer_list>
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
// "<label><colour><number><colour>[...]" shared by the 8DCC0/8DD80/8DE40/8DEE0 builders.
static void labelNum(Env& e,wc* out,int32_t v){
 char b[12]; wc n[20]; const wc* c0=e.str(1); const wc* c1=e.str(2); const wc* lab=e.str(0);
 ucpy(out,lab); e.fmt(b,10,v); w2u(n,b,9); ucat(out,c0); ucat(out,n); ucat(out,c1);
}
static int32_t rd16(const void* p,unsigned o){ uint16_t v; std::memcpy(&v,(const uint8_t*)p+o,2); return v; }

// 1.13c D2Client+0x8D940
void tooltipRequirement(Env& e,int32_t unit,const uint8_t* data,wc* out){
 char b[12]; wc n[20]; const wc* c0=e.str(1); (void)e.str(2);
 int32_t v=data[0xff]; int32_t w=0;
 if(!e.q(10744,unit,0x33)){ if(!e.q(10744,unit,0xf)) return; }
 else {
  int32_t it=e.q(10630,0,0);
  if(it && e.q(10494,e.g(0xBCBBFC),0x65)){
   int32_t t=e.q(10306,it,1);
   v+=e.q(10567,t,0x75)>>8; w+=e.q(10297,t,0x75)>>8;
  }
 }
 ucpy(out,e.str(3)); e.fmt(b,10,v); w2u(n,b,9);
 ucat(out,c0); ucat(out,n); ucat(out,c0);
 ucat(out,e.str(4)); e.fmt(b,10,w); w2u(n,b,9);
 ucat(out,c0); ucat(out,n); ucat(out,e.str(5));
}

// 1.13c D2Client+0x8DAD0
void damageRange(Env& e,int32_t* flagA,int32_t* outMin,int32_t* flagB,int32_t* flagC){
 // ponytail: param shuffling in the decompile is lost; follows the visible control flow only.
 *flagA=1; *flagB=2;
 int32_t unit=e.q(11160,0,0); if(!unit) return;
 int32_t lo=e.q(10973,unit,0), hi=e.q(10973,unit,1);
 int32_t a=e.q(10587,unit,0), b=e.q(10587,unit,1);
 if(a<hi||b<lo) *flagA=1;
 if(lo<=hi) lo=hi;
 int32_t it=e.q(10973,unit,2);
 if(it && e.g(0xBD4)>0x110){
  int32_t d=e.q(10449,it,0); lo+=d; if(d) *flagC=1;
 }
 it=e.q(10973,unit,3);
 if(it && e.g(0xBD4)>0x111){
  int32_t d=e.q(10449,it,0); lo+=e.q(0x14A0,100,lo); if(d) *flagC=1;
 }
 *flagB=0x110; *outMin=lo;
}

// 1.13c D2Client+0x8DCC0
void tooltipField10c(Env& e,const uint8_t* data,int32_t add,wc* out){
 int32_t v=rd16(data,0x10c)+add; if(v<1){ out[0]=0; return; } labelNum(e,out,v);
}
// 1.13c D2Client+0x8DD80
void tooltipField10a(Env& e,const uint8_t* data,int32_t add,wc* out){
 int32_t v=rd16(data,0x10a)+add; if(v<1){ out[0]=0; return; } labelNum(e,out,v);
}
// 1.13c D2Client+0x8DE40
void tooltipNumber(Env& e,int32_t v,wc* out){ labelNum(e,out,v); }

// 1.13c D2Client+0x8DEE0
void tooltipStat46(Env& e,int32_t unit,wc* out){
 const wc* c0=e.str(1); const wc* c1=e.str(2);
 if(e.q(10973,unit,0x46)>0){
  ucpy(out,e.str(0)); char b[12]; wc n[20]; e.fmt(b,10,e.q(10973,unit,0x46));
  w2u(n,b,9); ucat(out,c0); ucat(out,n); ucat(out,c1); ucat(out,e.str(3));
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
