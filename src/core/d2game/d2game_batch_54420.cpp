// src/core/d2game/d2game_batch_54420.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// only arguments (in_EAX / unaff_EBX / unaff_ESI / unaff_EDI / unaff_retaddr) and stack locals the
// decompile could not tie to a parameter are explicit parameters and are named at each function.
// Unmapped callees are seams on D2GameBatch54420Env in retail argument order; a seam whose decompile
// shows no arguments is taken to have none. The two retail data tables the decompiles name
// (sgptDataTables and gdwBitMasks) arrive as host pointers.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EDI, in_EAX, ...).
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline int16_t  rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void     wr8 (uint32_t a, uint32_t o, uint8_t v) { wr8((void*)(uintptr_t)a, o, v); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

// Host seams for the callees and data tables the cached decompiles name but src/ does not implement.
struct D2GameBatch54420Env {
    // sgptDataTables: the DataTables block (count at +0xA80, records at +0xA78, +0xBA0, +0xB98, +0xC4).
    const uint8_t* dataTables;
    // gdwBitMasks: the mask bytes indexed at +0x14 and +0x1C.
    const uint8_t* bitMasks;

    // 0x54420 callees.
    int32_t  (*ordinal10417)(void* self);
    int32_t  (*ordinal11041)(int32_t owner, void* self, int32_t cls);
    int32_t  (*ordinal10405)(int32_t owner, void* self, uint32_t mode);
    void     (*ordinal10058)(void* self, uint32_t cls);
    void     (*ordinal11090)(void* self, uint32_t value);
    void     (*ordinal10012)(void* self, uint32_t value);
    void     (*ordinal10892)(void* self, uint32_t a, uint32_t b);
    void     (*ordinal10050)(int32_t owner, void* self);
    void     (*ordinal10201)(void* param2, uint32_t value);
    void     (*sub345A0)();
    int32_t  (*subD4A70)(uint32_t ebx, void* param2, uint32_t value, uint32_t* out);
    int32_t  (*subD76A0)(uint32_t ebx, void* param2, uint32_t value);
    void     (*ordinal10750)(uint32_t value);
    void     (*ordinal10867)(uint32_t value);

    // 0x771D0, 0x9F2B0 and 0xA1D10 data-table probe.
    int32_t  (*ordinal10494)();

    // 0xDB910 callees.
    int32_t  (*ordinal10494Two)(uint32_t param1, uint32_t value);
    int32_t  (*ordinal10913)(uint32_t param1);
    int32_t  (*sub1450)(uint32_t selector);
    int32_t  (*sub1560)(int32_t value, uint32_t mode);
    // The decompile's extraout_ECX: ECX observed after the second 0x1450 call.
    int32_t  (*lastEcx)();

    // 0x777F0 callee.
    void     (*subDEA00)(uint32_t param1, void* self, void* eaxIn);

    // 0x9F2B0 callees.
    int32_t  (*subE03A0)(uint32_t a, uint32_t b);
    void     (*sub9E330)(void* param1);

    // 0x47950 callees.
    int32_t  (*ordinal10826)();
    int32_t  (*ordinal10014)(int32_t value);
    void     (*sub47820)(void* esi, int32_t value);

    // 0x53260 callees.
    int32_t  (*ordinal11017)();
    int32_t  (*ordinal10460)();
    int32_t  (*ordinal11147)(int32_t record);
    int32_t  (*ordinal10464)(int32_t record);

    // 0xA1D10 callees.
    int32_t  (*ordinal10630)();
    int32_t  (*ordinal10306)();
    void     (*subA19A0)(void* ebx, int32_t param2, int32_t value, uint32_t a, uint32_t b, uint32_t c);
    void     (*sub9F3B0)(void* ebx, void* edi);
    void     (*ordinal10945)();
    int32_t  (*ordinal10871)();
    int32_t  (*ordinal11013)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*ordinal10785)(int32_t unit, int32_t value);
    void     (*ordinal10725)(int32_t unit, int32_t value);
    void     (*ordinal11081)(int32_t unit, int32_t value);
    void     (*ordinal10807)();
    void     (*ordinal10188)(int32_t unit, uint32_t stat, int32_t value, uint32_t zero);

    // 0x24BF0 callees.
    void     (*sub23260)();
    int32_t  (*getUModCount)(void* pc);
    void     (*ordinal10331)();
    void     (*sub248A0)(uint32_t ebx, uint32_t self);

    // 0x24C70 callees.
    void*    (*sub24950)(uint32_t param2, uint32_t ebx, uint32_t esi, void* param1,
                         uint32_t selfLo, uint32_t eaxLo, int32_t neg1);
    void     (*sub23CA0)(uint32_t esi, void* record, int32_t param3);

    // 0x10A20 callees.
    int32_t  (*ordinal10695)(uint32_t value);
    void     (*ordinal11050)(uint32_t unit, uint32_t value);
    void     (*ordinal10312)(uint32_t unit, uint32_t a, int32_t index);
    void     (*ordinal10525)(uint32_t unit, uint32_t a, int32_t index);
    void     (*ordinal10611)(uint32_t unit, uint32_t value);
    void     (*ordinal10883)(uint32_t unit, uint32_t value);
    uint32_t (*subEE90)(uint32_t unit);
    void     (*ordinal10875)(uint32_t unit, uint32_t value);
    int32_t  (*ordinal10710)(uint32_t unit);
    int32_t  (*ordinal10176)(uint32_t unit);
    int32_t  (*ordinal10516)(uint32_t unit);
    int32_t  (*ordinal11036)(uint32_t unit);
    int32_t  (*ordinal10106)(uint32_t unit);
    int32_t  (*ordinal10106NoArg)();
    int32_t  (*ordinal10119)(uint32_t unit);
    int32_t  (*ordinal10556)(uint32_t unit);
    void     (*ordinal10411)(uint32_t unit);
    void     (*ordinal10513)();
    int32_t  (*sub14AF0)();
    void     (*sub109B0)(void* param2);
    void     (*sub107B0)();
    void     (*ordinal10536)(uint32_t unit, uint32_t value);
    int16_t  (*ordinal10718)(uint32_t unit);
    int32_t  (*sub140B0)(uint32_t unit);
    int32_t  (*sub103C0)(void* param2, void* esi);
    int32_t  (*sub101E0)();
    int32_t  (*sub141E0)();
    int32_t  (*ordinal10865)(uint32_t unit);
    int32_t  (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t zero);
    int32_t  (*ordinal10587)(uint32_t unit, uint32_t stat, uint32_t zero);
    void     (*ordinal10887)(uint32_t unit, uint32_t stat, int32_t value, uint32_t zero);
    int32_t  (*sub104F0)();
    int32_t  (*sub154F0)();
    int32_t  (*subF370)(uint32_t a, uint32_t unit);
    int32_t  (*sub10730)(void* param2);
    int32_t  (*sub14BF0)();
    uint32_t (*sub14B30)();
    void     (*subEBB0)();
    void     (*subEC90)();
    uint32_t (*sub14B50)(uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*ordinal10248)(uint32_t value);
    void     (*ordinal10996)(uint32_t a, uint32_t b, uint32_t unit, uint32_t value,
                             uint32_t c, uint32_t d);
};

