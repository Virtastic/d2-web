// 1.13c D2Launch+0xDEC0 -- stdcall message handler (accept the selected character): returns 1 at once when the flag at image offset 0x25EAC is set. Clears the byte at +0x20C of the launch record (0x25ABC). A selection (0x24DB0) equal to the create-new slot (0x24DB4) runs the worker 0x16E80 unless the mode (0x25AD4) is 1 with the flag at 0x25EE4 set, counts 0x25EE8 down and sets 0x25EE4 when it reaches 0. One equal to the slot at 0x24DB8 clears the text control at 0x25EE0 (D2Win 10106) and writes strings 0x2B3C and 0x2B3D into it. Otherwise the node of the character list (0x25EC8) at the selection is taken: modes 1 and 3 copy its two 16-byte keys, its class and act bytes and its flags word into the record; a hardcore-dead node needs Fog 10227 non-zero (else returns 0); an expansion ladder node in mode 0 or 2 shows string 0x14B8; a node whose difficulty bits allow it in mode 0 (or mode 2 with record +0x15 = 8) goes to the difficulty picker 0xA700 with its hardcore bit; anything else narrows the realm name and runs the worker 0x1AAB0 (EAX = the node, its class byte and the name). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25eac;
__attribute__((weak)) uint32_t g_d2launch_24db0_a3[3];
__attribute__((weak)) uint32_t g_d2launch_25abc;
__attribute__((weak)) uint32_t g_d2launch_25ad4;
__attribute__((weak)) uint32_t g_d2launch_25ee0_a3[3];
__attribute__((weak)) uint32_t g_d2launch_25ec8;
namespace d2launch {
static uint32_t rd16(GE& e, uint32_t a) { return e.rd8(a) | (e.rd8(a + 1) << 8); }
static void wr16(GE& e, uint32_t a, uint32_t v) { e.wr8(a, v & 0xff); e.wr8(a + 1, (v >> 8) & 0xff); }
uint32_t fdec0(GE& e, uint32_t)
{
    if (g_d2launch_25eac) return 1;
    const uint32_t sel = g_d2launch_24db0_a3[0], rec = g_d2launch_25abc, mode = g_d2launch_25ad4;
    e.wr8(rec + 0x20c, 0);
    if (sel == g_d2launch_24db0_a3[1]) {
        if (mode == 1 && g_d2launch_25ee0_a3[1]) return 1;
        e.h(0x16E80);
        if ((int32_t)--g_d2launch_25ee0_a3[2] <= 0) g_d2launch_25ee0_a3[1] = 1;
        return 1;
    }
    if (sel == g_d2launch_24db0_a3[2]) {
        const uint32_t c = g_d2launch_25ee0_a3[0];
        e.h(0x996a, 0, c);
        e.h(0x99be, 0, c, e.h(0x9b32, 0, 0x2b3c), 0, 0, 0, 0, 0);
        e.h(0x99be, 0, c, e.h(0x9b32, 0, 0x2b3d), 0, 0, 0, 0, 0);
        return 1;
    }
    if (sel == 0xffffffffu) return 1;
    uint32_t n = g_d2launch_25ec8;
    for (uint32_t i = 0; n && i != sel; i++) n = e.rd32(n + 0x34c);
    if (!n) return 1;
    if (mode == 1 || mode == 3) {
        for (uint32_t k = 0; k < 0x20; k++) e.wr8(rec + 0x39d + k, e.rd8(n + 0x300 + k));
        e.wr8(rec + 0x3bd, e.rd8(n + 0x322));
        e.wr8(rec + 0x3be, e.rd8(n + 0x32e));
        wr16(e, rec + 0x3c7, rd16(e, n + 0x324));
    }
    if ((e.rd8(n + 0x324) & 0x20) && !e.h(0x980e)) return 0;
    const uint32_t fl = rd16(e, n + 0x324);
    if ((fl & 4) && (fl & 8) && (mode == 0 || mode == 2)) {
        e.h(0x15090, 0, 0, 0, 0, 0, e.h(0x9b32, 0, 0x14b8), 0xB2012b60u, 1);
        return 1;
    }
    if (mode == 0 || (mode == 2 && e.rd32(rec + 0x15) == 8)) {
        const uint32_t lvl = (fl >> 8) & 0x1f;
        if ((fl & 0x20) ? lvl >= 5 : lvl >= 4) { e.h(0xA700, 0, 0, 0, 0, 0, fl & 0x20); return 1; }
    }
    uint8_t buf[0x100];
    e.h(0x1c03c, 0, e.stk(buf), n + 0x100, 0, 0, 0xff);
    e.h(0x1AAB0, n, 0, 0, 0, 0, e.rd8(n + 0x320), e.stk(buf));
    return 1;
}
}
