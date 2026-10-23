// 1.13c Fog+0x1CB50
// Clock/critical-section initialiser. Initialises the Fog critical section (DAT_6ff816d8), then
// self-tests the 64-bit atomic helpers: write {0,0x29a} into a scratch pair with Fog+0x1C9C0, read
// it back with Fog+0x1C9A0 into a pair preset to -1; both halves must match (0x29a, 0). On success
// the stamp DAT_6ff816d0 is stored (Fog+0x1C9C0) as {_time32(0), GetTickCount()}. Otherwise
// FogError(6, "", 0x3c, "Unrecoverable internal error %08x") is raised and the process _exit(-1)s.
#include <stdint.h>

struct FogInitClockApi {
    void (*init_critical_section)(void* cs);                 // KERNEL32 InitializeCriticalSection
    void (*write64)(uint32_t* dst, const uint32_t* src);     // Fog+0x1C9C0
    void (*read64)(uint32_t* out, const uint32_t* src);      // Fog+0x1C9A0
    int32_t (*time32)(int32_t* timer);                       // CRT _time32
    uint32_t (*tick_count)(void);                            // KERNEL32 GetTickCount
    void (*fail)(void);   // Fog+0xFC20, ordinal 116, FogError(6,"",0x3c,fmt), Fog+0x184A0, _exit(-1)
};

extern "C"
void fog_sub_1cb50(const FogInitClockApi* api, void* cs, uint32_t* stamp) {
    api->init_critical_section(cs);
    uint32_t scratch[2] = {0, 0};
    const uint32_t value[2] = {0x29a, 0};
    uint32_t got[2] = {0xffffffffu, 0xffffffffu};
    api->write64(scratch, value);
    api->read64(got, scratch);
    if (got[0] == 0x29a && got[1] == 0) {
        const uint32_t fresh[2] = {(uint32_t)api->time32(nullptr), api->tick_count()};
        api->write64(stamp, fresh);
        return;
    }
    api->fail();
}