// Unit-class dispatch: a type-1 unit whose class probe succeeds is stopped (ordinal 10058, flag bits
// 0xFFFFFFFD and 0xFDFFFFFF cleared, 11090(1), 10012(0xFF), 10892(8,1), owner 10050, 10201(1), 0x345A0)
// and answers 0. A type-6 unit with a non-null EAX calls 0xD4A70 and answers 0 on success. Every other
// non-zero class, and the class-0 teardown, answer 0xD after 11090(4); the class-0 path conditionally
// pays 10750/10867 on self+0x2C for types other than 2 and 4..5, then 0xD76A0.
// 1.13c D2Game+0x54420
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_54420(void* self, uint32_t param1, void* param2,
                                                 uint32_t eaxIn, uint32_t ebx,
                                                 const D2GameBatch54420Env* env)
{
    (void)param1;
    const int32_t cls = self != nullptr ? (int32_t)rd32(self, 0x10) : 0;

    if (cls != 0) {
        if (cls == 1) {
            const int32_t owner = (int32_t)rd32(param2, 0x60);   // param_2[0x18]
            const int8_t cv = (int8_t)env->ordinal10417(self);
            if (cv != 0 && env->ordinal11041(owner, self, cv) != 0) {
                uint32_t mode = 3;
                if (cv == 0xb || cv == 0xc) mode = 4;
                if (env->ordinal10405(owner, self, mode) != 0) {
                    env->ordinal10058(self, (uint32_t)(int32_t)cv);
                    if (self != nullptr) wr32(self, 0xc4, rd32(self, 0xc4) & 0xfffffffd);
                    env->ordinal11090(self, 1);
                    env->ordinal10012(self, 0xff);
                    env->ordinal10892(self, 8, 1);
                    if (self != nullptr) wr32(self, 0xc4, rd32(self, 0xc4) & 0xfdffffff);
                    env->ordinal10050(owner, self);
                    env->ordinal10201(param2, 1);
                    env->sub345A0();
                    return 0;
                }
            }
        }
        else if (cls == 6 && eaxIn != 0) {
            env->ordinal11090(self, 4);
            uint32_t local = 0;                                  // stack0xfffffffc
            if (env->subD4A70(ebx, param2, rd32(eaxIn, 0xc), &local) != 0) return 0;
        }
        return 0xd;
    }

    env->ordinal11090(self, 4);
    int32_t type = (int32_t)rd32(self, 0);
    if (type != 2 && !(type >= 4 && type <= 5)) {
        const uint32_t sub = rd32(self, 0x2c);
        if (sub != 0) env->ordinal10750(sub);
    }
    type = (int32_t)rd32(self, 0);
    if (type != 2 && !(type >= 4 && type <= 5)) {
        const uint32_t sub = rd32(self, 0x2c);
        if (sub != 0) env->ordinal10867(sub);
    }
    if (env->subD76A0(ebx, param2, rd32(self, 0xc)) != 0) return 0;
    return 0xd;
}

