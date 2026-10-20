// src/core/d2game/d2game_batch_bad40.cpp -- twelve 1.13c D2Game.dll functions (a4q3 quest block and its
// neighbours) transcribed from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c).
// Base 0x6FC20000. Register-only arguments (in_EAX / unaff_E**) are explicit parameters, named per function.
// Unmapped callees and D2Common ordinals are seams on D2GameBatchBad40Env; guest pointers are uint32_t.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2GameBatchBad40Env {
    // BAD40: guest addresses stored into the quest record's callback slots.
    uint32_t cbFn_a0, cbFn_a8, cbFn_ac, cbFn_b0, cbFn_c0, cbFn_c4, cbFn_c8, cbFn_cc, cbFn_d4, cbFn_d8,
             cbData_dc, cbFn_ec, cbFn_f0;
    void*    (*alloc)(const char* file, int line, int zero);     // Ordinal_10045
    void (*fatal)(uint32_t msgId);                  // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    // Per-offset lookups used by several functions.
    int32_t  (*ord)(uint32_t ordinal, uint32_t a, uint32_t b, uint32_t c);   // generic D2Common ordinal call
    uint32_t (*unitData)(uint32_t unit);                         // Ordinal_10920 (player data of a unit)
    uint32_t (*findQuestState)(uint32_t quest, uint32_t id);     // FUN_6fc566a0 etc.
    uint32_t (*sub31A70)(uint32_t unit, uint32_t a, void* b, uint32_t c, uint32_t d, uint32_t e);
    uint32_t (*seedRoll21180)(void* seed);                       // 0x21180 / 0x211D0 (seed*, ret roll)
    void     (*sub8EA80)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*sub9E170)(void* eax);
    void     (*sub8A3E0)(const void* args, uint32_t n);
    void     (*sub58930)();
    void     (*subB4F0)();
    uint32_t (*sub8F5C0)(uint32_t unit);                         // FUN_6fcaf5c0
    uint32_t (*levelOf)(uint32_t unit);
    const uint8_t* strTable;                                     // s_r01_r02_r03... (D2Game+0x?)
};

// Quest a4q3 init: clears 15 dwords at +0xa0, installs callbacks, allocates the 32 byte state.
// 1.13c D2Game+0xBAD40  (ECX = quest record)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BAD40(uint8_t* q, const D2GameBatchBad40Env* e)
{
    std::memset(q + 0xa0, 0, 15 * 4);
    wr32(q, 0x18, 0);
    q[0xb] = 0; q[0xc] = 0;
    wr32(q, 0xe8, 0);
    wr32(q, 0xa0, e->cbFn_a0);
    wr32(q, 0xec, e->cbFn_ec);
    wr32(q, 0xdc, e->cbData_dc);
    q[10] = 1;
    wr32(q, 0xf0, e->cbFn_f0);
    wr32(q, 0x10, 0x17);
    wr32(q, 0xe0, 0x1b);
    q[0xd] = 4;
    wr32(q, 200, e->cbFn_c8);
    wr32(q, 0xac, e->cbFn_ac);
    wr32(q, 0xa8, e->cbFn_a8);
    wr32(q, 0xcc, e->cbFn_cc);
    wr32(q, 0xd4, e->cbFn_d4);
    wr32(q, 0xd8, e->cbFn_d8);
    wr32(q, 0xc4, e->cbFn_c4);
    wr32(q, 0xc0, e->cbFn_c0);
    wr32(q, 0xb0, e->cbFn_b0);
    uint8_t* s = (uint8_t*)e->alloc("..\\Source\\D2Game/Quests/a4q3.cpp", 0x358, 0);
    wr32(q, 0x18, (uint32_t)(uintptr_t)s);
    std::memset(s, 0, 32);
}

