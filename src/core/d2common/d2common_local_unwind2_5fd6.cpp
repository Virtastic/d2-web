#include <cstdint>

// 1.13c D2Common+0x05FD6 __local_unwind2(frame, target): walk the scope table (frame+8) from the
// index at frame+0xC until it is -1 or target. Each 12-byte record is {next index, filter, handler};
// the index advances to next first, and a zero filter runs the 0x606A prologue then the handler.
// The SEH chain head swap (FS:[0]) is host-side and not modelled.
struct D2CommonUnwindEnv {
    void (*prologue)();  // FUN 0x606A
};

void d2common_local_unwind2_5fd6(const D2CommonUnwindEnv& e, uint8_t* frame, int32_t target)
{
    for (;;) {
        uint32_t table = *(uint32_t*)(frame + 8);
        int32_t idx = *(int32_t*)(frame + 0xc);
        if (idx == -1 || idx == target) break;
        uint8_t* rec = (uint8_t*)(uintptr_t)table + (uint32_t)idx * 12;
        *(uint32_t*)(frame + 0xc) = *(uint32_t*)rec;
        if (*(uint32_t*)(rec + 4) == 0) {
            if (e.prologue) e.prologue();
            ((void (*)())(uintptr_t)*(uint32_t*)(rec + 8))();
        }
    }
}
