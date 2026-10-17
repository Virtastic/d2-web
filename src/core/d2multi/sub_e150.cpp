// 1.13c D2Multi+0x0E150 stub: the multiplayer (gateway/MCP) screen --
// one-shot setup, run, teardown; returns 1. The context pointer arrives in EAX (see the spec).
//
// Stub: it exists so the row has a symbol while the real body is written; it does NOT meet the
// contract below and earns no coverage. The routine reads ~30 module globals and its context
// struct, calls a COM-style interface vtable (ctx+0x221, methods +0x10/+0x54/+0x58/+0x68/+0xa8/+0xac),
// GetTickCount, Enter/DeleteCriticalSection, _exit, _sprintf, and ten unmapped internal functions
// (0xE760, 0x9550, 0x8E70, 0xB300, 0xB670, 0xBE00, 0x83E0, 0x9D60, 0x90D0, 0x10AA0), so it cannot
// be replayed without a bespoke capture block (see the spec's "what a vector cannot cover").
#include <cstdint>

extern "C" int32_t d2_multi_sub_e150(void* ctx) {
    (void)ctx;
    return 1;   // retail returns 1 (or the status word 0x6FA0A0DC from sub_E730)
}
