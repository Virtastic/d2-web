// 1.13c D2Game+0xADFA0, transcribed from the 1.13c disassembly: use the first inventory item that carries a skill.
// Convention: ECX = game, EDX = unit, stack skill, (unused) (ret 8). No inventory ([unit+0x60]) or an empty 0xA3DC list return 0.
// Each entry (0xA46C, next 0xA4CC) is a match when 0xA430 (it, 0x12) holds, its 0xA9D6 (low word of 0xA90A (it, 0)) row has
// [+0xc] == skill and 0xA42A (it, 0x46, 0) > 0; or, failing 0x12, when 0xA430 (it, 0x16) holds and the row has [+8] == skill.
// On a match 0xA736 (entry) picks the action: 2 = 0xD8BA0 (EAX = the skill stack word's address, ECX = it ? [it+0xc] : -1,
// stack game, unit, 0x1100 (ECX = it), 0x1130 (ECX = it), 0; ret 0x14), 1 = 0xD9BF0 (EAX = that id, ECX = unit, EDX = game, stack
// x, y, the skill word's address; ret 0xc); both return 1, any other kind returns 0. 0x1130 is called before 0x1100.
#pragma once
#include <cstdint>

namespace radfa0 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t skill)
{
    const uint32_t inv = e.rd32(unit + 0x60);
    if (!inv) return 0;
    for (uint32_t n = e.h(0xA3DC, 0, 0, 0, 0, 0, inv); n; n = e.h(0xA4CC, 0, 0, 0, 0, 0, n)) {
        const uint32_t it = e.h(0xA46C, 0, 0, 0, 0, 0, n);
        if (e.h(0xA430, 0, 0, 0, 0, 0, it, 0x12)) {
            const uint32_t row = e.h(0xA9D6, 0, 0, 0, 0, 0, e.h(0xA90A, 0, 0, 0, 0, 0, it, 0) & 0xFFFF);
            if (!row || e.rd32(row + 0xc) != skill) continue;
            if ((int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, it, 0x46, 0) <= 0) continue;
        } else {
            if (!e.h(0xA430, 0, 0, 0, 0, 0, it, 0x16)) continue;
            const uint32_t row = e.h(0xA9D6, 0, 0, 0, 0, 0, e.h(0xA90A, 0, 0, 0, 0, 0, it, 0) & 0xFFFF);
            if (!row || e.rd32(row + 8) != skill) continue;
        }
        const uint32_t kind = e.h(0xA736, 0, 0, 0, 0, 0, n);
        if (kind != 1 && kind != 2) return 0;
        const uint32_t id = it ? e.rd32(it + 0xc) : 0xFFFFFFFFu;
        uint32_t slot = skill;
        const uint32_t y = e.h(0x1130, 0, it);
        const uint32_t x = e.h(0x1100, 0, it);
        if (kind == 2) {
            const uint32_t w[1] = { 0 };
            e.h(0xD8BA0, (uint32_t)(uintptr_t)&slot, id, 0, 0, 0, game, unit, x, y);
            e.x(0xD8BA0, w, 1);
        } else {
            e.h(0xD9BF0, id, unit, game, 0, 0, x, y, (uint32_t)(uintptr_t)&slot);
        }
        return 1;
    }
    return 0;
}
}
