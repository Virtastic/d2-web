// 1.13c D2Client+0x4C900: EAX = unit. Releases a client unit's attached resources: the 0x69700 / 0xA5C50 pair, the +0x60 handle (D2Common 10820),
// then (flag DAT_6FBCC3B8) D2Common 10505 / 11016 and the +0x64 object (0xA8630), the +0x2C handle (D2Common 10692), D2Common 10327's result (0xA8630) and, for a type-1 unit
// with pData+0x2C set, Fog 10043(0x66, 0).
#include <emscripten.h>
#include <cstdint>
struct D2c4c900Env {
    int32_t flag;                                   // DAT_6FBCC3B8
    void (*f69700)(const int32_t* unit);            // EAX = unit
    void (*fa5c50)(const int32_t* unit);            // EBX = unit, stack 1
    void (*o10820)(uint32_t h);
    void (*o10505)(const int32_t* unit);
    void (*o11016)(const int32_t* unit);
    void (*fa8630)(uint32_t obj);                   // ESI = object
    void (*o10692)(uint32_t a, uint32_t h);
    uint32_t (*o10327)(const int32_t* unit, uint32_t a);
    void (*o10043)(uint32_t a, uint32_t b);
};
extern "C" void d2_client_4c900(int32_t* u, const D2c4c900Env* e) {
    if (!u) return;
    e->f69700(u);
    e->fa5c50(u);
    e->o10820((uint32_t)u[0x18]);
    u[0x18] = 0;
    if (e->flag) {
        e->o10505(u);
        e->o11016(u);
        if (u[0x19]) { e->fa8630((uint32_t)u[0x19]); u[0x19] = 0; }
    }
    e->o10692(0, (uint32_t)u[0xb]);
    u[0xb] = 0;
    if (uint32_t r = e->o10327(u, 0)) e->fa8630(r);
    if (u[0] == 1) {
        int32_t* pd = (int32_t*)(uintptr_t)(uint32_t)u[5];
        if (pd && pd[0xb]) { e->o10043(0x66, 0); pd[0xb] = 0; }
    }
}
