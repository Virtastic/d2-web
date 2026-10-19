// Prior art used: D2MOO MonsterUnique.cpp (modifier stats, minion cleanup, mode
// changes, amplify-damage curse). Rules below follow the cached 1.13c bodies.
// Retail address fields remain uint32_t. resolve maps those addresses to native
// memory; no retail field ever stores a native pointer. Calls erased by Ghidra
// retain explicit context at the seam rather than guessing a native calling ABI.
#include <cstdint>
#include <cstring>

namespace {
template<class T> T read(const void* p, uint32_t offset) {
    T value; std::memcpy(&value, static_cast<const uint8_t*>(p) + offset, sizeof value); return value;
}
template<class T> void write(void* p, uint32_t offset, T value) {
    std::memcpy(static_cast<uint8_t*>(p) + offset, &value, sizeof value);
}
}

struct D2GameBatch22D00Env {
    void* context;
    void* (*resolve)(void*, uint32_t);
    // Game/unit and preserved register context accompany calls whose arguments
    // Ghidra did not recover. The adapter owns construction of the mode-change
    // and curse records, which cannot use retail stack offsets on wasm64.
    void (*scaleLife21EC0)(void*, void* unit, int32_t adjustment);
    const void* (*difficulty10237)(void*, uint8_t);
    void (*prepare18A0)(void*, void* unit);
    int32_t (*monsterId1850)(void*, void* unit);
    void (*addStat10551)(void*, void* unit, int32_t stat, int32_t value, int32_t layer);
    void (*modeInfo11103)(void*, void* unit, int32_t mode, void* modeChange);
    void (*changeMode75E10)(void*, void* game, void* modeChange);
    void (*event8EA80)(void*, void* game, void* unit, int32_t type, uint32_t tick,
                       int32_t, int32_t, int32_t);
    void (*freeMinions216A0)(void*, void* unit);
    void (*champion22DA0)(void*, void* unit, int32_t unique);
    uint32_t (*targetAllowedE0A60)(void*, void* source, void* target, void* game);
    const void* dataTables; // retail byte layout; pointer fields resolved above
    int32_t (*evaluate10786)(void*, void* target, uint32_t formula, int32_t skill, int32_t level);
    void* (*applyCurseA00B0)(void*, void* game, void* target, void* source,
                            int32_t level, int32_t duration, int32_t value,
                            int32_t stat, int32_t state);
    void (*setStat10188)(void*, void* statList, int32_t stat, int32_t value, int32_t layer);
};

// 1.13c D2Game+0x22D00
extern "C" void d2_fn_d2game_22D00(void* unit, int32_t unique, const D2GameBatch22D00Env* e) {
    if (!unit || read<int32_t>(unit, 0) != 1) return;
    const uint32_t gameAddress = read<uint32_t>(unit, 0x80);
    if (!gameAddress || !unique) return;
    e->scaleLife21EC0(e->context, unit, -75);
    int32_t toHit = 300, damage = 300;
    const void* difficulty = e->difficulty10237(e->context,
        read<uint8_t>(e->resolve(e->context, gameAddress), 0x6d));
    if (difficulty) {
        // x86 multiplication wraps before signed division.
        toHit = static_cast<int32_t>(read<uint32_t>(difficulty, 0x34) * 300u) / 100;
        e->prepare18A0(e->context, unit);
        damage = toHit;
        if (e->monsterId1850(e->context, unit) == 0x76) damage = toHit / 2;
    }
    e->addStat10551(e->context, unit, 0x19, damage, 0);
    e->addStat10551(e->context, unit, 0x77, toHit, 0);
}

// modeChange is caller-owned native scratch passed through both retail calls;
// its layout belongs to the adapter, never a fabricated x86 stack address.
// 1.13c D2Game+0x23390
extern "C" void d2_fn_d2game_23390(void* game, void* unit, void* modeChange,
                                   const D2GameBatch22D00Env* e) {
    if (unit) {
        const int32_t mode = read<int32_t>(unit, 0x10);
        if (mode == 3) {
            e->modeInfo11103(e->context, unit, 0, modeChange);
            e->changeMode75E10(e->context, game, modeChange);
        } else if (mode != 0 && mode != 12) return;
    }
    e->event8EA80(e->context, game, unit, 7, read<uint32_t>(game, 0xa8) + 4u, 0, 0, 0);
}

