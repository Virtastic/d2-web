// 1.13c D2Win+0xB420 (ordinal 10121, sub_B420): return the dword at +0xC of the object.
// Null object: fatal (Fog #10265(0xd60) -> #10024("", text)) then _exit(-1).
#include <cstdint>
#include <unistd.h>

struct D2WinB420Env { void (*fatal)(uint32_t stringId); };

// 1.13c D2Win+0xB420
uint32_t d2win_sub_b420(const uint8_t* obj, const D2WinB420Env& env) {
    if (!obj) { env.fatal(0xd60); _exit(-1); }
    return *(const uint32_t*)(obj + 0xc);
}
