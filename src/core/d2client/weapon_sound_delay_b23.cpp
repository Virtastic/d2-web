#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// The client unit as 0x22220 sees it: only the attack/animation rate at +0x4C is read.
struct B23Unit { uint8_t p0[0x4C]; int16_t rate; };
static_assert(offsetof(B23Unit, rate) == 0x4C, "unit rate");

// 1.13c D2Client+0xF553C (VA 0x6FBA553C): the second dword of each 8-byte pair whose base is
// 0x6FBA5538; the first dword of a pair is a Sounds.txt row base (sub_242B0's selector), this is
// the paired delay in frames. Retail bytes, pair index 0..13; read-only, never written.
static const uint32_t kSoundDelay[14] = { 0, 6, 6, 6, 8, 8, 6, 8, 6, 6, 0, 0, 0, 0 };

// 1.13c D2Client+0x22220: ticks for the paired sound delay at the unit's rate.
// retail: ECX = unit, EAX = selector; the Fog assert (#10265/#10024) + _exit(-1) sites (codes 0xB6B for a
// null unit, 0xB6F for (sel & 0xF) >= 0xE) are fatal in retail, so no vector tuple carries those inputs and
// ours returns 0 where retail would abort the process.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_22220(const B23Unit* u, uint32_t sel) {
    if (!u) return 0;
    sel &= 0xF;
    if (sel >= 0xE) return 0;
    int32_t rate = u->rate;                       // movsx: sign-extended into the 32-bit divisor
    if (rate == 0) rate = 1;
    return (kSoundDelay[sel] * 0x100u + 0x80u) / (uint32_t)rate;   // xor edx,edx; div ecx
}
