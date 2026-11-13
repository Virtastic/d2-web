// 1.13c D2Client+0xAF940: server packet {?, byte}: 0x899A0(byte) runs, then 0xBE8A(M(0x83588), M(0x851DC), 0, &local) fills a stack
// dword that starts as the packet pointer; 0x10DF0 runs with ESI = 2 when that dword is 1, else 0, and its result is returned.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_af940(uint32_t pk, const Env& e, const GEnv& g) {
    e.H(0x899a0, B(pk + 1));
    uint32_t loc = pk;
    e.H(0xbe8a, g.M(0x83588), g.M(0x851dc), 0, Stk(&loc));
    return e.H(0x10df0, 0, 0, 0, 0, 0, 0, 0, 0, 0, loc == 1 ? 2 : 0);
}