// 1.13c D2Game+0x23730
extern "C" void d2_fn_d2game_23730(void* unit, uint32_t gameAddress, void* modeChange,
                                   const D2GameBatch22D00Env* e) {
    e->freeMinions216A0(e->context, unit);
    if (!unit) return;
    if (read<int32_t>(unit, 0) == 1) {
        const uint32_t monsterAddress = read<uint32_t>(unit, 0x14);
        const uint32_t aiAddress = monsterAddress
            ? read<uint32_t>(e->resolve(e->context, monsterAddress), 0x28) : 0;
        void* ai = e->resolve(e->context, aiAddress);
        write<uint32_t>(ai, 0x2c, 0xffffffffu);
        write<uint32_t>(ai, 0x30, 1);
        write<uint32_t>(ai, 0x28, gameAddress);
    }
    if (read<int32_t>(unit, 0x10) != 0) {
        e->modeInfo11103(e->context, unit, 0, modeChange);
        e->changeMode75E10(e->context, e->resolve(e->context, gameAddress), modeChange);
    }
}

// 1.13c D2Game+0x237C0
extern "C" void d2_fn_d2game_237C0(void* unit, int32_t unique, const D2GameBatch22D00Env* e) {
    if (!unit || read<int32_t>(unit, 0) != 1 || !unique) return;
    const uint32_t monsterAddress = read<uint32_t>(unit, 0x14);
    if (monsterAddress) {
        void* monster = e->resolve(e->context, monsterAddress);
        write<uint8_t>(monster, 0x16, read<uint8_t>(monster, 0x16) | 0x20u);
    }
    e->scaleLife21EC0(e->context, unit, 100);
    e->champion22DA0(e->context, unit, unique);
}

// Source is an explicit preserved-register input for curse construction, absent
// from the decompiler's signature. Skill 66 occupies 0x9378 in the retail table.
// 1.13c D2Game+0x24030
extern "C" void d2_fn_d2game_24030(void* game, void* target, void* source, int32_t level,
                                   const D2GameBatch22D00Env* e) {
    if (!e->targetAllowedE0A60(e->context, source, target, game)) return;
    if (read<int32_t>(e->dataTables, 0xba0) <= 0x42) return;
    const uint32_t skillsAddress = read<uint32_t>(e->dataTables, 0xb98);
    const uint32_t recordAddress = skillsAddress + 0x9378u;
    if (!recordAddress) return;
    const void* record = e->resolve(e->context, recordAddress);
    const int32_t statCount = read<int32_t>(e->dataTables, 0xbd4);
    const int32_t stat = read<int16_t>(record, 0x54);
    if (stat < -1 || stat >= statCount) return;
    const int32_t state = read<int16_t>(record, 0x82);
    if (state < 0 || state >= read<int32_t>(e->dataTables, 0xc4)) return;
    const int32_t duration = e->evaluate10786(e->context, target, read<uint32_t>(record, 0x60), 0x42, level);
    const int32_t value = e->evaluate10786(e->context, target, read<uint32_t>(record, 0x68), 0x42, level);
    void* list = e->applyCurseA00B0(e->context, game, target, source, level, duration, value, stat, state);
    if (!list) return;
    for (uint32_t i = 0; i < 5; ++i) {
        const int32_t extraStat = read<int16_t>(record, 0x56 + i * 2);
        if (extraStat >= 0 && extraStat < read<int32_t>(e->dataTables, 0xbd4)) {
            const int32_t extra = e->evaluate10786(e->context, target, read<uint32_t>(record, 0x6c + i * 4), 0x42, level);
            if (extra) e->setStat10188(e->context, list, extraStat, extra, 0);
        }
    }
}
