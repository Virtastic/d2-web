// Batch 6D740: D2Client gfx-util helpers from cached 1.13c decompile (D2MOO has no D2Client).
// Register inputs (EAX/EBX/ECX/EDI/ESI) are explicit parameters; Ordinal_N / FUN_ callees are hooks
// (unresolved ordinals: see per-function notes). Retail fatal-error paths (_exit) return false/0.
#include <cstdint>
#include "d2client_query_env.h"
namespace d2client_b6d740 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rdu(const uint8_t*p){U v=0;for(int i=3;i>=0;--i)v=v<<8|p[i];return v;}
static void wru(uint8_t*p,U v){for(int i=0;i<4;++i)p[i]=uint8_t(v>>(8*i));}

// Shared walk of 6D740/6D780: table {u8 count; ... 9-byte entries from +0x1c}; match entry.id == key (EDI).
static uint8_t entryByte(const uint8_t* t,I key,int off){
 if(!t||key>=0x10) return 0;
 const uint8_t* e=t+0x1c;
 for(int i=0;i<t[0];++i,e+=9) if(int32_t(e[0])==key) return e[off];
 return 0;
}
// 1.13c D2Client+0x6D740 (ecx=t, edi=key)
uint8_t sub_6D740(const uint8_t* t,I key){ return entryByte(t,key,1); }
// 1.13c D2Client+0x6D780 (ecx=t, edi=key)
uint8_t sub_6D780(const uint8_t* t,I key){ return entryByte(t,key,3); }

// sgptDataTables+0xbbc = 0x84-byte records, count at +0xbc0.
static const uint8_t* rec84(const uint8_t* recs,I count,I idx){ return (idx>=0&&idx<count&&recs)?recs+idx*0x84:nullptr; }
// 1.13c D2Client+0x6D7C0 (eax=idx)
uint8_t sub_6D7C0(const uint8_t* recs,I count,I idx){ const uint8_t*r=rec84(recs,count,idx); return r?r[0x80]:1; }
// 1.13c D2Client+0x6D800 (edx=obj; writes obj+0x2c = &rec+2 as 32-bit pointer value `recAddr+2`)
void sub_6D800(uint8_t* obj,const uint8_t* recs,I count,U recsAddr){
 I idx=I(rdu(obj+0x14)); if(rec84(recs,count,idx)) wru(obj+0x2c,recsAddr+U(idx)*0x84+2);
}

// Ordinal hooks. Names are the export ordinals; semantics unrecovered where noted.
struct H {
 I (*o10695)(I);  U (*o10106)(); I (*o10620)(); I (*o10931)(); I (*o10202)(I,U,I,const char*);
 U (*o10813)(); I (*o10208)(); U (*f1b50)(); U (*f18a0)(I); U (*f1870)(I);
 P (*f1a80)(); I (*o10866)(); I* (*f560d0)(); I (*o10062)(I,I); I (*o11066)(I); I (*o10394)(I);
 I (*o10893)(); I (*o10973)(); I (*o10460)(I); I (*o11147)(I); I (*o10464)(I); I (*o10431)();
 I (*o10494)(); I (*o10854)(I,I); I (*f1a3d0)(P,I,I); I (*f1d280)(I,I,I); I (*f1e2c0)(U*,uint8_t*);
 I (*o10005)(uint8_t*,I,I); I (*o10663)(P); I (*o10569)(P,U*,U*); I (*o10060)(I); I (*o10000)(I);
 I (*o10069)(I); I (*o11142)(P);
};

