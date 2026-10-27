#include <cstdint>

// 1.13c D2Game+0x30150 (fastcall pGame, pRoom + stack density, objectId, probability; ret 0xC): crate/urn field populate,
// Every D2Common ordinal, callee, DLL table and game/unit memory access goes through the seam so a replay can
// feed recorded values. Seam ids (w = up to 6 dword args):
//  1 O10688{id, store} objects.txt record (store = 1: the value to write into memory)   2 O10561{room} room coordinates (ptr to x,y,w,h dwords)   3 0x2E5D0{room,sizeX,sizeY,x,y} placement check
//  4 O10459 alloc unit{pool}         5 O10826{room} level   6 O10459 act{level}   7 0xE0110{game+0xD0, unit}   8 Fog alloc{pool, 0x38, line}
//  9 0xA724{unit}   10 0x2F300{counter, room, x, y, unit, game}   11 0xE0F40{room, y, unit, x, game}   12 rd{ptr, off, size}   13 wr{ptr, off, size, value}
//  14 const{i}: 0 urn count (DAT 0x6FD2E224), 1..5 urn ids (0x6FD2E1F8), 8..15 signX (0x6FD2E00C), 16..23 signY (0x6FD2E02C)
//  15 assert-exit{line} (retail _exit(-1): the caller must treat it as the end)
struct D2G30150Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); };

// lcg = the 64-bit seed (low dword multiplicand, high dword addend) game+0x10F0 points at. Returns the last unit made (retail's eax).
// 1.13c D2Game+0x30150
uint32_t GAME_30150(const D2G30150Seam& s, uint32_t game, uint32_t room, uint32_t density, uint32_t objectId, uint32_t prob, uint64_t& lcg, uint32_t* assertLine)
{
    auto step = [&] { lcg = (uint64_t)(uint32_t)lcg * 0x6AC690C5ull + (uint32_t)(lcg >> 32); return (uint32_t)lcg; };
    // retail's inlined limited roll: n < 1 -> 0 without a step; power of two -> step & (n - 1); else step % n
    auto limited = [&](int32_t n) -> uint32_t { if (n < 1) return 0; const uint32_t r = step(); return ((n & (n - 1)) == 0) ? (r & (uint32_t)(n - 1)) : r % (uint32_t)n; };
    auto rd = [&](uint32_t p, uint32_t off, uint32_t n) { const uint32_t w[6] = { p, off, n }; return s.call(12, w); };
    auto wr = [&](uint32_t p, uint32_t off, uint32_t n, uint32_t v) { const uint32_t w[6] = { p, off, n, v }; s.call(13, w); };
    auto c1 = [&](uint32_t id, uint32_t a, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0) { const uint32_t w[6] = { a, b, c, d, e, f }; return s.call(id, w); };

    const uint32_t rec = c1(1, objectId);
    if ((density & 0xFF) > 0x80) { c1(15, 0x9D7); *assertLine = 0x9D7; return 0; }
    if (!room) { /* 0xA06 assert is reached after the first roll below in retail only when a unit is about to be made */ }
    if (step() % 100 > (prob & 0xFFFF)) return 0;
    const uint32_t cp = c1(2, room);
    const int32_t cx = (int32_t)rd(cp, 0, 4), cy = (int32_t)rd(cp, 4, 4), cw = (int32_t)rd(cp, 8, 4), ch = (int32_t)rd(cp, 12, 4);
    int32_t j = ((cw * ch >> 7) * (int32_t)(density & 0xFF)) >> 8;
    int32_t k = 2 * j, l = 4 * j;
    uint32_t pObject = 0;
    int32_t counter = 0;
    const uint32_t sizeX = rd(rec, 0xD0, 4), sizeY = rd(rec, 0xD4, 4);
    while (j > 0 && k > 0) {
        uint32_t cls = objectId;
        if (cls != 0x2E) cls = c1(14, 1 + limited((int32_t)c1(14, 0)));
        const int32_t x = cx + (int32_t)limited(cw);
        const int32_t y = cy + (int32_t)limited(ch);
        int32_t px = x, py = y;
        if (c1(3, room, sizeX, sizeY, (uint32_t)px, (uint32_t)py) == 1) {
            if (cls != 0x2E) cls = c1(14, 1 + limited((int32_t)c1(14, 0)));
            if (!room) { c1(15, 0xA06); *assertLine = 0xA06; return pObject; }
            auto makeUnit = [&](uint32_t cx2, uint32_t cy2) -> uint32_t {
                const uint32_t pool = rd(game, 0x1C, 4);
                const uint32_t u = c1(4, pool, 2);
                if (!u) return 0;
                wr(u, 4, 4, cls); wr(u, 0, 4, 2); wr(u, 8, 4, rd(game, 0x1C, 4)); wr(u, 0x80, 4, game);
                uint32_t f = rd(u, 0xC8, 4);
                f = rd(game, 0x70, 4) ? (f | 0x2000000) : (f & 0xFDFFFFFFu);
                wr(u, 0xC8, 4, f); wr(u, 0xC8, 4, f | 0x4000000);
                const uint32_t act = c1(6, c1(5, room)) & 0xFF;
                wr(u, 0x18, 1, act); wr(u, 0x1C, 4, rd(game, 0xBC + 4 * act, 4)); wr(u, 0x74, 4, 0);
                c1(7, game + 0xD0, u);
                wr(u, 0xC4, 4, rd(u, 0xC4, 4) | 0x10); wr(u, 0xD0, 4, 0xB);
                uint32_t cnt = rd(game, 0x98, 4) + 1; if (cnt == 0xFFFFFFFFu) cnt = 1;
                wr(game, 0x98, 4, cnt);
                const uint32_t ud = c1(8, rd(game, 0x1C, 4), 0x38, 0x265);
                wr(u, 0x14, 4, ud);
                for (uint32_t i = 0; i < 0x38; i += 4) wr(ud, i, 4, 0);
                wr(ud, 0, 4, c1(1, cls, 1));   // the pointer value stored (a replay stores its scrubbed form)
                wr(u, 0x10, 4, 0);
                c1(9, u);
                c1(10, cnt, room, cx2, cy2, u, game);
                c1(11, room, cy2, u, cx2, game);
                return u;
            };
            makeUnit((uint32_t)px, (uint32_t)py);
            bool spawn = true; int32_t nMax = 1; pObject = 0;
            while (!limited(nMax / 2) && spawn) {   // retail: nMax >> 1 (sar), roll, then the flag
                spawn = false;
                const int32_t n = l >= 4 ? l : 4;
                for (int32_t i = 0; !spawn && i < n; i++) {
                    const uint32_t v = step() & 7;
                    px += (int32_t)(rd(rec, 0x162, 1) * c1(14, 8 + v));
                    py += (int32_t)(rd(rec, 0x163, 1) * c1(14, 16 + v));
                    spawn = c1(3, room, sizeX, sizeY, (uint32_t)px, (uint32_t)py) != 0;
                }
                if (spawn) {
                    pObject = makeUnit((uint32_t)px, (uint32_t)py);
                    if (pObject) { if (++counter >= 8) return pObject; }
                    nMax++;
                }
            }
            --j; l -= 4;
        }
        --k;
    }
    return pObject;
}
