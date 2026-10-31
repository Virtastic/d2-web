// Batch D2Game+0x961A0..0xA2A40 (11 functions), transcribed from the cached 1.13c decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Same pattern as d2game_batch_6e400.cpp: retail records are
// read by byte offset; every callee (Ordinal_N and FUN_6fc..) is a seam on D2Game961Env in retail argument
// order; Ghidra register-only inputs (in_EAX / unaff_ESI / unaff_EBX / unaff_EDI) are explicit parameters.
// Not implemented here: 0x9DE60 and 0x9DB00 (loop bound is a stack-address compare in the decompile),
// 0xA7580 (AI think body with implicit EBP/EBX/stack state). See their specs.
#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef uintptr_t A;  // a retail record address
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint8_t  rd8 (A p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void wr32(A p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline A    rdA (A p, uint32_t o) { return (A)rd32(p, o); }

struct D2Game961Env {
    A dataTables;                  // *sgptDataTables_exref (the table base, already dereferenced)
    const char* assertFile;        // DAT_6FD19FF9
    // D2Common ordinals, retail argument order
    A    (*o10265)(A);             A  (*o10920)();
    void (*o10024)(const char*, A);
    A    (*o11139)(A, A);          A  (*o11006)(A);
    A    (*o10744)(A, A);          A  (*o10326)(A);
    A    (*o10814)(A);             A  (*o10101)(A, A, A);
    A    (*o10711)(A);             A  (*o10910)(A, A, A);
    A    (*o10321)(A);
    void (*o10867)(A);             void (*o10750)(A);
    void (*o10945)(A, A, A);
    A    (*o10331)(A, A, A);       A  (*o10973)(A, A, A);
    void (*o10887)(A, A, A);
    // D2Game FUN_ seams, retail argument order
    void (*f95690)(A, A, A);       // FUN_6fcb5690 (0x95690)
    A    (*f9FA30)(A, A);          // FUN_6fcbfa30 (0x9FA30)
    A    (*f95D50)();              // FUN_6fcb5d50 (0x95D50)
    A    (*f9EE50)(A, A, A, A, A, A); // FUN_6fcbee50 (0x9EE50)
    A    (*f21130)(A);             A  (*f21100)(A);
    void (*fE19C0)(A, A, A, A, A); // FUN_6fd019c0 (0xE19C0)
    void (*f21290)(A, A);          // FUN_6fc41290 (0x21290)
    void (*f8E6F0)();              // FUN_6fcae6f0 (0x8E6F0)
    void (*f8EA80)(A, A, A, A, A, A, A); // FUN_6fcaea80 (0x8EA80)
    void (*f9A1A0)(A, A);          // FUN_6fcba1a0 (0x9A1A0)
    void (*fDE0C0)(A, A, A, A);    // FUN_6fcfe0c0 (0xDE0C0)
    void (*fDCF20)(A, A, A);       // FUN_6fcfcf20 (0xDCF20)
    void (*fA0C70)(A, A, A, A, A, A); // FUN_6fcc0c70 (0xA0C70)
    void (*f2E5D0)(A);             // FUN_6fc2e5d0 (0x2E5D0)
    A    (*f31490)(A, A);          // FUN_6fc31490 (0x31490)
    void (*fF60D0)(A, A, A, A, A); // FUN_6fcf60d0 (0xF60D0)
    void (*fA28F0)(A, A, A, A, A, A, A, A, A, A); // FUN_6fcc28f0 (0xA28F0)
    void (*fA1880)(A, A, A, A, A, A, A, A); // FUN_6fcc1880 (0xA1880)
    void (*f9E4F0)();              // FUN_6fcbe4f0 (0x9E4F0)
};

static void fatal961(const D2Game961Env* e, A code)
{
    e->o10024(e->assertFile, e->o10265(code));
    std::exit(-1);
}

// 1.13c D2Game+0x961A0 (212 bytes). Sets or clears the 0x40 flag on unit+0xC4 from unit+0x38 and the
// two item ids at unit+0x60, then calls FUN_6fcb5690 and returns 1.
A D2GAME_961A0(const D2Game961Env* e, A p1, A unit, A p3)
{
    A iVar2 = rdA(unit, 0x60);
    bool good = true;
    if (iVar2 != 0) {
        A iVar1 = e->o11139(iVar2, 4);
        A second = e->o11139(iVar2, 5);
        good = (iVar1 != second) && (iVar1 != 0) && (second != 0) &&
               e->o11006(iVar1) && e->o11006(second) &&
               e->o10744(iVar1, 0x2D) && e->o10744(second, 0x2D);
    }
    if (good) {
        // (unit+0x38 >> 8) & 0x80000001 is 0 or 0x80000000: clear 0x40, else set it.
        uint32_t u = (rd32(unit, 0x38) >> 8) & 0x80000001u;
        bool clear = (u == 0) || (u == 0x80000000u);
        uint32_t f = rd32(unit, 0xC4);
        wr32(unit, 0xC4, clear ? (f & ~0x40u) : (f | 0x40u));
    } else {
        wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40u);
    }
    e->f95690(p1, unit, p3);
    return 1;
}

// 1.13c D2Game+0x96280 (141 bytes). Range-checks the index against dataTables+0xBA0 (count) and
// dataTables+0xB98 (base, stride 0x23C), sets unit+0xC4 bit 0x40 when unit is non-null, then returns
// whether FUN_6fcb5d50 yields a record after FUN_6fcbfa30 succeeds.
A D2GAME_96280(const D2Game961Env* e, A p1, A unit, int32_t idx)
{
    uint32_t n = rd32(e->dataTables, 0xBA0);
    uint32_t base = rd32(e->dataTables, 0xB98);
    if (idx < 0 || (uint32_t)idx >= n || ((uint32_t)idx * 0x23Cu + base) == 0) return 0;
    if (unit != 0) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40u);
    int32_t i = idx;
    if (e->f9FA30(p1, (A)&i) != 0) {
        return e->f95D50() != 0;
    }
    return 0;
}