// 1.13c D2Game+0xBAF20  (ECX = game/quest owner, EDX = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BAF20(void* game, uint32_t unit, const D2GameBatchBad40Env* e)
{
    uint32_t pd = e->unitData(unit);
    uint32_t q = rd32((void*)(uintptr_t)pd, 0x10 + ((const uint8_t*)game)[0x6d] * 4);
    if (e->ord(10174, q, 0x1b, 0) == 1) return;
    if (e->ord(10174, q, 0x1b, 1) == 1) return;
    int32_t a = e->ord(10331, unit, 0, 0);
    if (!a) return;
    a = e->ord(10826, a, 0, 0);
    if (!a) return;
    if ((char)e->ord(10459, a, 0, 0) != 3) return;
    e->findQuestState((uint32_t)(uintptr_t)game, 0);
    e->findQuestState((uint32_t)(uintptr_t)game, 0);
    e->ord(10003, q, 0x1b, 0xd);
    e->ord(10003, q, 0x1b, 1);
    e->ord(10495, q, 0x1b, 0);
    const uint32_t* list = (const uint32_t*)(uintptr_t)rd32(game, 0x10f4);
    if (!list) return;
    const uint8_t* n = (const uint8_t*)(uintptr_t)*list;
    if (!n) return;
    while (rd32(n, 0) != 0x18) { n = (const uint8_t*)(uintptr_t)rd32(n, 0xf4); if (!n) return; }
    uint8_t* st = (uint8_t*)(uintptr_t)rd32(n, 0x18);
    wr32(st, 0x14, rd32(st, 0x14) + 1);
}

// 1.13c D2Game+0xBB4F0  (EAX = arg to 0x9E170, ESI = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BB4F0(void* eax, int32_t* esi, const D2GameBatchBad40Env* e)
{
    e->sub9E170(eax);
    if (esi && *esi == 0) e->unitData((uint32_t)(uintptr_t)esi);
    uint8_t msg[6] = {0x5d, 0x17, 0x01, 0x00, 0, 0};   // local_8 = 0x1175d, local_4 = 0
    e->sub8A3E0(msg, 6);
}

// 1.13c D2Game+0xBB6A0  (stack arg = quest record, ESI = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BB6A0(uint8_t* rec, int32_t* unit, const D2GameBatchBad40Env* e)
{
    uint8_t* st = (uint8_t*)(uintptr_t)rd32(rec, 0x18);
    if (st[4]) e->ord(11090, 0, 0, 0);
    if (!st[3]) return;
    int32_t arg = 0x32;
    int32_t hits = 0;
    const uint32_t cnt = rd32(st, 0x14);
    auto rollOff = [&](uint32_t off, uint8_t* g) -> int32_t {
        uint8_t* sd = (uint8_t*)(uintptr_t)rd32(g, 0x10f4);
        if (!sd) e->fatal(0x1fb);
        uint64_t s; std::memcpy(&s, sd + 0x18, 8);
        s = (uint64_t)(uint32_t)s * 0x6ac690c5ull + rd32(sd, 0x1c);
        std::memcpy(sd + 0x18, &s, 8);
        return (int32_t)rd32(e->strTable, (uint32_t)(s & 0xffffffff) % 7 * 4 + off);
    };
    uint8_t* game = (uint8_t*)(uintptr_t)rd32(rec, 4);
    for (uint32_t i = 0; (int32_t)i < (int32_t)cnt; i++) {
        uint32_t off;
        switch (rd32(st, 8)) {
        case 1: off = 0xbc; break;
        case 2: case 3: off = 0xa0; break;
        case 4: off = 0x84; break;
        default: return;
        }
        unit[0x2e] = rollOff(off, game);
        if (e->sub31A70((uint32_t)(uintptr_t)unit, 2, &arg, 0, 0xffffffff, 0)) hits++;
    }
    if (!hits) return;
    int32_t left = (int32_t)rd32(st, 8) - 1;
    wr32(st, 8, (uint32_t)left);
    if (left >= 1) {
        e->sub8EA80((uint32_t)(uintptr_t)game, (uint32_t)(uintptr_t)unit, 7, rd32(game, 0xa8) + 0x14, 0, 0, 0);
        return;
    }
    st[3] = 0;
    if (!rd32(game, 0x70)) return;
    uint32_t base = 0;
    if (game[0x6d] == 1) base = 0x2c; else if (game[0x6d] == 2) base = 0x58;
    uint8_t* sd = (uint8_t*)(uintptr_t)rd32(game, 0x10f4);
    if (!sd) e->fatal(0x1fb);
    uint32_t r = e->seedRoll21180(sd + 0x18);
    unit[0x2e] = (int32_t)rd32(e->strTable, base + r * 4);
    e->sub31A70((uint32_t)(uintptr_t)unit, 2, &arg, 0, 0xffffffff, 0);
}