// Flag-4 records whose target is a type-1 unit with a non-negative +0x04 index: when the index is in
// range the record's +0x02 short is reloaded, and a final 0x4E index whose ordinal 10494 probe answers 0
// sets bit 3 of the record's +0x04 byte.
// 1.13c D2Game+0x771D0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_771D0(uint32_t ebx, void* esi, const D2GameBatch54420Env* env)
{
    if ((rd8(ebx, 4) & 4) == 0 || esi == nullptr || rd32(esi, 0) != 1) return;
    int32_t i2 = (int32_t)rd32(esi, 4);
    if (i2 < 0) return;
    if (i2 < (int32_t)rd32(env->dataTables, 0xa80)) {
        const uint32_t rec = (uint32_t)i2 * 0x1a8 + rd32(env->dataTables, 0xa78);
        if (rec != 0) i2 = (int32_t)rd16(rec, 2);
    }
    if (i2 < 0 || i2 >= (int32_t)rd32(env->dataTables, 0xa80)) return;
    if (i2 == 0x4e && env->ordinal10494() == 0) wr8(ebx, 4, (uint8_t)(rd8(ebx, 4) | 8));
}

// Store param_1 into the +0x54 dword of a type-1 unit's +0x14 record.
// 1.13c D2Game+0xAF710
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_AF710(uint32_t param1, void* eaxIn,
                                             const D2GameBatch54420Env* env)
{
    (void)env;
    if (eaxIn == nullptr || rd32(eaxIn, 0) != 1) return;
    const uint32_t sub = rd32(eaxIn, 0x14);                    // in_EAX[5]
    if (sub != 0) wr32((void*)(uintptr_t)sub, 0x54, param1);
}

// Distance gate for a candidate target: the 0x1A68E probe and the +0x28/+0x4C self-reference must both
// fail, the +0x4C distance must reach 0x100, and the distance divided by a per-type divisor, half and
// quarter of it (or the 0x1450 fallback) must all be at most it; a type-1 ESI unit additionally needs
// 0x1560(limit, 3). Answer 1 when any gate fails, 0 when all pass.
// 1.13c D2Game+0xDB910
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_DB910(uint32_t param1, uint32_t param2, uint32_t retaddr,
                                                 void* esi, const D2GameBatch54420Env* env)
{
    if (env->ordinal10494Two(param1, 1) != 0) return 1;

    const uint32_t a = rd32(param2, 0x28);
    const int32_t limit = (int32_t)rd32(param2, 0x4c);
    if ((a != 0 && a == (uint32_t)limit) || limit < 0x100) return 1;

    const int32_t iVar1 = env->ordinal10913(param1);
    int32_t uVar4;
    switch (retaddr) {
    default:                 uVar4 = 0x10; break;
    case 2: case 6: case 10: case 0xb: uVar4 = 8; break;
    case 4: case 8:          uVar4 = 0x20; break;
    case 5:                  uVar4 = 0x40; break;
    }

    if (iVar1 / uVar4 > limit) return 1;
    if (!(iVar1 / (uVar4 / 2) <= limit) && env->sub1450(2) == 0) return 1;

    int32_t iVar3 = limit;
    bool ok = iVar1 / (uVar4 >> 2) <= iVar3;
    if (!ok) {
        const int32_t r = env->sub1450(4);
        iVar3 = env->lastEcx();
        ok = r != 0;
    }
    if (!ok) return 1;

    if (esi != nullptr && rd32(esi, 0) == 1 && env->sub1560(iVar3, 3) == 0) return 1;
    return 0;
}

