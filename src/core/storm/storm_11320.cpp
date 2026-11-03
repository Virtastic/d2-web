// 1.13c Storm+0x11320 (thiscall: ECX = this, AL = queue index, one stack word `link`, ret 4): serve one SNet queue.
// AL == 2: only the relink step on the anchor at this+0x1C4. Otherwise, with q = this + AL*12: if the list whose first is q[0x198]
// holds a node, finish it as 0x115B0 does (kind 0 calls the provider callback; any other kind frees a ("delete") and b (SNET.CPP 0x3B6);
// then 0x4D60 and free as AUMESSAGE); then the relink step on q+0x194. Relink (x = link, or the anchor when link is 0): unlink x from
// the list it is in when x[0] != 0; then x[0] = head, x[1] = head[1], head[1] = link, head = x.
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
void Storm_Sub4D60(uint8_t* node);
namespace {
inline uint32_t& W(uint32_t a) { return *(uint32_t*)(uintptr_t)a; }
void relink(uint32_t x, uint32_t link, uint32_t headAddr) {
    const uint32_t esi = W(x);
    if (esi != 0) {
        uint32_t edx = W(x + 4);
        if ((int32_t)edx < 0) edx = ~edx; else edx += x - W(esi + 4);
        W(edx) = esi;
        W(W(x) + 4) = W(x + 4);
        W(x) = 0; W(x + 4) = 0;
    }
    const uint32_t ecx = W(headAddr);
    W(x) = ecx;
    W(x + 4) = W(ecx + 4);
    W(ecx + 4) = link;
    W(headAddr) = x;
}
}  // namespace
extern "C" void Storm13c_11320(uint32_t self, uint32_t al, uint32_t link, void (*providerCb)(uint32_t, uint32_t, uint32_t)) {
    al &= 0xFF;
    if (al == 2) { relink(link ? link : self + 0x1C4, link, self + 0x1C4); return; }
    const uint32_t q = self + al * 12;
    const int32_t first = (int32_t)W(q + 0x198);
    if (first > 0) {
        uint32_t* n = (uint32_t*)(uintptr_t)(uint32_t)first;
        if (n[5] == 0) providerCb(n[2], n[3], n[4]);
        else {
            const uint32_t a = n[2], b = n[3];
            if (a != 0) Storm_Sub1B960((void*)(uintptr_t)a, "delete", -1);
            if (b != 0) Storm_Sub1B960((void*)(uintptr_t)b, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x3B6);
        }
        Storm_Sub4D60((uint8_t*)n);
        Storm_Sub1B960(n, "__AUMESSAGE__", -2);
    }
    relink(link ? link : q + 0x194, link, q + 0x194);
}
