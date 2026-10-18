// src/core/d2game/d2game_batch_ed540.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Stage A batch lane:
// implementation + marker only, no spec, no vector, no test runner. Base 0x6FC20000.
//
// Register-only arguments Ghidra leaves as in_EAX / unaff_ESI / unaff_EBX / unaff_EDI / unaff_EBP /
// unaff_retaddr are explicit parameters here, named after the decompile. Unmapped callees are seams
// on D2GameBatchED540Env in retail argument order; a callee reached with and without a visible
// argument list gets a second `_v` name for the no-visible-argument form. Retail module data blocks
// (the sgptDataTables slots at +0xBC4/+0xBC8 and the 0x6FD1A01C / 0x6FD1A048 delta tables) arrive as
// host pointers. The fatal tail is the same Ordinal_10265 / Ordinal_10024 / _exit(-1) assert as in
// d2game_batch_10160.cpp.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
// Retail record addresses arrive as 32-bit integers in the decompile; these take them as such.
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8 ((const void*)(uintptr_t)a, o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void*    rdptr(uint32_t a, uint32_t o) { return rdptr((const void*)(uintptr_t)a, o); }
static inline void     wr8 (uint32_t a, uint32_t o, uint8_t v)  { wr8 ((void*)(uintptr_t)a, o, v); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

// Every callee of the twelve that src/ does not implement, at its retail address.
struct D2GameBatchED540Env {
    // ---- 0xED540 ------------------------------------------------------------------------------
    uint32_t (*ordinal10265)(uint32_t id);
    void     (*ordinal10024)(const char* module, uint32_t id);   // tail is _exit(-1)
    void*    (*ordinal11153)(uint32_t a, uint32_t b);
    uint32_t (*ordinal10826)(uint32_t v);
    uint8_t  (*ordinal10459)(uint32_t v);
    void     (*sub38620)(int32_t v);                   // 0x38620
    void     (*subEFED0)();                            // 0xEFED0
    void     (*subEBA40)(void* v);                     // 0xEBA40
    void     (*sub7B0A0)(void* edi, void* v);          // 0x7B0A0
    void     (*subEB2C0)(void* v);                     // 0xEB2C0
    void     (*sub385C0)(void* v, void* edi);          // 0x385C0
    int32_t  (*ordinal10920)(void* unit);              // the visible-argument form
    int32_t  (*ordinal10630)();
    void     (*subA2100)(void* unit, uint32_t a, void* p, uint32_t d);  // 0xA2100
    void     (*sub35090)(void* edi, void* v, int32_t one);               // 0x35090
    const uint8_t* dataTables;                         // *sgptDataTables: rows at +0xBC4, count +0xBC8
    // ---- 0x70B70 ------------------------------------------------------------------------------
    int32_t  (*ordinal10920_v)();                      // the no-visible-argument form
    void     (*sub8A3E0)(void* buf, uint32_t len);     // 0x8A3E0
    // ---- 0xD22C0 ------------------------------------------------------------------------------
    void*    (*ordinal10858_v)();
    void*    (*ordinal11034)(int32_t a, uint32_t size);
    int32_t  (*subD1A30)(void* esi);                   // 0xD1A30
    // ---- 0xD2310 ------------------------------------------------------------------------------
    void     (*ordinal10180_v)();
    void     (*ordinal10289_v)();
    void*    (*ordinal10708)(void* esi);
    int32_t  (*ordinal10787_v)();
    // ---- 0xDA9E0 ------------------------------------------------------------------------------
    int32_t  (*ordinal10989)(uint32_t v);
    // ---- 0xDAA40 ------------------------------------------------------------------------------
    int32_t  (*sub14D0)(uint32_t a, int32_t b);        // 0x14D0
    const int32_t* table1A048;                         // 0x6FD1A048
    const int32_t* table1A01C;                         // 0x6FD1A01C
    // ---- 0xDB6B0 ------------------------------------------------------------------------------
    void*    (*subE03A0)(uint32_t a, uint32_t b);      // 0xE03A0
    void*    (*sub96570_v)();                          // 0x96570
    void*    (*ordinal11156)(uint32_t a, uint32_t b, int32_t c);
    int32_t  (*ordinal10448)(int32_t a, uint32_t b);
    // ---- 0x9C070 ------------------------------------------------------------------------------
    void*    (*ordinal10331_v)();
    void*    (*ordinal10331)(void* v);                 // the visible-argument form
    int16_t  (*sub9BEB0_v)();                          // 0x9BEB0
    void*    (*sub9BD00)(int16_t idx);                 // 0x9BD00
    // ---- 0xF4FE8 ------------------------------------------------------------------------------
    void     (*fdivpSTiST)();                          // 0xF4EF6 __fdivp_sti_st
    // ---- 0xF01D0 ------------------------------------------------------------------------------
    int32_t  (*ordinal10920r_v)();                     // no-visible-argument form returning the record
    // ---- 0x78B60 ------------------------------------------------------------------------------
    uint32_t (*ordinal10829)(uint32_t v);
    uint8_t  (*ordinal10020)(void* v);
    void*    (*ordinal10391)(int32_t a, void* b);
    void*    (*subD2610)(uint32_t unit, uint32_t* p, uint32_t zero);  // 0xD2610
    int32_t  (*ordinal11139)(int32_t a, uint32_t b);
    void     (*subD57F0)(void* ctx, uint32_t v);       // 0xD57F0
    void     (*ordinal11055)(uint32_t unit, uint32_t zero);
};

// 0xED540: NULL-check the caller record, mark bit 0 of its +0x3D4, then (for record byte +8 < 7)
// allocate and initialise a unit from the owner's +0x1C, stamp it with a fresh id, link it and run the
// two global fixups. Afterwards the record's owner slot is replaced by the new unit, and when the
// caller's +0x1AC byte was zero the unit is given the owner's monster-row +0xAC weapon (via 0xA2100).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_ED540(void* param_1, void* edi, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xED540
    static const char kModule[] = "D2Game.dll";        // 0x6FD19FF9
    if (param_1 == nullptr) {
        const uint32_t id = env->ordinal10265(0x206);
        env->ordinal10024(kModule, id);
        _exit(-1);
    }
    const uint8_t bVar2 = rd8(param_1, 8);
    wr32(param_1, 0x3D4, rd32(param_1, 0x3D4) | 1u);
    void* piVar8 = nullptr;
    if (bVar2 < 7) {
        piVar8 = env->ordinal11153(rd32(edi, 0x1C), 0);
        if (piVar8 != nullptr) {
            wr32(piVar8, 0x00, 0);
            wr32(piVar8, 0x04, bVar2);
            wr32(piVar8, 0x08, rd32(edi, 0x1C));
            wr32(piVar8, 0x80, (uint32_t)(uintptr_t)edi);
            uint32_t uVar4 = (rd32(edi, 0x70) == 0) ? (rd32(piVar8, 0xC8) & 0xFDFFFFFFu)
                                                    : (rd32(piVar8, 0xC8) | 0x02000000u);
            wr32(piVar8, 0xC8, uVar4);
            wr32(piVar8, 0xC8, uVar4 | 0x04000000u);
            const uint32_t uVar3 = env->ordinal10826(0);
            const uint8_t b = env->ordinal10459(uVar3);
            wr8(piVar8, 0x18, b);
            wr32(piVar8, 0x1C, rd32(edi, 0xBC + (uint32_t)b * 4));
            wr32(piVar8, 0x74, 0);
            wr32(piVar8, 0xC4, rd32(piVar8, 0xC4) | 0x10u);
            wr32(piVar8, 0xD0, 0x0B);
            int32_t iVar5 = (int32_t)rd32(edi, 0x90) + 1;
            if (iVar5 == -1) iVar5 = 1;
            wr32(edi, 0x90, (uint32_t)iVar5);
            env->sub38620(iVar5);
            env->subEFED0();
        }
    }
    env->subEBA40(piVar8);
    env->sub7B0A0(edi, piVar8);
    const int8_t cVar1 = (int8_t)rd8(param_1, 0x1AC);
    env->subEB2C0(piVar8);
    env->sub385C0(piVar8, edi);
    void* iVar5p = (void*)(intptr_t)env->ordinal10920(piVar8);
    wr32(iVar5p, 0x74, 0);
    wr32(iVar5p, 0x70, 0);
    if (cVar1 == 0 && piVar8 != nullptr) {
        int32_t iVar6 = (int32_t)rd32(piVar8, 4);
        if (iVar6 >= 0 && iVar6 < (int32_t)rd32(env->dataTables, 0xBC8)) {
            iVar6 = iVar6 * 0xC4 + (int32_t)rd32(env->dataTables, 0xBC4);
            const int16_t w = (int16_t)rd16((int32_t)iVar6, 0xAC);
            if (iVar6 != 0 && w != 0 && env->ordinal10630() != 0) {
                env->subA2100(piVar8, 0, (void*)(intptr_t)(int32_t)w, 0xFFFFFFFFu);
                wr32(iVar5p, 0x70, (uint32_t)(int32_t)w);
            }
        }
    }
    env->sub35090(edi, piVar8, 1);
    wr32(param_1, 0, (uint32_t)(uintptr_t)piVar8);
    return 0;
}

// 0x70B70: two identical sends of the 0x0677 message (low word only) to the two units in EAX/ESI,
// then put both units into mode 3 at +0x50.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_70B70(void* self, uint32_t param_1, uint32_t param_2, void* eax, void* esi, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0x70B70
    uint32_t local_4 = (uint32_t)(uintptr_t)self;
    if (eax != nullptr && rd32(eax, 0) == 0) env->ordinal10920_v();
    local_4 = (local_4 & 0xFFFF0000u) | 0x0677u;
    env->sub8A3E0(&local_4, 2);
    if (esi != nullptr && rd32(esi, 0) == 0) env->ordinal10920_v();
    local_4 = (local_4 & 0xFFFF0000u) | 0x0677u;
    env->sub8A3E0(&local_4, 2);
    wr32(param_1, 0x50, 3);
    wr32(param_2, 0x50, 3);
}

