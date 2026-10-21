// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: FUN_6fae8e80, the three Storm/Fog ordinals and the template table come in through Env.
#include <cstdint>
#include <cstring>
namespace d2client_39A60 {
using I=int32_t;
struct Env {
 I (*probe)();                                  // FUN_6fae8e80 (D2Client+0x38E80)
 const uint8_t* tmpl;                           // DAT_6fbca2e0, 0x474 bytes
 void (*o10111)(I a,const void* p,I c);         // Ordinal_10111 (calls 1..3 by index)
 void (*o10109)();                              // Ordinal_10109
 void (*o10116)();                              // Ordinal_10116
 I (*o10107)();                                 // Ordinal_10107
};
// 1.13c D2Client+0x39A60
void sub_39A60(const Env& e){
 if(e.probe()==-1) return;
 uint8_t blk[0x476]; blk[0]=0x25; blk[1]=0;   // local_480 = 0x25; the 0xffff fill (local_47a) is fully overwritten by the copy
 std::memcpy(blk+2,e.tmpl,0x474);
 e.o10111(0,blk,0x476); e.o10109(); e.o10116();
 // ponytail: argument identities of the Ordinal calls are not pinned (decompiler lost the stack args); "default key" string is appended to local_594
 if(e.o10107()!=-1){ e.o10111(6,nullptr,0); e.o10111(0x474,nullptr,0); e.o10109(); }
}
}
