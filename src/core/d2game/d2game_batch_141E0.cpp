// src/core/d2game/d2game_batch_141E0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// only arguments (in_EAX / unaff_EBX / unaff_ESI / unaff_EDI / in_ECX / in_EDX) are explicit
// parameters named at each function; the stdcall stack arguments follow the disassembly. Unmapped
// callees are seams on D2GameBatch141E0Env in retail argument order; a seam whose decompile shows no
// arguments is taken to have none. d2game_fatal collapses the Fog 10265/10024 diagnostic and the
// _exit(-1) that ends every retail assert path (assert code preserved in the call site).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EDI, in_EAX, ...).
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline int16_t  rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void     wr8 (uint32_t a, uint32_t o, uint8_t v) { wr8((void*)(uintptr_t)a, o, v); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

[[noreturn]] static void d2game_fatal(uint32_t code) { (void)code; _exit(-1); }

struct D2GameBatch141E0Env {
    // sgptDataTables (the global every table probe here dereferences): item-class table count at
    // +0xC54, the 0x20-byte rows at +0xC50.
    const uint8_t* dataTables;

    // D2Common ordinals, in the shape the decompile calls them.
    int16_t  (*ordinal10718)(uint32_t unit);      // ITEMS_GetItemFormat (stdcall, unit on the stack)
    uint32_t (*ordinal10331)(uint32_t unit);      // UNITS_GetRoom (stdcall, unit on the stack)
    int32_t  (*ordinal10826)(uint32_t value);
    uint32_t (*ordinal10411)(uint32_t unit);      // the unit's RNG state (returned as a pointer)
    void     (*ordinal10611)(uint32_t unit, uint32_t value);
    void     (*ordinal10883)(uint32_t unit, uint32_t value);
    uint32_t (*ordinal11036)(uint32_t unit);
    uint16_t (*ordinal10257)(void);
    void*    (*ordinal10248)(uint16_t handle);
    void     (*ordinal10525)(void);
    uint16_t (*ordinal11028)(void);
    void     (*ordinal10977)(void);
    void     (*ordinal10996)(uint32_t a, uint32_t b, uint32_t unit, uint32_t value,
                             uint32_t c, uint32_t d);
    void     (*ordinal10312)(void);

    // D2Game seams.
    void     (*sub13C20)(void* item, uint32_t arg);
    void     (*sub13A30)(void* item);
    void     (*sub13E60)(void* item, uint32_t arg);
    void     (*sub137F0)(void* item);
    int32_t  (*sub34210)(void* item, uint32_t arg);
    int32_t  (*sub34370)(void* item, uint32_t arg);
    int32_t  (*sub344D0)(void* item, uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                         uint32_t e, uint32_t f);
    int32_t  (*sub348A0)(void* item, uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                         uint32_t e);
    int32_t  (*sub33DE0)(void* item, void* record);
    void     (*sub14F20)(void* item);
    void     (*sub15240)(void* item);
    int32_t  (*subE73C0)(void* item);             // 0xE73C0 (item in ECX)
    void*    (*subEDDB0)(uint32_t monsterId, int32_t a2, void* unit, void* game,
                         uint32_t a3, uint32_t a4);   // 0xEDDB0 (MonsterSpawn.cpp prior art)
    void     (*sub20CF0)(void* unit, void* ctx, void* game);   // 0x20CF0 AddTargetNode
    void     (*sub8A3E0)(void* client, const void* src, uint32_t n);   // 0x8A3E0 packet append
    uint32_t (*storm501)(void* dst, const void* src, uint32_t n);      // Storm Ordinal_501 strCopy
    void     (*subDFC10)(void* ctx);
    void     (*sub977E0)(void* game, uint32_t guid);                   // 0x977E0 (NAMED)
    void     (*fog10046)(void* pool, void* mem, const char* file, uint32_t line, uint32_t zero);
};

// Format dispatch: a non-zero ITEMS_GetItemFormat picks 0x13C20(item, arg), otherwise the format-0
// tail 0x13A30 runs with the item still in EDI. Answer is the callee's.
// 1.13c D2Game+0x141E0
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_141E0(void* item, uint32_t arg,
                                                           const D2GameBatch141E0Env* env)
{
    if (env->ordinal10718((uint32_t)(uintptr_t)item) != 0) {
        env->sub13C20(item, arg);
        return 0;
    }
    env->sub13A30(item);
    return 0;
}

// The same dispatch for the second item layout: non-zero format -> 0x13E60(item EDI, arg), zero ->
// 0x137F0 with the item in EDI.
// 1.13c D2Game+0x14AF0
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_14AF0(void* item, uint32_t arg,
                                                           const D2GameBatch141E0Env* env)
{
    if (env->ordinal10718((uint32_t)(uintptr_t)item) != 0) {
        env->sub13E60(item, arg);
        return 0;
    }
    env->sub137F0(item);
    return 0;
}

// Four stack arguments, item in ESI, EBX and EDI carried into the creation call. Format non-zero ->
// 0x344D0(item, arg1, arg2, arg3, ebx, edi, arg4); format zero -> 0x348A0(item, arg1, arg2, arg3,
// ebx, edi). The low 16 bits of the answer are the created unit handle.
// 1.13c D2Game+0x14B50
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_14B50(void* item, void* ebx, void* edi,
                                                           uint32_t arg1, uint32_t arg2,
                                                           uint32_t arg3, uint32_t arg4,
                                                           const D2GameBatch141E0Env* env)
{
    if (env->ordinal10718((uint32_t)(uintptr_t)item) != 0) {
        return env->sub344D0(item, arg1, arg2, arg3, (uint32_t)(uintptr_t)ebx,
                             (uint32_t)(uintptr_t)edi, arg4);
    }
    return env->sub348A0(item, arg1, arg2, arg3, (uint32_t)(uintptr_t)ebx,
                         (uint32_t)(uintptr_t)edi);
}

// Random item generation. Two format-picked tier rolls (0x34210 when the format is non-zero, 0x34370
// otherwise) must both be non-zero; then the record's level (+0xC) picks the floor 1/2/3/4 at >0x1E,
// >0x32, >0x46 and the unit's RNG (ordinal 10411, LCG *0x6AC690C5 + high word) rolls the count in
// 0..4, floored by that tier. Count 0 runs the level-prefix tail (three 10257/10248/10996 rounds over
// 11028/10248, then 0x33DE0(item, record)) and answers 1. Each other roll generates one affix or
// prefix unit (0x344D0/0x348A0 with the record's +0x68 or +0x74 dword array and the picked flag),
// probing 11028/10257 + 10248 until the created unit matches, and descends the array index on three
// misses. The tail record pointer is the stack argument (the dispatcher's EDI).
// 1.13c D2Game+0x14BF0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_14BF0(void* item, void* record,
                                                            const D2GameBatch141E0Env* env)
{
    int16_t a, b;
    if (env->ordinal10718((uint32_t)(uintptr_t)item) != 0) a = (int16_t)env->sub34210(item, 1);
    else                                                  a = (int16_t)env->sub34370(item, 1);
    if (env->ordinal10718((uint32_t)(uintptr_t)item) != 0) b = (int16_t)env->sub34210(item, 0);
    else                                                  b = (int16_t)env->sub34370(item, 0);
    if (a == 0 || b == 0) return 0;

    const uint32_t unit = (uint32_t)(uintptr_t)item;
    env->ordinal10611(unit, 0);
    env->ordinal10883(unit, 0);

    uint32_t* rng = (uint32_t*)(uintptr_t)env->ordinal10411(unit);
    const int32_t level = (int32_t)rd32(record, 0xC);
    uint32_t tier = 1;
    if (level > 0x1E) tier = 2;
    if (level > 0x32) tier = 3;
    if (level > 0x46) tier = 4;

    uint64_t roll = (uint64_t)rng[0] * 0x6AC690C5u + (uint64_t)rng[1];
    rng[0] = (uint32_t)roll;
    rng[1] = (uint32_t)(roll >> 32);
    uint32_t left = (uint32_t)((uint32_t)roll % 5u);
    if (left < tier) left = tier;

    int32_t index = 0;       // the decompile's iVar13: the item at first, then a 10248 result
    int32_t slot = 0;        // the record array index (iVar15)
    for (;;) {
        if (left == 0) {
            env->ordinal10977();
            for (int32_t i = 0; i < 3; ++i) {
                const uint16_t h1 = (uint16_t)env->ordinal10257();
                void* p1 = env->ordinal10248(h1);
                if (p1 != nullptr) env->ordinal10996(0, 0, unit, (uint32_t)(uintptr_t)p1, 0, 0);
                const uint16_t h2 = (uint16_t)env->ordinal11028();
                void* p2 = env->ordinal10248(h2);
                if (p2 != nullptr) env->ordinal10996(0, 0, unit, (uint32_t)(uintptr_t)p2, 0, 0);
            }
            env->sub33DE0(item, record);
            return 1;
        }

        roll = (uint64_t)rng[0] * 0x6AC690C5u + (uint64_t)rng[1];
        rng[0] = (uint32_t)roll;
        rng[1] = (uint32_t)(roll >> 32);

        if (index == 3 || (slot != 3 && (roll & 1) != 0)) {
            int32_t made = 0;
            do {
                const uint32_t affix = rd32(record, 0x74 + (uint32_t)slot * 4);
                uint32_t created;
                if (env->ordinal10718(unit) != 0)
                    created = (uint32_t)env->sub344D0(item, 1, 1, 0, 1, affix, 0);
                else
                    created = (uint32_t)env->sub348A0(item, 1, 1, 0, 1, affix);
                int32_t tries = 0;
                for (;;) {
                    const uint16_t h = (uint16_t)env->ordinal11028();
                    void* other = env->ordinal10248(h);
                    if (other != nullptr) {
                        void* mine = env->ordinal10248((uint16_t)created);
                        if ((int16_t)created == (int16_t)h ||
                            rd32(mine, 0x5C) == rd32(other, 0x5C)) break;
                    }
                    if (++tries > 2) { env->ordinal10525(); ++slot; goto next; }
                }
                ++made;
            } while (made < 0xFC);
            env->ordinal10525();
        }
        else {
            int32_t made = 0;
            do {
                const uint32_t prefix = rd32(record, 0x68 + (uint32_t)index * 4);
                uint32_t created;
                if (env->ordinal10718(unit) != 0)
                    created = (uint32_t)env->sub344D0(item, 1, 1, 0, 0, prefix, 0);
                else
                    created = (uint32_t)env->sub348A0(item, 1, 1, 0, 0, prefix);
                int32_t tries = 0;
                for (;;) {
                    const uint16_t h = (uint16_t)env->ordinal10257();
                    void* other = env->ordinal10248(h);
                    if (other != nullptr) {
                        void* mine = env->ordinal10248((uint16_t)created);
                        if ((int16_t)created == (int16_t)h ||
                            rd32(mine, 0x5C) == rd32(other, 0x5C)) break;
                    }
                    if (++tries > 2) { env->ordinal10312(); ++index; goto next; }
                }
                ++made;
            } while (made < 0xFC);
            env->ordinal10312();
        }
    next:
        --left;
    }
}

