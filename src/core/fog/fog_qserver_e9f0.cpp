// QServer connection table lookup (Fog.dll). Pointers are wasm64-sized; layout is by field, not offset.
#include <cstdint>
struct FogQSrvConn { uint32_t id; FogQSrvConn* next; uint32_t refs, closed, sock, pad[3]; uint32_t field8; };
struct FogQSrvTable { uint32_t mask; FogQSrvConn** buckets; };
// 1.13c Fog+0x0E4E0 (bucket walk; retail also takes the per-bucket critical section)
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
