// Derived from cached 1.13c code; D2MOO has no D2Client.
#include <cstdint>
namespace d2client_26410 {
using I=int32_t;
struct G { I a=0,b=0,c=0,d=0,e=0,f=0; };
// 1.13c D2Client+0x26410: reset three selection ids to -1 and three counters to 0 (DAT_6fba538c/90/94, DAT_6fbcc468/7c/80).
void sub_26410(G& g){ g.a=g.b=g.c=-1; g.d=g.e=g.f=0; }
struct Rect { I x0,x1,y0,y1; };
// 1.13c D2Client+0x26460: slot whose rect contains (x,y), else -1. x lives in EDI (lost by decompiler), y = param_1.
// count/rects come from D2Common #10991/#10333 on (DAT_6fba5384, DAT_6fbcb99c).
I sub_26460(I x,I y,int count,const Rect* r){
 for(int i=0;i<count;++i) if(r[i].x0<=x&&x<=r[i].x1&&r[i].y0<=y&&y<=r[i].y1) return i;
 return -1;
}
// 1.13c D2Client+0x265E0: DAT_6fbca7d0..3 = D2Gfx #10190 palette index of (128,0,0),(0,128,0),(0,0,128),(128,128,0).
void sub_265E0(uint8_t out[4], uint8_t (*pal)(I,I,I)){
 out[0]=pal(0x80,0,0); out[1]=pal(0,0x80,0); out[2]=pal(0,0,0x80); out[3]=pal(0x80,0x80,0);
}
}