// 0xD22C0: true when the EDI context is set, its #10858 record has a positive +0x18 and a +0x14 byte
// of 2, a size-0x30 #11034 block exists, and 0xD1A30 on ESI also comes back non-zero.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_fn_d2game_D22C0(void* edi, void* esi, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xD22C0
    if (edi == nullptr) return false;
    void* iVar1 = env->ordinal10858_v();
    if (iVar1 != nullptr && (int16_t)rd16(iVar1, 0x18) > 0) {
        void* iVar2 = env->ordinal11034((int32_t)(int16_t)rd16(iVar1, 0x18), 0x30);
        if (iVar2 != nullptr && (int8_t)rd8(iVar1, 0x14) == 2) return env->subD1A30(esi) != 0;
    }
    return false;
}

// 0xD2310: the EBX-gated pair of notify calls, then 0xA2100 on ESI unless the context record #10708
// returned is ESI itself or the ESI unit is in mode 2 / 7.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_D2310(void* esi, void* edi, uint32_t ebx, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xD2310
    if (esi == nullptr || edi == nullptr) return;
    if (ebx == 0) env->ordinal10180_v();
    else          env->ordinal10289_v();
    void* piVar1 = env->ordinal10708(esi);
    if (piVar1 != nullptr && piVar1 != edi) {
        void* piVar3 = esi;
        const int32_t iVar2 = env->ordinal10787_v();
        if (iVar2 != 2 && iVar2 != 7) env->subA2100(esi, ebx, piVar1, (uint32_t)(uintptr_t)piVar3);
    }
}

