// src/core/d2game/d2game_batch_1EF50.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// only arguments (Ghidra's in_EAX / unaff_EDI / unaff_ESI / unaff_EBX) are explicit parameters and are
// named at each function. Unmapped callees are seams on D2GameBatch1EF50Env in retail argument order.
// The retail data blocks the decompiles name (sgptDataTables slots, gdwBitMasks, the 0x6FD2E5FC /
// 0x6FD2E600 / 0x6FD2E604 dwords and the 0x6FD22CE0 callback table) arrive as host pointers/values.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EDI, in_EAX, ...).
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8 ((const void*)(uintptr_t)a, o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void     wr8 (uint32_t a, uint32_t o, uint8_t v)  { wr8 ((void*)(uintptr_t)a, o, v); }
static inline void     wr16(uint32_t a, uint32_t o, uint16_t v) { wr16((void*)(uintptr_t)a, o, v); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

// Host seams for the callees and data blocks the cached decompiles name but src/ does not implement.
struct D2GameBatch1EF50Env {
    // 0x1EF50: sgptDataTables+0xA78 (0x1A8-byte records) and its count at +0xA80.
    const uint8_t* levelRecords;
    uint32_t levelRecordCount;
    uint32_t dat6fd2e5fc, dat6fd2e600, dat6fd2e604;
    const uint8_t* bitMasks;                          // gdwBitMasks
    void     (*sub21290)(uint32_t unit, void* out);   // 0x21290
    void     (*ordinal10171)();                       // Fog #10171
    // 0x7A570: sgptDataTables+0xBC0 and Fog #10195 / #10848.
    uint32_t dataTablesBC0;
    int32_t  (*ordinal10195)();
    int32_t  (*ordinal10848)();
    // 0x8A3E0 send/message tail (all packet builders below).
    void     (*sub8A3E0)(void* buf, uint32_t len);
    uint32_t (*ordinal10750)(uint32_t record);        // D2Common
    uint32_t (*ordinal10867)(uint32_t record);        // D2Common
    uint32_t (*ordinal11027)();
    // 0xCB8F0: the 0xC-byte callback table at 0x6FD22CE0.
    const uint8_t* table22CE0;
    int32_t  (*isBadCodePtr)(uint32_t fn);
    // Fatal path shared by 0xCB8F0 / 0x96C00 / 0x972C0.
    uint32_t (*ordinal10265)(uint32_t size);
    void     (*ordinal10024)(const char* file, uint32_t value);
    const char* assertFile;                           // DAT_6FD19FF9
    // 0x9D8E0.
    void     (*sub8E6F0)();
    // 0x964B0.
    void     (*sub977E0)(uint32_t ebx, uint32_t value);
    void     (*ordinal10046)(const char* file, uint32_t line, uint32_t zero);
    // 0x96C00 / 0x972C0.
    void     (*sub97B50)(uint32_t param1, uint32_t param2, uint32_t id);
    int32_t  (*ordinal10769)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*sub971F0)(uint32_t ebx);
};

static void batch1EF50Fatal(const D2GameBatch1EF50Env* env, uint32_t size)
{
    uint32_t v = env->ordinal10265(size);
    env->ordinal10024(env->assertFile, v);
    std::exit(-1);
}

// Stat/mode block for a unit: level record lookup plus the "state 10" walk. Two flag bits of unit+0xC4
// OR the 0x6FD2E600 / 0x6FD2E604 masks into param_2+0x50 with param_2+0x4C set to the 0x6FD2E5FC dword.
// When the level record's +0xD byte hits bit masks 0..2, the unit's +0x14 sub-record at +0x28/+0x20 is
// walked for a node whose +8 word is 10; a miss builds and sends the 10-header via 0x21290, and a hit
// copies the node's +0xC/+0x10 into param_2+0x4C/+0x50. The same walk runs again for monster ids 0x10B,
// 0xF3 or the (param_2+0x18 bit 0x800, param_2+0x3C == 6) pair. Monster id 0x44 copies the sub-record's
// +0x1C and 0x21F its +0x14 through the record-table short at +2. Ids 0x137/0x138 take the unit's
// +0x14 byte +0xE (or 0). param_1 is unused; EDI is the unit.
// 1.13c D2Game+0x1EF50
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_1EF50(uint32_t param1, uint32_t param2, uint32_t edi,
                                             const D2GameBatch1EF50Env* env)
{
    (void)param1;

    const uint32_t dat5FC = env->dat6fd2e5fc;
    const int32_t iVar1 = (int32_t)rd32(edi, 4);

    uint32_t iVar3;
    if (iVar1 < 0 || (int32_t)env->levelRecordCount <= iVar1) {
        iVar3 = 0;
    }
    else {
        iVar3 = (uint32_t)iVar1 * 0x1A8 + (uint32_t)(uintptr_t)env->levelRecords;
    }

    if (((rd32(edi, 0xC4) >> 0x1A) & 1) != 0) {
        wr32(param2, 0x50, rd32(param2, 0x50) | env->dat6fd2e600);
        wr32(param2, 0x4C, dat5FC);
    }
    if (((rd32(edi, 0xC4) >> 0x11) & 1) != 0) {
        wr32(param2, 0x4C, dat5FC);
        wr32(param2, 0x50, rd32(param2, 0x50) | env->dat6fd2e604);
    }

    const uint32_t uVar4 = rd8(iVar3, 0xD);
    if (((rd32(env->bitMasks, 4) & uVar4) != 0) ||
        ((rd32(env->bitMasks, 0) & uVar4) != 0) ||
        ((rd32(env->bitMasks, 8) & uVar4) != 0))
    {
        bool found10 = false;
        if (rd32(edi, 0) == 1 && rd32(edi, 0x14) != 0) {
            const uint32_t t = rd32(rd32(edi, 0x14), 0x28);
            if (t != 0) {
                const uint32_t list = rd32(t, 0x20);
                if (list != 0) {
                    uint32_t node = rd32(list, 0);
                    int32_t kind = (int32_t)rd32(node, 8);
                    while (kind != 10 && node != list) {
                        node = rd32(node, 0);
                        kind = (int32_t)rd32(node, 8);
                    }
                    found10 = (kind == 10);
                }
            }
        }
        if (!found10) {
            // local_1c[8] followed by five dwords initialised 10,0,0,0,0; 0x21290 takes the block.
            uint8_t local[0x1C] = {};
            wr32(local, 8, 10);
            env->sub21290(edi, local);
        }

        if (rd32(edi, 0) == 1) {
            if (rd32(edi, 0x14) != 0) {
                const uint32_t t = rd32(rd32(edi, 0x14), 0x28);
                if (t != 0) {
                    const uint32_t list = rd32(t, 0x20);
                    if (list != 0) {
                        uint32_t node = rd32(list, 0);
                        int32_t kind = (int32_t)rd32(node, 8);
                        while (kind != 10 && node != list) {
                            node = rd32(node, 0);
                            kind = (int32_t)rd32(node, 8);
                        }
                        if (kind != 10) node = 0;
                        if (node != 0 && rd32(node, 0xC) != 0) {
                            wr32(param2, 0x4C, rd32(node, 0xC));
                            wr32(param2, 0x50, rd32(node, 0x10));
                        }
                    }
                }
            }
            if (rd32(edi, 0) != 1 || rd32(edi, 0x14) == 0) iVar3 = 0;
            else iVar3 = rd32(rd32(edi, 0x14), 0x28);
        }
        else {
            iVar3 = 0;
        }
        if (rd32(iVar3, 0x38) != 0) env->ordinal10171();
    }

    if ((iVar1 != 0x10B && iVar1 != 0xF3) &&
        ((rd32(param2, 0x18) & 0x800) == 0 || rd16(param2, 0x3C) != 6))
    {
        goto level_lookup;
    }

    {
        bool found10 = false;
        if (rd32(edi, 0) == 1 && rd32(edi, 0x14) != 0) {
            const uint32_t t = rd32(rd32(edi, 0x14), 0x28);
            if (t != 0) {
                const uint32_t list = rd32(t, 0x20);
                if (list != 0) {
                    uint32_t node = rd32(list, 0);
                    int32_t kind = (int32_t)rd32(node, 8);
                    while (kind != 10 && node != list) {
                        node = rd32(node, 0);
                        kind = (int32_t)rd32(node, 8);
                    }
                    found10 = (kind == 10);
                }
            }
        }
        if (!found10) {
            uint8_t local[0x1C] = {};
            wr32(local, 8, 10);
            env->sub21290(edi, local);
        }
    }

    if (rd32(edi, 0) == 1 && rd32(edi, 0x14) != 0) {
        const uint32_t t = rd32(rd32(edi, 0x14), 0x28);
        if (t != 0) {
            const uint32_t list = rd32(t, 0x20);
            if (list != 0) {
                uint32_t node = rd32(list, 0);
                int32_t kind = (int32_t)rd32(node, 8);
                while (kind != 10 && node != list) {
                    node = rd32(node, 0);
                    kind = (int32_t)rd32(node, 8);
                }
                if (kind != 10) node = 0;
                if (node != 0) {
                    wr32(param2, 0x4C, rd32(node, 0xC));
                    wr32(param2, 0x50, rd32(node, 0x10));
                }
            }
        }
    }

level_lookup:
    if (rd32(edi, 0) == 1 && (int32_t)rd32(edi, 4) > -1) {
        int32_t level = (int32_t)rd32(edi, 4);
        if (level < (int32_t)env->levelRecordCount) {
            const uint32_t rec = (uint32_t)level * 0x1A8 + (uint32_t)(uintptr_t)env->levelRecords;
            if (rec != 0) level = (int32_t)(int16_t)rd16(rec, 2);
        }
        if (level > -1 && level < (int32_t)env->levelRecordCount) {
            if (level == 0x44) {
                uint32_t v = 0;
                if (rd32(edi, 0x14) != 0) {
                    const uint32_t t = rd32(rd32(edi, 0x14), 0x28);
                    if (t != 0) v = rd32(t, 0x1C);
                }
                wr32(param2, 0x4C, v);
            }
            else if (level == 0x21F) {
                uint32_t v = 0;
                if (rd32(edi, 0x14) != 0) {
                    const uint32_t t = rd32(rd32(edi, 0x14), 0x28);
                    if (t != 0) v = rd32(t, 0x14);
                }
                wr32(param2, 0x4C, v);
            }
        }
    }

    if (iVar1 != 0x138) {
        if (iVar1 == 0x137) {
            if (rd32(edi, 0) == 1 && rd32(edi, 0x14) != 0) {
                wr32(param2, 0x4C, rd8(rd32(edi, 0x14), 0xE));
                return;
            }
            wr32(param2, 0x4C, 0);
        }
        return;
    }
    if (rd32(edi, 0) == 1 && rd32(edi, 0x14) != 0) {
        wr32(param2, 0x4C, rd8(rd32(edi, 0x14), 0xE));
        return;
    }
    wr32(param2, 0x4C, 0);
}

// Clear a one-shot flag: EAX+0x18 is a small record; when its +2 byte is 1 and its +4 dword equals
// param_1's +0xC dword (or both are the -1 sentinel for a null param_1), it becomes +2 = 0, +1 = 1.
// 1.13c D2Game+0xe3b00
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Game_E3B00(uint32_t eaxUnit, uint32_t param1,
                                             const D2GameBatch1EF50Env* env)
{
    (void)env;
    const uint32_t iVar1 = rd32(eaxUnit, 0x18);
    if (rd8(iVar1, 2) == 1) {
        const int32_t iVar2 = param1 == 0 ? -1 : (int32_t)rd32(param1, 0xC);
        if ((int32_t)rd32(iVar1, 4) == iVar2) {
            wr8(iVar1, 2, 0);
            wr8(iVar1, 1, 1);
        }
    }
}

// Player-join style notification: Fog #10195 gives the player token; when non-zero it is embedded in an
// 8-byte message whose tag becomes 0x11, type byte the ESI unit's type (6 when ESI is null), dword the
// unit's +0xC (0xFFFFFFFF when null) and word Fog #10848's value. Sent only when that value is in
// [0, sgptDataTables+0xBC0].
// 1.13c D2Game+0x7A570
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_7A570(uint32_t esiUnit, const D2GameBatch1EF50Env* env)
{
    uint8_t buf[8] = {};
    wr16(buf, 4, 0xA57E);
    wr16(buf, 6, 0x6FC9);

    int32_t iVar1 = env->ordinal10195();
    if (iVar1 == 0) return;

    wr16(buf, 4, 0xB2);
    wr16(buf, 6, 0);
    wr8(buf, 0, (uint8_t)iVar1);
    wr8(buf, 1, (uint8_t)((uint32_t)iVar1 >> 8));
    wr16(buf, 2, (uint16_t)((uint32_t)iVar1 >> 16));

    iVar1 = env->ordinal10848();
    if (iVar1 < 0 || iVar1 > (int32_t)env->dataTablesBC0) return;

    uint32_t uVar2;
    uint8_t type;
    if (esiUnit == 0) {
        uVar2 = 0xFFFFFFFF;
        type = 6;
    }
    else {
        uVar2 = rd32(esiUnit, 0xC);
        type = rd8(esiUnit, 0);
    }
    wr16(buf, 6, (uint16_t)iVar1);
    wr8(buf, 0, 0x11);
    wr8(buf, 1, type);
    wr16(buf, 2, (uint16_t)uVar2);
    wr16(buf, 4, (uint16_t)(uVar2 >> 16));

    env->sub8A3E0(buf, 8);
}

// 11-byte 0x47 message: tag 0x47, type byte the EAX unit's first dword (6 when null), the unit's +0xC
// dword (0xFFFFFFFF when null), then three zero bytes.
// 1.13c D2Game+0x8aec0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8AEC0(uint32_t eaxUnit, const D2GameBatch1EF50Env* env)
{
    uint8_t buf[0xB] = {};
    wr8(buf, 0, 0x47);
    wr8(buf, 1, 6);
    wr32(buf, 3, 0);
    wr8(buf, 7, 0);
    wr16(buf, 8, 0);
    wr8(buf, 10, 0);

    if (eaxUnit == 0) {
        wr32(buf, 3, 0xFFFFFFFF);
    }
    else {
        wr8(buf, 1, rd8(eaxUnit, 0));
        wr32(buf, 3, rd32(eaxUnit, 0xC));
    }
    env->sub8A3E0(buf, 0xB);
}

// Callback dispatch: EBX+0x10 indexes 0xC-byte rows of the 0x6FD22CE0 table; a non-null first dword is
// called as (param_2, index) after an IsBadCodePtr gate whose failure logs the 0x148 allocation and
// exits.
// 1.13c D2Game+0xcb8f0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_CB8F0(uint32_t param1, uint32_t param2, uint32_t ebx,
                                             const D2GameBatch1EF50Env* env)
{
    (void)param1;
    const uint32_t idx = ebx == 0 ? 0 : rd32(ebx, 0x10);
    const uint32_t lpfn = rd32(env->table22CE0, idx * 3 * 4);
    if (lpfn != 0) {
        if (env->isBadCodePtr(lpfn) != 0) batch1EF50Fatal(env, 0x148);
        ((void (*)(uint32_t, uint32_t))(uintptr_t)lpfn)(param2, idx);
    }
}

