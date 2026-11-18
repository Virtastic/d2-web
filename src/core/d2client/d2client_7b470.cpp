// 1.13c D2Client+0x7B470 (117 B, EDX = char*, EDI = object, one stack word, ret 4): EDI 0 does nothing. An empty string frees the
// handle at object +0xA4 (0xCAE4, stack 0 and the handle) when nonzero and stores 0. Otherwise: new = 0xCACC(0, str, dword [0x113ACE]),
// 0xCAD8(new, the stack word), free the old handle as above, store new at +0xA4. The length lives in the pushed-ECX local; the word
// handed to 0xCAD8 is read at [esp+0xC] after the stdcall returns, which is the caller's stack argument, not that local.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2c_port_7b470(uint8_t* obj, const char* str, uint32_t arg, uint32_t gword, uint32_t (*cacc)(uint32_t z, const char* s, uint32_t g),
                               void (*cad8)(uint32_t p, uint32_t w), void (*cae4)(uint32_t z, uint32_t p)) {
    if (obj == 0) return;
    uint32_t len = (uint32_t)strlen(str), old; memcpy(&old, obj + 0xa4, 4);
    if (len == 0) {
        if (old != 0) cae4(0, old);
        uint32_t z = 0; memcpy(obj + 0xa4, &z, 4);
        return;
    }
    uint32_t nw = cacc(0, str, gword);
    cad8(nw, arg);
    memcpy(&old, obj + 0xa4, 4);
    if (old != 0) cae4(0, old);
    memcpy(obj + 0xa4, &nw, 4);
}
