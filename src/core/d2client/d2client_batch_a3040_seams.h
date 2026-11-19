// Seams of the D2Client 0xA3040..0xA4B00 batch (shared with the FN replay probes).
#pragma once
#include <cstdint>
namespace d2client_batch_a3040 {
struct Seams {
 virtual ~Seams()=default;
 virtual void fatal(int32_t msg)=0;                       // Ordinal_10265 + 10024 + _exit(-1)
 virtual void sa750(uint32_t kind, uintptr_t p, int32_t* rec, uint32_t one)=0; // FUN_6fada750: ecx=kind, edx=p, stack (rec, 1), ret 8 // FUN_6fada750(0x16,..)
 virtual uintptr_t ord10511(uintptr_t)=0; virtual int32_t ord10494(uintptr_t,int32_t)=0;
 virtual uint32_t ord10397(uintptr_t)=0; virtual int32_t ord10601(uintptr_t,int32_t line)=0;
 virtual int32_t skillsCount()=0; virtual uintptr_t skillsRow(int32_t idx)=0; // sgptDataTables+0xb98/0xba0, stride 0x23c
 virtual void sb4c0()=0; virtual void sb6450()=0; virtual void sb7290()=0;
 virtual uintptr_t ord11013(int32_t,int32_t,int32_t,uint32_t,uint32_t)=0;
 virtual void ord10911(uintptr_t,void(*)(uint32_t))=0; virtual void ord10807(uintptr_t edi, uintptr_t handle, uint32_t one)=0; // 0xC36A: stack (edi, handle, 1)
 virtual uint32_t ord10184(uint32_t ecx)=0; virtual int32_t ord10028(uintptr_t str)=0; // d3a8 (ecx=arg, returns eax) / d35a (ecx=string ptr)
 virtual uintptr_t alloc(int32_t size)=0;                 // Ordinal_10042
 virtual void freeMem(uintptr_t)=0;                       // Ordinal_10043
 virtual uintptr_t ord10048()=0; virtual void sc60(uintptr_t,uintptr_t)=0; virtual bool s2090()=0;
 virtual uintptr_t ord10460(uintptr_t)=0; virtual uintptr_t ord10464(uintptr_t)=0; virtual uintptr_t ord11147(uintptr_t)=0;
 virtual bool s98c0(uint32_t,int32_t)=0;
};
}
