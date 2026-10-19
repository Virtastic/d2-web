// 1.13c D2Common implementation batch. Retail image references are 32-bit addresses;
// unresolved calls and process globals enter through the context supplied by the caller.
#include <cstddef>
#include <cstdint>

namespace {
using U8 = uint8_t;
inline U8* ptr(uint32_t a) { return reinterpret_cast<U8*>(static_cast<uintptr_t>(a)); }
inline uint32_t u32(const U8* p, size_t o) { return *reinterpret_cast<const uint32_t*>(p + o); }
inline int32_t i32(const U8* p, size_t o) { return *reinterpret_cast<const int32_t*>(p + o); }
inline uint16_t u16(const U8* p, size_t o) { return *reinterpret_cast<const uint16_t*>(p + o); }
inline int16_t i16(const U8* p, size_t o) { return *reinterpret_cast<const int16_t*>(p + o); }
struct ItemHead { uint32_t unitType; U8 pad[0x30]; int32_t ownerId; };
static_assert(offsetof(ItemHead, ownerId) == 0x34);
struct StatNode { uint32_t kind; U8 pad[8]; int32_t statId; U8 pad2[4]; uint32_t next; };
static_assert(offsetof(StatNode, statId) == 0x0c && offsetof(StatNode, next) == 0x14);
}

// 1.13c D2Common+0x50870: an inventory owner passes if its first stat-list node
// has no matching owner ID or has stat 0xCC. An absent list also passes.
extern "C" bool D2Common_50870_OwnerStatGate(uint32_t pOwner, uint32_t pItem,
    int32_t (*getStat)(uint32_t, uint32_t))
{
    if (!pOwner || !pItem) return false;
    U8* owner = ptr(pOwner);
    U8* item = ptr(pItem);
    U8* list = ptr(u32(owner, 0x60));
    if (!list || i32(item, 0x34) == -1) return true;
    if (u32(list, 0) != 0x01020304) return true;
    U8* node = ptr(u32(list, 0x0c));
    while (node && u32(node, 0) == 4) {
        if (i32(node, 0x0c) == i32(item, 0x34))
            return getStat && getStat(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(node)), 0xcc) != 0;
        uint32_t next = u32(node, 0x14);
        if (!next || next == 0xffffffa4u) return true;
        node = ptr(u32(ptr(next), 0x64));
    }
    return true;
}

// 1.13c D2Common+0x50930: equipment gate selected by item type. The six
// unresolved retail callees are explicit inputs, preserving their call order.
extern "C" uint32_t D2Common_50930_EquipmentGate(uint32_t pContext, uint32_t pUnit,
    uint32_t bitMask16, int32_t (*testUnit)(uint32_t),
    int32_t (*select)(uint32_t, uint32_t*, uint32_t*, uint32_t*),
    uint32_t (*getAlternate)(uint32_t), uint32_t (*canEquip)(uint32_t),
    uint32_t (*hasType)(uint32_t, uint32_t), uint32_t (*getStat)(uint32_t, uint32_t))
{
    if (!pContext || !pUnit) return 0;
    U8* context = ptr(pContext);
    U8* unit = ptr(pUnit);
    U8* rec = ptr(u32(context, 0));
    uint32_t inv = u32(unit, 0x60);
    if (rec && (rec[8] & bitMask16)) return inv && i32(context, 0x30) > 0;
    if (!rec) return 0; // Retail requires the record outside the bit-mask branch.
    int16_t kind = i16(rec, 0);
    if (kind != 2 && kind != 4 && kind != 5) return 1;
    if (!inv || !select) return 0;
    bool tested = (kind == 2 || kind == 5) && testUnit && testUnit(pUnit);
    if (kind == 5 && !tested) return 0;
    uint32_t first = 0, second = 0, selected = pUnit;
    auto finish = [&](uint32_t candidate) -> uint32_t {
        if (!candidate) return 0;
        uint32_t allowed = canEquip ? canEquip(candidate) : 0;
        return allowed ? allowed : (getStat ? getStat(candidate, 0x7d) : 0);
    };
    if (kind == 2 && !tested) {
        if (!select(inv, &first, &second, &selected)) return 0;
        if (!selected) return 0;
        // Retail returns the first output of FUN_6fd70700 here (local_8),
        // not its third output (param_1 / selected).
        return finish(first);
    }
    if (!select(inv, &first, &second, &selected)) return 0;
    if (kind == 2) {
        first = selected ? selected : (getAlternate ? getAlternate(inv) : 0);
        if (!first) return 0;
        return finish(first);
    }
    if (selected) first = getAlternate ? getAlternate(inv) : 0;
    if (!first) return 0;
    if (kind == 5) return hasType ? hasType(first, 0x2d) : 0;
    return finish(first);
}