// 11-byte 0x15 message for the ESI unit: tag, type byte, +0xC dword, then the two coordinate words -- the
// unit's +0x14 record +0xC (Fog #10867) at +6 and its +0x10 (Fog #10750) at +8 -- followed by param_2.
// Units of type 2 or 4/5 read the words straight out of that record instead.
// 1.13c D2Game+0xcbb10
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_CBB10(uint32_t param1, uint8_t param2, uint32_t esiUnit,
                                             const D2GameBatch1EF50Env* env)
{
    (void)param1;
    const uint32_t type = rd32(esiUnit, 0);
    const uint32_t rec = rd32(esiUnit, 0x2C);

    uint16_t wordB;                                 // decompile uStack_4: record +0x10
    if (type == 2 || (type > 3 && type < 6)) wordB = (uint16_t)rd32(rec, 0x10);
    else if (rec == 0) wordB = 0;
    else wordB = (uint16_t)env->ordinal10750(rec);

    uint16_t wordD;                                 // decompile uStack_6: record +0xC
    if (type == 2 || (type > 3 && type < 6)) wordD = (uint16_t)rd32(rec, 0xC);
    else if (rec == 0) wordD = 0;
    else wordD = (uint16_t)env->ordinal10867(rec);

    uint8_t buf[0xB] = {};
    wr8(buf, 0, 0x15);
    wr8(buf, 1, rd8(esiUnit, 0));
    wr32(buf, 2, rd32(esiUnit, 0xC));
    wr16(buf, 6, wordD);
    wr16(buf, 8, wordB);
    wr8(buf, 10, param2);

    env->sub8A3E0(buf, 0xB);
}

