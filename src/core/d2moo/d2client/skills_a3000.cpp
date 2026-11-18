// D2Client rva 0xA3000: the unit's selected skill (EDI = unit), its skill-table record and level, handed to sub_A2C90.
// Callees come through the environment in retail's call order.
#include <cstdint>

struct D2ClientA3000Callees {
    uint32_t (*thunkC310)(uint32_t unit);                                    // import (stdcall 1): the selected skill; 0 returns
    uint32_t (*thunkC42A)(uint32_t h, const char* file, uint32_t line);      // import (stdcall 3): skill record
    uint32_t (*thunkC718)(uint32_t unit, uint32_t h, uint32_t one);         // import (stdcall 3): skill level, bonus flag 1
    void (*subA2C90)(uint32_t unit, uint32_t rec, uint32_t level, uint32_t zero);   // stdcall 4
};

// 1.13c D2Client+0xA3000 (EDI = unit, kept through the call)
void d2_client_a3000(const D2ClientA3000Callees& c, uint32_t unit) {
    uint32_t h = c.thunkC310(unit);
    if (!h) return;
    uint32_t rec = c.thunkC42A(h, "..\\Source\\D2Client\\SKILLS\\Skills.cpp", 0x697);
    uint32_t lvl = c.thunkC718(unit, h, 1);
    c.subA2C90(unit, rec, lvl, 0);
}
