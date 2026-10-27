// src/core/d2client/crt_write_multi_4c34.cpp -- 1.13c D2Client.dll row 0x04C34 write_multi_char
// (MSVC CRT output.c _write_multi_char). Retail: EAX = int* written count (moved to ESI), stack
// (ch, num, FILE*); enters at the num test (0x1b): while num > 0 { num--; write_char(ch, FILE, written) (0x04C01);
// stop if *written == -1 }. Disassembly: 55 8b ec 56 8b f0 eb 13 ... 83 3e ff 74 06 83 7d 0c 00 7f e7.
#include <cstdint>
#include <emscripten.h>

struct D2cFile;  // layout in d2client_batch_03f82.cpp
extern "C" void d2_client_4c01(D2cFile* f, uint8_t ch, int32_t* written, void*, void*, void*, void*);

// 1.13c D2Client+0x04c34
extern "C"
void d2_client_4c34(int32_t* written, uint8_t ch, int32_t num, D2cFile* f) {
    while (num > 0) {
        num--;
        d2_client_4c01(f, ch, written, nullptr, nullptr, nullptr, nullptr);
        if (*written == -1) return;
    }
}
