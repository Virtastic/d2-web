// Prior art: none (D2Client has no D2MOO module). Derived from cached 1.13c decompile.
// Env-seam style: the D2 ordinal imports come in through Env.
#include <cstdint>
#include <cstring>
namespace d2client_2c230 {
using I=int32_t;
struct Env {
 I (*o10747)();            // Ordinal_10747 (gate)
 I (*o10061)(I);           // Ordinal_10061
 int8_t (*o10417)(I);      // Ordinal_10417
 I (*o11139)(I,int8_t);    // Ordinal_11139
 I (*o10744)(I,I);         // Ordinal_10744
 I (*o11006)(I);           // Ordinal_11006
 I (*o11160)(I,I);         // Ordinal_11160
 void (*o10164)();         // Ordinal_10164
};
// 1.13c D2Client+0x2C230
// ebx: object whose dword at +0x60 is the source handle (unaff_EBX).
I sub_2C230(const Env& e,const uint8_t* ebx){
 if(!e.o10747()) return 0;
 I a; std::memcpy(&a,ebx+0x60,4);
 if(!a) return 0;
 I b=e.o10061(a); if(!b) return 0;
 int8_t c=e.o10417(b);
 I d=e.o11139(a,(int8_t)((c==4)+4));
 if(!d||d==b) return 0;
 if(!e.o10744(d,0x2d)) return 0;
 if(!e.o11006(b)||!e.o11006(d)) return 0;
 if(e.o11160(b,0)) e.o10164();
 if(e.o11160(d,0)) e.o10164();
 return d;
}
}