// 1.13c D2Game+0x9D766 (22 bytes). Calls FUN_6fcbd6f0 when the dword at rec+0xBC+eax*4 is zero.
void D2GAME_9D766(A rec, uint32_t eax, void (*f9D6F0)())
{
    if (rd32(rec, 0xBC + eax * 4) == 0) f9D6F0();
}

// 1.13c D2Game+0x96A40 (159 bytes). Looks up the list entry esi in the table at unit+0x44 (stride 12),
// walks the node chain while ebx == 0 and the low flag bit is set (link at +0x14), then returns the node in
// the hash at edi+0x1320 (key = dword+4, 0x7F buckets, chain link at +0xE4) whose id at +0xC matches.
// Returns 0 when nothing matches. edi == 0 is an assert (0x2D3).
A D2GAME_96A40(const D2Game961Env* e, int32_t esi, int32_t ebx, A edi)
{
    A unit = e->o10920();
    if (unit == 0 || esi < 0 || (uint32_t)esi >= rd32(e->dataTables, 0xBF0)) return 0;
    A arr = rdA(unit, 0x44);
    if (arr == 0) return 0;
    A base = rdA(arr, 0);
    if (base == 0) return 0;
    A entry = base + (A)esi * 12;
    A pb = rdA(entry, 0);
    if (pb == 0) return 0;
    while (ebx == 0 && (rd8(pb, 0) & 1)) {
        pb = rdA(pb, 0x14);
        if (pb == 0) return 0;
    }
    if (edi == 0) fatal961(e, 0x2D3);
    uint32_t key = rd32(pb, 4);
    for (A it = rdA(edi, 0x1320 + (key & 0x7F) * 4); it != 0; it = rdA(it, 0xE4)) {
        if (rd32(it, 0xC) == key) return it;
    }
    return 0;
}

// Copy-in block shared by 0x9A630 and 0x9A7C0 (rec is param_1[3]): rec+0x48 nonzero copies 16 dwords into
// a zeroed 20-dword buffer that goes to FUN_6fcba1a0 with the pair (piVar1, piVar2); rec+0x4C nonzero
// copies 28 dwords and calls FUN_6fcfe0c0 and FUN_6fcfcf20.
struct Ctx961 { A a; A b; A c; };
static void block961(const D2Game961Env* e, A piVar1, A piVar2, A rec, A param2)
{
    if (rd32(rec, 0x48) != 0) {
        uint32_t buf[32];
        std::memset(buf, 0, sizeof(buf));
        for (int i = 0; i < 16; i++) buf[i] = rd32(rec, i * 4);
        Ctx961 ctx = { piVar1, piVar2, (A)buf };
        e->f9A1A0((A)&ctx, param2);
    }
    A src2 = rdA(rec, 0x4C);
    if (src2 != 0) {
        uint32_t buf[32];
        std::memset(buf, 0, sizeof(buf));
        for (int i = 0; i < 28; i++) buf[i] = rd32(src2, i * 4);
        e->fDE0C0(piVar1, piVar2, 1, (A)buf);
        e->fDCF20(piVar1, piVar2, (A)buf);
    }
}

