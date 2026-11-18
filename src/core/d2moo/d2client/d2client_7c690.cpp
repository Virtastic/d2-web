// 1.13c D2Client+0x7C690 (147 B, stdcall (text, mode), ret 8; callers 0x7D3C0, 0x7D200): build the text-scroll object (0x2C bytes) from a wide blob.
// obj = 0xBF7A(ECX = 0x2C, EDX = file literal 0xD4BDC, stack 0x659, 0). mode == 3: [obj] = 0xD378(ECX = text, EDX = obj+4, stack 0xB4) (the callee fills the
// count at +4). Any other mode: prev = 0xD3A8(ECX = 8); [obj] = 0x7BFB0(text, obj+4, obj+0x14) (count, rate); u16 [+0x10] = 0x145, u16 [+0x12] = 0x70,
// dwords +0x1A/+0x1E/+0x22/+0x26 = 0, u16 +0x2A = 0; 0xD3A8(ECX = prev). Both paths end with 0x7BF60 (EDI = obj; d2client_7bf60.cpp) and return obj.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c7C690Env {
    uint32_t file;                                                                   // 0xD4BDC literal (an opaque handle)
    uint32_t (*alloc)(uint32_t size, uint32_t file, uint32_t line, uint32_t z);      // 0xBF7A
    uint32_t (*head3)(uint32_t text, uint32_t countp, uint32_t arg);                 // 0xD378 (ECX, EDX, one stack word)
    uint32_t (*font)(uint32_t id);                                                   // 0xD3A8 (ECX), returns the previous font
    uint32_t (*split)(uint32_t text, uint32_t countp, uint32_t ratep);               // 0x7BFB0
    void (*layout)(uint32_t obj);                                                    // 0x7BF60 (EDI)
};

static void w32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static void w16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 2); }

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2c_port_7c690(uint32_t text, uint32_t mode, const D2c7C690Env* e) {
    const uint32_t o = e->alloc(0x2c, e->file, 0x659, 0);
    if (mode == 3) {
        w32(o, 0, e->head3(text, o + 4, 0xb4));
        e->layout(o);
        return o;
    }
    const uint32_t prev = e->font(8);
    w32(o, 0, e->split(text, o + 4, o + 0x14));
    w16(o, 0x10, 0x145);
    w16(o, 0x12, 0x70);
    w32(o, 0x1a, 0); w32(o, 0x1e, 0); w32(o, 0x22, 0); w32(o, 0x26, 0);
    w16(o, 0x2a, 0);
    e->font(prev);
    e->layout(o);
    return o;
}
