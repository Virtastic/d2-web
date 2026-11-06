// D2Client+0xBFCA0 -- the resistance line of the item tooltip: "Fire Resist ..." entries of the six resist stats at 100% or more.
#include <cstdint>
#include <cstring>
#include "d2client_bfca0.h"
static void wcat(uint16_t* d, const uint16_t* s) { while (*d) ++d; while ((*d++ = *s++)) {} }
static void wcpy(uint16_t* d, const uint16_t* s) { while ((*d++ = *s++)) {} }
namespace { struct Row { uint32_t stat; uint16_t id; uint32_t digit; }; }
// the table at D2Client+0xD34EC (6 rows of 10 bytes: stat u32, string id u16, colour digit u32)
static const Row kRows[6] = { {36, 10085, 4}, {37, 10086, 8}, {39, 10081, 1}, {41, 10083, 9}, {43, 10082, 3}, {45, 10084, 2} };

// 1.13c D2Client+0xBFCA0
void d2c_bfca0(const D2ClientBfca0Env& e, const uint8_t* unit, uint16_t* out, uint32_t* flag) {
    if (unit) { int32_t t; memcpy(&t, unit + 4, 4); if (t >= 0x2c0 && t <= 0x2c5) return; }
    const uint16_t* a = e.str(0xf9b);
    const uint16_t* b = e.str(0xf0c);
    uint16_t acc[256]; acc[0] = 0;
    for (const Row& r : kRows) {
        if (e.stat(unit, r.stat) < 100) continue;
        uint16_t t[256]; t[0] = 0;
        if (acc[0]) { wcat(acc, b); wcat(acc, a); }
        wcpy(t, e.str(r.id));
        e.colour(r.digit & 0xFF, t);
        wcat(acc, t);
    }
    wcpy(out, acc);
    *flag = 4;
}