// 1.13c D2Game+0x9A630 (388 bytes). ctx0 is param_1 (dwords: [0] piVar1, [1] piVar2, [3] rec). Returns 0
// when param2 is a type-1 owner whose state byte (owner+0x6D indexes the 0x168 table, entry stride 0x1A8
// from dataTables+0xA78, count dataTables+0xA80) is negative. Then block961 and the 64-bit LCG step on
// param2+0x20 feeds Ordinal_10945 with a 20-percent test.
A D2GAME_9A630(const D2Game961Env* e, A ctx0, A param2)
{
    A piVar1 = rdA(ctx0, 0), piVar2 = rdA(ctx0, 4), rec = rdA(ctx0, 0xC);
    if (param2 != 0 && rd32(param2, 0) == 1) {
        int32_t i4 = (int32_t)rd32(param2, 4);
        if (i4 < 0 || (uint32_t)i4 >= rd32(e->dataTables, 0xA80)) return 0;
        A entry = (A)(uint32_t)((uint32_t)i4 * 0x1A8 + rd32(e->dataTables, 0xA78));
        if (entry == 0) return 0;
        int8_t st = (int8_t)*(const uint8_t*)(uintptr_t)(rd8(piVar1, 0x6D) + 0x168 + entry);
        if (st >= 0) return 0;
    }
    block961(e, piVar1, piVar2, rec, param2);
    uint64_t v = (uint64_t)rd32(param2, 0x20) * 0x6AC690C5ull + rd32(param2, 0x24);
    std::memcpy((uint8_t*)param2 + 0x20, &v, 8);
    e->o10945(param2, 0x6B, (uint32_t)(v & 0xFFFFFFFFu) % 100 < 0x14);
    return 1;
}

// 1.13c D2Game+0x9A7C0 (261 bytes). The copy-in block of 0x9A630 without the guard and the LCG tail.
A D2GAME_9A7C0(const D2Game961Env* e, A ctx0, A param2)
{
    A piVar1 = rdA(ctx0, 0), piVar2 = rdA(ctx0, 4), rec = rdA(ctx0, 0xC);
    block961(e, piVar1, piVar2, rec, param2);
    return 1;
}

// 1.13c D2Game+0xA1000 (163 bytes). For unit (ESI) with type not in {2, 4..5}: releases the unit+0x2C
// handle through Ordinal_10867, then through Ordinal_10750, then calls FUN_6fcc0c70 with the 0x8783 tag.
A D2GAME_A1000(const D2Game961Env* e, A unit, int32_t param2)
{
    int32_t t = (int32_t)rd32(unit, 0);
    if (t != 2 && (t < 4 || t > 5) && rdA(unit, 0x2C) != 0) e->o10867(rdA(unit, 0x2C));
    t = (int32_t)rd32(unit, 0);
    if (t != 2 && (t < 4 || t > 5) && rdA(unit, 0x2C) != 0) e->o10750(rdA(unit, 0x2C));
    e->fA0C70(unit, 0, 0, (A)param2, 0x8783, 0x6FCBEDB0ull);
    return 0;
}

// 1.13c D2Game+0xA1420 (346 bytes). For unit (EBX) with type 0: looks up the item ids at unit+0x60 and
// their 0x2D tags to decide the slot (4 or 5), then requires Ordinal_10814(param_2) != -1 and FUN_6fc31490
// to succeed before calling FUN_6fcf60d0. Failing paths assert (0xF4C / 0xF55).
// aiStack[0] is unit+0x60 and aiStack[1] is the out slot (set to 4 before the first call, as in the decompile).
A D2GAME_A1420(const D2Game961Env* e, A param1, A param2, A unit)
{
    if (unit == 0 || rd32(unit, 0) != 0) return 0;
    A iVar2 = rdA(unit, 0x60);
    int32_t aStack[2];
    aStack[1] = 4;
    aStack[0] = (int32_t)iVar2;
    A iVar1 = e->o11139(iVar2, 4);
    iVar2 = e->o11139(iVar2, 5);
    uint8_t bVar4;
    if (iVar1 == 0) {
        if (iVar2 == 0) { bVar4 = 4; goto found; }
        if (e->o10326(iVar2) == 0) {
            if (e->o10744(iVar2, 0x2D) == 0) { bVar4 = 4; goto found; }
        }
    }
    if (iVar2 == 0 && iVar1 != 0) {
        if (e->o10326(iVar1) == 0) {
            if (e->o10744(iVar1, 0x2D) == 0) { bVar4 = 5; goto found; }
        }
    }
    return 0;
found: {
        uint32_t scratch[33];  // zeroed scratch block; the decompile shows no consumer
        std::memset(scratch, 0, sizeof(scratch));
        uint32_t code;
        if ((int32_t)e->o10814(param2) == -1) {
            code = 0xF4C;
        } else {
            e->f2E5D0(0);
            if (e->f31490(param1, (A)&aStack[1]) != 0) {
                e->fF60D0(unit, param1, bVar4, 0, (A)aStack);
                return 1;
            }
            code = 0xF55;
        }
        fatal961(e, code);
    }
    return 0;
}

