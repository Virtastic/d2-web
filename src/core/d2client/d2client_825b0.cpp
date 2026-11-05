// 1.13c D2Client+0x825B0 (EAX = unit, ECX = view; stack mode, mask; ret 8): nothing happens unless both are set and ebx = [view+0x60] is non-zero and
// 0xCA54(unit) has a low byte other than 0xB / 0xC. Then 0xCA2A(ebx, unit); 0x69A90 (EAX = mask, ECX = view); when mode == 1 and 0xC51A(unit, 0) == 0,
// 0x81FF0 runs (stack 1, ESI = unit, EDI = view); finally 0xCA18(ebx, unit).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c825B0Env {
    uint32_t (*unitKind)(uint32_t unit);                         // 0xCA54
    void (*ca2a)(uint32_t ebx, uint32_t unit);                   // 0xCA2A
    void (*f69a90)(uint32_t mask, uint32_t view);                // 0x69A90 (EAX, ECX)
    uint32_t (*c51a)(uint32_t unit, uint32_t zero);              // 0xC51A
    void (*f81ff0)(uint32_t one, uint32_t unit, uint32_t view);  // 0x81FF0 (stack 1, ESI, EDI)
    void (*ca18)(uint32_t ebx, uint32_t unit);                   // 0xCA18
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_825b0(uint32_t unit, uint32_t view, uint32_t mode, uint32_t mask, const D2c825B0Env* env) {
    if (!view || !unit) return;
    uint32_t ebx;
    std::memcpy(&ebx, (const void*)(uintptr_t)(view + 0x60), 4);
    if (!ebx) return;
    const uint32_t k = env->unitKind(unit) & 0xffu;
    if (k == 0xb || k == 0xc) return;
    env->ca2a(ebx, unit);
    env->f69a90(mask, view);
    if (mode == 1 && env->c51a(unit, 0) == 0) env->f81ff0(1, unit, view);
    env->ca18(ebx, unit);
}
