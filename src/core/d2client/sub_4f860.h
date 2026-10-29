// D2Client sub_4F860 seam types (see sub_4f860.cpp).
#pragma once
#include <cstdint>
namespace d2client_4f860 {
using U=uint32_t;
struct Env { // D2Client globals (RVA = VA-0x6fab0000)
 U bbfc,c280,c3b8,c254,c258; // read: game, enable, act-key, table base, table count
 U c310,c314,c318,c31c,c320,c324,c328,c32c; // read/write state
};
struct Services {
 virtual ~Services()=default;
 virtual U rd32(U addr)=0;
 virtual U game(U g)=0;            // +0xC1F0 (#10331)
 virtual U unit(U g)=0;            // +0xC1DE (#10826)
 virtual U act(U key)=0;           // +0xC7E4 (#10396)
 virtual U unitData(U u)=0;        // +0xC214 (#10014)
 virtual bool f80450()=0;          // +0x80450
 virtual bool weather(int32_t* scaled)=0; // 0x4F770 != const; *scaled = ftol(w*k)
 virtual void f88810(U a,U b)=0;
 virtual void f88770(U a,U b)=0;
 virtual void f88230(U eax,U self,U h,U a,U b)=0;
 virtual U f88a70(U row,U kind)=0; // row = EBX (Sounds row); kind 0 area loop, 1 weather handle, 2 one-shot (ours: the seam has no EBX)
 virtual void f87ed0(U h,U v)=0;
 virtual U f87f00(U h)=0;
 virtual void f88bf0(U h)=0;
 virtual void f88ba0(U h,int32_t x,int32_t y,int32_t z)=0;
 virtual U rand(U n)=0;            // 0x89640
 virtual void f5cf90(U v)=0;
 virtual void f4f750()=0;
 virtual void f4f720(U v)=0;
};
void sub_4F860(U now,Env& g,Services& s);
}
