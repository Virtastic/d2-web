// Declarations for d2client_batch_35140.cpp, shared with the FN probes in src/core/fn.
#pragma once
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
 virtual void ord10065(uint32_t)=0; virtual void ord10043(uint32_t ecx,uint32_t edxRva,int32_t,int32_t)=0; // fastcall: ecx=handle, edx=D2Client+edxRva
 virtual void copyString10003(int32_t p1)=0; virtual int32_t ord10028()=0; // 36580: D1EC(ecx=p1) -> string, then D35A(ecx=local buffer) -> width
 virtual uint32_t ord10190(int32_t,int32_t,int32_t)=0;
 virtual void ord10014(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t)=0;
 virtual int32_t ord10031()=0; virtual uint32_t sub_BF6C0(uint32_t esiRva)=0;  // FUN_6fb6f6c0, esi = D2Client+esiRva
};
struct State {
 Services& api; int32_t height{}, width{};          // DAT_6fb8bc4c, DAT_6fb8bc48
 uint32_t h438{},h43c{},h440{}, g44c{}, g444{}, g_bff8{};
};
void sub_35140(State& s,void* p1,int32_t p2,int16_t* p3,uint32_t p4);
void sub_35270(State& s,void* p1,int32_t p2,int16_t* p3,int32_t p4);
void sub_361D0(State& s,uint32_t handle); void sub_362E0(State& s,uint32_t handle);
void sub_359B0(State& s,uint32_t handle); void sub_35DF0(State& s,uint32_t handle);
void sub_358E0(State& s); void sub_35920(State& s);
int32_t sub_36580(State& s,int32_t p1,int32_t p2); void sub_36620(State& s); void sub_37310(State& s);
uint32_t sub_38310(const char16_t* text,int32_t max,uint16_t n);
}
