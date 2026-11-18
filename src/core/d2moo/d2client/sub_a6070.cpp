// D2Client+0xA6070: ESI = dword array (first word must be 1), EDI = caller's context handed to the two lookup imports; env-seam style.
// Retail pushes (edi, file 0x6FB84144 = image RVA 0xD4144, line) to the import, feeds its result to the second import, then calls A56C0(eax, stack edi, esi).
#include <cstdint>
#include <emscripten.h>
struct D2cA6070Env {
    uint32_t (*imp5BC)(uint32_t edi, uint32_t file, uint32_t line);   // import thunk 0xC5BC (stdcall 3), line 0x68A
    uint32_t (*imp5CE)(uint32_t edi, uint32_t file, uint32_t line);   // import thunk 0xC5CE (stdcall 3), line 0x68E
    uint32_t (*imp42A)(uint32_t v);                                    // import thunk 0xC42A (stdcall 1; result is EAX)
    uint32_t (*fA56C0)(uint32_t eax, uint32_t edi, uint32_t esi);      // D2Client+0xA56C0 (EAX, stack edi, esi; ret 8)
};
extern "C" {
// 1.13c D2Client+0xA6070
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a6070(const D2cA6070Env* e, const uint32_t* esi, uint32_t edi) {
    if (!esi || esi[0] != 1) return 0;
    const uint32_t file = 0xB2000000u | 0xD4144u, me = (uint32_t)(uintptr_t)esi;
    if (e->fA56C0(e->imp42A(e->imp5BC(edi, file, 0x68a)), edi, me)) return 1;
    return e->fA56C0(e->imp42A(e->imp5CE(edi, file, 0x68e)), edi, me) != 0;
}
}