// 1.13c D2Client+0x6DB40 (edi=obj; obj+0x10 kind, +4 flags, +0x2c out). Return paths via obj only.
void sub_6DB40(H&h,uint8_t* obj){
 uint8_t* base=reinterpret_cast<uint8_t*>(uintptr_t(h.o10695(I(h.f1b50())))); if(!base) return;
 I kind=I(rdu(obj+0x10));
 auto setp=[&](const uint8_t* p){ wru(obj+0x2c,U(uintptr_t(p))); };
 if(kind!=4){
  if(kind!=3) return;
  wru(obj+0x2c,U(uintptr_t(base)));
  const uint8_t* p;
  if(h.o10106()==7){ if(h.o10620()<0) return; I x=h.o10620(); p=reinterpret_cast<uint8_t*>(uintptr_t(h.f18a0(x))); if(!p) return; p+=0x3a; }
  else { if(h.o10106()!=5) return; I x=h.o10620(); p=reinterpret_cast<uint8_t*>(uintptr_t(h.f1870(x))); if(!p) return; p+=0x42; }
  if(*p) setp(p);
  return;
 }
 h.o10931();
 if(h.o10202(0,0,0,nullptr)&&h.o10106()==5){
  I x=h.o10620(); const uint8_t* a=reinterpret_cast<uint8_t*>(uintptr_t(h.f1870(x))); const uint8_t* q=nullptr;
  if(a&&a[0x62]) q=a+0x62; else if(base[0x60]) q=base+0x60;
  if(q){ obj[4]|=0x10; setp(q); return; }
 }
 if(h.o10202(0,0,0,nullptr)&&h.o10106()==7){
  const uint8_t* q=nullptr;
  I x=h.o10620(); if(x>0){ const uint8_t* a=reinterpret_cast<uint8_t*>(uintptr_t(h.f18a0(x))); if(a&&a[0x5a]) q=a+0x5a; }
  if(!q&&base[0x40]) q=base+0x40;
  if(q){ obj[4]|=1; setp(q); return; }
 }
 if(h.o10208()==0) setp(base+0x20); else wru(obj+0x2c,h.o10813());
}

// Common "owner gfx handle": flag bit3 of owner+200 and Ordinal_10866 + FUN_6fb560d0 -> piVar[0x18], else owner+0x60.
static I ownerHandle(H&h,const uint8_t* owner){
 if(owner&&((rdu(owner+200)>>3)&1)&&h.o10866()){ I* p=h.f560d0(); if(p) return p[0x18]; }
 return I(rdu(owner+0x60));
}
// 1.13c D2Client+0x6DEE0 (eax=owner, ecx=param_1 slot, edx=param_2 default). Strings: GfxUtil.cpp lines 0x656/0x657.
I sub_6DEE0(H&h,const uint8_t* owner,I slot,I def){
 static const char*F="..\\Source\\D2Client\\ENGINE\\GfxUtil.cpp";
 if(slot>0xf) return def;
 if(owner&&!h.f1a80()){
  I s=ownerHandle(h,owner); if(!s) return def;
  I r=h.o10062(s,slot);
  if(!r&&slot!=1&&h.o11066(slot)) r=h.o10062(s,1);
  if(r&&!h.o10202(r,0x100,0x656,F)&&!h.o10202(r,0x4000,0x657,F)){
   if(h.o10394(r)==0x2074696c) return 0;
   I v=h.o10893(); if(v) return v;
  }
 }
 return def;
}

// 1.13c D2Client+0x6DFE0 (eax=owner, ebx=unitArg, param_1=kind, param_2=out). Lines 0x5e6/0x611/0x61c/0x627.
bool sub_6DFE0(H&h,const uint8_t* owner,I ebx,I kind,U* out){
 static const char*F="..\\Source\\D2Client\\ENGINE\\GfxUtil.cpp";
 P m=h.f1a80();
 if(!m){ if(h.o10973()){ *out=1; return true; } }
 else if(m==P(1)){
  I n=h.o10973();
  if(n>0){
   if(n>0xf){
    I u=I(rdu(owner+0x60)); if(!u) return false;
    I it=h.o10460(u);
    for(;;){ if(!it) return false; I s=h.o11147(it); if(s&&h.o10202(s,0x400000,0x5e6,F)) break; it=h.o10464(it); }
   }
   *out=1; return true;
  }
 }
 if(ebx!=-1){ const uint8_t* p=reinterpret_cast<uint8_t*>(uintptr_t(h.o10695(0))); if(p&&p[300]){ *out=p[0x12d]; return true; } }
 if(m) return false;
 I s=ownerHandle(h,owner); if(!s) return false;
 int line;
 if(kind==5) line=0x611; else if(kind==6) line=0x61c;
 else { if(kind!=7) return false; I r=h.o10062(s,7); if(!r||!h.o10202(r,0x400000,0x627,F)) return false; *out=2; return true; }
 I r=h.o10062(s,kind); if(!r||!h.o10202(r,0x400000,line,F)) return false;
 *out=1; return true;
}

