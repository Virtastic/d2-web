// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: every Ordinal call comes in through Env; ctx is EBX (+0x60 read).
#include <cstdint>
namespace d2client_2c190 {
using U=uint32_t;
struct Env {
 U (*o10747)();            // gate; 0 -> return 0
 U (*o10061)(U h);         // h -> a
 int (*o10417)(U a);       // low byte compared with 4
 U (*o11139)(U h,int k);   // (h, (c==4)+4) -> b
 U (*o10744)(U b,int k);   // called with 0x2d
 U (*o11006)(U x);
 void (*o10164)();         // register args, none visible
};
// 1.13c D2Client+0x2C190
U sub_2C190(const Env& e,U ctx60){
 if(!e.o10747()) return 0;
 U h=ctx60; if(!h) return 0;
 U a=e.o10061(h); if(!a) return 0;
 int c=(int8_t)e.o10417(a);
 U b=e.o11139(h,(c==4)+4);
 if(b&&b!=a&&e.o10744(b,0x2d)&&e.o11006(a)&&e.o11006(b)){e.o10164();e.o10164();return a;}
 if(e.o11006(a)) e.o10164();
 return a;
}
}
