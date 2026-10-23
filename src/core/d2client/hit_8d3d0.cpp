// Prior art: none. Derived from cached 1.13c decompile.
#include <cstdint>
namespace d2client_hit_8d3d0 {
using I=int32_t;
struct Env {
 uint32_t bbfc_60;           // *(DAT_6fbcbbfc+0x60)
 I g_b9a4, g_b9a0;           // DAT_6fbcb9a4, DAT_6fbcb9a0
 I g_w, g_h;                 // DAT_6fb8bc4c, DAT_6fb8bc48
 int (*ord11017)(uint32_t);
};
// 1.13c D2Client+0x8D3D0  (EAX = x, EDI = y)
uint32_t sub_8D3D0(const Env& e, I x, I y){
 if(e.ord11017(e.bbfc_60)) return 0;
 I yb=e.g_h-e.g_b9a0;
 return e.g_b9a4-0x57+e.g_w<=x && x<=e.g_b9a4-0x45+e.g_w && yb-0xed<=y && y<=yb-0xd9;
}
}
