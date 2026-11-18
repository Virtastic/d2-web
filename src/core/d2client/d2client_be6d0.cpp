// 1.13c D2Client+0xBE6D0: EDX = source string, stack word 0 = value (ret 4). NULL source clears the first char of the
// message buffer (global 0x11CA30); otherwise Unicode::strcpy(buffer, src) (D2Lang import, slot 0xCEAB8) and the
// value is stored at global 0x11CC30. strcpy is injected; the globals are passed as pointers.
#include <cstdint>

struct D2c0xbe6d0Env {
    void (*strcpy_ceab8)(const char16_t* src);
};

extern "C" void d2c_port_be6d0(const char16_t* src, uint32_t value, char16_t* gBuf_11ca30, uint32_t* gVal_11cc30, const D2c0xbe6d0Env& e) {
    if (!src) { gBuf_11ca30[0] = 0; return; }
    e.strcpy_ceab8(src);
    *gVal_11cc30 = value;
}