// 1.13c D2Client+0x6E1E0 (ebx=slot, esi=owner). Returns 'lit '/'hsh ' style fourCC or a handle.
// Callees as retail calls them: CBDA = Ordinal_10866(owner, 0x25), A60D0 (eax = owner), C2AA = Ordinal_10431(owner, handle, &local, -1, 1),
// CBE0 = Ordinal_10062(handle, 5), C1BA = Ordinal_10695(r[+4]), C1FC = Ordinal_10494(owner, 0x65), C274 = Ordinal_10854(handle, 0).
struct D2c6E1E0Env {
 I (*o10866)(const uint8_t* owner,I k); const uint8_t* (*f560d0)(const uint8_t* owner); I (*o10431)(const uint8_t* owner,I handle,I* out,I a,I b);
 I (*o10062)(I handle,I k); I (*o10695)(I v); I (*o10494)(const uint8_t* owner,I k); I (*o10854)(I handle,I k);
};
I sub_6E1E0(const D2c6E1E0Env& e,I slot,const uint8_t* owner){
 if(slot>=0x10) return   /* signed: cmp ebx,0x10; jl */ 0x2074696c;
 I s;
 if(owner&&((rdu(owner+200)>>3)&1)&&e.o10866(owner,0x25)){ const uint8_t* p=e.f560d0(owner); s=I(rdu(p?p+0x60:owner+0x60)); }
 else s=I(rdu(owner+0x60));
 I loc=0;
 I t=e.o10431(owner,s,&loc,-1,1);
 if(t==0x20776278){
  if(slot==5||slot==6){
   I r=e.o10062(s,5);
   if(r){ const uint8_t* p=reinterpret_cast<uint8_t*>(uintptr_t(e.o10695(I(rdu(reinterpret_cast<uint8_t*>(uintptr_t(r))+4)))));
    if(p){ U v=rdu(p+0x90); return I(v?v:rdu(p+0x80)); } }
   return 0x2074696c;
  }
 } else if(t==0x20776f62&&slot==5) return 0x2074696c;
 if(slot==7&&e.o10494(owner,0x65)&&e.o10854(s,0)) return 0x20687368;
 return 0;
}

// Query block shared by 6E860/6E930/6E9C0/6EA50: 0x48 zeroed bytes; +0 = p5/out1, +5 = slot (1), +0x34 = 0, +0x40 = param, result handle at +0x3C
// after f1e2c0 (6E2C0) + o10005. Frame offsets read from the disassembly (the decompile's are off).
// 1.13c D2Client+0x6E860 (ecx=unit, edx=x, eax=eaxIn; globals gCC418 gC9960 gC995C = +0xCC418 +0xC9960 +0xC995C). Returns 6D280's eax or 0.
I sub_6E860(H&h,P unit,I x,I eaxIn,I gCC418,I gC9960,I gC995C){
 I ebp=eaxIn-gC995C+8, ebx=(gCC418-gC9960)+x;
 if(!h.f1a3d0(unit,ebx,ebp)) return 0;
 U out[2]={0,0}; h.o10569(unit,&out[0],&out[1]); U out1=out[0];
 uint8_t q[0x48]={0};
 wru(q+0x40,U(h.o10663(unit))&0xff); wru(q,out1); q[5]=1; wru(q+0x34,0);
 if(!h.f1e2c0(reinterpret_cast<U*>(unit),q)||!h.o10005(q,0,1)) return 0;
 U node=rdu(q+0x3c); if(!node) return 0;
 return h.f1d280(I(node),ebx,ebp);
}
// 1.13c D2Client+0x6E930 (stdcall unit,p2,p3,out1,out2; eax=-1 at 6E2C0).
void sub_6E930(H&h,P unit,U p2,U p3,U* out1,U* out2){
 uint8_t q[0x48]={0};
 wru(q,p3); q[5]=1; wru(q+0x34,0); wru(q+0x40,p2);
 if(h.f1e2c0(reinterpret_cast<U*>(unit),q)&&h.o10005(q,0,1)&&rdu(q+0x3c)){
  *out1=U(h.o10060(I(rdu(q+0x3c)))); *out2=U(h.o10000(I(rdu(q+0x3c))));
 }
}
// 1.13c D2Client+0x6E9C0 (ecx unused, edx=param_2, param_3)
I sub_6E9C0(H&h,const uint8_t* p2,P p3){
 uint8_t q[0x48]={0};
 wru(q+0x14,rdu(p2+4)); wru(q+0x3c,U(h.o10663(p3))&0xff); wru(q+0x30,0); q[1]=1;
 if(h.f1e2c0(nullptr,q)&&h.o10005(q,0,1)) return I(rdu(q+0x20)<<8);
 return 1;
}
// 1.13c D2Client+0x6EA50 (ebx=unit; stdcall p1 = byte count then slot list p2, p3 = running max, p4 = q+0x40, p5 = q+0; gOff=DAT_6fbc995c).
// Fatal path (slot>0xf: log writers then _exit, which falls into the next function) returns -1 here; the FN capture never takes it.
I sub_6EA50(H&h,const uint8_t* p1,const uint8_t* p2,I* maxOut,U p4,U p5,P unit,I gOff){
 I count=0;
 for(int i=0;i<p1[0];++i){
  uint8_t s=p2[i]; if(s>0xf) return -1;
  uint8_t q[0x48]={0}; wru(q+0x34,0); wru(q+0x40,p4); wru(q,p5); q[5]=s;
  if(h.f1e2c0(reinterpret_cast<U*>(unit),q)&&h.o10005(q,0,1)&&rdu(q+0x3c)){
   I v=(h.o10069(I(rdu(q+0x3c)))-(gOff-8))+h.o11142(unit);
   if(*maxOut<v) *maxOut=v;
   ++count;
  }
 }
 return count;
}
}