// Class probe gate: ordinal 11036 must be non-zero, then ITEMS_GetItemFormat picks 0x15240 (non-zero)
// or 0x14F20 (zero), both with the item in ESI.
// 1.13c D2Game+0x154F0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_154F0(void* item,
                                                        const D2GameBatch141E0Env* env)
{
    if (env->ordinal11036((uint32_t)(uintptr_t)item) == 0) return;
    if (env->ordinal10718((uint32_t)(uintptr_t)item) != 0) env->sub15240(item);
    else                                                  env->sub14F20(item);
}

// Room-class probe: UNITS_GetRoom(item) feeds ordinal 10826; below 0x6C the item goes to 0xE73C0
// (item in ECX). ESI is the item, the only stack argument is unused by the rule.
// 1.13c D2Game+0x36050
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_36050(void* item, uint32_t arg,
                                                        const D2GameBatch141E0Env* env)
{
    (void)arg;
    const uint32_t room = env->ordinal10331((uint32_t)(uintptr_t)item);
    if (env->ordinal10826(room) < 0x6C) env->subE73C0(item);
}

// Item-class table probe: an id in [0, count) indexes the 0x20-byte rows at sgptDataTables+0xC50 and
// answers the dword at row+0x1C; anything out of range, or a null row, answers 0. EAX is the id.
// 1.13c D2Game+0x21FB0
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_21FB0(int32_t id,
                                                           const D2GameBatch141E0Env* env)
{
    const int32_t count = (int32_t)rd32(env->dataTables, 0xC54);
    if (id < 0 || id >= count) return 0;
    const uint32_t row = rd32(env->dataTables, 0xC50) + (uint32_t)id * 0x20;
    if (row == 0) return 0;
    return (int32_t)rd32(row, 0x1C);
}

