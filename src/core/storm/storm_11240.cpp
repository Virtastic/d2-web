// 1.13c Storm+0x11240: dispatch and free every queued SNet user event (head terminator at Storm+0x55A1C, tick bias DAT 0x55830).
// Each node: unlink inline (the TSExplicitList removal 0x4D60 repeats), node[3] += tickBias, SEVT dispatch 0x35DD0
// (0x534E4554, 1, node[2], &node[2]), free the buffer (+0x10, SNET.CPP line 0x735), 0x4D60 again, free the node ("delete", -1).
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
void Storm_Sub4D60(uint8_t* node);
extern "C" void Storm13c_11240(uint32_t* term, uint32_t tickBias, void (*f35dd0)(uint32_t, uint32_t, uint32_t, uint32_t*)) {
    for (;;) {
        const int32_t first = (int32_t)term[1];
        if (first <= 0) return;
        uint32_t* n = (uint32_t*)(uintptr_t)(uint32_t)first;
        if (n[0] != 0) {
            const uint32_t link = n[0], nx = n[1];
            uint32_t* slot = (int32_t)nx < 0 ? (uint32_t*)(uintptr_t)~nx : (uint32_t*)((uintptr_t)n + (uint32_t)(nx - *(uint32_t*)((uintptr_t)link + 4)));
            *slot = link;
            *(uint32_t*)((uintptr_t)n[0] + 4) = n[1];
            n[0] = 0; n[1] = 0;
        }
        n[3] += tickBias;
        f35dd0(0x534E4554u, 1, n[2], n + 2);
        if (n[4] != 0) Storm_Sub1B960((void*)(uintptr_t)n[4], "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x735);
        Storm_Sub4D60((uint8_t*)n);
        Storm_Sub1B960(n, "delete", -1);
    }
}