// 1.13c D2Game+0xBBCE0  (ECX = game, EDX = unit)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BBCE0(uint8_t* game, int32_t* unit, const D2GameBatchBad40Env* e)
{
    e->sub58930();
    uint8_t* pd = (uint8_t*)(uintptr_t)e->unitData((uint32_t)(uintptr_t)unit);
    wr32(pd, 0x4c, 1);
    uint8_t* n = nullptr;
    const uint32_t* list = (const uint32_t*)(uintptr_t)rd32(game, 0x10f4);
    if (list) for (n = (uint8_t*)(uintptr_t)*list; n; n = (uint8_t*)(uintptr_t)rd32(n, 0xf4))
        if (rd32(n, 0) == 0x17) break;
    wr32((void*)(uintptr_t)rd32(n, 0x18), 0x20, (uint32_t)(uintptr_t)unit);
    pd = (uint8_t*)(uintptr_t)e->unitData((uint32_t)(uintptr_t)unit);
    uint32_t q = rd32(pd, 0x10 + game[0x6d] * 4);
    if (e->ord(10174, q, 0x1a, 0xd) != 0) { e->subB4F0(); return 0; }
    if (unit && *unit == 0) e->unitData((uint32_t)(uintptr_t)unit);
    uint8_t msg[6] = {0x50, 0x17, 0x00, 0x00, 0, 0};   // CONCAT13(0,0x1750); remaining stack bytes unnamed
    e->sub8A3E0(msg, 0xf);
    return 0;
}

// Pick a skill-bearing item from the unit's list; returns the item, writes the leftover count.
// 1.13c D2Game+0xBC6B0  (ECX = this, param_1 = flag out; EBX = out count; EDI = out remainder)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_BC6B0(uint8_t* self, uint32_t* flag, int32_t* ebx, int32_t* edi,
                                                           const D2GameBatchBad40Env* e)
{
    *flag = 1;
    uint32_t inv = rd32(self, 0x60);
    if (!inv || !e->ord(10048, 0, 0, 0) || !e->ord(10955, 0, 0, 0)) return 0;
    int32_t total = e->ord(10463, 0, 0, 0);
    int32_t it = e->ord(10643, inv, 0, 0);
    while (it) {
        if (e->ord(11102, 0, 0, 0)) {
            e->ord(10973, it, 0x46, 0);
            total = e->ord(10463, it, 0, 0);
            if (*ebx < total) { *edi = total - *ebx; return it; }
        }
        it = e->ord(10643, inv, 0, 0);
    }
    it = e->ord(10836, inv, 0, 0);
    for (;;) {
        if (!it) { *ebx = total; return 0; }
        int32_t prev = 0;
        if (e->ord(11102, 0, 0, 0)) {
            prev = e->ord(10973, it, 0x46, 0);
            total = e->ord(10463, it, 0, 0);
            if (prev < total) { *ebx = total - prev; return it; }
            prev = 0;
        }
        inv = (uint32_t)prev;   // decompile feeds the (zeroed) value back as the next argument
        it = e->ord(10836, (uint32_t)prev, 0, 0);
    }
}

// 1.13c D2Game+0xBC7D0  (EBX = out hi?, ESI = out max, EDI = out min)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_BC7D0(uint8_t* self, uint32_t* ebx, uint32_t* esi, uint32_t* edi,
                                                           const D2GameBatchBad40Env* e)
{
    uint8_t* r = (uint8_t*)(uintptr_t)e->ord(10427, rd32(self, 0x70), 0, 0);
    if (!r) { *edi = 0; *esi = 0; *ebx = 0; return -1; }
    *edi = rd16(r, 0x114);
    *esi = rd16(r, 0x116);
    if (ebx) *ebx = rd32(r, 8);
    int32_t n = (int32_t)(*esi - *edi) + 1;
    if (n == -1) e->fatal(0x5b);
    return n;
}

// 1.13c D2Game+0xBCA20  (EBX = game)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BCA20(uint8_t* ebx, const D2GameBatchBad40Env* e)
{
    if (!ebx || !rd32(ebx, 0x60)) return 0;
    int32_t it = e->ord(10460, rd32(ebx, 0x60), 0, 0);
    while (it) {
        int32_t next = e->ord(10464, it, 0, 0);
        int32_t* d = (int32_t*)(uintptr_t)e->ord(11147, it, 0, 0);
        it = next;
        if (d && *d == 4 && e->ord(11160, (uint32_t)(uintptr_t)d, 0, 0)) {   // 2nd arg: local on caller stack
            e->ord(11127, 0, 0, 0);
            e->ord(10164, 0, 0, 0);
        }
    }
    return 1;
}

