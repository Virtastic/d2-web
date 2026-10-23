// src/core/d2game/d2game_batch_97E20.cpp -- seven 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// only arguments (Ghidra's in_EAX / in_AL) and stack areas the decompile could not tie to a parameter
// (0x97E20's &stack0xffffffd4) are explicit parameters here. Unmapped callees are seams on
// D2GameBatch97E20Env in retail argument order; a seam whose decompile shows no arguments is taken to
// have none. The fatal path Ordinal_10265(id) -> Ordinal_10024(&0x6FD19FF9, id) -> _exit(-1) is
// modelled by d2game_fatal, as in the other batches.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { return (uint16_t)(rd8(p, o) | (rd8(p, o + 1) << 8)); }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
static inline void     put16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     put32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (in_EAX, ...).
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees and data tables the cached decompiles name but src/ does not implement.
struct D2GameBatch97E20Env {
    // sgptDataTables: the block whose +0xBF0 dword is the pet-type record count (0x97E20).
    const uint8_t* dataTables;
    // 0xA4C0, D2Common #10920(unit): the player-data record.
    uint32_t (*ordinal10920)(void* unit);
    // 0x97A20(unit, thing, blob): appends one pet node from the three-dword blob.
    uint32_t (*sub97A20)(void* unit, uint32_t thing, const uint32_t* blob);
    // 0xDFC10(record): the seven-dword notify record staged by 0x97E20.
    void     (*subDFC10)(const uint32_t* record);
    // 0xA40C, D2Common #10140(arg, dest): the 0x53720 attach path.
    void     (*ordinal10140)(void* arg, void* dest);
    // 0xE0110: 0x53720's post-attach callback.
    void     (*subE0110)();
    // 0xA4A2, D2Common #11090(arg, value).
    void     (*ordinal11090)(void* arg, uint32_t value);
    // 0x8A3E0: queue one packet payload of the given byte length (0x8ADE0).
    void     (*sendPacket)(const void* payload, uint32_t size);
    // 0xAFDC, D2Common #10516(p).
    uint32_t (*ordinal10516)(void* p);
    // 0xA8A4, D2Common #10202(p, a, b, file).
    void     (*ordinal10202)(void* p, uint32_t a, uint32_t b, const char* file);
    // 0x8D3D0: the shared SCmd tail of the five command handlers.
    void     (*sub8D3D0)();
    // 0xA6C4, D2Common #10020(p).
    void     (*ordinal10020)(void* p);
    // 0xAFD6, D2Common #10728(p).
    uint32_t (*ordinal10728)(void* p);
    // 0xA92E, D2Common #10012(p, value).
    void     (*ordinal10012)(void* p, uint32_t value);
    // 0xA8F2, D2Common #10882 SetX(p, 0) (0x8E090).
    void     (*ordinal10882)(void* p, uint32_t value);
    // 0xA964, D2Common #11118 SetY(p, 0) (0x8E090).
    void     (*ordinal11118)(void* p, uint32_t value);
};

// Pet-list append: with a player-data record whose +0x44 array pointer is non-null and a pet type
// index in [0, sgptDataTables[0xBF0]) whose 0xC-byte entry is non-zero, 0x97A20 appends a node built
// from param_1's +0x0C guid and the three-dword stack blob; on success the seven-dword record
// (guid, unit id, unit +0x04 low word, pet type, blob[0], blob[1], 0) goes to 0xDFC10. The decompile
// merges the index and pointer roles of param_3; the index is taken as the integer it is
// range-checked as, and the two blob reads come from the same stack area 0x97A20 reads.
// 1.13c D2Game+0x97e20
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_97E20(uint32_t param1, uint32_t param2, int32_t petType,
                                             uint32_t eaxUnit, const uint32_t* stackBlob,
                                             const D2GameBatch97E20Env* env)
{
    (void)param2;

    uint32_t playerData = env->ordinal10920((void*)(uintptr_t)param1);
    if (playerData == 0) {
        d2game_fatal(0x3b5);
    }

    const void* pets = rdptr((const void*)(uintptr_t)playerData, 0x44);
    if (pets == nullptr) return;
    if ((int32_t)petType < 0) return;
    if (petType >= (int32_t)rd32(env->dataTables, 0xBF0)) return;
    if ((int32_t)(rd32(pets, 0) + (uint32_t)petType * 0xC) == 0) return;

    if (env->sub97A20((void*)(uintptr_t)eaxUnit, param1, stackBlob) == 0) return;

    uint32_t record[7];
    record[6] = 0;
    record[3] = (uint32_t)petType;
    record[0] = param1 != 0 ? rd32(param1, 0xC) : 0xFFFFFFFF;
    record[1] = eaxUnit != 0 ? rd32(eaxUnit, 0xC) : 0xFFFFFFFF;
    record[2] = param1 != 0 ? (uint32_t)rd16((const void*)(uintptr_t)param1, 4) : 0xFFFF;
    record[4] = stackBlob[0];
    record[5] = stackBlob[1];
    env->subDFC10(record);
}

