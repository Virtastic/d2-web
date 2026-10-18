#include <cstdint>

// 1.13c D2Game+0x2F0D0 (stack pGame, pRoom in a register): per-room object-group population, 
// Every D2Common ordinal, DLL memory read and function-pointer-table access goes through the seam so a replay can feed recorded values.
// Seam ids: 1 O10826{room}, 2 O10014{that}, 3 gate 2EEA0{game}, 4 O11025{room}, 5 O10688{id} (objects.txt record ptr), 6 O10465{id} (group ptr),
// 7 rd{ptr, off, size} (little-endian unsigned read of DLL/region memory), 8 gore byte (DAT 0x6FD2E1B8), 9 slot state{slot} 0 null / 1 bad code ptr / 2 ok,
// 10 invoke slot{slot, density, id, 100}, 11 assert-exit{line} (retail _exit(-1): the caller must treat it as the end).
struct D2G2F0D0Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); };

// lcg = the 64-bit seed at room+0x6C (low dword is the multiplicand, high dword the addend); levelsRec/region come from seam reads.
// Returns 0 normally, 1 when an assert-exit fired.
// 1.13c D2Game+0x2F0D0
uint32_t GAME_2F0D0(const D2G2F0D0Seam& s, uint32_t game, uint32_t room, uint64_t& lcg)
{
    auto step = [&] { lcg = (uint64_t)(uint32_t)lcg * 0x6AC690C5ull + (uint32_t)(lcg >> 32); return (uint32_t)lcg; };
    auto rd = [&](uint32_t p, uint32_t off, uint32_t n) { const uint32_t w[7] = { p, off, n }; return s.call(7, w); };
    auto c1 = [&](uint32_t id, uint32_t a) { const uint32_t w[7] = { a }; return s.call(id, w); };

    const uint32_t rec = c1(2, c1(1, room));
    if (!rec) { c1(11, 0xBEC); return 1; }
    if (!c1(3, game)) return 0;
    for (uint32_t i = 0; i < 8; i++) {
        const int32_t g = (int8_t)rd(rec, 0xE5 + i, 1);
        uint32_t r = step() % 100;
        const uint32_t reg = rd(rd(game, 0x10F0, 4), 0x48 + 4 * c1(4, room), 4);
        const int32_t a = (int32_t)rd(reg, 8, 4), b = (int32_t)rd(reg, 4, 4);
        if (0 < a && 0x60 < (b << 7) / a && rd(reg, 0x10, 4) == 0 && (uint8_t)rd(c1(5, (uint32_t)g), 0x167, 1)) r = 100;
        if (g == 0 || r > rd(rec, 0xED + i, 1)) continue;
        const uint32_t grp = c1(6, (uint32_t)g);
        if (!grp) return 0;
        const uint32_t r2 = step() % 100;
        int32_t acc = 0;
        for (uint32_t k = 0; k < 8; k++) {
            const uint32_t id = rd(grp, 4 * k, 4);
            if (!id) break;
            acc += (uint8_t)rd(grp, 0x28 + k, 1);
            const uint32_t o = c1(5, id);
            if ((int32_t)r2 < acc && (uint8_t)rd(o, 0x172, 1) <= s.call(8, nullptr)) {
                const uint32_t slot = (uint8_t)rd(o, 0x1B2, 1);
                if (slot >= 10) { c1(11, 0xC43); return 1; }
                const uint32_t st = c1(9, slot);
                if (st == 0) break;
                if (st == 1) { c1(11, 0xC46); return 1; }
                const uint32_t dens = (uint8_t)rd(grp, 0x20 + k, 1);
                if (dens >= 0x80) { c1(11, 0xC47); return 1; }
                const uint32_t w[7] = { slot, dens, id, 100 };
                s.call(10, w);
                break;
            }
        }
    }
    return 0;
}