// 1.13c D2Game+0xBCAA0  (EBX = game, param_1 = item)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BCAA0(uint32_t item, uint8_t* ebx, const D2GameBatchBad40Env* e)
{
    uint8_t* r = (uint8_t*)(uintptr_t)e->sub8F5C0((uint32_t)(uintptr_t)ebx);
    uint32_t code = 0;
    if (!r) return 0;
    code = (uint32_t)e->ord(10802, item, 0, 0);
    if (ebx[0x6d] && (code == 0x20347068 || code == 0x20357068 || code == 0x2034706d || code == 0x2035706d)) return 1;
    int32_t n = (int32_t)rd32(r, 0x38);
    const uint32_t* tbl = (const uint32_t*)(uintptr_t)rd32(r, 0x34);
    for (int32_t i = 0; i < n; i++) if (code == tbl[i]) return 1;
    return code & 0xffffff00;
}

// 1.13c D2Game+0xBCC60  (ESI = unit; ordinals take their operands in registers, see decompile)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BCC60(int32_t* unit, const D2GameBatchBad40Env* e)
{
    int32_t a = e->ord(10973, 0, 0, 0);
    int32_t b = e->ord(10973, 0, 0, 0);
    if (b + a < 0) return 0;
    if (a < 0) {
        if (unit) {
            int32_t c = e->ord(10973, 0, 0, 0);
            if (c - a < 0) e->ord(10887, 0, 0, 0);
            else if (*unit == 0 && e->ord(10049, 0, 0, 0) < c - a) e->ord(10887, 0, 0, 0);
            else e->ord(10551, 0, 0, 0);
        }
        if (unit) {
            int32_t c = e->ord(10973, 0, 0, 0);
            if (c + a < 0 || (*unit == 0 && e->ord(11060, 0, 0, 0) < c + a)) { e->ord(10887, 0, 0, 0); return 1; }
            e->ord(10551, 0, 0, 0);
        }
    } else if (unit) {
        int32_t c = e->ord(10973, 0, 0, 0);
        if (c >= 0 && (*unit != 0 || c <= e->ord(10049, 0, 0, 0))) { e->ord(10551, 0, 0, 0); return 1; }
        e->ord(10887, 0, 0, 0);
        return 1;
    }
    return 1;
}

// 1.13c D2Game+0xBCE60  (ESI = unit, EDI = game)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BCE60(int32_t* unit, uint8_t* edi, const D2GameBatchBad40Env* e)
{
    e->ord(10977, 0, 0, 0);
    if (unit) {
        unit[0x32] |= 4;
        if (*unit == 4 && e->ord(10757, 0, 0, 0) && unit[0x18] && e->ord(10508, (uint32_t)unit[0x18], 0, 0))
            e->ord(10977, 0, 0, 0);
    }
    e->ord(10201, 0, 0, 0);
    e->ord(10050, rd32(edi, 0x60), 0, 0);
}

// 1.13c D2Game+0xBD220  (param_1 = unit, param_2 = out zero, param_3 = out ptr; EDI = seed)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_BD220(uint32_t unit, uint32_t* p2, uint32_t** p3, void* seed,
                                                            uint32_t ebp, uint32_t* retSlot,
                                                            const D2GameBatchBad40Env* e)
{
    uint32_t* out = *p3;
    uint8_t* r = (uint8_t*)(uintptr_t)e->sub8F5C0(unit);
    if (!r) e->fatal(900);
    if ((intptr_t)r == -0x20) e->fatal(0x37e);
    if (r[0x23]) {
        if (rd32(r, 0x30) && rd32(r, 0x2c)) {
            uint32_t k = e->seedRoll21180(seed);
            uint64_t s; std::memcpy(&s, seed, 8);
            s = (uint64_t)(uint32_t)s * 0x6ac690c5ull + rd32(seed, 4);
            std::memcpy(seed, &s, 8);
            uint32_t v = rd32((void*)(uintptr_t)rd32(r, 0x2c), 4 + k * 12);
            e->ord(10450, v, unit, 0);
            e->ord(10349, (uint32_t)(uintptr_t)out, 0, 0);
            *out = v;
            *retSlot = ebp;
            return 1;
        }
        *p2 = 0;
        return 0;
    }
    e->fatal(0x37f);
}
