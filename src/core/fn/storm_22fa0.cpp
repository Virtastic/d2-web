// storm+0x22fa0 handle_in_list: FUNCTION-mode probe.
// Register order per the descriptor (regs ecx edx): r[0]=ECX (the node), r[1]=EDX (the list head the retail global holds).
// The port (src/core/storm/storm_batch_b54b.cpp:Storm_Sub22FA0) reads its file-scope g_listHead; the probe sets it from r[1] and calls the port.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>

extern int32_t g_listHead;
uint32_t Storm_Sub22FA0(uint8_t* node);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_22fa0(uint32_t* r) {
    g_listHead = (int32_t)r[1];
    r[4] = Storm_Sub22FA0((uint8_t*)(uintptr_t)r[0]);
}
