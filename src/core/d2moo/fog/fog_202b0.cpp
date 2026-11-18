// 1.13c Fog+0x202B0: D2QServer worker shutdown. Retail: ESI = server object; the object keeps retail's 32-bit field layout.
// Under the section at +0x854 collect the live worker handles (four at +0xBD8, four at +0xC1C when +0xC18 is set), raising each
// slot's stop flag (+0xBF8.. and +0xC3C..); wait on them (1000 ms), then close each and clear the handle and its +0x10 / +0x54 word.
#include <cstdint>
#include <cstring>

struct Fog202B0Env {
    void (*csPrep)(void* cs, uint32_t line);                              // Fog+0x11690 (validator, EDX = 0xCE)
    void (*csEnter)(void* cs);                                            // EnterCriticalSection
    void (*csLeave)(void* cs);                                            // LeaveCriticalSection
    void (*waitAll)(uint32_t n, const uint32_t* handles, uint32_t ms);    // WaitForMultipleObjects(n, h, TRUE, ms)
    void (*closeHandle)(uint32_t handle);                                 // CloseHandle
};

static inline uint32_t rd(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void wr(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }

extern "C" void fog_202b0(uint8_t* o, const Fog202B0Env* e) {  // 1.13c Fog+0x202B0
    uint8_t* cs = o + 0x854;
    uint32_t h[8];
    uint32_t n = 0;
    e->csPrep(cs, 0xce);
    e->csEnter(cs);
    for (uint32_t i = 0; i < 4; i++) {
        uint32_t a = rd(o + 0xbd8 + 4 * i);
        if (a) h[n++] = a;
        wr(o + 0xbf8 + 4 * i, 1);
        if (rd(o + 0xc18)) {
            uint32_t b = rd(o + 0xc1c + 4 * i);
            if (b) h[n++] = b;
        }
        wr(o + 0xc3c + 4 * i, 1);
    }
    e->csLeave(cs);
    if ((int32_t)n > 0) e->waitAll(n, h, 1000);
    for (uint32_t i = 0; i < 4; i++) {
        uint8_t* p = o + 0xbd8 + 4 * i;
        if (rd(p)) { e->closeHandle(rd(p)); wr(p, 0); wr(p + 0x10, 0); }
        if (rd(o + 0xc18) && rd(p + 0x44)) { e->closeHandle(rd(p + 0x44)); wr(p + 0x44, 0); wr(p + 0x54, 0); }
    }
}
