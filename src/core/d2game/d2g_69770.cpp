// 1.13c D2Game+0x69770 -- reverse the unit's name buffer in place. ECX = effect context (+8 unit). Written from the 1.13c disassembly.
// 0xA4C0(unit) answers the buffer (null: nothing happens); then the retail CRT runs on it: __strrev(buf) and
// _strncpy(buf, buf, 16), which copies in place to the first NUL and zero-pads through 16 bytes. Both are reproduced here
// byte for byte (they are not hooked). EAX at exit is the buffer pointer (strncpy's result): an address, so the record masks it.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f69770(HookEnv& e, uint32_t ecx)
{
    const uint32_t s = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ecx + 8));
    if (!s) return 0;
    uint32_t n = 0; while (e.rd8(s + n)) n++;
    for (uint32_t i = 0; i < n / 2; i++) { const uint32_t a = e.rd8(s + i), b = e.rd8(s + n - 1 - i); e.wr8(s + i, b); e.wr8(s + n - 1 - i, a); }
    uint32_t i = 0;
    for (; i < 16; i++) if (!e.rd8(s + i)) break;
    for (; i < 16; i++) e.wr8(s + i, 0);
    return s;
}
}
