// storm+0x22fa0 handle_in_list: FUNCTION-mode probe.
// Register order per the descriptor (regs ecx edx): r[0]=ECX (the node), r[1]=EDX (the list head the retail global holds).
// The port (src/core/storm/storm_batch_b54b.cpp:Storm_Sub22FA0) reads its file-scope g_listHead; the probe sets it from r[1] and calls the port.
// The state-init write to retail global 0x559F0 (the list head) maps to the port's g_listHead.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>

extern "C" void Storm_SetListHead22FA0(int32_t head);
extern "C" uint32_t* Storm_ListHeadAddr22FA0();
uint32_t Storm_Sub22FA0(uint8_t* node);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_22fa0(uint32_t* r) {
    Storm_SetListHead22FA0((int32_t)r[1]);
    r[4] = Storm_Sub22FA0((uint8_t*)(uintptr_t)r[0]);
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fnw_storm_22fa0(uint32_t globalRva) {
    return globalRva == 0x559F0u ? (uint32_t)(uintptr_t)Storm_ListHeadAddr22FA0() : 0u;
}
