// 1.13c D2Client+0x72610. Registers: EAX=skill, ECX=a, EDX=unit; stack f, x, y (ret 0xC).
// Globals and import-thunk callees are explicit (Env / scalars); the retail argument orders come from the disassembly
// (the decompile drops several: 0xC4B4 takes (h, skill, y), 0xC562 takes (h, skill, x, y)).
#include <cstdint>
#include <emscripten.h>
struct D2c72610Env {
    uint32_t (*c1c0)(uint32_t, uint32_t);                                   // Ordinal_10871(unit, a)
    uint32_t (*c442)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);     // Ordinal_11013(0,0,0,unit[0],unit[3])
    void (*c2f2)(uint32_t, uint32_t);                                       // Ordinal_10785(h, a)
    void (*c36a)(uint32_t, uint32_t, uint32_t);                             // Ordinal_10807(unit, h, 1)
    uint32_t (*c4b4)(uint32_t, uint32_t, uint32_t);                         // Ordinal_10680(h, skill, y)
    void (*c562)(uint32_t, uint32_t, uint32_t, uint32_t);                   // Ordinal_10188(h, skill, x, y)
    void (*f559e0)(uint32_t, uint32_t);                                     // 0xA59E0(stack x, EBX = (x & ~0xFF) | local)
    void (*c5c8)(uint32_t, const char*, uint32_t);                          // Ordinal_10819(unit, file, 0x5D2)
};
static inline uint32_t r32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
extern "C" uint32_t d2_client_72610(uint32_t skill, uint32_t a, uint32_t unit, uint32_t f, uint32_t x, uint32_t y,
                                    int32_t dt_bd4, uint32_t dt_bcc, uint32_t mask_byte4, const D2c72610Env* e) {
    if (!unit) return 0;
    uint32_t local = skill == 0xac ? 4 : 0, h = f;
    if (!h) {
        h = e->c1c0(unit, a);
        if (!h) { h = e->c442(0, 0, 0, r32(unit), r32(unit + 12)); e->c2f2(h, a); e->c36a(unit, h, 1); }
        else local = e->c4b4(h, skill, y);
    }
    e->c562(h, skill, x, y);
    if (skill == 0xac) e->f559e0(x, (x & ~0xFFu) | (local & 0xFFu));
    else if ((int32_t)skill < 0) return h;
    if ((int32_t)skill < dt_bd4) {
        uint32_t rec = dt_bcc + skill * 0x144;
        if (rec && (mask_byte4 & *(const uint8_t*)(uintptr_t)(rec + 5))) e->c5c8(unit, "..\\Source\\D2Client\\UNIT\\PlrSkills.cpp", 0x5d2);
    }
    return h;
}
