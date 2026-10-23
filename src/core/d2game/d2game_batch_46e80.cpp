// src/core/d2game/d2game_batch_46e80.cpp -- seven 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): MonStats id resolution with
// the 0x13/0x3A special-case caps (0x46E80), the count bump at +0x90 (0xDF9C0), the per-node quest
// stamp loop and its flag-setting sibling (0xEF7D0, 0xEF8B0), the 0x7F-bucket callback dispatch
// (0x21490), the quest re-init entry (0x22FA0) and the weighted table roll (0x23980). Stage-A batch
// lane: implementation + marker only, no spec, no vector.
//
// Host model: units, records, list nodes and the resolved sgptDataTables block are opaque and are
// addressed by retail 32-bit byte offsets (rd8/rd16/rd32/rdptr/wr32). Register-only arguments
// (Ghidra's in_EAX / in_ECX / in_EDX / unaff_EDI / unaff_ESI / unaff_EBX) and values the decompile
// reads out of the caller's frame (unaff_retaddr + 0xC, in_stack_00000010) are explicit parameters,
// named in each function's comment. Unmapped callees are seams on D2GameBatch46E80Env in retail
// argument order. The fatal path Fog #10265(id) -> #10024(text) -> _exit(-1) is collapsed to the
// exit, which is the only observable effect.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/ (QUESTS/Quest.cpp, MISSILES, ITEMS); the RNG
// step reuses the 0x6AC690C5 multiplier already modelled in d2game_batch_1480.cpp.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void*    rdptr(uint32_t a, uint32_t o) { return rdptr((const void*)(uintptr_t)a, o); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees and module tables the cached decompiles name. Retail RVAs in comments.
struct D2GameBatch46E80Env {
    // sgptDataTables variable: MonStats rows at +0xA78 (stride 0x1A8) with count at +0xA80; the
    // 0x20-byte row array at +0xC50 with count at +0xC54 used by the weighted roll.
    const uint8_t* tables;
    void     (*ordinal10331)(void* param);                 // 0xA478 Fog #10331
    void*    (*subEEF10)(void* query);                     // 0xEEF10, returns a unit node
    void     (*sub20CF0)();                                // 0x20CF0
    int32_t  (*isBadCodePtr)(void* fn);                    // IsBadCodePtr
    void*    (*subE03A0)(int32_t unit, uint32_t id);       // 0xE03A0, returns a type-1 record
    int32_t  (*sub23100)(void* thisPtr, uint32_t param1, uint32_t entry, void* thisArg); // 0x23100
    void     (*sub21FB0)();                                // 0x21FB0
    void     (*sub21EC0)();                                // 0x21EC0
    void     (*sub22A80)(uint32_t mode);                   // 0x22A80
    void     (*ordinal10887)(void* unit, uint32_t stat, uint32_t v1, uint32_t v2); // 0xA472 D2Common #10887
};

// MonStats id resolution. in_EAX is a row id; when in range the record's +0x02 word replaces it. A
// resulting id in range with value 0x13 answers the clamped level 0x14 (EDI 6) or 0x15 (EDI 7/0xC/
// 0x10, left in EAX for the common return); 0x3A answers 0x3B for EDI 6/7 and 0x3C for EDI 0xC/0x10.
// Any other id, EDI value or out-of-range id returns in_EAX unchanged. "Clamped" is the retail idiom
// ((bound < count) - 1 & -bound) + bound, i.e. bound when count > bound else -1.
// 1.13c D2Game+0x46E80
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_46E80(int32_t eaxValue, int32_t edi,
                                                const D2GameBatch46E80Env* env)
{
    const uint8_t* tables = env->tables;
    const int32_t count = (int32_t)rd32(tables, 0xA80);
    int32_t resolved = eaxValue;

    if (eaxValue > -1 && eaxValue < count) {
        const uint32_t row = (uint32_t)eaxValue * 0x1A8 + rd32(tables, 0xA78);
        if (row != 0) resolved = (int32_t)(int16_t)rd16((const void*)(uintptr_t)row, 2);
    }
    if (resolved > -1 && resolved < count) {
        if (resolved == 0x13) {
            switch (edi) {
            case 6:  return count > 0x14 ? 0x14 : -1;
            case 7: case 0xC: case 0x10: eaxValue = count > 0x15 ? 0x15 : -1; break;
            }
        }
        else if (resolved == 0x3A) {
            switch (edi) {
            case 6: case 7:      return count > 0x3B ? 0x3B : -1;
            case 0xC: case 0x10: return count > 0x3C ? 0x3C : -1;
            }
        }
    }
    return eaxValue;
}

