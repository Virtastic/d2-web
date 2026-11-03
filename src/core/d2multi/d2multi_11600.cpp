// 1.13c D2Multi+0x11600: five Ordinal_10098(string) handles into DAT_6fa09d90/94/98/9c/a8, each then Ordinal_10020(handle, GetStringByIndex(id)).
#include "d2multi_env.h"

void d2multi_11600(const D2mEnv& e, uint32_t* g /* the dwords at DAT_6fa09d90.. (7 words) */) {
    static const struct { uint16_t str; uint16_t id; uint8_t slot; } k[5] = {
        { 0xb740, 0x1493, 0 }, { 0xb770, 0x1494, 1 }, { 0xb7a0, 0x1496, 2 }, { 0xb7d0, 0x1497, 3 }, { 0xb890, 0x1492, 6 } };
    for (const auto& c : k) {
        g[c.slot] = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x10000 + c.str));
        uint32_t u = d2m_call(e, 0x7ea8, 0, c.id);
        d2m_call(e, 0x7c56, 0, g[c.slot], u);
    }
}
