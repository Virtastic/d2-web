// 1.13c D2Client+0xACE20 (491 B): server-packet dispatch loop, from the cached decompile.
// Buffer arrives in EAX (usercall), remaining length on the stack. Imports/tables are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientPktEntry { void (*handler)(); uint32_t size; uint32_t flag; };  // 12 B @ 0x6FB8DE60

struct D2ClientAce20Env {
    const D2ClientPktEntry* tbl;                       // 0x6FB8DE60, 0xAF entries
    uint32_t (*getTick)();                             // GetTickCount (result unused)
    int32_t (*pktSize)(const uint8_t* p, uint32_t* n); // Ordinal_10033 (unverified ordinal), 0 = bad
    uint32_t* bigGapCount;                             // 0x6FBCC4EC
    uint32_t* passCount;                               // 0x6FBCC4E4
    uint32_t* bytesTotal;                              // 0x6FBCC4DC
    uint8_t* lastPkt;                                  // 0x6FBD3414
    void** unitHash;                                   // 0x6FBBA808, 128 buckets; next at word 0x39, id word 3, kind word 0
    void* (*findUnit)(uint32_t id, void* tbl, uint32_t kind); // D2Client+0x54E20
    void* unitTbl;                                     // 0x6FBBA608, stride 0x80
    void (*applyUnit)(void* unit, const uint8_t* p);   // D2Client+0x5C630
    void (*p18)();                                     // D2Client+0xAC240
    void (*p95)();                                     // D2Client+0xAC1B0
    void (*p96)();                                     // D2Client+0xAC140
    void (*fatal)(uint32_t code);                      // Ordinal_10265/10024 + _exit(-1) (unverified ordinals)
};

static void* find_hash(D2ClientAce20Env* e, uint32_t id) {
    for (uint32_t* u = (uint32_t*)e->unitHash[id & 0x7f]; u; u = (uint32_t*)(uintptr_t)u[0x39])
        if (u[3] == id) return u;
    return nullptr;
}

// 1.13c D2Client+0xACE20
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ace20(const uint8_t* p, int32_t len, D2ClientAce20Env* e) {
    if (len == -1) return;
    e->getTick();
    if (len > 500) ++*e->bigGapCount;
    while (len > 0) {
        uint8_t op = p[0];
        uint32_t n;
        if (!e->pktSize(p, &n)) return;
        if (op > 0xae) { e->fatal(0x1448); return; }
        const D2ClientPktEntry& t = e->tbl[op];
        if (t.size != 0xffffffffu && (n & 0xffff) != t.size) { e->fatal(0x144f); return; }
        if (t.flag) {
            uint32_t a, b; memcpy(&a, p + 1, 4); memcpy(&b, p + 2, 4);
            void* u;
            if (op >= 0x67 && op <= 0x6d) {
                u = find_hash(e, a);
                if (u && *(uint32_t*)u != 1) { e->fatal(0x5a); return; }
            } else u = e->findUnit(b, (uint8_t*)e->unitTbl + p[1] * 0x80, p[1]);
            if (u) e->applyUnit(u, p);
        }
        switch (op) {
        case 0x0d:
            if (p[1] == 1) { uint32_t id; memcpy(&id, p + 2, 4); void* u = find_hash(e, id);
                if (u && *(uint32_t*)u != 1) { e->fatal(0x5a); return; } }
            break;
        case 0x18: e->p18(); break;
        case 0x95: e->p95(); break;
        case 0x96: e->p96(); break;
        }
        if (t.handler) t.handler();
        n &= 0xffff;
        *e->bytesTotal += n; p += n; *e->lastPkt = op; len -= (int32_t)n;
    }
    ++*e->passCount;
}
