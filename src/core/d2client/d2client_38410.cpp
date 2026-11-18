// 1.13c D2Client+0x38410: destructor (fastcall ECX = this). this[0] = derived vtable (D2Client+0xD6870); the owned child this[2], when set,
// is deleted through its vtable slot 0 (thiscall, 1 stack word: the deleting flag); this[0] = base vtable (+0xD5048). The SEH frame has no unwinding work.
#include "d2c_seam.h"
extern "C" void d2c_38410(const D2cSeam* s, uint32_t* o) {
    o[0] = 0x6FB86870u;
    if (o[2]) (*s)(0xbf80, 0, o[2], 0, 0, 0, 1);
    o[0] = 0x6FB85048u;
}
