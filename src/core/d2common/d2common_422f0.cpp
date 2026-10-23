// 1.13c D2Common+0x422F0 : clear cell (x,y) in both per-room tables; the callee (0x36C20) is *p = v.
#include <cstdint>
typedef uintptr_t P;
static inline uint32_t ru(P b, uint32_t o) { return *(const uint32_t*)(b + o); }
// ecx = obj, edx = x, stack = y. Table A: row base +8, data +4; table B: row base +0x30, data +0x2c; both -> 0.
void D2Common_422F0(P obj, int32_t x, int32_t y)
{
    P s = (P)ru(obj, 0x14);
    *(uint32_t*)((P)ru(s, 4) + 4 * (ru((P)ru(s, 8), 4 * y) + x)) = 0;
    *(uint32_t*)((P)ru(s, 0x2c) + 4 * (ru((P)ru(s, 0x30), 4 * y) + x)) = 0;
}
