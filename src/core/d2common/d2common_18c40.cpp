// 1.13c D2Common+0x18C40 -- ECX = path info (plain ret). Info: [0] start point, [1] target point, [2] room, [0xa] pattern,
// [0xb] mask, [0xc] -> record Q (Q+0x30 -> unit-like block whose first dword is 0 or 1, Q+0x58 flag, Q+0x91 byte limit,
// Q+0x9c = the output points). Walks the line start->target (0x184C0, limit Q+0x91, or 0x28 when Q+0x58 is set and the byte is
// smaller); each point that 0x4CE90 reports blocked goes to the walker search 0x18830, which splices a detour in the same
// buffer; the points are finally thinned (0x18310) into Q+0x9c and their count returned. Fewer than three line points: 0.
// Frame slots as in retail: the index and the point count are two separate slots; 0x18830 receives &idx and &count, and the
// loop reloads the (possibly rewritten) count after a detour. The limit slot is only read (limit - i).
#include <cstdlib>
#include "d2common_18c40.h"

extern "C" int32_t D2Common_18C40_BuildPath(uint32_t self, const D2CommonWalkEnv* env)
{
    auto w = [](uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a + o)); };
    const uint32_t q = w(self, 0x30);
    const uint32_t r = q ? w(q, 0x30) : 0;
    if (!q || !r || (w(r, 0) != 0 && w(r, 0) != 1)) abort();   // assert line 0x2ad, then _exit(-1)
    const uint32_t room = w(self, 8), pat = w(self, 0x28), mask = w(self, 0x2c), start = w(self, 0);
    int32_t limit = int32_t(*reinterpret_cast<const uint8_t*>(uintptr_t(q + 0x91)));
    if (w(q, 0x58) != 0 && limit < 0x28) limit = 0x28;
    uint32_t arr[160] = {};
    int32_t idx = 0, dir = 0;
    int32_t n = D2Common_184C0_LineToPoints(start, w(self, 4), limit, &dir, reinterpret_cast<uint16_t*>(arr));
    if (n <= 2) return 0;
    uint32_t last = start;
    int32_t i = 0;
    for (;;) {
        if (env->chk(room, uint16_t(arr[i]), uint16_t(arr[i] >> 16), pat, mask)) {
            const int32_t rv = D2Common_18830_FindSubpathWithoutObstacles(self, last, arr, &idx, &n, limit - i, dir, env);
            if (rv == 0)
                return D2Common_18310_ThinPoints(arr, start, reinterpret_cast<uint32_t*>(uintptr_t(q + 0x9c)), idx);
            i = idx;
        }
        last = arr[i];
        ++i;
        idx = i;
        if (i >= n) break;
    }
    if (n == 0) return 0;   // retail: test edi,edi / je -> the plain zero return
    return D2Common_18310_ThinPoints(arr, start, reinterpret_cast<uint32_t*>(uintptr_t(q + 0x9c)), n);
}
