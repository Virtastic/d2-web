// D2Common 1.13c implementation batch (DRLG anim / logic / room neighbours). Register-carried
// inputs are explicit parameters; unresolved retail callees are callback seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
typedef uint32_t (*Alloc)(const char*, int, int);
}

extern "C" {
// 1.13c D2Common+0x12F80 lives in d2common_12f80.cpp (own file, exact seam signatures).
// 1.13c D2Common+0x69B50 lives in d2common_69b50.cpp (register-accurate injectable port).
}