// Attach a game/session record to an argument: EAX+0x16C = arg[0], +0x170 = arg[3], +0x174 = arg.
// When arg[0] is zero the player data's +0x9C is set to EAX, 10140(arg, EAX+0x0D) and 0xE0110 run,
// 11090(arg, 1) runs, and the player data's +0x50/+0x54/+0x58/+0x5C are zeroed. A null EAX, a null
// arg, or a non-zero arg[0] is fatal (0x114 / 0x4BB / 0x4BC).
// 1.13c D2Game+0x53720
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_53720(uint32_t eaxGame, uint32_t param1,
                                             const D2GameBatch97E20Env* env)
{
    if (eaxGame == 0) d2game_fatal(0x114);
    if (param1 == 0) {
        wr32((void*)(uintptr_t)eaxGame, 0x174, 0);
        d2game_fatal(0x4BB);
    }

    wr32((void*)(uintptr_t)eaxGame, 0x16C, rd32(param1, 0));
    wr32((void*)(uintptr_t)eaxGame, 0x170, rd32(param1, 0xC));
    wrptr((void*)(uintptr_t)eaxGame, 0x174, (const void*)(uintptr_t)param1);
    if (rd32(param1, 0) != 0) d2game_fatal(0x4BC);

    uint32_t playerData = env->ordinal10920((void*)(uintptr_t)param1);
    wr32((void*)(uintptr_t)playerData, 0x9C, eaxGame);
    env->ordinal10140((void*)(uintptr_t)param1, (void*)(uintptr_t)(eaxGame + 0xD));
    env->subE0110();
    env->ordinal11090((void*)(uintptr_t)param1, 1);

    playerData = env->ordinal10920((void*)(uintptr_t)param1);
    wr32((void*)(uintptr_t)playerData, 0x5C, 0);
    wr32((void*)(uintptr_t)playerData, 0x50, 0);
    wr32((void*)(uintptr_t)playerData, 0x58, 0);
    wr32((void*)(uintptr_t)playerData, 0x54, 0);
}

// Eighteen-byte packet 0x7D: type byte, a byte from *in_EAX (6 when in_EAX is null), in_EAX[3]
// split into two words, *(param_2+0x0C) split the same way, then param_4 and param_5 as dwords.
// param_1 and param_3 are unused.
// 1.13c D2Game+0x8ADE0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8ADE0(uint32_t param1, uint32_t param2, uint32_t param3,
                                             uint32_t param4, uint32_t param5,
                                             uint32_t eaxUnit, const D2GameBatch97E20Env* env)
{
    (void)param1;
    (void)param3;

    uint8_t buf[0x12];
    uint32_t unitId, otherId;

    buf[0] = 0x7D;
    if (eaxUnit == 0) {
        buf[1] = 6;
        unitId = 0xFFFFFFFF;
    }
    else {
        buf[1] = rd8((const void*)(uintptr_t)eaxUnit, 0);
        unitId = rd32(eaxUnit, 0xC);
    }
    put16(buf, 2, (uint16_t)unitId);
    put16(buf, 4, (uint16_t)(unitId >> 16));

    otherId = param2 != 0 ? rd32(param2, 0xC) : 0xFFFFFFFF;
    put16(buf, 6, (uint16_t)otherId);
    put16(buf, 8, (uint16_t)(otherId >> 16));

    put32(buf, 10, param4);
    put32(buf, 14, param5);
    env->sendPacket(buf, 0x12);
}

