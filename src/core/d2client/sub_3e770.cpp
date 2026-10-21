// Prior art: none (D2Client has no D2MOO module). Derived from cached 1.13c decompile.
#include <cstdint>
namespace d2client_3e770 {
using I=int32_t;
struct Env {
 I mouseX, mouseY;   // DAT_6fbcb828, DAT_6fbcb824
 const I* player;    // DAT_6fbcbbfc
 void (*tip)(const I* p1,I p2,I p3,I p4,I p5,I p6); // FUN_6fae3290
};
// 1.13c D2Client+0x3E770: EAX = x0, ESI = y1 (box x0..x0+0x30, y1-0x30..y1).
void sub_3e770(const Env& e,I eax,I esi,const int16_t* p1,I p2,I p3,I p4){
 bool in=eax<=e.mouseX&&e.mouseX<=eax+0x30&&esi-0x30<=e.mouseY&&e.mouseY<=esi;
 if(!in) return;
 e.tip(e.player,(I)*p1,p3,p4,p2==1?1:0,eax);
}
}