// Class-id filter. EAX is the source record, ECX the destination unit, the stack argument the mode:
// mode 0 appends at the destination string's NUL (scanning at most 9 bytes), any other mode starts at
// 0. Source bytes are copied while the table count allows the id and the row's byte +7 is non-zero,
// stopping at 9 written bytes, at the source NUL, at an out-of-range id, or at a null row.
// 1.13c D2Game+0x23060
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_23060(void* self, uint32_t mode, void* record,
                                                        const D2GameBatch141E0Env* env)
{
    uint8_t* src = nullptr;
    if (record != nullptr && rd32(record, 0) == 1 && rd32(record, 0x14) != 0)
        src = (uint8_t*)(uintptr_t)rd32(record, 0x14) + 0x1C;
    uint8_t* dst = nullptr;
    if (self != nullptr && rd32(self, 0) == 1 && rd32(self, 0x14) != 0)
        dst = (uint8_t*)(uintptr_t)rd32(self, 0x14) + 0x1C;
    if (src == nullptr || dst == nullptr) return;

    int32_t at = 0;
    if (mode == 0) {
        do {
            if (dst[at] == 0) break;
            ++at;
        } while (at < 9);
        if (at >= 9) at = 9;
    }
    if (at >= 9) return;

    const int32_t count = (int32_t)rd32(env->dataTables, 0xC54);
    while (*src != 0) {
        const uint32_t id = *src;
        if (count <= (int32_t)id) return;
        const uint32_t row = rd32(env->dataTables, 0xC50) + id * 0x20;
        if (row == 0) return;
        if (rd8(row, 7) != 0) dst[at++] = (uint8_t)id;
        ++src;
        if (++at > 8) return;
    }
}

