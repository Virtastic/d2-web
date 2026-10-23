// 1.13c Fog+0x0E420 -- QServer refcount release: on last ref, log via ctx hook, free payload, finalize.
#include <cstdint>

struct FogQServerRef { uint32_t payload; uint32_t pad; int32_t refs; };  // refs at +8
struct FogQServerCtx { uint8_t pad[0xBC8]; void (*logHook)(const char*, int); };  // hook at +0xBC8

// Seams for Fog+0xE1B0 (release payload) and Fog+0xE3B0 (finalize); set by the host.
void (*fog_e1b0)(uint32_t) = nullptr;
void (*fog_e3b0)() = nullptr;

// obj = EAX, ctx = ESI in retail (register convention).
void fog_qserverRelease(FogQServerRef* obj, FogQServerCtx* ctx) {
    if (--obj->refs != 0) return;
    if (ctx->logHook) ctx->logHook("..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x374);
    if (fog_e1b0) fog_e1b0(obj->payload);
    if (fog_e3b0) fog_e3b0();
}
