// QServer connection table lookup (Fog.dll). Pointers are wasm64-sized; layout is by field, not offset.
#include <cstdint>
struct FogQSrvConn { uint32_t id; FogQSrvConn* next; uint32_t refs, closed, sock, pad[3]; uint32_t field8; };
struct FogQSrvTable { uint32_t mask; FogQSrvConn** buckets; };
// bucket walk by field (the retail-layout row is fog_sub_e4e0 below)
static FogQSrvConn* fog_qsrv_find(FogQSrvTable* t, uint32_t id) {
    if (!t) return nullptr;
    FogQSrvConn* c = t->buckets[t->mask & id];
    while (c && c->id != id) c = c->next;
    return c;
}
// 1.13c Fog+0x0E9F0
uint32_t fog_qsrv_e9f0(FogQSrvTable* t, uint32_t id) {
    if (id) if (FogQSrvConn* c = fog_qsrv_find(t, id)) return c->field8;
    return 0;
}
// 1.13c Fog+0x0E4E0 -- retail layout: mask +0xC, bucket heads +0x14, per-bucket lock +0x1C (0x18 stride, a no-op
// here: single-threaded); node key +0, next +4. A null table returns 0.
extern "C" void FUN_6ff61690(void*);
extern "C" void EnterCriticalSection(void*);
extern "C" void LeaveCriticalSection(void*);
extern "C" uint32_t* fog_sub_e4e0(const uint8_t* tbl, uint32_t key) {
    if (!tbl) return nullptr;
    const uint32_t b = *(const uint32_t*)(tbl + 0xc) & key;
    uint8_t* cs = (uint8_t*)(uintptr_t)*(const uint32_t*)(tbl + 0x1c) + b * 0x18;
    FUN_6ff61690(cs);
    EnterCriticalSection(cs);
    uint32_t* n = (uint32_t*)(uintptr_t)((const uint32_t*)(uintptr_t)*(const uint32_t*)(tbl + 0x14))[b];
    while (n && *n != key) n = (uint32_t*)(uintptr_t)n[1];
    LeaveCriticalSection(cs);
    return n;
}