// Outgoing item-name packet: the record's 0x1A string must be shorter than 0x100 (assert 0x5BA),
// byte +0x119 is cleared, the first 10 record bytes are copied, then the +0x0A string (16-byte copy)
// and the +0x1A string (0x100-byte copy) are appended back to back, the second overwriting the first
// copy's terminator. 0x8A3E0(client, buffer, length) sends it. EAX is the record, the stack argument
// the client.
// 1.13c D2Game+0x8B470
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8B470(void* record, void* client,
                                                        const D2GameBatch141E0Env* env)
{
    if (std::strlen((const char*)record + 0x1A) >= 0x100) d2game_fatal(0x5BA);
    wr8(record, 0x119, 0);

    uint8_t buf[0x11C];
    std::memcpy(buf, record, 0x0A);
    const uint32_t nameLen = env->storm501(buf + 0x0A, (const uint8_t*)record + 0x0A, 0x10);
    const uint32_t tailLen = env->storm501(buf + 0x0A + nameLen - 1,
                                           (const uint8_t*)record + 0x1A, 0x100);
    env->sub8A3E0(client, buf, 0x09 + nameLen + tailLen);
}

// Eight-byte action packet: 0xA5, the low byte of ECX, the dword in EDX, the low word of the first
// stack argument. The second stack argument is the client 0x8A3E0 sends to.
// 1.13c D2Game+0x8BA10
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8BA10(uint32_t p1, uint32_t p2, void* client,
                                                        uint16_t arg, const D2GameBatch141E0Env* env)
{
    uint8_t buf[8];
    std::memset(buf, 0, sizeof buf);
    buf[0] = 0xA5;
    buf[1] = (uint8_t)p1;
    std::memcpy(buf + 2, &p2, 4);
    std::memcpy(buf + 6, &arg, 2);
    env->sub8A3E0(client, buf, 8);
}

