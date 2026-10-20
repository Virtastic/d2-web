// Prior art: ; D2MOO has no D2Client module, pointer tables name none of these.
// Batch D2Client+0x35140..0x38310. Imported ordinals go through Services (low-confidence rows noted).
// 0x359B0/0x35DF0/0x361D0/0x362E0: Ordinal_10041 takes a pointer to a stack record; only word 0 (index) and,
// for the first call, word 13 (+0x34, panel handle) are recovered. Later records alias stale argument
// slots in retail; w[0]=0xffffffff marks that unrecovered state (low confidence).
#include <cstdint>
#include <cstdlib>
#include <string>
namespace d2client_batch_35140 {
struct Rec { uint32_t w[18]; };
struct Services {
 virtual ~Services()=default;
 virtual void ord10041(const Rec&,int32_t,int32_t,int32_t,int32_t,int32_t)=0;
 virtual void ord10010(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t)=0;
 // 35140/35270 helper chain (FUN_2C830, 34D70, 2F0A0 and ordinals 10567/10297/10510/10121/11091)
 virtual void sub_2C830()=0; virtual void ord10567(void*,int32_t,uint32_t,int32_t)=0;
 virtual int32_t ord10297(void*,int32_t,uint32_t,int32_t)=0; virtual void sub_34D70(void*,int32_t*,int32_t*)=0;
 virtual void ord10510(void*,int32_t)=0; virtual int32_t ord10121(void*,int32_t,int32_t,int32_t)=0;
 virtual void ord11091(void*,int32_t,int32_t,int32_t)=0; virtual void sub_2F0A0(void*,int32_t,int32_t,int32_t)=0;
 virtual void sub_C2790(int32_t,int32_t,int32_t)=0;          // FUN_6fb72790
 virtual void ord10065(uint32_t)=0; virtual void ord10043(int32_t,int32_t)=0;
 virtual void copyString10003()=0; virtual int32_t ord10028()=0; // 36580: current string, then its width
 virtual uint32_t ord10190(int32_t,int32_t,int32_t)=0;
 virtual void ord10014(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t)=0;
 virtual int32_t ord10031()=0; virtual uint32_t sub_BF6C0()=0;  // FUN_6fb6f6c0
};
struct State {
 Services& api; int32_t height{}, width{};          // DAT_6fb8bc4c, DAT_6fb8bc48
 uint32_t h438{},h43c{},h440{}, g44c{}, g444{}, g_bff8{};
};

// 1.13c D2Client+0x35140 (low confidence: stack/ESI arguments of the ordinals are partly unrecovered)
void sub_35140(State& s,void* p1,int32_t p2,int16_t* p3,uint32_t p4) {
 if(!p2||!p3)return;
 s.api.sub_2C830(); int32_t id=*p3;
 s.api.ord10567(p1,id,p4,0);
 int32_t a=s.api.ord10297(p1,id,p4,0)>>8, b=0; (void)b;
 s.api.sub_34D70(p1,&b,&a);
 if(reinterpret_cast<const uint8_t*>(p3)[0xee*2]==5){ s.api.ord10510(p1,id); s.api.ord10121(p1,id,0,1); s.api.ord11091(p1,id,0,1); }
 else { s.api.ord10121(p1,id,0,0); s.api.ord11091(p1,id,0,1); }
 s.api.sub_2F0A0(p1,id,0,1);
}
// 1.13c D2Client+0x35270 (low confidence, as above)
void sub_35270(State& s,void* p1,int32_t p2,int16_t* p3,int32_t p4) {
 if(!p2||!p3)return;
 int32_t l8=0,l4=0; s.api.sub_2C830(); s.api.sub_34D70(p1,&l8,&l4);
 int32_t id=*p3; s.api.ord10567(p1,id,p4,0); s.api.ord10297(p1,id,p4,0);
 if(reinterpret_cast<const uint8_t*>(p3)[0xee*2]==5){ s.api.ord10510(p1,id); s.api.ord10121(p1,id,p4,1); s.api.ord11091(p1,id,p4,1); }
 else { s.api.ord10121(p1,id,p4,1); s.api.ord11091(p1,id,p4,1); }
 s.api.sub_2F0A0(p1,id,id,p4);
}
// 1.13c D2Client+0x361D0
void sub_361D0(State& s,uint32_t handle) {
 Rec r{}; r.w[13]=handle; // first record: zeroed 72-byte block, +0x34 = DAT_6fbcc438
 r.w[0]=0; s.api.ord10041(r,0,256,-1,5,0);
 r.w[0]=1; s.api.ord10041(r,0,512,-1,5,0);
 r.w[0]=2; s.api.ord10041(r,66,20,-1,5,0);
 r.w[0]=3; s.api.ord10041(r,322,20,-1,5,0);
 r.w[0]=4; s.api.ord10041(r,535,256,-1,5,0);
 r.w[0]=5; s.api.ord10041(r,780,256,-1,5,0);
 r.w[0]=6; s.api.ord10041(r,780,512,-1,5,0);
}
// 1.13c D2Client+0x362E0
void sub_362E0(State& s,uint32_t handle) {
 Rec r{}; r.w[13]=handle; // first record: zeroed 72-byte block, +0x34 = DAT_6fbcc438
 r.w[0]=0; s.api.ord10041(r,0,256,-1,5,0);
 r.w[0]=1; s.api.ord10041(r,256,256,-1,5,0);
 r.w[0]=2; s.api.ord10041(r,0,s.height-0x30,-1,5,0);
 r.w[0]=3; s.api.ord10041(r,256,s.height-0x30,-1,5,0);
 r.w[0]=4; s.api.ord10041(r,320,256,-1,5,0);
 r.w[0]=5; s.api.ord10041(r,576,256,-1,5,0);
 r.w[0]=6; s.api.ord10041(r,320,s.height-0x30,-1,5,0);
 r.w[0]=7; s.api.ord10041(r,576,s.height-0x30,-1,5,0);
}
// 1.13c D2Client+0x359B0
void sub_359B0(State& s,uint32_t handle) {
 Rec r{}; r.w[13]=handle; // first record: zeroed 72-byte block, +0x34 = DAT_6fbcc43c
 r.w[0]=0; s.api.ord10041(r,55,535,-1,5,0);
 s.api.ord10010(58,486,58,532,255,255);
 s.api.ord10010(59,486,59,532,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,130,564,-1,5,0);
 s.api.ord10010(133,445,133,561,255,255);
 s.api.ord10010(134,445,134,561,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,217,575,-1,5,0);
 s.api.ord10010(220,375,220,572,255,255);
 s.api.ord10010(221,375,221,572,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,260,585,-1,5,0);
 s.api.ord10010(263,526,263,582,255,255);
 s.api.ord10010(264,526,264,582,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,310,585,-1,5,0);
 s.api.ord10010(313,486,313,582,255,255);
 s.api.ord10010(314,486,314,582,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,365,565,-1,5,0);
 s.api.ord10010(368,525,368,562,255,255);
 s.api.ord10010(369,525,369,562,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,445,540,-1,5,0);
 s.api.ord10010(448,451,448,537,255,255);
 s.api.ord10010(449,451,449,537,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,530,569,-1,5,0);
 s.api.ord10010(533,525,533,566,255,255);
 s.api.ord10010(534,525,534,566,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,572,575,-1,5,0);
 s.api.ord10010(575,375,575,572,255,255);
 s.api.ord10010(576,375,576,572,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,670,564,-1,5,0);
 s.api.ord10010(673,445,673,561,255,255);
 s.api.ord10010(674,445,674,561,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,740,535,-1,5,0);
 s.api.ord10010(743,486,743,532,255,255);
 s.api.ord10010(744,486,744,532,255,255);
}
// 1.13c D2Client+0x35DF0
void sub_35DF0(State& s,uint32_t handle) {
 Rec r{}; r.w[13]=handle; // first record: zeroed 72-byte block, +0x34 = DAT_6fbcc43c
 r.w[0]=0; s.api.ord10041(r,51,414,-1,5,0);
 s.api.ord10010(54,365,54,411,255,255);
 s.api.ord10010(55,365,55,411,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,127,444,-1,5,0);
 s.api.ord10010(130,325,130,441,255,255);
 s.api.ord10010(131,325,131,441,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,178,464,-1,5,0);
 s.api.ord10010(181,405,181,461,255,255);
 s.api.ord10010(182,405,182,461,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,227,464,-1,5,0);
 s.api.ord10010(230,365,230,461,255,255);
 s.api.ord10010(231,365,231,461,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,284,445,-1,5,0);
 s.api.ord10010(287,405,287,442,255,255);
 s.api.ord10010(288,405,288,442,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,350,415,-1,5,0);
 s.api.ord10010(353,325,353,411,255,255);
 s.api.ord10010(354,325,354,411,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,445,450,-1,5,0);
 s.api.ord10010(448,405,448,446,255,255);
 s.api.ord10010(449,405,449,446,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,503,445,-1,5,0);
 s.api.ord10010(506,325,506,441,255,255);
 s.api.ord10010(507,325,507,441,255,255);
 r.w[0]=0xffffffffu; s.api.ord10041(r,582,415,-1,5,0);
 s.api.ord10010(585,365,585,411,255,255);
 s.api.ord10010(586,365,586,411,255,255);
}

// 1.13c D2Client+0x358E0
void sub_358E0(State& s) { if(s.g44c){ s.api.sub_C2790(0x15,1,0); s.g44c=0; } }
static void release(State& s,uint32_t& h){ if(h){ s.api.ord10065(h); s.api.ord10043(0x7a,0); h=0; } }
// 1.13c D2Client+0x35920
void sub_35920(State& s) { release(s,s.h438); release(s,s.h43c); release(s,s.h440); }
// 1.13c D2Client+0x36580
int32_t sub_36580(State& s,int32_t, int32_t p2) { s.api.copyString10003(); return p2 - s.api.ord10028()/2; }
// 1.13c D2Client+0x36620
void sub_36620(State& s) { s.g_bff8=s.api.ord10190(0,0,0); s.api.ord10014(0,0,s.width,s.height-0x31,int32_t(s.g444),1); }
// 1.13c D2Client+0x37310
void sub_37310(State& s) {
 s.api.ord10031(); // both retail branches release h438 identically
 release(s,s.h438);
 s.h438=s.api.sub_BF6C0(); if(!s.h43c)s.h43c=s.api.sub_BF6C0(); if(!s.h440)s.h440=s.api.sub_BF6C0();
}
// 1.13c D2Client+0x38310 -- text=*(p+0x28) (UTF-16), max=*(int*)(p+0x30), n=*(u16*)(p+0x34)
uint32_t sub_38310(const char16_t* text,int32_t max,uint16_t n) {
 if(!text||!*text)return 0;
 std::string u; for(uint16_t i=0;i<n&&text[i];++i){ uint32_t c=text[i];
  if(c<0x80)u+=char(c); else if(c<0x800){u+=char(0xc0|c>>6);u+=char(0x80|(c&63));}
  else{u+=char(0xe0|c>>12);u+=char(0x80|((c>>6)&63));u+=char(0x80|(c&63));} }
 uint32_t v=uint32_t(int32_t(std::atol(u.c_str()))); return max<int32_t(v)?0:v;
}
}
