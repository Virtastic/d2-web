// 1.13c Fog+0x247AA: MSVC CRT _umatherr-style math-error dispatcher (statically linked in Fog).
// Maps the _exception type (1 DOMAIN,2 SING,3 OVERFLOW,4 UNDERFLOW,5 TLOSS,7,8) to an FP-status mask;
// type 7 is rewritten to 1 and returns. Wasm has no x87/SSE exception machinery, so __handle_exc /
// __raise_exc / matherr hook (callees 0xA5AC..) are not reproduced: the observable result is *type.
#include <cstdint>

extern "C" void fog_sub_247aa(int fn, int* exc, uint16_t* fpcw) {  // 1.13c Fog+0x247AA
    (void)fn; (void)fpcw;
    if (*exc == 7) *exc = 1;
}
