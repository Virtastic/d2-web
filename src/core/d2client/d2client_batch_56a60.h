#pragma once
// D2Client batch 56A60: unit, row and service seam declarations (shared by the batch and the FN probes).
#include <cstdint>
namespace d2client_batch_56a60 {
using I=int32_t; using U=uint32_t;
struct Unit { I type; I pad[0x30]; U flags; U flags2; };   // +0 type; +0xC4 flags (word 0x31); +0xC8 flags2
static_assert(__builtin_offsetof(Unit,flags)==0xc4 && __builtin_offsetof(Unit,flags2)==0xc8);
struct Row { I a[0x4e]; I f138, f13c; };                     // monster-row fields at +0x138/+0x13c only
struct Svc {
 virtual ~Svc()=default;
 virtual Row* row(I idx)=0;            // sgptDataTables+0xb98 table (0x23c stride), bound +0xba0; nullptr if out of range
 virtual const void* ent(I idx)=0;     // sgptDataTables+0xb64 table (0x1a4 stride), bound +0xb6c
 virtual I o10511(Unit*)=0; virtual I o11086(Unit*,I,I,U,void*,I*,I*)=0; virtual void o10554(I,I)=0;
 virtual void o10748(I,I)=0; virtual I o10331(Unit*)=0; virtual I f1400()=0; virtual void o10565(I,I,I,I,I)=0;
 virtual U o10397(I)=0; virtual void o10137(I,I)=0; virtual I o10786(const void*,I,I=0,U=0)=0; virtual void o10542(Unit*,I)=0;
 virtual I unitX(Unit*)=0; virtual I unitY(Unit*)=0;     // X = path+0xc (else ord 10867 / 0), Y = path+0x10 (else ord 10750 / 0); type in {2,4,5} reads the path directly
 virtual void o11073(I room,I x,I y,I a,I b)=0; virtual I entCount()=0;  // sgptDataTables+0xb6c
 virtual const uint8_t* f1260(I eaxUnitId)=0;     // FUN_6fab1260 -> record, byte +0x109 used
 virtual void o10542r(Unit*,I)=0;      // Ordinal_10542 (args REG?)
 virtual void fatal(I line)=0;         // Ordinal_10265/10024(&DAT_6fb8359f) + _exit(-1)
 virtual void f0b0(Unit* eaxUnit)=0;   // FUN_6fada0b0 (EAX = unit)
 virtual I o10392(I)=0;
 virtual void* o10392p(I)=0;
 virtual void o10168(I,I)=0; virtual void o10441(void*,I)=0; virtual void o10558(void*,I)=0; virtual void o10970(void*,I)=0;
 virtual void o10243(I,I)=0; virtual I f1380(U* seed,I esiRange)=0; virtual void o10936(U* seed)=0; virtual void o10614(U* seed,I edx)=0;
 virtual void o10595(Unit*)=0; virtual I o10826(I)=0; virtual I o10459(I)=0; virtual I f1660(Unit*)=0;
 virtual I f1630(Unit*)=0; virtual void f60450(I,I,I,I,U)=0; virtual I o10589(Unit*,I,I)=0;
 virtual I o10075(I handle,Unit*,I r,I x,I y)=0; virtual I o10509(I room,const I* xy,I a)=0;
 virtual void* f51540(Unit* eaxUnit,I code,I p2,I p3)=0; virtual void* f5ff10(Unit* edx,I code,I x,I y,I p2,I p3)=0;  // 0x57540 lookups (VA 6fb51540 / 6fb5ff10; both stdcall, p2/p3 on the stack)
 virtual I* gfxBlock(void* obj)=0;     // obj null/+0x54 null/(+0x54)->+0x30 null -> nullptr, else that block
 virtual I o10630()=0; virtual I o10980(I)=0; virtual void f6c0(I ediArg)=0;
};
void sub_56DA0(Svc& s,const uint8_t* p1,Unit* esi,I* w);
I sub_56B20(Svc& s,Unit* u,I idx,U a4);
// sgptDataTables+0xb98 row idx (stride 0x23c, bound +0xba0): retail tests idx<0, idx>=count, then the row address against 0 (32-bit words).
inline Row* rowAt(const uint8_t* dt,I idx){
 U n,b; __builtin_memcpy(&n,dt+0xba0,4); __builtin_memcpy(&b,dt+0xb98,4);
 if(idx<0||idx>=(I)n) return nullptr; U a=b+(U)idx*0x23cu; return a?(Row*)(uintptr_t)a:nullptr;
}
I sub_57000(Svc& s,Unit* u);
}
