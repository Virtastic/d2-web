// Prior art: none (D2Client has no D2MOO module). Derived from cached 1.13c decompile.
// Env-seam style: ordinal imports and globals come in through Env; structs are 32-bit byte-offset blobs.
#include <cstdint>
#include <cstring>
namespace d2client_a4e80 {
typedef uint32_t U; typedef uint8_t B;
struct Env {
 U  g_6fbcc394;                       // DAT_6fbcc394 (D2Client RVA 0x1CC394), abort gate
 U  (*o10265)(U);                     // string lookup
 void (*o10024)(U,U);                 // fatal message
 void (*exit_)(int);                  // _exit(-1)
 B* (*o10039)(U type,U id);           // unit lookup -> record, +0x2c = source
 U  (*o10331)(const B* unit);
 U  (*f1400)();                       // FUN_6fab1400
 void (*o10075)(B*,B*,U,int16_t,int16_t);
 U  (*o11046)(const B* src);
 void (*o10559)(B*,U);
 U  (*o10057)(U);
 U  (*o10511)(const B*);
 U  (*o10397)(U);
 void (*o10137)(U,U);
 void (*o10054)(B*,U);                // end/cleanup
 void (*o11090)(B*,U);
};
static inline U rd(const B* p,U o){U v;std::memcpy(&v,p+o,4);return v;}
static inline void wr(B* p,U o,U v){std::memcpy(p+o,&v,4);}
// 1.13c D2Client+0xA4E80
// param_1 (ecx): unit; esi: unaff_ESI unit (source-side state). Mirrors the decompile literally.
// ponytail: decompile aliases (puVar8 = *param_1, puVar10 = param_1[0xb]) kept as-is; ordinals unverified.
void sub_A4E80(const Env& e,B* param_1,B* esi){
 if(e.g_6fbcc394){ U s=e.o10265(0xbaa); e.o10024(0x6fb8359f,s); e.exit_(-1); return; }
 B* p8; U t;
 if(!param_1){ p8=reinterpret_cast<B*>(uintptr_t(6)); t=0xffffffffu; }
 else { p8=reinterpret_cast<B*>(uintptr_t(rd(param_1,0))); t=rd(param_1,12); }
 B* rec=e.o10039(reinterpret_cast<uintptr_t>(p8)&0xffffffffu,t);
 if(!rec) return;
 B* p3=reinterpret_cast<B*>(uintptr_t(rd(rec,0x2c)));
 B* p10=reinterpret_cast<B*>(uintptr_t(rd(param_1,0x2c)));
 B* p9=p10;
 U i5=e.o10331(param_1); if(!i5) return;
 int16_t s1,s2; std::memcpy(&s1,p3+6,2); std::memcpy(&s2,p3+2,2);
 int16_t a,b; std::memcpy(&a,p10+2,2); std::memcpy(&b,p10+6,2);
 if(s2!=a||s1!=b){ if(!e.f1400()) return; e.o10075(p10,p9,i5,s2,s1); }
 static const U dw[]={0,1,2,3,9,10,14,4,5,6,0x17,0x18,0x1f,0x20,0x21,0x22,0x23,0x26};
 for(U o:dw) wr(p10,o*4,rd(p3,o*4));
 p10[0x67]=p3[0x67]; p10[0x68]=p3[0x68]; // 0x1a*4 = 0x68
 p10[0x91]=p3[0x91]; p10[0x92]=p3[0x92]; p10[0x93]=p3[0x93]; p10[0x94]=p3[0x94]; // 0x25*4
 wr(p10,0x6a,rd(p3,0x6a)); wr(p10,0x6e,rd(p3,0x6e));
 U n=rd(p3,10*4);
 if(n){ for(U k=0;k<n;k++) wr(p8,(0x27+k)*4,rd(p3,(0x27+k)*4)); p10=p8; }
 n=rd(p3,0x75*4);
 if(n){ for(U k=0;k<n;k++) wr(p8,(0x76+k)*4,rd(p3,(0x76+k)*4)); p10=p8; }
 wr(p10,0x75*4,rd(p3,0x75*4));
 U v=rd(p3,0xf*4); if(v!=8&&v!=0xb) wr(p10,0xf*4,v);
 v=rd(p3,0x10*4);  if(v!=8&&v!=0xb) wr(p10,0x10*4,v);
 wr(p10,0x10*4,rd(p3,0x10*4)); wr(p10,0x11*4,rd(p3,0x11*4));
 e.o10559(p10,e.o11046(p3));
 U u7=rd(esi,16), u12=rd(p8,16), st;
 if(u7==u12){ e.o10054(esi,0); return; }
 if(rd(esi,0)==0){
  switch(u12){
  case 1: case 5:
   if(u7<2||(u7>3&&u7!=6)){ e.o10054(esi,0); return; }
   if(!e.o10057(e.o10331(esi))){ st=1; break; } st=5; break;
  case 2: case 6:
   if(u7!=1&&u7!=5){ e.o10054(esi,0); return; }
   if(!e.o10057(e.o10331(esi))){ st=2; break; } st=6; break;
  case 3:
   if(u7!=1&&u7!=3&&u7!=5){ e.o10054(esi,0); return; }
   st=3; break;
  case 0x12: e.o10054(esi,0); return;
  default:
   if(u7!=0x13){
    U x=e.o10511(esi), y=e.o10511(p8);
    if(x&&((!y||!(e.o10397(y)&1))||(e.o10397(y)&2))&&(u7=e.o10397(x))&1){
     e.o10137(x,u7|2); e.o10054(esi,0); return; }
    e.o10054(esi,0); return;
   }
   st=u12; break;
  }
 } else {
  if(rd(esi,0)!=1){ e.o10054(esi,0); return; }
  if(u12==1){ if(u7!=2&&u7!=0xf){ e.o10054(esi,0); return; } st=1; }
  else if(u12==2){ if(u7!=1){ e.o10054(esi,0); return; } st=2; }
  else { if(u12!=0xf||u7!=1){ e.o10054(esi,0); return; } st=0xf; }
 }
 e.o11090(esi,st); e.o10054(esi,0);
}
}
