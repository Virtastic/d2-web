// probe for Fog 0x15C00 (sub_15C00, imagehlp stack walk trigger): regs eax = thread handle (r[0]); blobs g r[14], lg r[16].
// The port fog_dump_stack_15C00 is retail's early-out as a no-op (no imagehlp in the engine), so the probe returns 0 and records nothing:
// retail's hook log (the registry read, the 0x190A0 lines, the kernel32 calls) is what the vector holds and the port does not reproduce.
#include <emscripten.h>
#include <cstdint>
void fog_dump_stack_15C00(void*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_15c00(uint32_t* r) {
    fog_dump_stack_15C00((void*)(uintptr_t)r[0]);
    r[4] = 0;
}
