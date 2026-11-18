// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
#include <cstdint>
namespace d2client_close_panels {
using U=uint32_t;
struct State {
 U active[0x26];   // D2Client+0x4AD80 (slot open == 1)
 U closable[0x26]; // D2Client+0x209D8 (slot may be closed == 1)
 U sel[6];         // D2Client+0x30EA8..0x30EBC, reset to -1 for slots 1/0x19
 U c28,c2c,c38,c3c; // D2Client+0x6BC28/2C/38/3C, reset to 0 for slots 1/0x19
};
// 1.13c D2Client+0xBE350
// close(slot,1,arg) is FUN 0xC2790 (fastcall ecx=slot, edx=1, stack arg).
U sub_BE350(State& s,int param1,int param2,void(*close)(U,U,int)){
 U r=0;
 for(U i=0;i<0x26;++i){
  if(s.active[i]==1&&(s.closable[i]==1||(param1!=0&&i==10))){
   close(i,1,param2); r=1;
   if(i==0x19||i==1){
    for(U&v:s.sel)v=0xffffffffu;
    s.c28=s.c2c=s.c38=s.c3c=0;
   }
  }
 }
 return r;
}
}
