// D2Client+0xBB210: read unit position, forward to D2Client+0xB6D40. Ordinals unverified (names per sibling batches).
#include <cstdint>
#include <cstring>
namespace d2client_bb210 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
struct Calls {
 I (*pathY)(P); I (*pathX)(P); // Ordinal_10750 / 10867 (unverified)
 void (*fnB6D40)(P,I,I,U,P,U,U,U); // D2Client+0xB6D40
};
// 1.13c D2Client+0xBB210
// unit arrives in EAX (usercall); types 2,4,5 read the path struct (unit+0x2c) at +0x10 (y) / +0xc (x) directly.
void unit_pos_forward(P unit,U p1,U p2,P p3,U p4,U p5,U p6,Calls& c){
 I t=I(rd(unit,0)); P path=reinterpret_cast<P>(uintptr_t(rd(unit,0x2c)));
 bool direct=(t==2||(t>3&&t<6));
 I y=direct?I(rd(path,0x10)):(path?c.pathY(path):0);
 I x=direct?I(rd(path,0xc)):(path?c.pathX(path):0);
 (void)p2; c.fnB6D40(p3,x,y,p1,p3,p4,p5,p6);
}
}
