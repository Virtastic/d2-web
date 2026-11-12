// 1.13c D2Launch+0x17780 -- Panel builder, no arguments (account log-in panel): runs the worker 0x1B310, then creates thirteen controls through D2Win 10098 appending each to the control list (0x252B8, count 0x25E30; 64 is fatal): three plain ones (descriptors at image offsets 0x21310, 0x21340, 0x21370), three captioned through D2Win 10020 (0x21400, 0x21430, 0x21460 with strings 0x1468, 0x1469, 0x2A52), the text line 0x214F0 with string 0x1466 (skipped when Fog 10227 answers non-zero) and 0x21520 with string 0x1467 through D2Win 10000, the name box 0x21EE0 (saved at 0x254FC; filter 0x12630, limit 16, text from the launch record +0x89), the password box 0x21F10 (0x2529C; filter 0x12620, limit 13, text from +0xA1), the third box 0x21F40 (0x25518; limit 13); the focus (D2Win 10053) goes to the name box when the name is empty, else the password box when the password is empty, else the third; the three are chained through D2Win 10146; then 0x21F70 and 0x21FA0. Returns the last control.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e30;
__attribute__((weak)) uint32_t g_d2launch_252b8_a16[16];
__attribute__((weak)) uint32_t g_d2launch_254fc;
__attribute__((weak)) uint32_t g_d2launch_2529c;
__attribute__((weak)) uint32_t g_d2launch_25518;
__attribute__((weak)) uint32_t g_d2launch_25abc;
namespace d2launch {
uint32_t f17780(GE& e, uint32_t)
{
    e.h(0x1B310);
    auto add = [&](uint32_t def) { const uint32_t v = e.h(0x9952, 0, 0, 0, 0, 0, def); g_d2launch_252b8_a16[g_d2launch_25e30] = v; g_d2launch_25e30++; return v; };
    add(0xB2021310u);
    add(0xB2021340u);
    add(0xB2021370u);
    uint32_t v;
    const uint32_t cap[3][2] = { { 0xB2021400u, 0x1468 }, { 0xB2021430u, 0x1469 }, { 0xB2021460u, 0x2a52 } };
    for (const auto& c : cap) { v = add(c[0]); e.h(0x98c8, 0, v, e.h(0x9b32, 0, c[1])); }
    if (!e.h(0x980e)) { v = add(0xB20214f0u); e.h(0x99be, 0, v, e.h(0x9b32, 0, 0x1466), 0, 0, 0, 0, 0); }
    v = add(0xB2021520u);
    e.h(0x99be, 0, v, e.h(0x9b32, 0, 0x1467), 0, 0, 0, 0, 0);
    const uint32_t rec = g_d2launch_25abc;
    uint32_t& nm = g_d2launch_254fc;
    uint32_t& pw = g_d2launch_2529c;
    uint32_t& x3 = g_d2launch_25518;
    nm = add(0xB2021ee0u);
    e.h(0x9934, 0, nm, 0xB2012630u);
    e.h(0x998e, 0, nm, 0x10);
    e.h(0x98bc, 0, nm, rec + 0x89);
    pw = add(0xB2021f10u);
    e.h(0x9934, 0, pw, 0xB2012620u);
    e.h(0x998e, 0, pw, 0xd);
    e.h(0x98bc, 0, pw, rec + 0xa1);
    x3 = add(0xB2021f40u);
    e.h(0x998e, 0, x3, 0xd);
    e.h(0x988c, 0, 0, 0, 0, 0, !e.rd8(rec + 0x89) ? nm : !e.rd8(rec + 0xa1) ? pw : x3);
    e.h(0x9988, 0, nm, pw);
    e.h(0x9988, 0, pw, x3);
    e.h(0x9988, 0, x3, nm);
    add(0xB2021f70u);
    return add(0xB2021fa0u);
}
}
