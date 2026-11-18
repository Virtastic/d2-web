// 1.13c D2Client+0x28BC0: item hover/description dispatch (EAX unit, stack item, x, y, z; ret 0x10). Leaves at once when
// the unit or the item is zero or #10858(ECX = item) has no record. The flag from 0x3DA60(EAX = item) becomes 1 when the
// record byte +5 misses the D2Common bitmask byte and #C1F0(unit) has a #C208-true owner. #10537 fills three out-slots
// (-1, 0 and the caller's item slot, zeroed first); on failure #C718(unit, item, 1) refills the second and #CE9E(item)
// names the alternative. 0x3E5E0 always draws; then 0x3D8A0 (alternative present) or 0x3E770.
#include "d2client_callee.h"
// 1.13c D2Client+0x28BC0
extern "C" void d2c_28bc0(const D2cSvc& v, uint32_t unit /*EAX*/, uint32_t item, uint32_t a1, uint32_t a2, uint32_t a3,
                          const uint8_t* mask /*[0xCEE50]*/) {
    if (!unit || !item) return;
    const uint32_t rec = d2c_call(v, 0xc640, 0, item, 0, 0, 0);
    if (!rec) return;
    const uint32_t l3 = d2c_call(v, 0x3daa0, 0, 0, 0, 0, 0);
    uint32_t b = d2c_call(v, 0x3da60, item, 0, 0, 0, 0);
    if (!(mask[0] & ((const uint8_t*)(uintptr_t)rec)[5])) {
        const uint32_t o = d2c_call(v, 0xc1f0, 0, 0, 0, 0, 0, unit);
        if (o && d2c_call(v, 0xc208, 0, 0, 0, 0, 0, o)) b = 1;
    }
    uint32_t l1 = 0xffffffffu, l0 = 0, slot = 0;
    const uint32_t ok = d2c_call(v, 0xc766, 0, 0, 0, 0, 0, item, (uint32_t)(uintptr_t)&l1, 0, (uint32_t)(uintptr_t)&l0,
                                 (uint32_t)(uintptr_t)&slot);
    if (!ok) l0 = d2c_call(v, 0xc718, 0, 0, 0, 0, 0, unit, item, 1);
    const uint32_t alt = ok ? 0 : d2c_call(v, 0xce9e, 0, 0, 0, 0, 0, item);
    d2c_call(v, 0x3e5e0, a1, b, 0, 0, 0, l3, rec, l1, slot, a2, a3);
    if (alt) { d2c_call(v, 0x3d8a0, 0, 0, 0, 0, 0, rec, b, alt, a1, a2); return; }
    d2c_call(v, 0x3e770, a1, 0, 0, 0, a2, rec, b, l0, slot);
}
