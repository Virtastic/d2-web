// Implementation batch 1bdf0: NPC AI cmd type 3 (walk-to-town-portal style) setup, from the cached 1.13c decompile.
// Callees by retail RVA (r_NNNN) / ordinal. Register-passed args (EAX = unit) become explicit; not yet linked (RE-DONE only).
#include <cstdint>
using i32 = int32_t; using u8 = uint8_t;
extern "C" {
i32 r_21100(i32 unit); i32 r_21130(i32 unit);
i32 r_56A00(i32 unit); i32 r_60400(i32 unit); i32 r_663A0(i32 unit); i32 r_5CC30(i32 unit);
void r_21290(i32 unit, void* cmd); void r_F21E0(i32 game, i32 x, i32 y); void r_F22C0(i32 v);
void o10887();
}
// Find AI-command node of type 3 in unit->info(+0x14)->(+0x28)->list(+0x20); circular list, null if absent.
static i32* findCmd3(i32* unit)
{
    if (!unit || unit[0] != 1 || !unit[5]) return nullptr;
    i32 c = *(i32*)(intptr_t)(unit[5] + 0x28);
    if (!c) return nullptr;
    i32* head = *(i32**)(intptr_t)(c + 0x20);
    if (!head) return nullptr;
    i32* n = (i32*)(intptr_t)*head;
    while (n[2] != 3 && n != head) n = (i32*)(intptr_t)*n;
    return n[2] == 3 ? n : nullptr;
}

// 1.13c D2Game+0x1BDF0
i32 D2GAME_1BDF0(i32 game, i32* unit)
{
    if (!findCmd3(unit)) {
        i32 cmd[7] = { 0, 0, 3, 0, 0, 0, 0 };
        r_21290((i32)(intptr_t)unit, cmd);
    }
    i32* n = findCmd3(unit);
    if (!n || n[3] || n[4]) return 0;
    i32 u = (i32)(intptr_t)unit;
    n[3] = r_21100(u) + 3;
    n[4] = r_21130(u) + 3;
    bool q = unit[1] == 0x20f;
    if ((q ? r_56A00(u) : r_60400(u)) == 0) {
        if (q) r_5CC30(u); else r_663A0(u);
        o10887();
        r_F21E0(game, r_21100(u), r_21130(u));
    }
    n[5] = 1;
    n[6] = 0;
    r_F22C0(1);
    return 1;
}
