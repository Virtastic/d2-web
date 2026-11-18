// Derived from cached 1.13c code; ECX=kind, EDX=object. Callee 0x6B300 is a seam.
#include <cstdint>
#include <cstring>
namespace d2client_2a590 {
using U=uint32_t; using P=void*;
struct Calls { void (*f6b300)(P,U,U,U,U); };
// 1.13c D2Client+0x2A590
void sub_2A590(U kind,P obj,Calls& c){
 if(!obj) return;
 U t; std::memcpy(&t,obj,4);
 if(t==0){ if(kind!=0&&kind!=4&&kind!=0x13) return; }
 else if(t==1){ if(kind!=0&&kind!=3&&kind!=0xd) return; }
 else return;
 U f; std::memcpy(&f,static_cast<uint8_t*>(obj)+0xb0,4);
 U hi=f&0xf0;
 if(hi==0||(f&0xf)==0) return;
 // each hi nibble loads its own Overlay.txt row into EAX before the call (the row is the 6B300 seam's third argument here)
 static const struct { U hi, row; } kRow[] = {{0x10,0x36},{0x20,0x51},{0x30,0x24},{0x40,0x01},{0x70,0x93},{0x80,0x15},{0x90,0x71}};
 for(const auto& r:kRow) if(hi==r.hi){ c.f6b300(obj,2,r.row,0,0); return; }
}
}
