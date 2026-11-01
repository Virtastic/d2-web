// Declarations for d2client_batch_38410.cpp (keybinding and small-widget helpers, D2Client+0x38410..0x3CCC0), shared with the FN probes.
#pragma once
#include <cstdint>
namespace d2client_batch_38410 {
struct Bind { int32_t cmd; uint16_t key; int32_t mod; };      // 10-byte packed record at D2Client+0xCA2E0 (key @+4, mod @+6 in file)
constexpr unsigned kBinds = 114;                              // (0xCA754-0xCA2E0)/10
struct Cmd { uint32_t a; uint32_t handler; uint32_t flag; };  // 12-byte records at D2Client+0xA4F10
struct Services {
 virtual ~Services()=default;
 virtual bool badCodePtr(uint32_t)=0; virtual void callHandler(uint32_t)=0;
 virtual void ord511(void*)=0; [[noreturn]] virtual void fatal(int32_t)=0;
 virtual bool f2060()=0; virtual bool f2030()=0; virtual bool f20e0()=0; virtual bool f2090()=0;
 virtual void ord10003(uint32_t key)=0;           // low confidence: decompile shows no args
 virtual uint8_t jumpByte(uint32_t key)=0;        // byte table at D2Client+0xEA15C (not recovered)
 virtual void f391e0(uint32_t idx,int32_t mod)=0;
 virtual void ord(int32_t n,uint32_t eax,uint32_t ecx,uint32_t edx)=0; // Storm stdcall(eax,ecx,edx): 512-515, 414-417
 virtual void sendClose(void* hwnd)=0; virtual void destroyObj(void* o)=0;
 virtual void ord10043(int32_t a,int32_t b)=0; virtual void ord10065(int32_t)=0;
 virtual int32_t listFirst()=0; virtual int16_t* listData(int32_t)=0; virtual int32_t listCount(int32_t)=0; virtual int32_t listNext(int32_t)=0;
};
struct State { Services& api; Cmd cmds[0xE0]{}; Bind binds[kBinds]{}; int32_t mode{}; };


void sub_38410(Services& api,uint32_t* p,uint32_t vtA,uint32_t vtB);
void sub_38770(Services& api,uint32_t* p,uint32_t vt,uint32_t baseVt);
void sub_38AD0(State& s,uint8_t* msg);
bool sub_38CD0(uint32_t esi);
bool sub_39BA0(State& s,uint16_t ax,uint16_t* esi,int32_t edi);
void sub_39F30(State& s,int32_t eax,int32_t edx);
bool sub_3A270(State& s,int32_t cmd,int32_t mod,int16_t key,uint16_t* out);
void sub_3CA80(Services& api,uint32_t handle,int32_t sel,uint32_t id,uint32_t edx);
void sub_3CCC0(Services& api,uint32_t count,uint32_t handle,const uint32_t* table);
}
