// 1.13c D2Client+0xA8C20 (265 B): allocate a 0x34-byte list node and push it on the list head
// (DAT_6fbcbbec, next = +0x28); count in DAT_6fbcbbf0. Register convention (unverified from decompile):
// EBX=kind, EDI=unit*, param_1 on stack = size, ESI/EBP-low/retaddr bytes are caller-supplied bytes.
// Ordinals 10042 (alloc), 11080, 11168 unverified (named from decompile only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientA8c20Env {
    uint32_t* head;                    // DAT_6fbcbbec
    uint32_t* count;                   // DAT_6fbcbbf0
    uint8_t* (*alloc)(uint32_t n, uint32_t z);   // Ordinal_10042
    uint32_t (*ord11080)();
    uint32_t (*ord11168)();
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a8c20(
    int32_t size, uint32_t kind, const uint32_t* unit, uint8_t b24, uint8_t b25, uint8_t b26,
    D2ClientA8c20Env* e) {
    if (size <= 0) return 0;
    int32_t n = size > 0x11 ? 0x12 : size;
    ++*e->count;
    uint32_t* p = (uint32_t*)e->alloc(0xff, 0);
    p[0] = unit ? unit[0] : 6;
    p[1] = unit ? unit[3] : 0xffffffffu;
    ((uint8_t*)p)[8] = (unit && ((unit[0x31] >> 0x15) & 1)) ? 1 : 0;
    p[4] = (e->ord11080() >> 0xd) + 4;
    p[5] = (e->ord11168() >> 0xd) + 4;
    ((uint8_t*)p)[0x24] = b24; (void)b25; // decompile writes +0x24 from ESI-low
    p[6] = p[7] = (uint32_t)n * 8;
    ((uint8_t*)p)[0x25] = (uint8_t)((unit && ((unit[0x31] >> 0x15) & 1)) ? 1 : 0); // BP-low = that bit
    ((uint8_t*)p)[0x26] = b26;
    p[3] = kind; p[8] = 0;
    ((uint8_t*)p)[0x27] = (uint8_t)size;
    p[10] = *e->head; p[11] = 0;
    p[0xc] = kind == 2 ? (uint32_t)(uintptr_t)e->alloc(0x116, 0) : 0;
    *e->head = (uint32_t)(uintptr_t)p;
    return (uint32_t)(uintptr_t)p;
}
