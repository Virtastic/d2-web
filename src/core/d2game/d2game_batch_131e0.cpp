// src/core/d2game/d2game_batch_131e0.cpp -- ten 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// arguments (unaff_EBX / in_EAX / in_ECX / in_EDX / ESI / EDI) are explicit parameters named at each
// function; the stdcall stack arguments follow the disassembly. Unmapped callees are seams on
// D2GameBatch131E0Env in retail argument order. The D2Common ordinal seams are named by number, as in
// the sibling batches; their 1.13c identities are noted where they shape the rule.
#include "d2game_131e0_env.h"

// The affix-group probe (0x131E0) and the item-code lookup (0x132A0) live in d2game_131e0.cpp and d2game_132a0.cpp.

// Random skill-layer writer for one item (EBX). Three draws off the item's seed LCG select how many
// layers and their stat value; each layer draws a candidate skill index biased to a base computed from
// arg2, rejects candidates whose skills.txt entry (+0x18 item type, stride 0x23C) names a type the item
// is not, rejects repeats of the three earlier picks, and gives up after six draws. The stat value is
// either a fixed tier from arg1/format or a second roll tempered by arg3. The stat list comes from the
// unit's own state list (flag 0x40) or is allocated on the unit's pool and posted to it; stat 0x6B is
// then written with the candidate as layer. Empty result answers the layer count.
// 1.13c D2Game+0x13320
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_rule_13320(void* item, int32_t arg1, int32_t arg2,
                                                           int32_t arg3, const D2GameBatch131E0Env* env)
{
    uint32_t roll = d2game_lcg_step(env->ordinal10411(item));

    int32_t count;
    const int32_t tier = (int32_t)(roll % 100u) + arg3;
    if (tier <= 0x5A) {
        if (tier <= 0x46) {
            if (tier <= 0x1E && arg3 == 0) return (int32_t)(roll / 100u);
            count = 1;
        } else {
            count = 2;
        }
    } else {
        count = 3;
    }

    int32_t delta = 1;
    if (arg1 <= 0x24 || env->ordinal10718(item) < 100) {
        if (arg1 > 0x18) delta = 4;
        else if (arg1 > 0x12) delta = 3;
        else if (arg1 > 0x0B) delta = 2;
    } else {
        delta = 5;
    }

    const int32_t skillsCount = (int32_t)rd32(env->dataTables, 0xBA0);
    const uint32_t skillsBase = rd32(env->dataTables, 0xB98);
    int32_t picks[3] = { -1, -1, -1 };
    int32_t candidate = 0;   // survives the pick loop: the stat write uses the last tried index
    int32_t i = 0;
    while (i < count) {
        roll = d2game_lcg_step(env->ordinal10411(item));
        const uint32_t r = roll % 100u;
        int32_t value = delta;
        if (r <= 0x50) {
            if (r <= 0x1E) value = (r <= 0x0A) ? delta - 2 : delta - 1;
        } else {
            ++value;
        }
        if (value < 1) value = 1;
        if (env->ordinal10106(item) == 1 && value >= 4) value = 4;

        int32_t tries = 6;
        const int32_t base = arg2 + value * 5 - 5;
        do {
            roll = d2game_lcg_step(env->ordinal10411(item));
            candidate = base + (int32_t)(roll % 5u);
            bool accept = true;
            if (candidate >= 0 && candidate < skillsCount) {
                const void* rec = (const void*)(uintptr_t)(skillsBase + (uint32_t)candidate * 0x23C);
                if (rec != nullptr) {
                    const int16_t itemType = (int16_t)rd16(rec, 0x18);
                    if (itemType > 0 && env->ordinal10744(item, itemType) == 0) accept = false;
                }
            }
            if (accept && candidate != picks[0] && candidate != picks[1] && candidate != picks[2]) {
                picks[i] = candidate;
                break;
            }
        } while (--tries > 0);

        int32_t level;
        if (env->ordinal10718(item) >= 100 && env->ordinal10106(item) == 1) {
            level = 1;
        } else {
            roll = d2game_lcg_step(env->ordinal10411(item));
            const int32_t x = (int32_t)(roll % 100u) + arg3 / 2;
            if (x < 0x5A) level = (x < 0x3C) ? 1 : 2;
            else level = 3;
        }

        void* list = env->ordinal10930(item, 0, 0x40);
        if (list == nullptr) {
            void* pool;
            int32_t ownerId;
            if (item == nullptr) {
                pool = nullptr;
                ownerId = -1;
            } else {
                pool = (void*)(uintptr_t)rd32(item, 8);
                ownerId = (int32_t)rd32(item, 0xC);
            }
            list = env->ordinal11013(pool, 0x40, 0, 4, ownerId);
            env->ordinal10807(item, list, 1);
        }
        if (list != nullptr) env->ordinal10188(list, 0x6B, level, candidate);
        ++i;
    }
    return i;
}