// 13-byte message for the EAX unit: tag 0xD, zero byte, +0xC dword, 0x13, the coordinate words (+0xC
// through Fog #10867 at +7, +0x10 through Fog #10750 at +9), the unit's byte at +0xB0 and Fog #11027.
// 1.13c D2Game+0xCBBB0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_CBBB0(uint32_t eaxUnit, const D2GameBatch1EF50Env* env)
{
    const uint32_t iVar2 = rd32(eaxUnit, 0xB0);     // in_EAX[0x2C]
    const uint8_t uVar3 = (uint8_t)env->ordinal11027();
    const uint32_t type = rd32(eaxUnit, 0);
    const uint32_t rec = rd32(eaxUnit, 0x2C);

    uint16_t wordB;                                 // uStack_b: record +0x10
    if (type == 2 || (type > 3 && type < 6)) wordB = (uint16_t)rd32(rec, 0x10);
    else if (rec == 0) wordB = 0;
    else wordB = (uint16_t)env->ordinal10750(rec);

    uint16_t wordD;                                 // uStack_d: record +0xC
    if (type == 2 || (type > 3 && type < 6)) wordD = (uint16_t)rd32(rec, 0xC);
    else if (rec == 0) wordD = 0;
    else wordD = (uint16_t)env->ordinal10867(rec);

    uint8_t buf[0xD] = {};
    wr8(buf, 0, 0xD);
    wr8(buf, 1, 0);
    wr32(buf, 2, rd32(eaxUnit, 0xC));
    wr8(buf, 6, 0x13);
    wr16(buf, 7, wordD);
    wr16(buf, 9, wordB);
    wr8(buf, 11, (uint8_t)iVar2);
    wr8(buf, 12, uVar3);

    env->sub8A3E0(buf, 0xD);
}

