#pragma once
// D2Client batch 47D60 (dialog/menu state machine): the state struct G, the seam struct H and the instances g, h (shared by the batch and the FN probes).
// Derived from cached 1.13c decompile. Globals are named by their 1.13c address (G::aXXXX = 0x6fbaXXXX..);
// every callee not in this batch is a seam in H (UNVERIFIED: arguments the decompiler dropped are marked).
#include <cstdint>
#include <cstring>
#include <cstddef>
namespace d2client_b47d60 {
using U=uint32_t; using I=int32_t; using P=uint8_t*;
static U rd(const P p,U o){U v;std::memcpy(&v,p+o,4);return v;}
#pragma pack(push,1)
struct Menu{I id,count;int16_t s[5];U f[5];uint8_t pad;};
#pragma pack(pop)
static_assert(sizeof(Menu)==0x27,"menu stride");
static_assert(offsetof(Menu,s)==8&&offsetof(Menu,f)==0x12,"menu layout");
enum:U{H6D10=0x6faf6d10,H9440=0x6faf9440,H8380=0x6faf8380,H8350=0x6faf8350,H8290=0x6faf8290,H82F0=0x6faf82f0,H81C0=0x6faf81c0,H9250=0x6faf9250,H8650=0x6faf8650};
struct Ctl{U w[0x18];};
struct G{
 P player=nullptr;                 // DAT_6fbcbbfc
 I bc90=0,bc34=0,a97a8=0,c334=0,v23e8=0,b834=0,c854=0,a3b10=0,a91590=0,a91c3c=0;
 U a9777=0,v973b=0,v971d=0,c390=0,v9747=0,v9743=0,d22fd=0,d22f9=0,t2309=0,t2301=0,d2305=0;
 U v9725=0,c2f4=0,c2f8=0,d22f8=0,v97b4=0,v97b8=0,v97bc=0,v974f=0,v974b=0,c394=0;
 U a9675=0,a9679=0,cbf94=0,cbf98=0,c404=0,c3a4=0,a8c9c=0;
 uint8_t v9729=0,v9663=0,v9661=0,v9662=0,v9648=0;
 Ctl*ctl9763=nullptr,*ctl9767=nullptr,*ctl976b=nullptr,*ctl976f=nullptr,*ctl9773=nullptr;
 U v975b=0,v975f=0,v9757=0; void**v977b=nullptr;
 Menu*tbl=nullptr; I menuCount=0;  // 6fba3ba0, 6fba42f0
 P tab37a8=nullptr;                // 6fba37a8 stride 0x16
 P tab9675rec=nullptr;             // *DAT_6fbc9675 record (+5 ptr,+9 cnt,+0xd,+0x11)
 P tab91400=nullptr;
};
struct H{
 I* (*cur)();                                      // 6150
 U (*o10071)(I,I,I,U,U,I); U (*o10107)(P,P,U,U,I,I); I (*o10705)(P); I (*o11088)(P); I (*o11017)(U);
 U (*tick)(); I (*f5b40)(); I (*f5bd0)(); void (*f6ee0)(I,uint8_t); void (*f38a70)(I,I,I,I); void (*f7500)();
 void (*f67c0)(); void (*f8150)(); void (*f2ad0)(); void (*f3840)(); void (*f42850)(); void (*f3d20)(); void (*f4660)();
 void (*f7920)(); U (*r1380)(P); void (*f6880)(); void (*f7b20)(); void (*f6e00)(); void (*f2d3c0)(I*,int16_t);
 Ctl* (*newCtl)(U,U); void (*addLine)(Ctl*,const char16_t*,I,I,I,U,I); void (*ctlOpen)(Ctl*);
 const char16_t* (*str)(U key);                    // Ordinal_10003; key UNVERIFIED (dropped by decompiler)
 const char16_t* (*title)(); void (*fatal)(U);     // 55d90; Ordinal_10265+10024+_exit
 void (*send)(const uint8_t*,I);                   // 43e0
 I (*o10174)(U,I,I); I (*o10973)(P,I,I); I (*o10913)(P); I (*o10504)(P); I (*o10640)(P);
 I (*o10720)(); U (*o10109)(I,I); I (*rowOf)(U,I,I); I (*o10460)(U); I (*o10464)(I); I (*o11147)(I); U (*o10802)(I);
 U (*lower)(U); I (*f5c00)(); void (*f72790)(I,I,I); void (*f3ca20)(); void (*f4fd70)(); I (*o10227)();
 void (*unitFlag)(P,I);                            // 7b20 side
 P (*unitLookup)(U);                               // slot (&DAT_6fbba808)[id&0x7f]
};
inline G g; inline H h;
}
