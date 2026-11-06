// 1.13c D2Client+0x68BE0 (73 B, no arguments): call 0x68810 with the address of the 0x203-dword cache block at 0x1144C8, take the
// block's dword +8 off the counter word [0x11C044], free the pointer held at block +4 when nonzero (Fog free thunk 0xBF86,
// EDX = the file string, stack line 0xE61 and 0), then zero the whole block.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2c_port_68be0(uint32_t* blk, uint32_t* counter, void (*c68810)(), void (*free0xbf86)(uint32_t p, uint32_t line, uint32_t z)) {
    c68810();
    *counter = *counter - blk[2];
    if (blk[1] != 0) free0xbf86(blk[1], 0xe61, 0);
    for (int i = 0; i < 0x203; ++i) blk[i] = 0;
}
