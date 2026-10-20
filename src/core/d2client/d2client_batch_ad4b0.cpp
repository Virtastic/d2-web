// Implementation batch D2Client+0xAD4B0..0xAEBE0 (packet thunks, unit-table lookups, packet dispatch).
// Decompile-derived; unrecovered callees are function pointers on Env, register args are explicit params.
// Pointers are uintptr_t; wire-packet fields are read at their byte offsets with memcpy.
#include <cstdint>
#include <cstring>
namespace d2client_batch_ad4b0 {
struct Env {
 uint8_t* (*ga)(uint32_t rva);                    // address of D2Client global at RVA
 void (*fatal)(int32_t msgId);                    // Ordinal_10265 + Ordinal_10024 + _exit(-1)
 void (*f62E60)(uint32_t a);
 void (*f4C3C0)(uintptr_t pkt,const void* copy7);
 void (*f7DFB0)(const void* copy40);
 uintptr_t (*f4D630)(uintptr_t pkt,uint32_t id);
 void (*f4DA70)(uint32_t v);
 void (*f272E0)();
 void (*f1A1B0)(uintptr_t a,uint32_t b,uint32_t c);   // FUN_6fada1b0
 void (*f194E0)();
 void (*f34BB0)(); void (*f34AF0)(); void (*f34990)(); void (*f36810)(const void* pkt);
 void (*f34750)(); void (*f32320)(); void (*f34080)(); void (*f35AA0)(); void (*f33AD0)();
 void (*f32C60)(const void* pkt); void (*f32E10)(); void (*f339E0)();
 uintptr_t (*f54E20)(uint32_t key,uintptr_t row,uint32_t idx);
 void (*f32FE0)(uintptr_t unit);
 void (*f21D60)(uintptr_t a,uint32_t b,uint32_t c);
 uint16_t (*f5C1B0)();                            // FUN_6fb5c1b0; result = local_f (GUESS: decompiler lost the out value)
 void (*o10887)(uintptr_t a,uint32_t b,uint32_t c,uint32_t d);
 void (*f6A6C0)(); void (*f56210)();
};
static const uint32_t NEXT_OFF=0x39*4; // puVar1[0x39] link in unit node (original 32-bit layout)
static uintptr_t rdp(uintptr_t a){ uintptr_t v; std::memcpy(&v,(const void*)a,sizeof v); return v; }
static uint32_t r32(uintptr_t a){ uint32_t v; std::memcpy(&v,(const void*)a,4); return v; }
static uint16_t r16(uintptr_t a){ uint16_t v; std::memcpy(&v,(const void*)a,2); return v; }
static uintptr_t row(Env& e,uint32_t t){ return (uintptr_t)e.ga(0xFBA608 + 0) + (uintptr_t)t*0x80*sizeof(uintptr_t); }

// 1.13c D2Client+0xAD4B0
void sub_AD4B0(Env& e,uint32_t a){ e.f62E60(a); }

// 1.13c D2Client+0xAD4F0
void sub_AD4F0(Env& e,uintptr_t p){ uint8_t l[7]; std::memcpy(l,(const void*)p,7); e.f4C3C0(p,l); }

// 1.13c D2Client+0xAD6B0
void sub_AD6B0(Env& e,uintptr_t p){ uint8_t l[40]; std::memcpy(l,(const void*)p,40); e.f7DFB0(l); }

// 1.13c D2Client+0xAD780
void sub_AD780(Env& e,uintptr_t p){
 uintptr_t o=e.f4D630(p,r32(p+1));
 if(o){ uint16_t v=r16(p+5); std::memcpy((void*)(o+0x22),&v,2); e.f4DA70(v); e.f272E0(); }
}

// 1.13c D2Client+0xAD9D0
void sub_AD9D0(Env& e,uintptr_t p){
 uint8_t n=*(const uint8_t*)(p+3);
 if(!n){ e.fatal(0x855); return; }
 uintptr_t b=p+4;
 for(int i=0;i<n;i++,b+=9){
  uint8_t type=*(const uint8_t*)b; uint32_t id=r32(b+1); uint16_t arg=r16(b+7);
  uintptr_t slot=(uintptr_t)e.ga(0xFBA608)+((uintptr_t)type*0x80+(id&0x7f))*sizeof(uintptr_t);
  for(uintptr_t u=rdp(slot);u;u=rdp(u+NEXT_OFF)){
   if(r32(u+12)==id){ if(r32(u)!=type){ e.fatal(0x5a); return; } e.f1A1B0(arg,0,0); break; }
  }
 }
}

// 1.13c D2Client+0xADEF0
void sub_ADEF0(Env& e){ e.f194E0(); }

// 1.13c D2Client+0xAE080
void sub_AE080(Env& e,const uint8_t* src){
 uint8_t l[0xFC]; std::memcpy(l,src,sizeof l);
 switch(l[1]){
 case 0: e.f34BB0(); return; case 1: e.f34AF0(); return; case 2: e.f34990(); return;
 case 3: e.f36810(l); return; case 4: e.f34750(); return;
 case 5: case 6: case 7: case 8: case 9: case 0x11: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
  e.fatal(0xf6d); return;
 case 10: e.f32320(); return; case 0xb: case 0xc: e.f34080(); return;
 case 0xd: e.f35AA0(); return; case 0xe: e.f33AD0(); return;
 case 0xf: e.f32C60(l); return; case 0x10: e.f32E10(); return; case 0x12: e.f339E0(); return;
 }
}

static void lookupAndFree(Env& e,uint32_t key,uint32_t t){
 uintptr_t u=e.f54E20(key,row(e,t),t);
 if(u) e.f32FE0(u);
}
// 1.13c D2Client+0xAE260
void sub_AE260(Env& e,const uint32_t* p){ lookupAndFree(e,((p[1]&0xFFFFFF)<<8)|(p[0]>>24),(p[0]>>8)&0xff); }
// 1.13c D2Client+0xAE2B0 (identical body to AE260)
void sub_AE2B0(Env& e,const uint32_t* p){ lookupAndFree(e,((p[1]&0xFFFFFF)<<8)|(p[0]>>24),(p[0]>>8)&0xff); }
// 1.13c D2Client+0xAE3F0 (unit passed in EAX)
void sub_AE3F0(Env& e,uintptr_t unit){ if(unit) lookupAndFree(e,r32(unit+12),r32(unit)&0xff); }

// 1.13c D2Client+0xAEBA0
void sub_AEBA0(Env& e,uintptr_t p){ e.f21D60(rdp(p+8),r32(p+4),r16(p+2)); }

// 1.13c D2Client+0xAEBE0  (GUESS: decompiler garbled the ordinal-10887 args and local_f source)
void sub_AEBE0(Env& e){
 uint16_t lf=e.f5C1B0();
 uintptr_t o=rdp((uintptr_t)e.ga(0x11BBFC));
 if(!o) return;
 uint32_t v=(uint32_t)lf<<8;
 e.o10887(o,6,v,0);
 e.o10887(o&0xff,8,0,0);
 e.o10887(o,10,0,0);
 uint16_t u=(uint16_t)((o>>24)&0xff); if(u>0x80) u-=0x100;
 e.f1A1B0(o,0,(uint16_t)(u+(uint16_t)((o&0xff)*0x100)));
 if(lf!=0 && r32(o+0x10)==0x11){ e.f6A6C0(); e.f56210(); }
}
} // namespace
