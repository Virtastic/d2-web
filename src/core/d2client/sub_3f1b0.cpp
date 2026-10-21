// D2Client sub_3F1B0: per-frame draw of a two-part widget record (callback tick + two Gfx draws).
#include <cstdint>
#include <cstring>
namespace d2client_3f1b0 {
using U=uint32_t;
static U rd(const void* p,unsigned o){U v;std::memcpy(&v,(const uint8_t*)p+o,4);return v;}
static void wr(void* p,unsigned o,U v){std::memcpy((uint8_t*)p+o,&v,4);}
struct Services {
 virtual ~Services()=default;
 virtual void callback(void* rec,U fn)=0; // *(rec+0x3c), called with rec in ECX/EDX per retail
 virtual void draw(const U* args,U cell,U a,U b,U c,U d,int nargs)=0; // Ordinal_10041 (via D2Client+0xD096)
};
// 1.13c D2Client+0x3F1B0
void sub_3F1B0(void* r,Services& s){
 if(rd(r,0x38)||rd(r,0x34)){wr(r,0x40,rd(r,0x40)+1);s.callback(r,rd(r,0x3c));}
 U a[4]={rd(r,0x14),0xffffffffu,5,0};
 U l[18]={}; l[13]=rd(r,0x50); l[0]=rd(r,0x34)<<1;
 s.draw(l,rd(r,0x10),0,0,0,0,2);
 a[0]=rd(r,0x38)*2+1;
 s.draw(a,rd(r,0x18),rd(r,0x1c),0xffffffffu,5,0,6);
}
}