// Call 0xDEA00(param_1, self, EAX) when self and EAX are non-null and EAX's +0xC4 bit 26 is clear.
// 1.13c D2Game+0x777F0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_777F0(void* self, uint32_t param1, void* eaxIn,
                                             const D2GameBatch54420Env* env)
{
    if (self != nullptr && eaxIn != nullptr && ((rd32(eaxIn, 0xc4) >> 0x1a) & 1) == 0) {
        env->subDEA00(param1, self, eaxIn);
    }
}

// Type-1 unit with a +0x14 record whose +0x28 record has a non-null +0x28: the 0xE03A0 probe must
// succeed, the unit's +0x04 index must select a data-table record whose signed +0xA8 short is positive
// and within DataTables+0xBA0; then 0x9E330(param_1) runs.
// 1.13c D2Game+0x9F2B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9F2B0(void* param1, void* eaxIn,
                                             const D2GameBatch54420Env* env)
{
    if (eaxIn == nullptr || rd32(eaxIn, 0) != 1) return;
    const uint32_t sub = rd32(eaxIn, 0x14);
    if (sub == 0) return;
    const uint32_t a = rd32(sub, 0x28);
    if (a == 0) return;
    const uint32_t b = rd32(a, 0x28);
    if (b == 0) return;
    if (env->subE03A0(b, rd32(a, 0x2c)) == 0) return;

    const int32_t i3 = (int32_t)rd32(eaxIn, 4);                // in_EAX[1]
    if (i3 < 0) return;
    if (i3 >= (int32_t)rd32(env->dataTables, 0xa80)) return;
    const uint32_t rec = (uint32_t)i3 * 0x1a8 + rd32(env->dataTables, 0xa78);
    if (rec == 0) return;
    const int16_t s = rd16(rec, 0xa8);
    if (s <= 0) return;
    if ((int32_t)s > (int32_t)rd32(env->dataTables, 0xba0)) return;
    env->sub9E330(param1);
}

// LCG tick on the +0x6C qword (*0x6AC690C5 + the +0x70 dword): the low 15 bits zero and a clear +0x78
// dword pay 0x10826 -> 0x10014, and a positive +0x2E byte of a unit other than -0x1C then calls
// 0x47820(esi, value).
// 1.13c D2Game+0x47950
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_47950(void* esi, const D2GameBatch54420Env* env)
{
    void* puVar4 = esi != nullptr ? (void*)((uint8_t*)esi + 0x6c) : nullptr;
    const uint64_t uVar1 = (uint64_t)rd32(puVar4, 0) * 0x6ac690c5ULL + (uint64_t)rd32(puVar4, 4);
    wr64(puVar4, 0, uVar1);
    if ((uVar1 & 0x7fff) == 0 && rd32(esi, 0x78) == 0) {
        const int32_t iVar2 = env->ordinal10826();
        const int32_t iVar3 = env->ordinal10014(iVar2);
        if (rd8((uint32_t)iVar3, 0x2e) != 0 && iVar3 != -0x1c) env->sub47820(esi, iVar2);
    }
}

