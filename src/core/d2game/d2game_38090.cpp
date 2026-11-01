// D2Game: an item is spawned for a source unit and given its owner's class stat, in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static int16_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }

struct D2Game38090Env {
    void (*prepare)(uint32_t src);                                                     // 0xE5D0: EAX = src; stack 0 (ret 4)
    uint32_t (*spawn)(uint32_t src, uint32_t a3, uint32_t a1);                         // 0x11980: ECX = EDX = 1; stack src, a3, a1, 4, 2, 0, 0, 0 (ret 0x20)
    uint32_t (*getList)(uint32_t item, uint32_t zero, uint32_t flag);                  // 0xAFBE thunk (stdcall 3)
    void (*detachList)(uint32_t item, uint32_t list);                                  // 0xABB6 thunk (stdcall 2)
    void (*freeList)(uint32_t list);                                                   // 0xABAA thunk (stdcall 1)
    uint32_t (*newList)(uint32_t pool, uint32_t flag, uint32_t zero, uint32_t kind, uint32_t id);   // 0xA6A0 thunk (stdcall 5)
    void (*attachList)(uint32_t item, uint32_t list, uint32_t one);                    // 0xA60A thunk (stdcall 3)
    void (*setListStat)(uint32_t list, uint32_t stat, uint32_t one, uint32_t value);   // 0xA6E2 thunk (stdcall 4)
    uint32_t (*hasFlag)(uint32_t item);                                                // 0xA940 thunk (stdcall 1)
    uint32_t (*flagValue)(uint32_t item);                                              // 0xAA06 thunk (stdcall 1)
    void (*setStat)(uint32_t item, uint32_t stat, uint32_t value, uint32_t layer);     // 0xA472 thunk (stdcall 4)
};

// 1.13c D2Game+0x38090
// EAX = src, EBX = owner; stack a1, a2, a3; ret 0xC; returns the new item. A null src returns 0 untouched. The item's
// 0x40 stat list is dropped; when a2 is 0 and the start skill word (+0xAC) is in [0, skillCount) a fresh 0x40 list
// (pool +8, id +0xC of the item, or 0 / -1 for no item) carries stat 0x6B = that skill (1.10f PLAYER_CreateStartItem); a stackable item gets stat 0x46.
uint32_t ITEMS_SpawnForOwner_38090(uint32_t src, uint32_t owner, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t skillCount, const D2Game38090Env& env)
{
    if (src == 0) return 0;
    env.prepare(src);
    const uint32_t item = env.spawn(src, a3, a1);
    uint32_t list = env.getList(item, 0, 0x40);
    if (list != 0) { env.detachList(item, list); env.freeList(list); }
    const int16_t cls = rd16s(owner + 0xAC);
    if (a2 == 0 && cls >= 0 && (int32_t)cls < (int32_t)skillCount) {
        list = env.getList(item, 0, 0x40);
        if (list == 0) {
            const uint32_t id = item ? rd32(item + 0xC) : 0xFFFFFFFFu, pool = item ? rd32(item + 8) : 0;
            list = env.newList(pool, 0x40, 0, 4, id);
            env.attachList(item, list, 1);
        }
        env.setListStat(list, 0x6B, 1, (uint16_t)cls);
    }
    if (env.hasFlag(item) != 0) env.setStat(item, 0x46, env.flagValue(item), 0);
    return item;
}
