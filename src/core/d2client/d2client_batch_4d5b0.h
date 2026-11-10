#pragma once
#include <cstdint>
#include <cstring>
namespace d2client_batch_4d5b0 {
template<class T> static T rd(const uint8_t* p,unsigned o){T v;std::memcpy(&v,p+o,sizeof v);return v;}
template<class T> static void wr(uint8_t* p,unsigned o,T v){std::memcpy(p+o,&v,sizeof v);}

struct Svc {
 virtual ~Svc()=default;
 [[noreturn]] virtual void fatal(int32_t code)=0;           // Ordinal_10265(code)+Ordinal_10024 + _exit(-1)
 // 0x4D5B0
 virtual void* o10421(uint8_t* u)=0; virtual int32_t o10034(uint8_t* u)=0;   // both stdcall(EDI)
 virtual void o10292(void*,int32_t,int32_t,int32_t*,int32_t*,int32_t*,int32_t*)=0;
 virtual void sub6A220()=0;                                 // D2Client+0x6A220
 // 0x4DA20
 virtual int32_t* sub560D0(uint32_t a)=0;                   // D2Client+0xA60D0
 virtual void sub61420(int32_t* p,int32_t v)=0;             // D2Client+0xB1420
 // 0x4EF50
 virtual uint32_t o10988(uint32_t)=0; virtual int32_t o10458(uint32_t,uint32_t,uint32_t,void*)=0;
 virtual uint8_t* o11156(uint32_t,uint32_t,uint32_t)=0;
 virtual int32_t dtRows()=0;                                // sgptDataTables +0xba0
 virtual void o10302(int32_t*,int32_t,int32_t,int32_t,const char*,int32_t)=0;
 virtual uint32_t o10630(int32_t)=0; virtual void o10368(uint32_t)=0;
 virtual void o10003()=0; virtual void o10971()=0; virtual void sub32FE0(int32_t*)=0; // D2Client+0x82FE0
 virtual uint32_t g6fbcc390()=0; virtual uint32_t g6fbc9854()=0;
 // 0x4F720
 virtual void sub0DE90(uint32_t)=0;                         // D2Client+0x5DE90
 // 0x4FCE0..0x4FE20
 virtual uint8_t* sub4D630(uint32_t a,int32_t b)=0;         // D2Client+0x9D630
 virtual void* alloc(int32_t sz,int32_t z)=0;               // Ordinal_10042
 // 0x4FF10 globals
 virtual int32_t g8bc48()=0; virtual int32_t g8bc4c()=0; virtual int32_t gcb828()=0; virtual int32_t gcb824()=0;
 // 0x4A8C0 (all callees unrecovered or imports; unverified seams)
 virtual void resetA()=0;                                   // stores of 0x10/0 to 6fba46c4/c8, 6fbcc3dc/e0/f8/fc/2f8/2f4
 virtual int32_t* getCur()=0;                               // 0x46150
 virtual void s48150()=0; virtual void s46070()=0; virtual void s6e350(int32_t,int32_t)=0;
 virtual int32_t o10973(int32_t*,int32_t,int32_t)=0; virtual void o10227()=0;
 virtual uint32_t& gaada0()=0; virtual int32_t g8d5d0()=0; virtual int32_t gc3a68()=0;
 virtual uint8_t* gaad80()=0; virtual int32_t entryType(uint32_t i)=0; // PTR_DAT_6fb8d538[i]+0x20
 virtual void s72790(uint32_t,int32_t,int32_t)=0; virtual int32_t gc9721()=0; virtual void s47b20()=0;
 virtual int32_t*& gcbbfc()=0; virtual int32_t s55440()=0; virtual void s6e2e0()=0; virtual void s492f0()=0;
 virtual void s48930()=0; virtual int32_t s45c00()=0; virtual uint8_t* ga37bd(int32_t idx)=0; // [idx*0x16]
 virtual int32_t s4cce0(int32_t)=0; virtual void s22eb0()=0; virtual void s38a70(int32_t*,int32_t,int32_t,int32_t)=0;
 virtual uint8_t* gc9663()=0; virtual uint8_t* gc9729()=0; virtual int32_t* gc966b()=0;
};
void sub_4D5B0(Svc& s,uint8_t* edi,uint32_t ebp);
}
