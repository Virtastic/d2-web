// 1.13c Storm+0x115B0: drain an SNet message queue (EBX = the queue; its TSExplicitList head terminator is {prevlink @+4, first @+8}).
// Each AUMESSAGE node {prev, next, a, b, c, kind}: kind 0 hands (a, b, c) to the provider callback (table DAT 0x55834, slot +0xC) when a
// provider is set; any other kind frees a ("delete", -1) and b (SNET.CPP line 0x3B6) when set. Then 0x4D60 unlinks the node, which is freed
// as AUMESSAGE (-2). The loop runs while first > 0 (signed: an empty list holds ~terminator).
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
void Storm_Sub4D60(uint8_t* node);
extern "C" void Storm13c_115B0(uint32_t* queue, uint32_t provider, void (*providerCb)(uint32_t, uint32_t, uint32_t)) {
    for (;;) {
        const int32_t first = (int32_t)queue[2];
        if (first <= 0) return;
        uint32_t* n = (uint32_t*)(uintptr_t)(uint32_t)first;
        if (n[5] == 0) {
            if (provider != 0) providerCb(n[2], n[3], n[4]);
        } else {
            const uint32_t a = n[2], b = n[3];
            if (a != 0) Storm_Sub1B960((void*)(uintptr_t)a, "delete", -1);
            if (b != 0) Storm_Sub1B960((void*)(uintptr_t)b, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x3B6);
        }
        Storm_Sub4D60((uint8_t*)n);
        Storm_Sub1B960(n, "__AUMESSAGE__", -2);
    }
}
