// src/core/d2client/crt_write_multi_4c34.cpp -- 1.13c D2Client.dll row 0x04C34 write_multi_char, from
// the cached decompile (build-wasm/ghidra-d2client/decomp/0x4C34.c). Alpha lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// MSVC 2003 Release CRT _write_multi_char (output.c): emit `num` copies of ch, stopping after the write
// that leaves the running written count at -1 (the test is at the loop bottom in retail). The retail
// (cdecl) arguments are (ch, num); the stream and the running count reach the helper through registers
// the decompile does not recover (count in EAX), so they follow as host context, as in
// src/core/realm/bnclient_batch_2544.cpp 0x027F9.
#include <cstdint>
#include <emscripten.h>

// MSVC 2003 32-bit FILE (VS2003 layout): ptr 0x0, cnt 0x4, base 0x8, flag 0xC (crt_write.cpp).
struct D2CrtFile32 {
    uint32_t ptr;
    int32_t  cnt;
    uint32_t base;
    uint32_t flag;
};

// The one callee 0x04C01 _write_char is not recovered yet; it arrives as the seam with retail's
// (ch, stream, written count) call shape. Replace with a direct call once 0x04C01 lands.
typedef void (*D2cWriteCharFn)(uint8_t ch, D2CrtFile32* f, int32_t* written);

// 1.13c D2Client+0x04c34
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_4c34(uint8_t ch, int32_t num, D2CrtFile32* f,
                                              int32_t* written, D2cWriteCharFn write_char) {
    do {
        if (num < 1) return;
        num--;
        write_char(ch, f, written);
    } while (*written != -1);
}
