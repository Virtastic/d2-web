// D2Client+0x6F0B0: IME candidate-list refresh (EDI=HIMC); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c6F0B0Env {
    uint32_t (*getCount)(uint32_t himc, uint32_t* out);                 // ImmGetCandidateListCountA
    uint32_t (*getList)(uint32_t himc, uint32_t idx, void* buf, uint32_t len);  // ImmGetCandidateListA
    void (*free_)(void* p, int32_t tag, int32_t zero);                  // Fog Ordinal_10043
    void* (*alloc)(int32_t size, int32_t zero);                         // Fog Ordinal_10042
};
static void* g_cand;  // DAT_6fbcc034
extern "C" {
// 1.13c D2Client+0x6F0B0
EMSCRIPTEN_KEEPALIVE void d2c_6f0b0(const D2c6F0B0Env* e, uint32_t himc) {
    uint32_t cnt;
    uint32_t len = e->getCount(himc, &cnt);
    uint32_t r = e->getList(himc, 0, nullptr, 0);
    if (r != 0 && len != 0) {
        if (g_cand) e->free_(g_cand, 0xe5, 0);
        g_cand = e->alloc(0xe7, 0);
        e->getList(himc, 0, g_cand, len);
        const uint32_t* c = (const uint32_t*)g_cand;  // dwStyle +8, dwCount +0xC
        if (c[2] == 2 && c[3] == 1) { e->free_(g_cand, 0xec, 0); g_cand = nullptr; }
    }
}
}