// 0xDA9E0: scale EAX by the #10989 value of param_1 (an integer percentage), saturating to the
// 0x7FFFFFFF ceiling when the product would overflow; a #10989(0) outside (0,0x20) leaves EAX alone.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_DA9E0(uint32_t param_1, int32_t eax, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xDA9E0
    if (eax > 0) {
        const int32_t iVar1 = env->ordinal10989(param_1);
        const int32_t iVar2 = env->ordinal10989(0);
        if (iVar2 > 0 && iVar2 < 0x20) {
            const uint32_t bVar3 = (uint8_t)iVar2;
            const int32_t shift = (int32_t)(((int8_t)(iVar1 >> (bVar3 & 0x1F)) + (int32_t)bVar3) & 0x1F);
            if ((int32_t)(0x7FFFFFFF >> shift) < eax) return (int32_t)((uint32_t)eax >> (bVar3 & 0x1F)) * iVar1;
            eax = (iVar1 * eax) >> (bVar3 & 0x1F);
        }
    }
    return eax;
}

// 0xDAA40: look the unsigned delta between EAX and param_1 up in one of the two 11-entry delta tables
// (param_1 above EAX uses 0x6FD1A01C, below uses 0x6FD1A048) and return param_2 when the entry is
// 0x100, else re-enter 0x14D0 at 0x100; the out-of-range path calls 0x14D0 on param_1 directly.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_DAA40(uint32_t param_1, int32_t param_2, int32_t eax, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xDAA40
    int32_t iVar1;
    if (eax < (int32_t)param_1) {
        if (eax > 0x18 && (int32_t)param_1 > 0) return env->sub14D0(param_1, param_2);
        iVar1 = (int32_t)param_1 - eax;
        if (iVar1 > 10) iVar1 = 10;
        iVar1 = env->table1A048[iVar1];
    } else {
        iVar1 = eax - (int32_t)param_1;
        if (iVar1 > 10) iVar1 = 10;
        iVar1 = env->table1A01C[iVar1];
    }
    if (iVar1 == 0x100) return param_2;
    return env->sub14D0(0x100, param_2);
}