// 1.13c D2Game+0xA2A40 (248 bytes). For unit with a non-null item at unit+0x60 (Ordinal_10101 must succeed):
// when edi != 0 the slot is 4 or 5 (by esi), resolved by Ordinal_11139; the slot must pass Ordinal_10711 or
// Ordinal_10910(0x7D). Then sets unit+0xC4 bit 0x40, calls FUN_6fcc28f0 or FUN_6fcc1880 by the 0x26 test,
// and FUN_6fcbe4f0. The decompile passes the address of its locals to Ordinal_10744 and FUN_6fcc28f0's
// neighbours; those inputs are not pinned (see the spec).
A D2GAME_A2A40(const D2Game961Env* e, A p1, A unit, A edi, A esi)
{
    A iVar2 = rdA(unit, 0x60);
    if (iVar2 == 0) return 0;
    uint8_t local8[4] = {0, 0, 0, 0};
    uint8_t local4[4] = {0, 0, 0, 0};
    uint32_t local_c = 0;
    A puStack = (A)local8;
    A puVar4 = (A)local4;
    if (e->o10101(iVar2, (A)&local_c, puVar4) != 0) {
        if (edi != 0) {
            A slot = (esi == 5) ? 4 : 5;
            puStack = e->o11139(iVar2, slot);
        }
        if (e->o10711(puStack) == 0) {
            if (e->o10910(puStack, 0x7D, 0) == 0) return 0;
        }
    }
    if (puStack != 0) {
        wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40u);
        A uVar3 = e->o10321(puStack);
        if (e->o10744(puVar4, 0x26) == 0) {
            e->fA28F0(p1, uVar3, unit, edi, esi, 0, 0, 0, 0, 1);
        } else {
            e->fA1880(p1, uVar3, edi, esi, 0, 0, 0, 1);
        }
        e->f9E4F0();
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0x96310 (402 bytes). Validates the index like 0x96280, sets unit+0xC4 bit 0x40, resolves the
// owner record with FUN_6fcbfa30 and FUN_6fcb5d50, then runs the Ordinal_10973 / Ordinal_10887 / FUN_6fc21130
// / FUN_6fc21100 / Ordinal_10331 sequence and FUN_6fd019c0. The third out word of FUN_6fcbee50 feeds
// FUN_6fd019c0 (uStack_18). Walks the list at this+0xDC for type-2 entries (FUN_6fcae6f0), then calls
// FUN_6fcaea80 with the level+1 at unit+0xA8 and clears bit 0x08 of this+0xC4.
// The decompile's stack slot (stack0xffffffc4) passed to FUN_6fc41290 is an unpinned scratch: zeroed here.
A D2GAME_96310(const D2Game961Env* e, A p1, A unit, int32_t idx, uint32_t p4)
{
    uint32_t n = rd32(e->dataTables, 0xBA0);
    uint32_t base = rd32(e->dataTables, 0xB98);
    if (idx < 0 || (uint32_t)idx >= n || ((uint32_t)idx * 0x23Cu + base) == 0) return 0;
    uint32_t out[3] = {0, 0, 0};
    int32_t r = (int32_t)e->f9EE50(0, (A)out, unit, 0, (A)(uint32_t)idx, (A)p4);
    if (r < 0) return 0;
    if (unit != 0) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40u);
    int32_t local24 = 0;
    if (e->f9FA30(p1, (A)&local24) == 0) return 0;
    A th = e->f95D50();
    if (th == 0) return 0;
    e->o10887(th, 0xC, e->o10973(unit, 0xC, 0));
    e->f21130(unit);
    e->f21100(unit);
    A uVar3 = e->o10331(unit, 0, 0);
    e->fE19C0((A)out[2], uVar3, unit, th, unit);
    local24 = 0;
    e->f21100(unit);
    e->f21130(unit);
    local24 = 12;
    uint32_t scratch[16];
    std::memset(scratch, 0, sizeof(scratch));
    e->f21290(th, (A)scratch);
    for (A p = rdA(th, 0xDC); p != 0; p = rdA(p, 0x24)) {
        if (rd8(p, 0) == 2) e->f8E6F0();
    }
    e->f8EA80(unit, th, 2, (A)rd32(unit, 0xA8) + 1, 0, 0, 0);
    wr32(th, 0xC4, rd32(th, 0xC4) & ~0x8u);
    return 1;
}