// Bump the per-slot counter at param_2+0x90+param_1*4, wrapping -1 back to 1; param_1 above 5 is the
// retail fatal (Fog #10265 case 0x1C5). __fastcall: param_1 is ECX, param_2 EDX.
// 1.13c D2Game+0xdf9c0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DF9C0(int32_t param1, void* param2)
{
    if (param1 > 5) d2game_fatal(0x1C5);
    int32_t v = (int32_t)rd32(param2, (uint32_t)(0x90 + param1 * 4)) + 1;
    if (v == -1) v = 1;
    wr32(param2, (uint32_t)(0x90 + param1 * 4), (uint32_t)v);
}

// Stamp loop over at most in_EAX nodes: ordinal 10331(param_1) first, then per node subEEF10 fills the
// frame query; a non-null type-1 node has its +0x28 record's +0x2C set to the caller's stack value at
// retaddr+0xC, +0x30 to 1 and +0x28 to ESI (unaff_ESI), followed by sub20CF0. The return is 0 when
// param_1 is null, else 1 as soon as any node was seen. in_EAX, ESI and the caller's retaddr+0xC are
// explicit here.
// 1.13c D2Game+0xEF7D0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EF7D0(void* param1, int32_t eaxCount, uint32_t esiValue,
                                                 uint32_t stackArg0xC,
                                                 const D2GameBatch46E80Env* env)
{
    if (param1 == nullptr) return 0;

    env->ordinal10331(param1);
    int32_t result = 0;                    // retail's uStack_34; unset when the loop never runs
    int32_t query = 0;                     // the 0xFFFFFFD4 frame slot passed to subEEF10
    for (; eaxCount > 0; --eaxCount) {
        void* node = env->subEEF10(&query);
        if (node != nullptr) {
            result = 1;
            if (rd32(node, 0) == 1) {
                void* rec = rdptr(node, 0x14);
                const uint32_t link = rec != nullptr ? rd32(rec, 0x28) : 0;
                if (link != 0) {           // retail writes through a null link unchecked
                    wr32(link, 0x2C, stackArg0xC);
                    wr32(link, 0x30, 1);
                    wr32(link, 0x28, esiValue);
                }
            }
            env->sub20CF0();
        }
    }
    return (uint32_t)result;
}

// Flag-setting sibling: one subEEF10 query; for a non-null type-1 node, its +0x28 record's +0x08 byte
// gets bit 1 when the stack argument at +0x10 is non-zero, always bit 0, and +0x2C = node[3], +0x30 =
// 1, +0x28 = ESI. in_stack_00000010 and unaff_ESI are explicit.
// 1.13c D2Game+0xEF8B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_EF8B0(uint32_t esiValue, int32_t stack10,
                                             const D2GameBatch46E80Env* env)
{
    int32_t query = 0;                     // the 0xFFFFFFD8 frame slot
    void* node = env->subEEF10(&query);
    if (node == nullptr) return;

    const uint32_t id = rd32(node, 0xC);
    if (rd32(node, 0) != 1) return;

    void* rec = rdptr(node, 0x14);
    const uint32_t link = rec != nullptr ? rd32(rec, 0x28) : 0;
    if (link == 0) return;                 // retail writes through a null link unchecked
    uint8_t flags = rd8((const void*)(uintptr_t)link, 8);
    if (stack10 != 0) flags |= 2;
    flags |= 1;
    ((uint8_t*)(uintptr_t)link)[8] = flags;
    wr32(link, 0x2C, id);
    wr32(link, 0x30, 1);
    wr32(link, 0x28, esiValue);
}

// Callback dispatch: for a type-1 EAX unit with a +0x14 record whose +0x28 record links to +0x28, the
// 0xE03A0 lookup ((record+0x28, record+0x2C)) is followed for each key in the record's +0x34 list:
// the key is masked to 7 bits into the base+0x1320 bucket array, each chain node's +0xC is compared
// and a match calls EBX(param_2) once, then the walk continues with the captured next pointer. The
// callback is validated with IsBadCodePtr (fatal 0x24E); a null bucket base is fatal 0x2D3.
// 1.13c D2Game+0x21490
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_21490(uint32_t param1, uint32_t param2, void* eaxUnit,
                                             void (*ebxCallback)(uint32_t),
                                             const D2GameBatch46E80Env* env)
{
    (void)param1;

    if (env->isBadCodePtr((void*)ebxCallback) != 0) d2game_fatal(0x24E);
    if (eaxUnit == nullptr || rd32(eaxUnit, 0) != 1) return;

    void* unitRec = rdptr(eaxUnit, 0x14);
    if (unitRec == nullptr) return;
    void* rec = rdptr(unitRec, 0x28);
    if (rec == nullptr || rd32(rec, 0x28) == 0) return;

    void* sought = env->subE03A0((int32_t)rd32(rec, 0x28), rd32(rec, 0x2C));
    if (sought == nullptr || rd32(sought, 0) != 1) return;

    void* inner = rdptr(sought, 0x14);
    const uint32_t base = inner != nullptr ? rd32(inner, 0x28) : 0;
    if (base == 0) return;                 // retail dereferences the null inner record
    uint32_t node = rd32((const void*)(uintptr_t)base, 0x34);
    while (node != 0) {
        const uint32_t next = rd32((const void*)(uintptr_t)node, 4);
        const uint32_t key = rd32((const void*)(uintptr_t)node, 0);
        uint32_t entry = rd32(base, 0x1320 + (key & 0x7F) * 4);
        while (entry != 0) {
            if (rd32(entry, 0xC) == key) {
                ebxCallback(param2);
                break;
            }
            entry = rd32(entry, 0xE4);
        }
        node = next;
    }
}