// FUNCTION-mode entry for the 6E9C0 probe (src/core/fn/d2client_6e9c0.cpp): the three seams sub_6E9C0 calls, nothing else.
struct D2c6E9C0Env { int32_t (*f1e2c0)(uint32_t*, uint8_t*); int32_t (*o10005)(uint8_t*, int32_t, int32_t); int32_t (*o10663)(void*); };
extern "C" int32_t d2c_port_6e9c0(const uint8_t* p2, void* p3, const D2c6E9C0Env* e) {
 using namespace d2client_b6d740;
 H h{}; h.f1e2c0=e->f1e2c0; h.o10005=e->o10005; h.o10663=e->o10663;
 return sub_6E9C0(h,p2,p3);
}

// FUNCTION-mode entries for the 6E860 / 6E930 / 6EA50 probes (src/core/fn/d2client_6e*.cpp): the seams those routines call, nothing else.
static d2client_b6d740::H d2cQueryH(const D2cQueryEnv* e){
 d2client_b6d740::H h{}; h.f1a3d0=e->f1a3d0; h.o10569=e->o10569; h.o10663=e->o10663; h.f1e2c0=e->f1e2c0; h.o10005=e->o10005;
 h.f1d280=e->f1d280; h.o10060=e->o10060; h.o10000=e->o10000; h.o10069=e->o10069; h.o11142=e->o11142; return h;
}
extern "C" int32_t d2c_port_6e860(void* unit, int32_t x, int32_t eaxIn, int32_t gCC418, int32_t gC9960, int32_t gC995C, const D2cQueryEnv* e) {
 auto h=d2cQueryH(e); return d2client_b6d740::sub_6E860(h,unit,x,eaxIn,gCC418,gC9960,gC995C);
}
extern "C" void d2c_port_6e930(void* unit, uint32_t p2, uint32_t p3, uint32_t* out1, uint32_t* out2, const D2cQueryEnv* e) {
 auto h=d2cQueryH(e); d2client_b6d740::sub_6E930(h,unit,p2,p3,out1,out2);
}
extern "C" int32_t d2c_port_6ea50(const uint8_t* p1, const uint8_t* p2, int32_t* maxOut, uint32_t p4, uint32_t p5, void* unit, int32_t gC995C, const D2cQueryEnv* e) {
 auto h=d2cQueryH(e); return d2client_b6d740::sub_6EA50(h,p1,p2,maxOut,p4,p5,unit,gC995C);
}
