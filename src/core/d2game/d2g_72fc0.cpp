// 1.13c D2Game+0x72FC0 -- ITEM_FindCubeRecipeAndApply: stdcall (game, cube), ret 8; no result. `dt` is *sgptDataTables
// (IAT slot 0xF829C), `en` the module global 0x111C14 (disabled recipes allowed), `st` the SYSTEMTIME GetLocalTime filled
// (the FN descriptor pins it). Written from the 1.13c disassembly (jump table 0x734A4, 14 cases).
// n = 0xB0D8(); cnt = 0x70190(EAX cube+0x60); cnt <= 0 ends before the clock is read. For i < n: rec = 0xB0D2(i) (null ends);
// skipped unless (byte +0 or en), (game+0x70 or word +0x12 < 100), (byte +6A or +74 of the game, or byte +1 clear),
// byte +2 <= game byte +6D, byte +3 0xFF or the cube's class, byte +0x10 == cnt. Byte +4 picks the date or stat rule:
// 1 wDay in [+8, +0xC]; 2 wDayOfWeek + 1 == +0xC; 3..14 a stat id +8 (negative or past dt+0xBD4 passes) whose
// ItemStatCost row 0x10D0(EAX id) must exist, compared with +0xC >> row byte +0x18 against 0xA42A / 0xA3E2 / 0xB0CC
// (cube, id, 0) as >=, <=, !=, == per group of four; anything else passes. A passing record gets the zeroed 9-dword
// buffer filled by 0x70CF0(EAX rec; game, &buf); nonzero runs 0x72220(game, cube, rec, &buf) and ends.
#include "../fn/hookenv.h"
namespace d2g {
void f72fc0(HookEnv& e, uint32_t game, uint32_t cube, uint32_t dt, uint32_t en, uint32_t st)
{
    if (!cube) return;
    const int32_t n = (int32_t)e.h(0xB0D8, 0);
    const int32_t cnt = (int32_t)e.h(0x70190, e.rd32(cube + 0x60));
    if (cnt <= 0) return;
    const int32_t dow1 = (int32_t)(e.rd16s(st + 4) & 0xFFFF) + 1, day = (int32_t)(e.rd16s(st + 6) & 0xFFFF);
    static const uint32_t kFn[3] = { 0xA42A, 0xA3E2, 0xB0CC };
    for (int32_t i = 0; i < n; i++) {
        const uint32_t rec = e.h(0xB0D2, 0, 0, 0, 0, 0, (uint32_t)i);
        if (!rec) return;
        if (e.rd8(rec) == 0 && en == 0) continue;
        if (e.rd32(game + 0x70) == 0 && (e.rd16s(rec + 0x12) & 0xFFFF) >= 0x64) continue;
        if (e.rd8(game + 0x6A) == 0 && e.rd32(game + 0x74) == 0 && e.rd8(rec + 1) != 0) continue;
        if (e.rd8(rec + 2) > e.rd8(game + 0x6D)) continue;
        const uint32_t b3 = e.rd8(rec + 3);
        if (b3 != 0xFF && b3 != e.rd32(cube + 4)) continue;
        if ((int32_t)e.rd8(rec + 0x10) != cnt) continue;
        const uint32_t k = (uint32_t)e.rd8(rec + 4) - 1;
        if (k == 0) {
            if ((int32_t)e.rd32(rec + 8) > day || (int32_t)e.rd32(rec + 0xC) < day) continue;
        } else if (k == 1) {
            if ((int32_t)e.rd32(rec + 0xC) != dow1) continue;
        } else if (k <= 0xD) {
            const int32_t s = (int32_t)e.rd32(rec + 8);
            if (s >= 0 && s <= (int32_t)e.rd32(dt + 0xBD4)) {
                const uint32_t row = e.h(0x10D0, (uint32_t)s);
                if (!row) continue;
                const int32_t want = (int32_t)e.rd32(rec + 0xC) >> (e.rd8(row + 0x18) & 31);
                const int32_t v = (int32_t)e.h(kFn[(k - 2) / 4], 0, 0, 0, 0, 0, cube, (uint32_t)s, 0);
                const uint32_t op = (k - 2) % 4;
                if ((op == 0 && v < want) || (op == 1 && v > want) || (op == 2 && v == want) || (op == 3 && v != want)) continue;
            }
        }
        uint32_t buf[9] = {};
        if (!e.h(0x70CF0, rec, 0, 0, 0, 0, game, e.stk(buf))) continue;
        e.h(0x72220, 0, 0, 0, 0, 0, game, cube, rec, e.stk(buf));
        return;
    }
}
}