// 1.13c D2Common+0x50AF0: item level gate. The table-driven stat-6 branch
// applies only to unit kinds 0, 1 and 3; an item flag may override it.
extern "C" bool D2Common_50AF0_ItemLevelGate(uint32_t pUnit, uint32_t pItem,
    int32_t qualityBonus, int32_t levelCap, bool stat72Available,
    int32_t (*getStat)(uint32_t, uint32_t))
{
    if (!pUnit) return false; // Null is a fatal assertion in retail.
    U8* unit = ptr(pUnit);
    if (i32(unit, 0x34) != -1) return i32(unit, 0x38) > 0;
    U8* rec = ptr(u32(unit, 0));
    if (!rec) return false; // Also a retail fatal assertion.
    int32_t level = 0;
    if (pItem) {
        int32_t n = i32(unit, 0x28) + qualityBonus;
        if (n > 0) level = n < levelCap ? n : levelCap;
    }
    int32_t scale = level > 0 ? level - 1 : 0;
    int32_t required = (i16(rec, 0x18c) * scale + i16(rec, 0x18a)) << (rec[0x188] & 31);
    U8* item = ptr(pItem);
    if (item) {
        int32_t kind = i32(item, 0);
        // Retail checks ItemStatCost availability for stat 0x72 before it
        // asks for the unit's stat 6 value.
        if ((kind == 0 || kind == 1 || kind == 3) && stat72Available)
            return getStat && required <= getStat(pItem, 6);
        if (i16(rec, 0x2e) == 0x74 && (u32(item, 0xc8) & 8)) return true;
    }
    return getStat && required <= getStat(pItem, 8);
}

// 1.13c D2Common+0x50F00: check two equipped weapons against the item's
// requirements, excluding an existing hand item for kinds 4 and 5.
extern "C" bool D2Common_50F00_EquipmentRequirements(uint32_t pOwner, uint32_t pItem,
    uint32_t right, uint32_t left, uint32_t (*slotItem)(uint32_t, uint32_t),
    uint32_t (*equipped)(uint32_t),
    bool (*requirements)(uint32_t, uint32_t, uint32_t))
{
    U8* item = ptr(pItem);
    if (!item) return false;
    U8* rec = ptr(u32(item, 0));
    if (!rec) return false;
    if (i16(rec, 0x24) < 1 && i16(rec, 0x18) < 1) return true;
    U8* owner = ptr(pOwner);
    if (!owner || !u32(owner, 0x60)) return false;
    // Retail resolves each hand slot independently before checking whether
    // the candidate replaces one of the equipped items.
    if (slotItem) {
        right = slotItem(u32(owner, 0x60), 0);
        left = slotItem(u32(owner, 0x60), 1);
    }
    int16_t kind = i16(rec, 0);
    if ((kind == 4 || kind == 5) && equipped) {
        uint32_t current = equipped(u32(owner, 0x60));
        if (current == right) right = 0;
        else if (current == left) left = 0;
    }
    if (!requirements) return false;
    // Retail repeats the first call if it fails, then checks the reverse order.
    if (requirements(right, left, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(rec))) ||
        requirements(right, left, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(rec))))
        return requirements(left, right, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(rec)));
    return false;
}

// 1.13c D2Common+0x52210: mana cost gate for the fixed skill 0x79.
extern "C" bool D2Common_52210_ManaCostGate(uint32_t pSkill, uint32_t pUnit,
    int32_t skillCount, bool stat79Available, uint32_t manaLimit, int32_t qualityBonus,
    int32_t (*cost)(uint32_t, uint32_t, int32_t, uint32_t))
{
    U8* unit = ptr(pUnit);
    if (!unit || i32(unit, 0) != 0 || skillCount <= 0x79 || !stat79Available || !pSkill) return true;
    U8* skill = ptr(pSkill);
    U8* rec = ptr(u32(skill, 0));
    if (!rec) return true;
    int32_t n = i32(skill, 0x28);
    if (i32(skill, 0x34) == -1) n += qualityBonus;
    uint32_t level = n > 0 ? static_cast<uint32_t>(n) : 0;
    if (static_cast<int32_t>(level) >= static_cast<int32_t>(manaLimit)) level = manaLimit;
    return !cost || cost(pUnit, u32(rec, 0x190), i16(rec, 0), level) < 1;
}

