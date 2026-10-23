#include <cstdint>
#include <emscripten.h>

// Host seams: DAT_6f9014a4 (list head), DAT_6f9014a8 (busy counter), gdwBitMasks, Fog #10043 free.
struct D2Win17970Env {
    uint8_t** head;
    const int32_t* busy;
    const uint32_t* bitMasks;
    void (*fogFree)(void* node, int32_t line, int32_t zero);
};

// 1.13c D2Win+0x17970
// If busy < 1 and the list (next at +0x3c, flags at +8) is non-empty: drop leading nodes lacking
// bitMasks[0] (free line 0x249), then drop nodes up to the first with bitMasks[1] (free line 0x252). Returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_17970(const D2Win17970Env* e) {
    if (*e->busy >= 1 || !*e->head) return 1;
    while (!(e->bitMasks[0] & *(uint32_t*)(*e->head + 8))) {
        uint8_t* cur = *e->head;
        *e->head = (uint8_t*)(uintptr_t)*(uint32_t*)(cur + 0x3c);
        e->fogFree(cur, 0x249, 0);
        if (!*e->head) return 1;
    }
    do {
        if (e->bitMasks[1] & *(uint32_t*)(*e->head + 8)) return 1;
        uint8_t* cur = *e->head;
        *e->head = (uint8_t*)(uintptr_t)*(uint32_t*)(cur + 0x3c);
        e->fogFree(cur, 0x252, 0);
    } while (*e->head);
    return 1;
}