// Single-item variant of the skill-layer writer (EBX item, EAX level, one stack argument that is the candidate base). The
// first draw picks the layer count exactly as 0x13320 does with a zero tier; a low draw answers the
// quotient instead. Each layer draws a candidate skill index around arg1 + 5*value - 5, rerolls while it
// lands on 0x49 or repeats any of the three picks it has remembered, then rolls its stat value 1..3 by
// the same 0x5A/0x3C thresholds. No item-type, format or quality filtering happens here. The stat list
// is fetched (flag 0x40) or allocated on the item's pool and posted, then stat 0x6B is set with the
// candidate as layer. Answers the layer count.
// 1.13c D2Game+0x135E0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_135E0(void* item, int32_t arg1, int32_t arg2,
                                                           const D2GameBatch131E0Env* env)
{
    uint32_t roll = d2game_lcg_step(env->ordinal10411(item));

    int32_t count;
    const uint32_t r0 = roll % 100u;
    if (r0 > 0x5A) {
        count = 3;
    } else if (r0 > 0x46) {
        count = 2;
    } else if (r0 > 0x1E) {
        count = 1;
    } else {
        return (int32_t)(roll / 100u);
    }

    int32_t delta = 1;
    if (arg1 > 0x18) delta = 4;
    else if (arg1 > 0x12) delta = 3;
    else if (arg1 > 0x0B) delta = 2;

    int32_t picks[3] = { -1, -1, -1 };
    int32_t candidate = 0;
    int32_t i = 0;
    while (i < count) {
        roll = d2game_lcg_step(env->ordinal10411(item));
        const uint32_t r = roll % 100u;
        int32_t value = delta;
        if (r <= 0x50) {
            if (r <= 0x1E) value = (r <= 0x0A) ? delta - 2 : delta - 1;
        } else {
            ++value;
        }
        if (value < 1) value = 1;

        const int32_t base = arg2 + value * 5 - 5;   // arg2 = the stack word; arg1 (EAX) only picks the tier ladder
        for (;;) {
            roll = d2game_lcg_step(env->ordinal10411(item));
            candidate = base + (int32_t)(roll % 5u);
            if (candidate != 0x49 && candidate != picks[0] && candidate != picks[1] &&
                candidate != picks[2]) break;
        }
        picks[i] = candidate;

        int32_t level = 1;
        roll = d2game_lcg_step(env->ordinal10411(item));
        const uint32_t r2 = roll % 100u;
        if (r2 >= 0x5A) level = 3;
        else if (r2 >= 0x3C) level = 2;

        void* list = env->ordinal10930(item, 0, 0x40);
        if (list == nullptr) {
            void* pool;
            int32_t ownerId;
            if (item == nullptr) {
                pool = nullptr;
                ownerId = -1;
            } else {
                pool = (void*)(uintptr_t)rd32(item, 8);
                ownerId = (int32_t)rd32(item, 0xC);
            }
            list = env->ordinal11013(pool, 0x40, 0, 4, ownerId);
            env->ordinal10807(item, list, 1);
        }
        if (list != nullptr) env->ordinal10188(list, 0x6B, level, candidate);
        ++i;
    }
    return i;
}
