// 1.13c D2Game+0x55840 sub_55840: create the player unit for one game. EDI = game, stack = client record, ppUnit (ret 8).
// A null client is fatal (line 0x206, _exit). Callees are seams on E: E::h(rva, eax, ecx, edx, ebx, esi, stack words...) and E::rd*/wr*.
// `dt` is the data-tables record (*sgptDataTables): +0xBC4 the 0xC4-byte rows, +0xBC8 the signed row count.
#pragma once
#include "d2game_fatal.h"

namespace d2g55840 {
template <class E> uint32_t Run(E& e, uint32_t client, uint32_t outp, uint32_t game, uint32_t dt, uint32_t mod)
{
    if (client == 0) { d2gfatal::Fatal(e, 0x206, mod); return 0; }
    const uint32_t sel = e.rd8(client + 8);
    e.wr32(client + 0x3D4, e.rd32(client + 0x3D4) | 1);
    uint32_t unit = 0;
    if (sel < 7) {
        unit = e.h(0xA6B8, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 0);
        if (unit) {
            e.wr32(unit, 0);
            e.wr32(unit + 4, sel);
            e.wr32(unit + 8, e.rd32(game + 0x1C));
            e.wr32(unit + 0x80, game);
            const uint32_t f = e.rd32(unit + 0xC8);
            e.wr32(unit + 0xC8, (e.rd32(game + 0x70) == 0 ? (f & 0xFDFFFFFFu) : (f | 0x02000000u)) | 0x04000000u);
            const uint32_t cls = e.h(0xA556, 0, 0, 0, 0, 0, 0);
            const uint32_t idx = e.h(0xA56E, 0, 0, 0, 0, 0, cls) & 0xFF;
            e.wr8(unit + 0x18, idx);
            e.wr32(unit + 0x1C, e.rd32(game + 0xBC + idx * 4));
            e.wr32(unit + 0x74, 0);
            e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x10);
            e.wr32(unit + 0xD0, 0xB);
            uint32_t seq = e.rd32(game + 0x90) + 1;
            if (seq == 0xFFFFFFFFu) seq = 1;
            e.wr32(game + 0x90, seq);
            e.h(0x38620, game, 0, 0, 0, 0, seq);
            e.h(0xEFED0, game, 0, 0, unit, 0, 0);
        }
    }
    e.h(0x53720, client, unit, 0, 0, 0, game);
    e.h(0x7B0A0, 0, 0, 0, 0, 0, game, unit, client);
    const uint32_t gate = e.rd8(client + 0x1AC);
    e.h(0xEB2C0, gate, unit, 0, 0, 0, game);
    e.h(0x385C0, 0, 0, 0, 0, 0, unit, game);
    const uint32_t st = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    e.wr32(st + 0x74, 0);
    e.wr32(st + 0x70, 0);
    if (gate == 0 && unit) {
        const int32_t id = (int32_t)e.rd32(unit + 4);
        if (id >= 0 && id < (int32_t)e.rd32(dt + 0xBC8)) {
            const uint32_t row = (uint32_t)id * 0xC4 + e.rd32(dt + 0xBC4);
            if (row && e.rd16s(row + 0xAC) != 0) {
                const uint32_t v = (uint32_t)e.rd16s(row + 0xAC);
                if (e.h(0xA3F4, 0, unit, v, 0, 0)) {
                    e.h(0xA2100, 0, unit, 0, 0, 0, v, 0xFFFFFFFFu);
                    e.wr32(st + 0x70, v);
                }
            }
        }
    }
    e.h(0x35090, 0, 0, 0, 0, 0, game, unit, 1);
    e.wr32(outp, unit);
    return 0;
}
}
