#include <cstdint>
#include <emscripten.h>

// 1.13c D2Gfx+0x03450 (61 B, __chkstk). `size` arrives in EAX, `esp` is the entry stack pointer (the
// address the return address was pushed to) and `retaddr` the word the probe re-stores. A frame under
// one page stores the return address at esp - size and returns; otherwise it walks one 0x1000-byte page
// at a time downward from esp + 4 and finally stores the return address below the last page. Retail
// touches each guard page so the OS faults before the frame is entered; wasm has no guard page, so the
// observable is the sequence of stored addresses. Same object as D2DDraw 0x2710 / D2Game 0x6820.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2gfx_chkstk(uint32_t size, uint8_t* esp, uint32_t retaddr) {
    if (size < 0x1000) {
        *(uint32_t*)(esp - size) = retaddr;
        return;
    }
    uint8_t* p = esp + 4;
    do {
        p -= 0x1000;
        size -= 0x1000;
    } while (size > 0xFFF);
    *(uint32_t*)(p - 4 - size) = retaddr;
}
