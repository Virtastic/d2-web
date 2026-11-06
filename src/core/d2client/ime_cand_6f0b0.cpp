// D2Client+0x6F0B0: IME candidate-list refresh (EDI = HIMC, plain ret); env-seam style.
// Retail: ImmGetCandidateListCountA (0xBE30, stdcall), ImmGetCandidateListA (0xBE24, stdcall),
// Fog alloc (0xBF7A: ECX = size, EDX = file, 2 stack words), Fog free (0xBF86: ECX = block, EDX = file, 2 stack words).
// The list pointer lives in the module global DAT_6fbcc034 (RVA 0x11C034): the caller passes its cell.
#include <cstdint>
#include <emscripten.h>
struct D2c6F0B0Env {
    uint32_t (*getCount)(uint32_t himc, uint32_t* out);                          // ImmGetCandidateListCountA: returns the byte count
    uint32_t (*getList)(uint32_t himc, uint32_t idx, void* buf, uint32_t len);   // ImmGetCandidateListA
    void (*free_)(void* p, uint32_t file, int32_t tag, int32_t zero);            // Fog free (0xBF86)
    void* (*alloc)(uint32_t size, uint32_t file, int32_t tag, int32_t zero);     // Fog alloc (0xBF7A)
};
extern "C" {
// 1.13c D2Client+0x6F0B0. cell = &DAT_6fbcc034, file = the EDX literal retail passes (module address 0x6fb84e5c, RVA 0xD4E5C).
EMSCRIPTEN_KEEPALIVE void d2c_6f0b0(const D2c6F0B0Env* e, uint32_t himc, void** cell, uint32_t file) {
    uint32_t cnt = 0;
    const uint32_t len = e->getCount(himc, &cnt);
    const uint32_t r = e->getList(himc, 0, nullptr, 0);
    if (r != 0 && len != 0) {
        if (*cell) e->free_(*cell, file, 0xe5, 0);
        *cell = e->alloc(len, file, 0xe7, 0);
        e->getList(himc, 0, *cell, len);
        const uint32_t* c = (const uint32_t*)*cell;   // CANDIDATELIST: dwSize +0, dwStyle +4, dwCount +8
        if (c[1] == 2 && c[2] == 1) { e->free_(*cell, file, 0xec, 0); *cell = nullptr; }
    }
}
}
