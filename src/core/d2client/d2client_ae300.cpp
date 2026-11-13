// 1.13c D2Client+0xAE300: server packet {?, length, bit stream from +2}. The bit reader (Fog 10126 init over packet+2, length-2; 10128 reads)
// yields a variable-width value (1 bit: 0 reads 8 bits; else 1 bit: 0 reads 16 bits, else 32 bits), a 1-bit flag, a 9-bit field that is dropped,
// a second variable-width value, and a 1-bit selector (0 reads 8 bits, else 16 bits). 0x85990 gets EAX = first value, ECX = second,
// stack flag (low byte) and selector value; its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ae300(uint32_t pk, const Env& e) {
    uint32_t bs[6] = {};
    const uint32_t b = Stk(bs);
    e.H(0xbf9e, b, pk + 2, B(pk + 1) - 2);
    auto var = [&]() -> uint32_t {
        if (!e.H(0xbfb0, b, 1)) return e.H(0xbfb0, b, 8);
        if (!e.H(0xbfb0, b, 1)) return e.H(0xbfb0, b, 0x10);
        return e.H(0xbfb0, b, 0x20);
    };
    const uint32_t v1 = var();
    const uint32_t flag = e.H(0xbfb0, b, 1) & 0xff;
    e.H(0xbfb0, b, 9);
    const uint32_t v2 = var();
    const uint32_t v4 = e.H(0xbfb0, b, 1) ? e.H(0xbfb0, b, 0x10) : e.H(0xbfb0, b, 8);
    return e.H(0x85990, flag, v4, 0, 0, 0, v1, v2);
}
