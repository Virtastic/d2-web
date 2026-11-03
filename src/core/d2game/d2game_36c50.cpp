// D2Game+0x36C50 -- quest dispatcher keyed on a unit's class id (the unit is EAX, the game is ECX).
// Walks the game's quest list (game+0x10f4 -> list head at +0, next at node+0xF4, type id at node+0) and
// hands the unit to the handler of the matching quest node. Written from the 1.13c decompile and its
// disassembly (register assignments below come from the disassembly). Callees are taken by retail RVA
// through an env, as in d2game_36890.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game36C50Env {
    uint32_t (*unitRoom)(uint32_t unit);                  // #10331
    uint32_t (*roomLevel)(uint32_t room);                 // #10826
    char     (*levelAct)(uint32_t level);                 // #10459
    void     (*setMode)(uint32_t unit, uint32_t mode);    // #11090 (UNITS_ChangeAnimMode)
    void     (*sub_E9EA0)(uint32_t node);                 // eax = node
    void     (*sub_68F00)(uint32_t node);                 // eax = ebx = node
    void     (*sub_5EE30)(uint32_t node);                 // eax = ebx = node
    void     (*sub_5E840)(uint32_t node);                 // eax = ebx = node
    void     (*sub_C8CD0)(uint32_t unit, uint32_t node);  // eax = unit, stack = node
    void     (*sub_67730)(uint32_t game, uint32_t unit);  // stack
    void     (*sub_EDDB0)(uint32_t unit, uint32_t game);  // ecx = 1, eax = 0x12d, edx = unit, stack = game, -1, 0
    void     (*sub_BBC00)(uint32_t unit, uint32_t game);  // ebx = unit, edi = game
    void     (*sub_68E20)(uint32_t node, uint32_t unit);  // stack
    void     (*sub_E9960)(uint32_t node);                 // stack
    void     (*sub_BB6A0)(uint32_t node, uint32_t unit);  // stack = node, esi = unit
    void     (*sub_5E420)(uint32_t node, uint32_t unit);  // stack
    void     (*sub_5E790)();
    void     (*sub_5E940)(uint32_t unit, uint32_t game);  // eax = unit, ecx = game
    void     (*sub_5DEF0)(uint32_t game);                 // eax = game
    void     (*sub_68D20)(uint32_t unit);                 // stack
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// First quest node of the given type in the game's quest list, or 0.
static uint32_t findQuest(uint32_t game, uint32_t type)
{
    const uint32_t list = rd32(game + 0x10f4);
    if (!list) return 0;
    for (uint32_t n = rd32(list); n; n = rd32(n + 0xf4))
        if (rd32(n) == type) return n;
    return 0;
}

// 1.13c D2Game+0x36C50
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_36C50(uint32_t unit, uint32_t game, const D2Game36C50Env* e)
{
    const int32_t id = unit ? (int32_t)rd32(unit + 4) : -1;
    const uint32_t room = e->unitRoom(unit);
    uint32_t n;
    switch (id) {
    case 0x16f:
        if ((n = findQuest(game, 0x10))) e->sub_E9EA0(n);
        return;
    case 0xbd: {
        const uint32_t level = e->roomLevel(room);
        if (e->levelAct(level) == 0) {
            if ((n = findQuest(game, 4))) e->sub_68F00(n);
        } else if (level == 0x6d || (int32_t)level > 0x70) {
            if ((n = findQuest(game, 0x21))) e->sub_5E840(n);
        } else {
            if ((n = findQuest(game, 0x20))) e->sub_5EE30(n);
        }
        return;
    }
    case 0x1a:
        e->sub_67730(game, unit);
        return;
    case 0x7a:
        if ((n = findQuest(game, 0xb))) e->sub_C8CD0(unit, n);
        return;
    case 0x83:
        if (!room) return;
        if (unit && rd32(unit + 0x10) == 1) e->setMode(unit, 2);
        {
            const uint32_t level = e->roomLevel(room);
            if (level == 0x4c) e->sub_EDDB0(unit, game);
            else if (level == 0x6c) e->sub_BBC00(unit, game);
        }
        return;
    case 0x10c:
        if ((n = findQuest(game, 4))) e->sub_68E20(n, unit);
        return;
    case 0x155:
        if ((n = findQuest(game, 0x14))) e->sub_E9960(n);
        return;
    case 0x173:
        if ((n = findQuest(game, 5))) e->sub_68D20(unit);
        return;
    case 0x178:
        if ((n = findQuest(game, 0x18))) e->sub_BB6A0(n, unit);
        return;
    case 0x1cb: e->sub_5E790(); return;
    case 0x1cc: e->sub_5E940(unit, game); return;
    case 0x1cd: e->sub_5DEF0(game); return;
    case 0x1da: case 0x1db: case 0x1dc:
        if ((n = findQuest(game, 0x23))) e->sub_5E420(n, unit);
        return;
    default: return;
    }
}