// Pet-list node removal. Unlink the node whose +4 equals ECX from the list at *arg1 (+0x14 next),
// run the 0xDFC10 iterator over the single-element context { this }, then for the game's unit at
// +0x1320+(id&0x7F)*4 with +0xC == id either clear bit 31 of +0xC4 (arg2 zero) or call 0x977E0,
// free the node through Fog 10046 on pool game+0x1C (line 0x25C), and decrement the list count at
// +4; a negative count asserts 0x260, a null game asserts 0x2D3. ECX is the node id owner, EAX the
// game.
// 1.13c D2Game+0x97920
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_97920(uint32_t self, void* game, void* list,
                                                        int32_t flag, const D2GameBatch141E0Env* env)
{
    uint32_t node = rd32(list, 0);
    uint32_t prev = 0;
    while (node != 0) {
        if (rd32(node, 4) == self) break;
        prev = node;
        node = rd32(node, 0x14);
    }
    if (node == 0) return;

    if (prev != 0) wr32(prev, 0x14, rd32(node, 0x14));
    else           wr32(list, 0, rd32(node, 0x14));

    uint32_t ctx[7] = { self, 0, 0, 0, 0, 0, 0 };
    env->subDFC10(ctx);

    const uint32_t guid = rd32(node, 4);
    if (game == nullptr) d2game_fatal(0x2D3);

    const uint32_t first = rd32(game, 0x1320 + (guid & 0x7F) * 4);
    for (uint32_t ent = first; ent != 0; ent = rd32(ent, 0xE4)) {
        if (rd32(ent, 0xC) == guid) {
            if (flag == 0) wr32(ent, 0xC4, rd32(ent, 0xC4) & 0x7FFFFFFFu);
            else           env->sub977E0(game, guid);
            break;
        }
    }

    env->fog10046((void*)(uintptr_t)rd32(game, 0x1C), (void*)(uintptr_t)node,
                  "..\\Source\\D2Game\\PLAYER\\PlayerPets.cpp", 0x25C, 0);

    uint32_t count = rd32(list, 4) - 1;
    wr32(list, 4, count);
    if ((int32_t)count < 0) d2game_fatal(0x260);
}

// Monster summon batch. ESI is the summoner, EAX the count, EBX the game, and four stack arguments
// feed 0xEDDB0(monsterId, arg2, summoner, game, arg3, arg4). Each spawned unit that comes back type 1
// records the summoner's GUID (+0xC) and the game in its +0x14 record's +0x2C/+0x30/+0x28, then
// 0x20CF0 pushes it onto the summoner's minion list. Answer 1 when at least one spawn succeeded.
// 1.13c D2Game+0xEDE90
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EDE90(void* unit, int32_t count, void* game,
                                                            uint32_t arg1, uint32_t arg2,
                                                            uint32_t arg3, uint32_t arg4,
                                                            const D2GameBatch141E0Env* env)
{
    if (unit == nullptr) return 0;

    const uint32_t guid = rd32(unit, 0xC);
    uint32_t spawned = 0;
    for (int32_t left = count; left > 0; --left) {
        void* rec = env->subEDDB0(arg1, (int32_t)arg2, unit, game, arg3, arg4);
        if (rec == nullptr) continue;
        spawned = 1;
        if (rd32(rec, 0) == 1) {
            const uint32_t owner = rd32(rec, 0x14);
            const uint32_t rec2 = owner != 0 ? rd32(owner, 0x28) : 0;
            wr32(rec2, 0x2C, guid);
            wr32(rec2, 0x30, 1);
            wr32(rec2, 0x28, (uint32_t)(uintptr_t)game);
        }
        env->sub20CF0(unit, rec, game);
    }
    return spawned;
}