// SCmd command dispatch on the byte in AL. '8' asserts through #10202 (SCmd.cpp line 0x9C2) when
// #10516 answers non-zero, then runs the shared tail. '9' brackets the tail with #10020 before and
// #10728 -> #10012 after, then restores the original value with #10012. Other bytes return.
// 1.13c D2Game+0x8dda0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8DDA0(uint32_t param1, uint8_t cmd,
                                             const D2GameBatch97E20Env* env)
{
    static const char kScmdFile[] = "..\\Source\\D2Game\\GAME\\SCmd.cpp";

    if (cmd == '8') {
        if (env->ordinal10516((void*)(uintptr_t)param1) != 0) {
            env->ordinal10202((void*)(uintptr_t)param1, 0x10, 0x9C2, kScmdFile);
        }
        env->sub8D3D0();
    }
    else if (cmd == '9') {
        env->ordinal10020((void*)(uintptr_t)param1);
        const uint32_t saved = param1;
        const uint32_t value = env->ordinal10728((void*)(uintptr_t)param1);
        env->ordinal10012((void*)(uintptr_t)param1, value);
        env->sub8D3D0();
        env->ordinal10012((void*)(uintptr_t)param1, saved);
    }
}

// Shared assertion tail of the three handlers: with a unit whose first dword is non-zero (or a null
// unit) the expected id is 0, otherwise the player-data +0x9C; a mismatch is fatal (0x85C / 0x853 /
// 0x849 respectively), then 0x8D3D0 runs. The no-argument #10920 call carries the unit in EAX.
// 1.13c D2Game+0x8de50
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8DE50(uint32_t eaxUnit, uint32_t param1,
                                             const D2GameBatch97E20Env* env)
{
    uint32_t expected = 0;
    if (eaxUnit != 0 && rd32(eaxUnit, 0) == 0) {
        expected = rd32(env->ordinal10920((void*)(uintptr_t)eaxUnit), 0x9C);
    }
    if (expected != param1) d2game_fatal(0x85C);
    env->sub8D3D0();
}

// 1.13c D2Game+0x8DEB0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8DEB0(uint32_t eaxUnit, uint32_t param1,
                                             const D2GameBatch97E20Env* env)
{
    uint32_t expected = 0;
    if (eaxUnit != 0 && rd32(eaxUnit, 0) == 0) {
        expected = rd32(env->ordinal10920((void*)(uintptr_t)eaxUnit), 0x9C);
    }
    if (expected != param1) d2game_fatal(0x853);
    env->sub8D3D0();
}

// 1.13c D2Game+0x8DF10
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8DF10(uint32_t eaxUnit, uint32_t param1,
                                             const D2GameBatch97E20Env* env)
{
    uint32_t expected = 0;
    if (eaxUnit != 0 && rd32(eaxUnit, 0) == 0) {
        expected = rd32(env->ordinal10920((void*)(uintptr_t)eaxUnit), 0x9C);
    }
    if (expected != param1) d2game_fatal(0x849);
    env->sub8D3D0();
}

// 1.13c D2Game+0x8DF70
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8DF70(uint32_t eaxUnit, uint32_t param1,
                                             const D2GameBatch97E20Env* env)
{
    uint32_t expected = 0;
    if (eaxUnit != 0 && rd32(eaxUnit, 0) == 0) {
        expected = rd32(env->ordinal10920((void*)(uintptr_t)eaxUnit), 0x9C);
    }
    if (expected != param1) d2game_fatal(0x7EF);
    env->sub8D3D0();
}

// 1.13c D2Game+0x8e090
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8E090(uint32_t eaxUnit, uint32_t param1, uint32_t param2,
                                             const D2GameBatch97E20Env* env)
{
    uint32_t expected = 0;
    if (eaxUnit != 0 && rd32(eaxUnit, 0) == 0) {
        expected = rd32(env->ordinal10920((void*)(uintptr_t)eaxUnit), 0x9C);
    }
    if (expected != param2) d2game_fatal(0x7D2);
    env->ordinal10882((void*)(uintptr_t)param1, 0);
    env->ordinal11118((void*)(uintptr_t)param1, 0);
    env->sub8D3D0();
}
