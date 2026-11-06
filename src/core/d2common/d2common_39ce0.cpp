// 1.13c D2Common+0x39CE0: recursive stat dependency walk.
// Retail (decompile + disassembly): value = 0x39530(owner, key). A record whose byte +0x51 is zero is recorded once and returned.
// Otherwise a zero value is recorded first, then each of up to three child ids (u16 at rec+0x58, stop at 0xFFFF) is resolved
// recursively and recorded; a nonzero child value clears "keep" by rec+0x54 (mode): 0, 8..11 never; 2..3 when the owner mode (+8) is
// 0 or 1, the child's stat (u16 at rec+0xDE+6*i) is not 0xFFFF and the owner's list (+0x48 when owner+0x10 is negative, else +0x24)
// holds that stat with a positive value; 4..5 when the unit (owner+4) is of type 0 or 1 and 0x38B70(unit, stat) > 0; 6..7 when the
// owner mode is 0 or 1; 13 when the owner mode is 4; every other mode always. If still kept and the value nonzero, the record is
// written again and up to 64 dependants (u16 at rec+0x5E) are resolved and recorded. A child past the table is a null record.
#include "d2common_39ce0.h"

namespace {
inline const uint8_t* B(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
inline uint32_t D(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(B(a) + o); }
inline uint32_t H(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint16_t*>(B(a) + o); }
}

extern "C" int32_t D2Common_39CE0_Injected(const D2Common39CE0Callees* c, uint32_t owner, uint32_t key, uint32_t rec, uint32_t flags,
                                           uint32_t iscBase, int32_t iscCount)
{
    auto row = [&](uint32_t id) { return id < static_cast<uint32_t>(iscCount) ? iscBase + id * 0x144 : 0u; };
    const int32_t value = c->lookup(c->ctx, owner, key);
    if (B(rec)[0x51] == 0) { c->record(c->ctx, owner, key, value, rec, flags); return value; }
    if (value == 0) c->record(c->ctx, owner, key, 0, rec, flags);
    bool keep = true;
    for (uint32_t i = 0; i < 3; ++i) {
        const uint32_t id = H(rec, 0x58 + 2 * i);
        if (id == 0xFFFF) break;
        const uint32_t child = row(id);
        const int32_t cv = D2Common_39CE0_Injected(c, owner, id << 16, child, flags, iscBase, iscCount);
        c->record(c->ctx, owner, id << 16, cv, child, flags);
        if (cv == 0) continue;
        const uint32_t stat = H(rec, 0xDE + 6 * i);
        const uint32_t om = D(owner, 8);
        switch (B(rec)[0x54]) {
        case 0: case 8: case 9: case 10: case 11: break;
        case 2: case 3:
            if ((om == 0 || om == 1) && stat != 0xFFFF) {
                const uint32_t list = owner + (static_cast<int32_t>(D(owner, 0x10)) >= 0 ? 0x24 : 0x48);
                const int32_t n = c->find(c->ctx, list, stat << 16);
                if (n >= 0) {
                    const uint32_t ent = D(list, 0) + static_cast<uint32_t>(n) * 8;
                    if (ent != 0 && static_cast<int32_t>(D(ent, 4)) > 0) keep = false;
                }
            }
            break;
        case 4: case 5: {
            const uint32_t unit = D(owner, 4);
            if (unit != 0 && (D(unit, 0) == 0 || D(unit, 0) == 1) && stat != 0xFFFF && c->readStat(c->ctx, unit, stat) > 0) keep = false;
            break; }
        case 6: case 7:
            if (om == 0 || om == 1) keep = false;
            break;
        case 13:
            if (om == 4) keep = false;
            break;
        default: keep = false; break;
        }
    }
    if (keep && value != 0) {
        c->record(c->ctx, owner, key, value, rec, flags);
        for (uint32_t i = 0; i < 64; ++i) {
            const uint32_t id = H(rec, 0x5E + 2 * i);
            if (id == 0xFFFF) break;
            const uint32_t child = row(id);
            const int32_t cv = D2Common_39CE0_Injected(c, owner, id << 16, child, flags, iscBase, iscCount);
            c->record(c->ctx, owner, id << 16, cv, child, flags);
        }
    }
    return value;
}