// Walking the EAX+0x74 list (next link +0xE8), for every type-1 node run two passes over its +0xDC list
// (next link +0x24): first reporting each byte-tag-2 entry and then each byte-tag-3 entry to 0x8E6F0.
// 1.13c D2Game+0x9D8E0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9D8E0(uint32_t eax, const D2GameBatch1EF50Env* env)
{
    for (uint32_t node = rd32(eax, 0x74); node != 0; node = rd32(node, 0xE8)) {
        if (rd32(node, 0) == 1) {
            for (uint32_t it = rd32(node, 0xDC); it != 0; it = rd32(it, 0x24)) {
                if (rd8(it, 0) == 2) env->sub8E6F0();
            }
            for (uint32_t it = rd32(node, 0xDC); it != 0; it = rd32(it, 0x24)) {
                if (rd8(it, 0) == 3) env->sub8E6F0();
            }
        }
    }
}

// Pet-list teardown: from the head at param_1, walk each node's +0x14 link paying 0x977E0(EBX, node+4)
// and one Fog #10046 (PlayerPets.cpp, line 0x4A5, 0) per node, then clear both head words.
// 1.13c D2Game+0x964b0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_964B0(uint32_t param1, uint32_t ebx,
                                             const D2GameBatch1EF50Env* env)
{
    static const char kFile[] = "..\\Source\\D2Game\\PLAYER\\PlayerPets.cpp";

    uint32_t iVar2 = rd32(param1, 0);
    while (iVar2 != 0) {
        const uint32_t value = rd32(iVar2, 4);
        iVar2 = rd32(iVar2, 0x14);
        env->sub977E0(ebx, value);
        env->ordinal10046(kFile, 0x4A5, 0);
    }
    wr32(param1, 0, 0);
    wr32(param1, 4, 0);
}

