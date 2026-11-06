// 1.13c D2Common+0x39B60 (ordinal 10381): walk the op-stat chain of a unit's effect list.
// Retail (decompile + disassembly): both arguments non-null, unit+0 is 0 or 1, param+0x5C non-null with +0x10 negative.
// The list at effect+0x48 has int16 effect+0x4C entries of 8 bytes; the u16 at entry+2 indexes itemstatcost. A row whose
// nOp (+0x54) is 6..7 contributes up to three op stat ids (u16 at +0x58, stop at 0xFFFF); per id the op row is the table
// row (null when the id is out of range), then 0x39530(unit+0x5C, id<<16) feeds 0x38940(id<<16, value, opRow, unit).
// A first-level row index out of range reads a null row in retail (a fault), so callers keep it in range.
#include "d2common_39b60.h"

namespace {
inline const uint8_t* B(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
inline uint32_t D(const uint8_t* p, uint32_t o) { return *reinterpret_cast<const uint32_t*>(p + o); }
}

extern "C" void D2Common_39B60_Injected(const D2Common39B60Callees* c, uint32_t unit, uint32_t param, uint32_t iscBase, int32_t iscCount)
{
    if (!unit || !param) return;
    const uint8_t* u = B(unit);
    if (D(u, 0) != 0 && D(u, 0) != 1) return;
    const uint8_t* eff = B(D(B(param), 0x5C));
    if (!eff || static_cast<int32_t>(D(eff, 0x10)) >= 0) return;
    const uint32_t unitData = D(u, 0x5C);
    const uint32_t end = D(eff, 0x48) + static_cast<uint32_t>(static_cast<int32_t>(*reinterpret_cast<const int16_t*>(eff + 0x4C)) * 8);
    for (uint32_t cur = D(eff, 0x48); cur < end; cur += 8) {
        const uint32_t idx = *reinterpret_cast<const uint16_t*>(B(cur) + 2);
        const uint32_t row = idx < static_cast<uint32_t>(iscCount) ? iscBase + idx * 0x144 : 0;
        const uint8_t op = B(row)[0x54];
        if (op < 6 || op > 7) continue;
        const uint16_t* ids = reinterpret_cast<const uint16_t*>(B(row) + 0x58);
        for (int i = 0; i < 3; ++i) {
            if (ids[i] == 0xFFFF) break;
            const uint32_t id = ids[i];
            const uint32_t opRow = id < static_cast<uint32_t>(iscCount) ? iscBase + id * 0x144 : 0;
            const int32_t v = c->readStat(c->ctx, unitData, id << 16);
            c->apply(c->ctx, unitData, id << 16, v, opRow, unit);
        }
    }
}