// Quest re-init on a type-1 unit whose +0x80 game record has +0x6D below 3: param_1 zero runs the two
// re-init helpers; non-zero first re-arms mode 4 via 0x22A80, runs the same helpers and then
// D2Common #10887(this, 0x4A, 0, 0).
// 1.13c D2Game+0x22FA0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_22FA0(void* thisPtr, int32_t param1,
                                             const D2GameBatch46E80Env* env)
{
    if (thisPtr == nullptr || rd32(thisPtr, 0) != 1) return;
    const uint32_t game = rd32(thisPtr, 0x80);
    if (game == 0 || rd8((const void*)(uintptr_t)game, 0x6D) >= 3) return;

    if (param1 == 0) {
        env->sub21FB0();
        env->sub21EC0();
        return;
    }
    env->sub22A80(4);
    env->sub21FB0();
    env->sub21EC0();
    env->ordinal10887(thisPtr, 0x4A, 0, 0);
}

// Weighted roll over the 0x20-byte table at sgptDataTables+0xC50: entries whose word at
// +0x14+level*2 is positive, whose +0x08 byte is clear, which pass 0x23100(this, param_1, entry,
// this) and whose param_2 flag byte is clear are collected (up to 256) with their weights. A
// 0x6AC690C5 64-bit roll of param_1+0x20/+0x24 (stored back) picks: mask when the total is a power of
// two, modulo otherwise; the walk subtracts weights until one covers the roll. Zero total or no hit
// answers 0. in_EAX is the unit, param_1 the roll state, param_2 the byte flags.
// 1.13c D2Game+0x23980
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_23980(uint32_t eaxUnit, uint32_t param1, uint32_t param2,
                                                const D2GameBatch46E80Env* env)
{
    if (eaxUnit == 0 || rd8(eaxUnit, 0x6D) > 2) return 0;

    void* thisPtr = rdptr(eaxUnit, 0x70);
    const int32_t count = (int32_t)rd32(env->tables, 0xC54);
    const uint32_t rowBase = count < 1 ? 0 : rd32(env->tables, 0xC50);
    const int32_t slot = (int32_t)rd8(eaxUnit, 0x6D) * 2 + 0x14;

    int32_t  ids[512];
    int32_t  n = 0;
    uint32_t total = 0;
    for (int32_t i = 0; i < count; ++i) {
        const uint32_t entry = rowBase + (uint32_t)i * 0x20;
        if ((int16_t)rd16(entry, (uint32_t)slot) <= 0) continue;
        if (rd8(entry, 8) != 0) continue;
        if (env->sub23100(thisPtr, param1, entry, thisPtr) == 0) continue;
        if (rd8(param2, (uint32_t)i) != 0) continue;
        const int32_t weight = (int32_t)(int16_t)rd16(entry, (uint32_t)slot);
        ids[n * 2] = i;
        ids[n * 2 + 1] = weight;
        total += (uint32_t)weight;
        if (++n > 0xFF) break;
    }

    uint32_t roll = total;
    if ((int32_t)roll < 1) {
        roll = 0;
    }
    else {
        const uint64_t v = (uint64_t)rd32(param1, 0x20) * 0x6AC690C5u + (uint64_t)rd32(param1, 0x24);
        std::memcpy((uint8_t*)(uintptr_t)param1 + 0x20, &v, 8);
        if ((roll & (roll - 1)) == 0) roll = (roll - 1) & (uint32_t)v;
        else                          roll = (uint32_t)((v & 0xFFFFFFFFull) % (uint64_t)roll);
    }

    for (int32_t i = 0; i < n; ++i) {
        if ((int32_t)roll < ids[i * 2 + 1]) return ids[i * 2];
        roll -= (uint32_t)ids[i * 2 + 1];
    }
    return 0;
}