// Range pass over the EAX list: skip flagged nodes (+0 byte bit 0), hash the node's +4 id into
// param_1+0x1320+(id&0x7F)*4, then walk that chain (next +0xE4) for a +0xC match. Both the chain node and
// param_2 supply their coordinate pairs (record +0xC/+0x10, Fog #10867/#10750 or direct for types 2/4/5);
// Fog #10769 of ((param2 +0xC),(param2 +0x10),(node +0xC),(node +0x10)) above param_3 calls 0x97B50.
// 1.13c D2Game+0x96c00
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_96C00(uint32_t param1, uint32_t param2, int32_t param3,
                                             uint32_t eaxList, const D2GameBatch1EF50Env* env)
{
    uint32_t pbVar1 = rd32(eaxList, 0);
    for (;;) {
        uint32_t pbVar3;
        do {
            pbVar3 = pbVar1;
            if (pbVar3 == 0) return;
            pbVar1 = rd32(pbVar3, 0x14);
        } while ((rd8(pbVar3, 0) & 1) != 0);

        if (param1 == 0) batch1EF50Fatal(env, 0x2D3);

        const uint32_t id = rd32(pbVar3, 4);
        for (uint32_t piVar2 = rd32(param1 + 0x1320 + (id & 0x7F) * 4, 0);
             piVar2 != 0; piVar2 = rd32(piVar2, 0xE4))
        {
            if (rd32(piVar2, 0xC) == id) {
                auto coord10 = [env](uint32_t rec) -> uint32_t {
                    const uint32_t type = rd32(rec, 0);
                    const uint32_t sub = rd32(rec, 0x2C);
                    if (type == 2 || (type > 3 && type < 6)) return rd32(sub, 0x10);
                    if (sub == 0) return 0;
                    return env->ordinal10750(sub);
                };
                auto coord0C = [env](uint32_t rec) -> uint32_t {
                    const uint32_t type = rd32(rec, 0);
                    const uint32_t sub = rd32(rec, 0x2C);
                    if (type == 2 || (type > 3 && type < 6)) return rd32(sub, 0xC);
                    if (sub == 0) return 0;
                    return env->ordinal10867(sub);
                };
                const uint32_t uVar4 = coord10(piVar2);
                const uint32_t uVar5 = coord0C(piVar2);
                const uint32_t uVar6 = coord10(param2);
                const uint32_t uVar7 = coord0C(param2);
                const int32_t iVar8 = env->ordinal10769(uVar7, uVar6, uVar5, uVar4);
                if (param3 < iVar8) env->sub97B50(param1, param2, id);
                break;
            }
        }
    }
}