// Live-unit census on a non-zero EAX: one when 0x11017 answers non-zero, plus one per 0x10460 list node
// whose 0x11147 probe is non-zero, walking with 0x10464.
// 1.13c D2Game+0x53260
extern "C"
EMSCRIPTEN_KEEPALIVE uint16_t d2_fn_d2game_53260(uint32_t eaxIn, const D2GameBatch54420Env* env)
{
    if (eaxIn == 0) return 0;
    uint16_t uVar3 = (uint16_t)(env->ordinal11017() != 0);
    int32_t iVar1 = env->ordinal10460();
    while (iVar1 != 0) {
        const int32_t iVar2 = env->ordinal11147(iVar1);
        iVar1 = env->ordinal10464(iVar1);
        if (iVar2 != 0) uVar3 = (uint16_t)(uVar3 + 1);
    }
    return uVar3;
}

// Level-spawn probe: a +0xB98 record selected by param_2 whose +0x04 byte carries the 0x14 mask bit,
// whose +0x80 short is non-negative and below DataTables+0xC4, and whose level passes the 0x10630 and
// 0x10306 probes. The 0x1C mask bit takes the 0xA19A0 + 0x9F3B0 tail; otherwise 0x10945/0x10871
// supply a unit (0x11013 from EDI when 0x10871 answers 0) that 0x10785/0x10725/0x11081/0x10807 and two
// 0x10188 stat writes are applied to, then 0x9F3B0(1, EDI).
// 1.13c D2Game+0xA1D10
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A1D10(uint32_t param1, int32_t param2, void* ebx, void* edi,
                                             const D2GameBatch54420Env* env)
{
    (void)param1;
    if (param2 < 0) return;
    if ((int32_t)rd32(env->dataTables, 0xba0) <= param2) return;
    const uint32_t rec = (uint32_t)param2 * 0x23c + rd32(env->dataTables, 0xb98);
    if (rec == 0) return;
    if ((rd8(env->bitMasks, 0x14) & rd8(rec, 4)) == 0) return;
    if (rd16(rec, 0x80) < 0) return;
    if ((int32_t)rd32(env->dataTables, 0xc4) <= (int32_t)rd16(rec, 0x80)) return;

    int32_t iVar1 = env->ordinal10630();
    if (iVar1 == 0) return;
    iVar1 = env->ordinal10306();
    if (iVar1 < 1) return;

    if ((rd8(env->bitMasks, 0x1c) & rd8(rec, 5)) != 0) {
        env->subA19A0(ebx, param2, iVar1, 1, 0, 0);
        env->sub9F3B0(ebx, edi);
        return;
    }

    const int32_t iVar2 = (int32_t)rd16(rec, 0x80);
    env->ordinal10945();
    int32_t iVar3 = env->ordinal10871();
    if (iVar3 == 0) {
        uint32_t uVar4, uVar5, uVar6;
        if (edi == nullptr) {
            uVar6 = 0xffffffff;
            uVar5 = 6;
            uVar4 = 0;
        }
        else {
            uVar6 = rd32(edi, 0xc);
            uVar5 = rd32(edi, 0);
            uVar4 = rd32(edi, 8);
        }
        iVar3 = env->ordinal11013(uVar4, 0, 0, uVar5, uVar6);
        if (iVar3 == 0) {
            env->sub9F3B0(ebx, edi);
            return;
        }
    }

    env->ordinal10785(iVar3, iVar2);
    env->ordinal10725(iVar3, iVar2);
    env->ordinal11081(iVar3, iVar1);
    env->ordinal10807();
    env->ordinal10188(iVar3, 0x15e, iVar2, 0);
    env->ordinal10188(iVar3, 0x15f, iVar1, 0);
    env->sub9F3B0((void*)(uintptr_t)1, edi);
}

// Append a unique modifier: a type-1 unit whose +0x14 record has bit 2 of +0x16 clear pays 0x23260,
// sets bits 0 and 2 of that byte, and while the 0x1C list holds fewer than nine entries stores param_1
// at the next one, pays 0x10331 and calls 0x248A0(0, self).
// 1.13c D2Game+0x24BF0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_24BF0(void* self, int32_t param1, void* eaxIn,
                                             const D2GameBatch54420Env* env)
{
    if (eaxIn == nullptr || rd32(eaxIn, 0) != 1) return;
    const uint32_t rec = rd32(eaxIn, 0x14);                    // in_EAX[5]
    if (rec == 0) return;
    if ((rd8(rec, 0x16) & 4) != 0) return;

    env->sub23260();
    if (rd32(eaxIn, 0) == 1 && rd32(eaxIn, 0x14) != 0) {
        const uint32_t sub = rd32(eaxIn, 0x14);
        wr8(sub, 0x16, (uint8_t)(rd8(sub, 0x16) | 5));
    }

    const uint32_t pc = rec + 0x1c;
    const int32_t n = env->getUModCount((void*)(uintptr_t)pc);
    if (n < 9) {
        wr8(pc, (uint32_t)n, (uint8_t)param1);
        env->ordinal10331();
        env->sub248A0(0, (uint32_t)(uintptr_t)self);
    }
}

