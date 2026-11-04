// 1.13c D2Common+0x46E50: item-unit stat application by item-type class. The unit comes in ESI, the stat id (the switch selector, 0x10..0x1f) is the one stack word (ret 4, void); the items row key only feeds the link lookup.
// Every callee and module global goes through D2Common46E50Env, so the FN probe (src/core/fn/d2common_46e50.cpp) can mirror each call.
#include <cstdint>

extern "C" {
struct D2Common46E50Env {
    const uint8_t* (*itemRow)(uint32_t txtId);                                      // 0x719A0: stack txt id, ret 4, the items.txt row
    int32_t (*linkIndex)(const uint8_t* linker, uint32_t key, uint32_t zero);      // Fog ordinal 10213 (IAT 0x890BC): stdcall, ret 12
    uint32_t (*itemTypeIs)(void* unit, uint32_t typeId);                           // 0x24430 ITEMS_CheckItemTypeId: ret 8
    uint32_t (*throwable)(void* unit);                                              // 0x24C30 ITEMS_CheckIfThrowable: ret 4, byte result
    int32_t (*getStat)(void* unit, int32_t stat, int32_t layer);                   // 0x38C20 STATLIST get: ret 0xC
    void (*setStat)(void* unit, int32_t stat, int32_t value, int32_t layer);       // 0x3A740 STATLIST set: ret 0x10
    void (*fatal)(uint32_t msg);                                                    // Ordinal 10265 report, 10024, _exit(-1): does not return
    const uint8_t* linker;                                                          // DAT_6FDEFF6C (RVA 0x9FF6C), the fog link table
    const uint8_t* itemsTxt;                                                        // DAT_6FDEFB98 (RVA 0x9FB98), items.txt rows of 0x1A8 bytes
};
void D2Common_46E50_ApplyItemClassStats(const D2Common46E50Env* e, void* unit, int32_t stat);
}

extern "C" void D2Common_46E50_ApplyItemClassStats(const D2Common46E50Env* e, void* unit, int32_t stat) {
    const int32_t* u = static_cast<const int32_t*>(unit);
    if (!u) { e->fatal(0x61b); return; }
    if (u[0] != 4) { e->fatal(0x61c); return; }
    const uint8_t* it = e->itemRow(static_cast<uint32_t>(u[1]));
    if (!it) { e->fatal(0x61f); return; }
    const uint32_t key = *reinterpret_cast<const uint32_t*>(it + 0x80);
    const int32_t idx = e->linkIndex(e->linker, key, 0);
    if (idx < 0) return;
    const uint8_t* r = e->itemsTxt + static_cast<int64_t>(idx) * 0x1a8;
    if (!r) return;
    switch (stat) {
    case 0x10: case 0x1f:
        if (e->itemTypeIs(unit, 0x32)) {
            const int32_t lim = *reinterpret_cast<const int32_t*>(r + 0xd0);
            if (lim) {
                int32_t v = e->getStat(unit, 0x1f, 0) + 1;
                if (v <= lim) v = lim + 1;
                e->setStat(unit, 0x1f, v, 0);
            }
        }
        return;
    case 0x11: case 0x16:
        if (e->itemTypeIs(unit, 0x2d)) {
            if (r[0xff]) e->setStat(unit, 0x16, r[0xff], 0);
            if (r[0x103]) e->setStat(unit, 0x18, r[0x103], 0);
            if ((e->throwable(unit) & 0xff) && r[0x101]) e->setStat(unit, 0xa0, r[0x101], 0);
        }
        return;
    case 0x12: case 0x15:
        if (e->itemTypeIs(unit, 0x2d)) {
            if (r[0xfe]) e->setStat(unit, 0x15, r[0xfe], 0);
            if (r[0x102]) e->setStat(unit, 0x17, r[0x102], 0);
            if ((e->throwable(unit) & 0xff) && r[0x100]) e->setStat(unit, 0x9f, r[0x100], 0);
        }
        return;
    default:
        return;
    }
}