// Single pass over the EAX list: for each unflagged node, hash the +4 id into EBX+0x1320 and walk the
// chain (next +0xE4) for a +0xC match, paying 0x971F0(EBX) per hit.
// 1.13c D2Game+0x972c0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_972C0(uint32_t eaxList, uint32_t ebx,
                                             const D2GameBatch1EF50Env* env)
{
    uint32_t pbVar1 = rd32(eaxList, 0);
    for (;;) {
        if (pbVar1 == 0) return;
        if ((rd8(pbVar1, 0) & 1) == 0) {
            if (ebx == 0) batch1EF50Fatal(env, 0x2D3);
            const uint32_t id = rd32(pbVar1, 4);
            for (uint32_t iVar2 = rd32(ebx + 0x1320 + (id & 0x7F) * 4, 0);
                 iVar2 != 0; iVar2 = rd32(iVar2, 0xE4))
            {
                if (rd32(iVar2, 0xC) == id) {
                    env->sub971F0(ebx);
                    break;
                }
            }
        }
        pbVar1 = rd32(pbVar1, 0x14);
    }
}

// 7-byte 0x9B message; the decompile initialises only byte 0 and hands the stack block to 0x8A3E0, so
// bytes 1..6 are indeterminate there and are zero here.
// 1.13c D2Game+0x8a690
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8A690(const D2GameBatch1EF50Env* env)
{
    uint8_t buf[7] = {};
    buf[0] = 0x9B;
    env->sub8A3E0(buf, 7);
}