// 0xDB6B0: for an EAX context record whose +0 is 1, walk +0x14 -> +0x28 -> +0x28/+0x2C through
// 0xE03A0 and 0x96570, resolve the unit with #11156, and return the 0x40-scaled #10448 delta between
// param_2+1 and param_2, clamped to param_3. A +0 context returns param_3; everything else returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_DB6B0(int32_t param_1, int32_t param_2, uint32_t param_3, void* eax, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xDB6B0
    if (eax == nullptr) return 0;
    if (rd32(eax, 0) == 0) return param_3;
    if (rd32(eax, 0) != 1) return 0;
    const uint32_t eax5 = rd32(eax, 0x14);
    if (eax5 == 0) return 0;
    const int32_t iVar2 = (int32_t)rd32(eax5, 0x28);
    if (iVar2 == 0) return 0;
    if (rd32((int32_t)iVar2, 0x28) == 0) return 0;
    void* piVar1 = env->subE03A0(rd32((int32_t)iVar2, 0x28), rd32((int32_t)iVar2, 0x2C));
    if (piVar1 == nullptr || rd32(piVar1, 0) != 0) return 0;
    void* p = env->sub96570_v();
    if (p == nullptr) return 0;
    void* r = env->ordinal11156(rd32((int32_t)param_1, 0x70), rd32(p, 8), param_2);
    if (r == nullptr) return 0;
    const int32_t iVar3 = env->ordinal10448(param_2, rd32(r, 0x20));
    const int32_t iVar2b = env->ordinal10448(param_2 + 1, rd32(r, 0x20));
    const uint32_t uVar4 = (uint32_t)(iVar2b - iVar3) >> 6;
    return (param_3 <= uVar4) ? param_3 : uVar4;
}

// 0x9C070: walk the units of one context (EBX) matching the #10331 -> #10826 identity of the EAX
// record and call the EBP callback for each match; a failed prefix calls the return-address callback.
// The 0x1120 per-unit buckets are linked through +0xE4 and keyed by +0xC.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9C070(uint32_t param_1, uint32_t param_2, void* eax, uint32_t ebx, void (*ebp_cb)(uint32_t), void (*ret_cb)(uint32_t), const D2GameBatchED540Env* env) {  // 1.13c D2Game+0x9C070
    static const char kModule[] = "D2Game.dll";
    (void)param_1;
    int32_t iVar2 = (int32_t)(intptr_t)env->ordinal10331_v();
    if (iVar2 == 0) { ret_cb(param_2); return; }
    iVar2 = (int32_t)env->ordinal10826((uint32_t)iVar2);
    if (eax == nullptr || rd32(eax, 0x80) == 0 || rd32(eax, 0) != 0) { ebp_cb(param_2); return; }
    const int16_t sVar1 = env->sub9BEB0_v();
    if (sVar1 == -1) { ebp_cb(param_2); return; }
    void* psVar3 = env->sub9BD00(sVar1);
    if (psVar3 == nullptr) return;
    const uint32_t* puVar6 = (const uint32_t*)rdptr(psVar3, 4);
    if (puVar6 == nullptr) return;
    for (;;) {
        if (ebx == 0) {
            const uint32_t id = env->ordinal10265(0x2D3);
            env->ordinal10024(kModule, id);
            _exit(-1);
        }
        int32_t iVar4 = (int32_t)rd32(ebx, 0x1120 + (rd32(puVar6, 0) & 0x7F) * 4);
        for (; iVar4 != 0; iVar4 = (int32_t)rd32((int32_t)iVar4, 0xE4)) {
            if (rd32((int32_t)iVar4, 0xC) == rd32(puVar6, 0)) {
                const int32_t t = (int32_t)(intptr_t)env->ordinal10331((void*)(intptr_t)iVar4);
                if (t != 0 && (int32_t)env->ordinal10826((uint32_t)t) == iVar2) ebp_cb(param_2);
                break;
            }
        }
        puVar6 = (const uint32_t*)rdptr(puVar6, 4);
        if (puVar6 == nullptr) return;
    }
}

// 0xF4FE8 __adj_fdiv_m32i: the CRT x87 adjust tail. Both status-word arms run 0xF4EF6
// __fdivp_sti_st and return EAX unchanged; the status word only selects the assert path.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_F4FE8(uint32_t eax, uint16_t fpuStatusWord, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xF4FE8
    if ((fpuStatusWord & 0x3800) != 0) { env->fdivpSTiST(); return eax; }
    env->fdivpSTiST();
    return eax;
}

