// 1.13c D2Client+0xAD950 -- env-seam style; callees are injected.
#include <cstdint>
namespace d2client_ad950 {
struct Env {
 void (*fnAC140)();                 // D2Client+0xAC140
 uint32_t (*ord10887)(void*);       // ordinal 10887 (UNVERIFIED which DLL/name); result low 16 bits reach 2A1B0 (decompile shows stale local_b)
 void (*fn2A1B0)(uint32_t,uint32_t,uint32_t); // D2Client+0x2A1B0
 void* player;                      // D2Client+0x10BBFC
};
// 1.13c D2Client+0xAD950
void sub_AD950(Env& e){
 e.fnAC140();
 if(e.player){uint32_t r=e.ord10887(e.player);e.fn2A1B0(r&0xffff,0,0);}
}
}