// 1.13c D2Common+0x222B0: byte 0x143 of the indexed 0x1A8-byte record.
extern "C" uint8_t D2Common_222B0_RecordFlag(uint32_t pUnit, const U8* table,
    uint32_t count)
{
    uint32_t index = pUnit ? u32(ptr(pUnit), 4) : UINT32_MAX;
    if (index >= count || !table) __builtin_trap(); // Retail reports the table error and terminates.
    return table[static_cast<size_t>(index) * 0x1a8 + 0x143];
}

// 1.13c D2Common+0x39CE0: recursively walk stat dependencies. The caller
// supplies the five local stat-list operations and the ItemStatCost table.
extern "C" int32_t D2Common_39CE0_ResolveStatDependencies(uint32_t pOwner, uint32_t key,
    const U8* rec, uint32_t flags, const U8* statTable, int32_t statCount,
    int32_t (*lookup)(uint32_t, uint32_t), void (*record)(uint32_t, int32_t, const U8*, uint32_t),
    int32_t (*readStat)(uint32_t, uint32_t), int32_t (*selectIndex)())
{
    if (!rec || !lookup || !record) return 0;
    int32_t value = lookup(pOwner, key);
    if (!rec[0x51]) { record(key, value, rec, flags); return value; }
    if (!value) record(key, 0, rec, flags);
    bool keep = true;
    for (int i = 0; i < 3; ++i) {
        uint16_t id = u16(rec, 0x58 + 2 * i);
        if (id == 0xffff) break;
        const U8* child = id < statCount && statTable ? statTable + static_cast<size_t>(id) * 0x144 : nullptr;
        int32_t childValue = D2Common_39CE0_ResolveStatDependencies(pOwner, uint32_t(id) << 16,
            child, flags, statTable, statCount, lookup, record, readStat, selectIndex);
        record(uint32_t(id) << 16, childValue, child, flags);
        if (childValue) {
            uint8_t mode = rec[0x54];
            bool clearKeep = true;
            uint16_t stat = u16(rec, 0xde + i * 6);
            if (mode == 0 || mode == 8 || mode == 9 || mode == 10 || mode == 11) {
                clearKeep = false;
            } else if (mode == 2 || mode == 3) {
                clearKeep = false;
                if ((i32(ptr(pOwner), 8) == 0 || i32(ptr(pOwner), 8) == 1) &&
                    stat != 0xffff && selectIndex) {
                    uint32_t base = i32(ptr(pOwner), 0x10) >= 0 ?
                        u32(ptr(pOwner), 0x24) : u32(ptr(pOwner), 0x48);
                    int32_t n = selectIndex();
                    if (n >= 0 && base && i32(ptr(base + n * 8), 4) > 0) clearKeep = true;
                }
            } else if (mode == 4 || mode == 5) {
                clearKeep = false;
                uint32_t unit = u32(ptr(pOwner), 4);
                if (stat != 0xffff && readStat && unit &&
                    (i32(ptr(unit), 0) == 0 || i32(ptr(unit), 0) == 1) &&
                    readStat(unit, stat) > 0) clearKeep = true;
            } else if (mode == 6 || mode == 7) {
                clearKeep = i32(ptr(pOwner), 8) == 1;
            } else if (mode == 13) {
                clearKeep = i32(ptr(pOwner), 8) == 4;
            }
            if (clearKeep) keep = false;
        }
    }
    if (keep && value) {
        record(key, value, rec, flags);
        for (int i = 0; i < 64; ++i) {
            uint16_t id = u16(rec, 0x5e + 2 * i);
            if (id == 0xffff) break;
            const U8* child = id < statCount && statTable ? statTable + static_cast<size_t>(id) * 0x144 : nullptr;
            int32_t childValue = D2Common_39CE0_ResolveStatDependencies(pOwner, uint32_t(id) << 16,
                child, flags, statTable, statCount, lookup, record, readStat, selectIndex);
            record(uint32_t(id) << 16, childValue, child, flags);
        }
    }
    return value;
}
