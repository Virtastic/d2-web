// 1.13c D2Common batch b58b. Only +0x3E190 is a plain stack-arg function; the other rows in the
// slice take register args (ESI/EBX/EAX), are CRT, or need unpinned globals, and are left unmapped.
#include <cstdint>
#include <initializer_list>

// 1.13c D2Common+0x3E190
// Direction index between two packed (y<<16|x) points: delta signs/magnitudes pick one of the
// 5-wide rows, result = row*5 + 12 + col (or row*5 + 10 when col < -1).
int32_t d2_fn_d2common_3e190(uint32_t a, uint32_t b)
{
    int32_t dx = (int32_t)((b & 0xffff) - (a & 0xffff));
    int32_t dy = (int32_t)((b >> 16) - (a >> 16));
    const int32_t ax = dx < 0 ? -dx : dx;
    const int32_t ay = dy < 0 ? -dy : dy;
    if (ax < ay * 2) {
        if (ax * 2 <= ay) {
            if (dx < 0) {
                dx = -1;
                goto have_dx;
            }
            dx &= 1;
        }
    } else if (dy < 0) {
        dy = -1;
    } else {
        dy &= 1;
    }
    if (dx < -1) dx = -2;
    else if (dx > 1) dx = 2;
have_dx:
    if (dy < -1) return dx * 5 + 10;
    if (dy > 1) dy = 2;
    return dx * 5 + 12 + dy;
}