// 0xF5724 __ftoul2: the CRT long-double -> unsigned 64-bit conversion with the high-word carry
// adjustment. Ghidra's ROUND is the x87 fistp (nearest-even, the default control word).
extern "C" EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_d2game_F5724(long double st0) {  // 1.13c D2Game+0xF5724
    uint64_t uVar1 = (uint64_t)llrintl(st0);
    const uint32_t local_20 = (uint32_t)uVar1;
    const uint32_t uStack_1c = (uint32_t)(uVar1 >> 32);
    float fVar3 = (float)st0;
    if (local_20 != 0 || (fVar3 = (float)uStack_1c, (uVar1 & 0x7FFFFFFF00000000ull) != 0)) {
        if ((int32_t)fVar3 < 0) {
            uVar1 = uVar1 + (0x80000000u < (uint32_t)(-(float)(st0 - (long double)(int64_t)uVar1)) ? 1u : 0u);
        } else {
            const uint32_t uVar2 = (uint32_t)(0x80000000u < (uint32_t)(float)(st0 - (long double)(int64_t)uVar1));
            const uint32_t hi = (uint32_t)(int32_t)uStack_1c - (uint32_t)(local_20 < uVar2);
            const uint32_t lo = local_20 - uVar2;
            uVar1 = ((uint64_t)hi << 32) | lo;
        }
    }
    return uVar1;
}

// 0xF01D0: the context record's +0x38 linked list (+0x10 links) is searched for the ESI unit id
// (+0xC, -1 when ESI is NULL); returns 1 when EDI is ESI, else the matched node's +0xC low word, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_F01D0(void* esi, void* edi, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0xF01D0
    const int32_t iVar1 = env->ordinal10920r_v();
    const int32_t iVar4 = (esi == nullptr) ? -1 : (int32_t)rd32(esi, 0xC);
    if (edi == esi) return 1;
    const int32_t* piVar2 = (const int32_t*)rdptr((int32_t)iVar1, 0x38);
    for (; piVar2 != nullptr; piVar2 = (const int32_t*)rdptr(piVar2, 0x10)) {
        if (rd32(piVar2, 0) == (uint32_t)iVar4) break;
    }
    if (piVar2 == nullptr) return 0;
    return (uint32_t)(uint16_t)rd32(piVar2, 0xC);
}

// 0x78B60: resolve param_2 through #10829, find the +0xC match in the context's 0x1720 table, and if
// the unit is not already dead (#10020), #10391 accepts the +0x60 item and 0xD2610 validates, hand the
// item back (#11139) and run 0xD57F0. The result is then released with #11055. The decompile's
// `param_2 = in_EAX & 0xff` store is dead (param_2 is not read again), kept here as the unused EAX arg.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_78B60(uint32_t param_1, uint32_t param_2, uint32_t eax, uint32_t retaddr_ctx, const D2GameBatchED540Env* env) {  // 1.13c D2Game+0x78B60
    static const char kModule[] = "D2Game.dll";
    const uint32_t uVar2 = param_2;
    (void)(eax & 0xFFu);
    const uint32_t uVar4 = env->ordinal10829(uVar2);
    if (uVar4 == 0) return;
    if (retaddr_ctx == 0) {
        const uint32_t id = env->ordinal10265(0x2D3);
        env->ordinal10024(kModule, id);
        _exit(-1);
    }
    int32_t iVar6 = (int32_t)rd32(retaddr_ctx, 0x1720 + (uVar4 & 0x7F) * 4);
    for (; iVar6 != 0; iVar6 = (int32_t)rd32((int32_t)iVar6, 0xE4)) {
        if (rd32((int32_t)iVar6, 0xC) == uVar4) break;
    }
    const int32_t iVar1 = (int32_t)rd32(uVar2, 0x60);
    if (iVar6 != 0 && iVar1 != 0 && env->ordinal10020((void*)(intptr_t)iVar6) == 0) {
        void* r = env->ordinal10391(iVar1, (void*)(intptr_t)iVar6);
        if (r != nullptr) {
            uint32_t param_1_out = param_1;
            if (env->subD2610(uVar2, &param_1_out, 0) != nullptr) {
                if (env->ordinal11139(iVar1, param_1_out) != 0) {
                    const uint32_t id = env->ordinal10265(0x4E9);
                    env->ordinal10024(kModule, id);
                    _exit(-1);
                }
                env->subD57F0((void*)(uintptr_t)retaddr_ctx, uVar4);
            }
        }
    }
    env->ordinal11055(uVar2, 0);
}
