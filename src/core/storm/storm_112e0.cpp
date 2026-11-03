// 1.13c Storm+0x112E0: drain the SNet user-event list (head terminator at Storm+0x55A1C, first at +0x55A20).
// Each node: free its buffer (+0x10) with SNET.CPP line 0x723 when set, unlink it (0x4D60), free it as AUUSEREVENT (-2).
// `term` is the head terminator {prevlink, first}; the loop runs while first > 0 (a signed test: an empty list holds ~terminator).
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
void Storm_Sub4D60(uint8_t* node);
extern "C" void Storm13c_112E0(uint32_t* term) {
    for (;;) {
        const int32_t first = (int32_t)term[1];
        if (first <= 0) return;
        uint32_t* n = (uint32_t*)(uintptr_t)(uint32_t)first;
        if (n[4] != 0) Storm_Sub1B960((void*)(uintptr_t)n[4], "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x723);
        Storm_Sub4D60((uint8_t*)n);
        Storm_Sub1B960(n, "__AUUSEREVENT__", -2);
    }
}
