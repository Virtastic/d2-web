// Derived from cached 1.13c decompile. Globals/callees come in through Env.
#include <cstdint>
namespace d2client_27ef0 {
using I=int32_t;
// 1.13c D2Client+0x28420: button cel frame index (EAX=base slot, ecx=param_1, edx=param_2, param_3=kind)
I frame28420(I slot,I p1,I p2,I kind){
 I f=slot*11;
 switch(kind){
 case 1:return f+1; case 3:return f+4; case 8:return f+2; case 9:return f+3;
 case 4:return f+(p1?10:(p2?7:6));
 case 5:return f+(p1?9:(p2?5:8));
 }
 return f;}
// 1.13c D2Client+0x28320: 0..3 cel frame; bit1 = flag DAT_6fbcc3ec, bit0 = hover-ok (DAT_6fbcc4ac && FUN_65a0(mouseX))
I state28320(bool flag3ec,bool on4ac,bool hit65a0){return (flag3ec?2:0)|((on4ac&&hit65a0)?1:0);}
// hit rect on button: x in [w/2-0x91, w/2-0x80], y in [h-0x1c, h-8]
bool hit28320(I mx,I my,I w,I h){return mx>=w/2-0x91&&mx<=w/2-0x80&&my>=h-0x1c&&my<=h-8;}
// 1.13c D2Client+0x27EF0: builds string = base + (entry 0x23/state1 ? fmt : "") then (entry 0x23/state0 ...) ; entries 10 bytes {id@-6,x@-2?,state@+0}
// Entry scan: first record with id==0x23 && state==want; append sprintf(fmt(FUN_e9f30(0,want))) if key!=-1.
struct Rec{I id,key,state;};
bool want27ef0(const Rec*r,int n,I st,I*key){for(int i=0;i<n;i++)if(r[i].id==0x23&&r[i].state==st){*key=r[i].key;return r[i].key!=-1;}return false;}
}
