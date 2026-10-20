// Prior art: none (D2MOO has no D2Client module); Ghidra decompile only.
// Imported-DLL and unresolved-callee calls are seams (unverified).
#include <cstdint>
namespace d2client_batch_4c900 {
struct Unit { int32_t type, classId; uint8_t pad[0x24]; int32_t f2c; };
struct Obj { int32_t kind; uint8_t pad[0x14]; Unit* p5; };
struct Client { int32_t kind; uint8_t p1[0x28]; int32_t h0b, h18, h19, f5; };
struct Services {
 virtual ~Services()=default;
 virtual void s19700()=0; virtual void sA5C50()=0; virtual void sA8630()=0;
 virtual void ord10820(int32_t)=0; virtual void ord10505()=0; virtual void ord11016()=0;
 virtual void ord10692(int32_t,int32_t)=0; virtual int32_t ord10327()=0;
 virtual void ord10043(int32_t,int32_t,int32_t)=0;
 virtual bool dbOn()=0;                       // DAT_6fbcc3b8 != 0
 virtual int32_t classCount()=0; virtual int32_t classToId(int32_t)=0;   // sgptDataTables +0xa80 / +0xa78 (+2 short)
 virtual int32_t ord10522()=0; virtual uint8_t ord10042(int32_t)=0; virtual uint32_t ord10266(uint8_t)=0;
 virtual void ord10740(uint32_t,int32_t*,int32_t*)=0;
 virtual int32_t x(void*)=0; virtual int32_t y(void*)=0;
};
// 1.13c D2Client+0x4C900
void sub_4C900(Services& s, int32_t* e) {
 if(!e) return;
 s.s19700(); s.sA5C50(); s.ord10820(e[0x18]);
 bool db=s.dbOn(); e[0x18]=0;
 if(db){ s.ord10505(); s.ord11016(); if(e[0x19]){ s.sA8630(); e[0x19]=0; } }
 s.ord10692(0,e[0xb]); e[0xb]=0;
 if(s.ord10327()) s.sA8630();
 if(e[0]==1 && e[5]){ int32_t* u=reinterpret_cast<int32_t*>(static_cast<intptr_t>(e[5])); if(u[0xb]){ s.ord10043(0x66,0,0); u[0xb]=0; } }
}
// 1.13c D2Client+0x4C990
uint8_t sub_4C990(Services& s, void* a, int32_t* b) {
 if(!b||b[0]!=1) return 0;
 int32_t id=b[1]; if(id<0) return 0;
 if(id<s.classCount()){ int32_t r=s.classToId(id); if(r!=-1) id=r; }
 if(id<0||id>=s.classCount()||id!=0x12a) return 0;
 if(s.ord10522()>=2) return 0;
 uint8_t u=s.ord10042(b[0xb]); uint32_t k=s.ord10266(u);
 int32_t o[2]; s.ord10740(k&0xff,&o[0],&o[1]);   // decompile stack-garbled: out pair
 int32_t bx=s.x(b), by=s.y(b), ax=s.x(a);
 if(bx+o[0]==ax && by+o[1]==s.y(a)) return u;
 return 0;
}
}
