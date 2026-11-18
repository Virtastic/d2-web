// 1.13c D2Client+0x72530 (218 B, EAX = unit, stack idx, ret 4): a unit's state-record event. EAX on every exit is what retail
// leaves there:
//  unit null -> 0; idx < 0 -> the unit; idx >= count (DataTables+0xC4) or a null record -> the DataTables pointer.
//  rec = rows (DataTables+0xBC) + idx * 0x3C. When gdwBitMasks[0x1C] & rec[0x14], EAX = unit[0]: unit[0] 1 with unit+0x10 0xC,
//  or 0 with unit+0x10 0x11, return it. AX := short rec+0x1A (upper half kept); 0..0x1E with a nonzero slot of the handler table
//  0xE2678 calls it (ECX = unit, EDX = idx; EAX = its return), a zero slot leaves EAX 0. Short rec+0x30 below 0 returns.
//  u = 0xC1C0(unit, idx) (Ordinal_10871), 0 returns 0; a = 0xC4B4(u, 0x15E, 0), b = 0xC4B4(u, 0x15F, 0) (Ordinal_10680);
//  0xA0840(EAX short rec+0x32, ECX idx, EDX 1; short rec+0x30, a, b) and its EAX is returned.
#include "client_host.h"
#include "d2client_hw.h"

struct D2c72530State {
    const uint8_t* unit;    // EAX
    uint32_t unitTag;       // how the host names the unit (its retail EAX value)
    uint32_t tblTag;        // how the host names the DataTables block (*sgptDataTables, import slot D2Client+0xCE474)
    const uint8_t* rows;    // DataTables+0xBC
    int32_t count;          // DataTables+0xC4
    uint8_t mask28;         // byte gdwBitMasks+0x1C
    const uint32_t* slots;  // D2Client+0xE2678, 31 handler addresses
};

namespace {
int16_t rd16(const uint8_t* p, uint32_t off) { int16_t v; memcpy(&v, p + off, 2); return v; }
}

// 1.13c D2Client+0x72530
extern "C" uint32_t d2c_72530(const D2c72530State* s, int32_t idx, D2cHost h) {
    if (!s->unit) return 0;
    if (idx < 0) return s->unitTag;
    uint32_t eax = s->tblTag;
    if (idx >= s->count || !s->rows) return eax;
    const uint8_t* r = s->rows + idx * 0x3c;
    if (s->mask28 & r[0x14]) {
        eax = ld(s->unit, 0);
        const uint32_t k = ld(s->unit, 0x10);
        if ((eax == 1 && k == 0xc) || (eax == 0 && k == 0x11)) return eax;
    }
    const int16_t sl = rd16(r, 0x1a);
    eax = (eax & 0xffff0000u) | (uint16_t)sl;
    if (sl >= 0 && sl < 0x1f) {
        eax = s->slots[sl];
        if (eax) eax = hc(h, eax, 0, s->unitTag, (uint32_t)idx);
    }
    if (rd16(r, 0x30) < 0) return eax;
    const uint32_t u = hc(h, 0xc1c0, 0, 0, 0, 0, 0, s->unitTag, (uint32_t)idx);
    if (!u) return 0;
    const uint32_t a = hc(h, 0xc4b4, 0, 0, 0, 0, 0, u, 0x15e, 0);
    const uint32_t b = hc(h, 0xc4b4, 0, 0, 0, 0, 0, u, 0x15f, 0);
    return hc(h, 0xa0840, (uint32_t)(int32_t)rd16(r, 0x32), (uint32_t)idx, 1, 0, 0, (uint32_t)(int32_t)rd16(r, 0x30), a, b);
}
