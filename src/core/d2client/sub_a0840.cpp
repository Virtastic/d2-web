// D2Client 1.13c sub_A0840: guarded forward to the unit-event add (0x214B0).
#include <cstdint>

struct D2cA0840Env {
    void* context;
    // Reads the dword at D2Client+0xBEC54 + index*4 (retail table of two slots).
    uint32_t (*slot)(void*, uint32_t index);
    // D2Client+0x214B0; ESI is the caller's, passed through untouched.
    uint32_t (*addEvent)(void*, uint32_t p1, uint32_t p2, uint32_t p3);
};

// 1.13c D2Client+0xA0840
// index is EAX (register arg); p1..p3 are the three stack args.
uint32_t d2client_a0840_forward(D2cA0840Env& e, int32_t index, uint32_t p1, uint32_t p2, uint32_t p3) {
    if (index >= 0 && index < 2 && e.slot(e.context, (uint32_t)index) != 0)
        return e.addEvent(e.context, p1, p2, p3);
    return 0;
}