// Create one record with 0x24950 and, on success, apply 0x23CA0(ESI, record, param_3) and
// 0x248A0(EBX, ESI) before returning it.
// 1.13c D2Game+0x24C70
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_24C70(void* self, void* param1, uint32_t param2,
                                              int32_t param3, uint32_t eaxIn, uint32_t ebx,
                                              uint32_t esi, const D2GameBatch54420Env* env)
{
    void* record = env->sub24950(param2, ebx, esi, param1,
                                 (uint32_t)(uintptr_t)self & 0xffff, eaxIn & 0xffff, -1);
    if (record == nullptr) return nullptr;
    env->sub23CA0(esi, record, param3);
    env->sub248A0(ebx, esi);
    return record;
}

// Unit mode/stat synchronisation: the type-4 unit's 0x1A68E probe must pass, then 11050(0), three
// rounds of 10312/10525, 10611(0), 10883(0), 10875(the ESI+0x30 override or 0xEE90), and the
// 10710/10176/10516, 11036/10106, 10119, 10556 classification writes. A switch on the 10106 mode runs
// the per-mode 0x14xxx/0x10xxx action, then the high-99 stat scaling (0xEBB0/0xEC90 doubling, the
// 0x14B50 roll and the 11050/10248/10996 tail). Answer 1 when the final 10106 is positive and its
// no-argument probe is below 10, else 0.
// 1.13c D2Game+0x10A20
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_10A20(uint32_t param1, void* param2, void* esiPtr,
                                                 const D2GameBatch54420Env* env)
{
    (void)param1;
    const uint32_t unit = rd32(param2, 0);                     // *param_2
    if (unit == 0 || rd32(unit, 0) != 4) return 0;
    if (env->ordinal10695(rd32(unit, 4)) == 0) return 0;

    env->ordinal11050(unit, 0);
    for (int32_t i = 0; i < 3; ++i) {
        env->ordinal10312(unit, 0, i);
        env->ordinal10525(unit, 0, i);
    }
    env->ordinal10611(unit, 0);
    env->ordinal10883(unit, 0);

    uint32_t uVar5 = env->subEE90(unit);
    if (rd32(esiPtr, 0x30) != 0) uVar5 = rd32(esiPtr, 0x30);
    env->ordinal10875(unit, uVar5);

    int32_t iVar4 = (int32_t)unit;
    int32_t iVar6 = env->ordinal10710(unit);
    if (iVar6 != 0) {
        iVar6 = env->ordinal10176(unit);
        if (iVar6 == 0) {
            iVar6 = env->ordinal10516(unit);
            if (iVar6 == 0) env->ordinal10875(unit, 4);
        }
        else {
            env->ordinal10875(unit, 7);
        }
    }

    iVar6 = env->ordinal11036(unit);
    if (iVar6 == 0 && env->ordinal10106(unit) == 6) env->ordinal10875(unit, 4);
    if (env->ordinal10119(unit) != 0) env->ordinal10875(unit, 7);
    if (env->ordinal10556(unit) != 0) env->ordinal10875(unit, 2);

    uint32_t uVar8 = (uint32_t)env->ordinal10106(unit);
    switch (uVar8) {
    case 1:
        env->ordinal10411(unit);
        env->ordinal10513();
        if (env->sub14AF0() == 0) env->sub109B0(param2);
        break;
    case 2:
        env->sub107B0();
        break;
    case 3:
        env->ordinal10536(unit, 0xffffffff);
        env->ordinal10411(unit);
        env->ordinal10513();
        env->ordinal10718(unit);
        iVar4 = env->sub140B0(unit);
        if (iVar4 == 0) env->sub109B0(param2);
        break;
    case 4:
        env->ordinal10411(unit);
        env->ordinal10513();
        iVar4 = env->sub103C0(param2, esiPtr);
        if (iVar4 == 0 && env->sub101E0() == 0) env->sub109B0(param2);
        break;
    case 5:
        env->ordinal10536(unit, 0xffffffff);
        env->ordinal10411(unit);
        env->ordinal10513();
        iVar4 = env->sub141E0();
        if (iVar4 == 0) {
            iVar4 = env->ordinal10865(unit);
            if (iVar4 != 0 && env->ordinal10718(unit) != 0) {
                iVar4 = env->ordinal10973(unit, 0x48, 0);
                iVar6 = env->ordinal10587(unit, 0x49, 0);
                iVar4 *= 2;
                if (iVar4 > 0xfe) iVar4 = 0xff;
                env->ordinal10887(unit, 0x48, iVar4, 0);
                iVar6 *= 2;
                if (iVar6 > 0xfe) iVar6 = 0xff;
                env->ordinal10887(unit, 0x49, iVar6, 0);
            }
            iVar4 = env->sub104F0();
            if (iVar4 == 0 && env->sub101E0() == 0) env->sub109B0(param2);
        }
        break;
    case 6:
        env->ordinal10611(unit, 0);
        env->ordinal10883(unit, 0);
        env->ordinal10411(unit);
        env->ordinal10513();
        iVar4 = env->sub154F0();
        if (iVar4 == 0 && env->sub104F0() == 0 && env->sub101E0() == 0) env->sub109B0(param2);
        break;
    case 7:
        env->ordinal10536(unit, 0xffffffff);
        env->ordinal10411(unit);
        env->ordinal10513();
        iVar4 = env->subF370((uint32_t)iVar4, unit);
        if (iVar4 == 0) {
            iVar4 = env->ordinal10865(unit);
            if (iVar4 != 0 && env->ordinal10718(unit) != 0) {
                iVar4 = env->ordinal10973(unit, 0x48, 0);
                iVar6 = env->ordinal10587(unit, 0x49, 0);
                iVar4 *= 3;
                if (iVar4 > 0xfe) iVar4 = 0xff;
                env->ordinal10887(unit, 0x48, iVar4, 0);
                iVar6 *= 3;
                if (iVar6 > 0xfe) iVar6 = 0xff;
                env->ordinal10887(unit, 0x49, iVar6, 0);
            }
            iVar4 = env->sub10730(param2);
            if (iVar4 == 0 && env->sub104F0() == 0 && env->sub101E0() == 0) env->sub109B0(param2);
        }
        break;
    case 8:
        env->ordinal10611(unit, 0);
        env->ordinal10883(unit, 0);
        env->ordinal10411(unit);
        env->ordinal10513();
        iVar4 = env->sub14BF0();
        if (iVar4 == 0) env->sub109B0(param2);
        break;
    case 9:
        env->ordinal10611(unit, 0);
        env->ordinal10883(unit, 0);
        env->ordinal10411(unit);
        env->ordinal10513();
        uVar8 = env->sub14B30();
        {
            const uint32_t uVar7 = env->sub14B30();
            if ((int16_t)uVar8 == 0 || (int16_t)uVar7 == 0) {
                env->sub109B0(param2);
                break;
            }
            env->ordinal10611(unit, uVar8);
            env->ordinal10883(unit, uVar7);
        }
        break;
    default:
        return 0;
    }

    iVar4 = env->ordinal10106(unit);
    if (iVar4 > 0) {
        iVar6 = env->ordinal10106NoArg();
        if (iVar6 < 10) {
            uint16_t uVar2 = (uint16_t)env->ordinal10718(unit);
            if (uVar2 > 99) env->subEBB0();
            iVar6 = env->ordinal10106(unit);
            if (iVar6 > 0 && iVar6 < 4) env->subEC90();
            uVar2 = (uint16_t)env->ordinal10718(unit);
            if (uVar2 > 99) {
                uVar8 = (uint32_t)env->ordinal10106(unit);
                switch (uVar8) {
                case 1: case 2: case 3: case 4: case 6: case 8: case 9:
                    if (rd16(unit, 0xf8) != 0) {
                        uVar5 = env->sub14B50(0, 1, 0);
                        if ((int16_t)uVar5 != 0) {
                            env->ordinal11050(unit, uVar5);
                            uVar8 = env->ordinal10248(uVar5 & 0xffff);
                            env->ordinal10996(0, 0, unit, uVar8, 0, 0);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            return 1;
        }
    }
    return 0;
}
