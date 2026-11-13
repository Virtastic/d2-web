// Declarations and types for d2client_batch_3ceb0.cpp (list walks, digit-key / action-slot helpers, D2Client+0x3CEB0..0x3DAA0), shared with the FN probes.
#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
namespace d2client_batch_3ceb0 {
struct Services {
 virtual ~Services()=default;
 // list walk (Ordinals 10947 first, 10858 data, 10353 count, 10273 next)
 virtual void* first(void* list)=0; virtual int16_t* data(void* node)=0;
 virtual int32_t count(void* node)=0; virtual void* next(void* node)=0;
 virtual int32_t queryAC()=0; virtual void sendPacket(const uint8_t*)=0;   // 0xC42A/0x143E0 packet send
 virtual int32_t c42a(uint32_t esi,uint32_t str,uint32_t line)=0;  // 0xC42A(esi,str,line) -> value kept in the id table
 virtual void c766(uint32_t esi,int32_t* out,uint32_t n)=0;      // 0xC766(esi,&aC38[n],0,0) fills aC38[n]
 virtual int32_t ord10601()=0; virtual void ord10537()=0;     // used by the 3D470/3DAA0 probes
 virtual void release(uint32_t h)=0; virtual void ord10043(int32_t,int32_t)=0;
 virtual int32_t textWidth()=0; virtual void drawText(int32_t,int32_t,int32_t)=0;
 virtual std::u16string label(void* ctx,int32_t)=0;
 virtual std::u16string str10003()=0; virtual void fab17f0()=0;
 virtual void draw10085(const std::u16string&,int32_t y,int32_t mode,int32_t)=0;
 virtual int32_t ord10787(int32_t,int32_t)=0;   // (eax, esi) at the 0xCB74 thunk: the skill use state
 virtual int32_t ord10847(int32_t)=0; virtual uint32_t alloc6f6c0()=0;
 virtual std::u16string name10265(int32_t)=0; virtual void fatal(const std::u16string&)=0;
};
struct State {
 Services& api;
 int32_t a968[240]{}, aA58[240]{}, aB48[240]{}, aC38[240]{}; uint8_t n9d28{}; uint32_t d9d29{}; // byte at 9d28 (digit count), dword at 9d29
 int32_t a9d30[16]{}, a9e20[16]{}, a9dd0[16]{}; int32_t g_cc420{};
 uint8_t handles[7][34]{}; uint32_t h_cc41c{}; uint16_t idx991c{};
 void* list_cbbfc{}; uint8_t mask10{}; // gdwBitMasks_exref[0x10] low byte (low confidence)
 uint8_t table2e0[0x480]{}; int32_t mouseX{}, mouseY{}; int32_t g_c985c{}, g_c97f8{};
 std::u16string textBuf;
};
template<class T> T rd(const void*p,unsigned o){T v;std::memcpy(&v,(const uint8_t*)p+o,sizeof v);return v;}

int32_t sub_3CFA0(State& s,int32_t key,int32_t v2,uint32_t esi,int32_t p1,int32_t p2);
void sub_3D080(State& s,int32_t eax,int32_t p1);
void sub_3D180(State& s,int32_t p);
void sub_3D320(State& s,const int16_t* p1,int32_t p2);
void sub_3D470(State& s);
uint32_t sub_3DA60(State& s,int32_t eax,int32_t esi);
uint32_t sub_3DAA0(State& s,const int16_t* eax);
}
