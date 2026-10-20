#include <cstdint>
// 1.13c D2Common+0x08DC5 -- MSVC CRT __fptrap: `push 2; call __amsg_exit 0x2D73;
// pop ecx; ret`. Port has no process to end; reports the callee and its one argument word, returns 0.
extern "C" uint32_t d2common_fptrap(uint32_t* ev) {
    ev[0] = 0x2D73;     // __amsg_exit
    ev[1] = 2;          // _RT_FLOAT
    return 0;
}
