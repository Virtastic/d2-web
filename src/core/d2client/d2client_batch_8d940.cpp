// Implementation batch D2Client+0x8D940..0x8E770 (item-tooltip text builders and small HUD helpers).
// Decompile-derived; imported DLL calls (string table, D2Common item/stat queries, D2Gfx draws) go
// through Env. Register-passed args the decompiler lost are named explicitly in each signature.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <cstdio>
#include "d2client_8d940_env.h"
namespace d2client_batch_8d940 {
static void ucpy(wc* d,const wc* s){ while((*d++=*s++)); }
static void ucat(wc* d,const wc* s){ while(*d) ++d; ucpy(d,s); }
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

// 1.13c D2Client+0x8DAD0: ported to d2client_8dad0.cpp

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

// 1.13c D2Client+0x8E2F0: ported to d2client_8e2f0.cpp

// 1.13c D2Client+0x8E770
// Returns the EAX retail leaves: 0 on the early exits, else the second Ordinal_10184 answer.
int32_t drawBeltPanelCaps(Env& e){
 if(!e.g(0xBC9854)||!e.g(0xBAAE14)) return 0;
 if(!e.g(0xBCBED8)){ int32_t img=e.q(0xBF6C0,0,0); e.gset(0xBCBED8,img); if(!img) return 0; }
 // the cel block: 18 zeroed dwords on retail's stack, word 0 the frame, word 13 the cached image
 uint32_t blk[18]={}; blk[13]=(uint32_t)e.g(0xBCBED8);
 for(int32_t f=0;f<4;f++){
  blk[0]=(uint32_t)f;
  int32_t x=e.g(0xBCB9A0)+(f&1?0x100:0), y=e.g(0xBCB9A4)+e.g(0xB8BC4C)-(f<2?0xe0:0x30);
  e.cel(blk,14,x,y,-1,5,0);
 }
 int32_t prev=e.qr(10184,4,0,0,0,0);
 e.qr(10150,0xBCCC38,(uint32_t)(e.g(0xBCB9A0)+0x50),0x82-e.g(0xBCB9A4),e.g(0xBCBEF4),0);
 return e.qr(10184,(uint32_t)prev,0,0,0,0);
}
}
// FN probe entry for 0x8DCC0 (src/core/fn/d2client_8dcc0.cpp): the D2Lang string getter is the only seam; Storm _snprintf and the
// D2Lang Unicode copies run as the port's own helpers.
extern "C" void d2c_port_8dcc0(const uint8_t* data, int32_t add, uint16_t* out, const uint16_t* (*str)(uint32_t id)) {
    struct E final : d2client_batch_8d940::Env {
        const uint16_t* (*s)(uint32_t); explicit E(const uint16_t* (*f)(uint32_t)) : s(f) {}
        const d2client_batch_8d940::wc* str(int32_t id) override { return s((uint32_t)id); }
        void fmt(char*, int32_t, int32_t) override { __builtin_trap(); }
        int32_t q(int32_t, int32_t, int32_t) override { __builtin_trap(); }
        int32_t g(int32_t) override { __builtin_trap(); }
        void gset(int32_t, int32_t) override { __builtin_trap(); }
        void draw(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) override { __builtin_trap(); }
        bool personalize(uint16_t*, const uint16_t*, int32_t) override { __builtin_trap(); }
    } e(str);
    d2client_batch_8d940::tooltipField10c(e, data, add, out);
}
