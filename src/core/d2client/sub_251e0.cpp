// Derived from cached 1.13c code. 1.13c D2Client+0x251E0
#include <cstdint>
#include <cstring>
namespace d2client_251e0 {
using U=uint32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
struct Calls { void (*ord10408)(); void (*fatal)(); void (*sub_6A220)(P); void (*ord10595)(); P (*deref)(U); };
// deref(rd(obj,0x14)) -> pointer p; the record is **(obj+0x14): bytes at +0x129,+0x12b,+0xc4+state
void sub_251E0(P obj,Calls& c){
 P rec=c.deref(rd(obj,0x14));
 uint8_t* r=static_cast<uint8_t*>(rec); uint8_t b;
 if(rd(obj,0x10)==3){wr(obj,0x10,0);b=r[0x129];}
 else{ if(rd(obj,0x10)!=1){c.fatal();return;} c.ord10408(); wr(obj,0x10,2); b=r[299]; }
 wr(obj,0x44,b); c.sub_6A220(obj); c.ord10595();
 wr(obj,0xc4, r[0xc4+rd(obj,0x10)] ? rd(obj,0xc4)|2 : rd(obj,0xc4)&~2u);
}
}
