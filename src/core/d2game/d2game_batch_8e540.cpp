// Batch D2Game+0x8E540, 0x90260, 0x90F30, 0x91340, 0x93040, 0x9E680, transcribed from the cached 1.13c decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Same pattern as d2game_batch_961a0.cpp: retail records are read by
// byte offset, every callee is a seam on D2Game8E540Env in retail argument order, and Ghidra register-only inputs
// (in_EAX, unaff_ESI, unaff_EBX) are explicit parameters. The rest of 0x8C1B0..0x95980 is not here: those decompiles
// call helpers with arguments the decompiler dropped (see each row's STALLED note).
#include <cstdint>
#include <cstring>

typedef uintptr_t A;
static inline uint32_t r32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  r16s(A p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint16_t r16(A p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void w32(A p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void w16(A p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }

struct D2Game8E540Env {
    A dataTables;                                  // *sgptDataTables_exref
    const uint32_t* bitMasks;                      // gdwBitMasks_exref (bitMasks[i] == 1u << i)
    A    (*f8E420)();                              // FUN_6fcae420: fresh pool node
    void* (*o10045)(const char* file, int line, int zero); // allocator
    A    (*f8F5C0)(A unit);                        // FUN_6fcaf5c0
    void (*fDFC10)(A p);                           // FUN_6fcffc10
    void (*fE20A0)(A unit, A p);                   // FUN_6fd020a0
    int  (*o10198)(A unit);
    A    (*o11150)(A unit);
    int  (*o10252)(uint32_t v);                    // lowest set bit index, -1 when none
    A    (*o10871)(A unit, int idx);
    void (*o11111)(A unit, A item);
    void (*o11108)(A item);
    void (*o10945)(A unit, int idx, int zero);
    uint16_t (*fE5A0)(A a, uint32_t b, uint32_t c);   // FUN_6fcfe5a0
    void (*fE450)(A a, A b, A c, A d, uint32_t e, uint8_t f); // FUN_6fcfd450
    void (*fDDDE0)(uint8_t a, A b, A c);           // FUN_6fcfdde0
    void (*fED30)(A a, A b, A c);                  // FUN_6fcfed30
    int  (*o10680)(A stats, int id, int zero);
};

// 1.13c D2Game+0x8E540 (108 bytes). `game` = ESI. Walks the in-use chain from game+0xA1C through node+0x7084 to the
// first node whose free list (node+0x7080) is non-null; with none it takes a fresh node from 0x8E420, links it behind
// the head and uses it. Pops the first free entry (node+0x7080 = entry+0x1C), zeroes its twelve dwords, returns it.
A D2Game_8E540_AcquireZeroedPoolNode(const D2Game8E540Env* e, A game)
{
    A node = r32(game, 0xA1C);
    while (node != 0 && r32(node, 0x7080) == 0) node = r32(node, 0x7084);
    if (node == 0) {
        node = e->f8E420();
        A head = r32(game, 0xA1C);
        w32(node, 0x7084, r32(head, 0x7084));
        w32(head, 0x7084, (uint32_t)node);
    }
    A entry = r32(node, 0x7080);
    w32(node, 0x7080, r32(entry, 0x1C));
    std::memset((void*)entry, 0, 48);
    return entry;
}

// 1.13c D2Game+0x90260 (125 bytes). unit = EAX, `stackArg` = the stack parameter, ebx = EBX. Allocates a 20-byte
// proxy node; when 0x8F5C0(unit) yields a holder the node is chained at holder+0x14 (node+0x10 = old head) and filled
// (+0 stackArg, +0xC = 1, +8 = ebx); otherwise the node is left unfilled and 0xDFC10(stackArg), 0xE20A0(unit, stackArg) run.
void D2Game_90260_AttachUnitProxyNode(const D2Game8E540Env* e, A unit, A stackArg, uint32_t ebx)
{
    A node = (A)e->o10045("..\\Source\\D2Game\\UNIT\\sunitproxy.cpp", 0x85, 0);
    A holder = e->f8F5C0(unit);
    if (holder != 0) {
        w32(node, 0x10, r32(holder, 0x14));
        w32(holder, 0x14, (uint32_t)node);
        w32(node, 0, (uint32_t)stackArg);
        w32(node, 0xC, 1);
        w32(node, 8, ebx);
        return;
    }
    e->fDFC10(stackArg);
    e->fE20A0(unit, stackArg);
}

// 1.13c D2Game+0x90F30 (262 bytes). For every set bit of the unit's state mask (ordinal 11150, (dt+0xC4+31)/32 dwords,
// copied first) whose index is flagged in the table at dt+0xE4: remove the item from slot `index` (10871 -> 11111,
// 11108) when there is one, then clear the slot (10945(unit, index, 0)). Returns nothing.
void D2Game_90F30_ClearUnitStateBitsAndRemove(const D2Game8E540Env* e, A unit)
{
    if (e->o10198(unit) == 0) return;
    const uint32_t* src = (const uint32_t*)e->o11150(unit);
    int count = (int)r32(e->dataTables, 0xC4) + 0x1F;
    int dwords = (count + ((count >> 31) & 0x1F)) >> 5;
    uint32_t copy[9] = {};
    for (int i = 0; i < dwords && i < 9; i++) copy[i] = src[i];
    int base = 0;
    for (int d = 0; d < dwords; d++, base += 0x20) {
        uint32_t bits = copy[d];
        for (int bit = e->o10252(bits); bit > -1; bit = e->o10252(bits)) {
            uint32_t idx = (uint32_t)(bit + base);
            bits &= ~(1u << (bit & 0x1F));
            uint32_t flags = r32(r32(e->dataTables, 0xE4), ((int)idx >> 5) * 4);
            if (e->bitMasks[idx & 0x1F] & flags) {
                A item = e->o10871(unit, (int)idx);
                if (item) {
                    e->o11111(unit, item);
                    e->o11108(item);
                }
                e->o10945(unit, (int)idx, 0);
            }
        }
        copy[d] = bits;
    }
}

// 1.13c D2Game+0x91340 (182 bytes). rec = EAX index into the 0x23C-byte table at dt+0xB98 (count dt+0xBA0), stats = the
// stack parameter. Returns -1 for a bad index. When the record's flag word (rec+4) has bit 8 and its short at +0x80 is
// a valid index below dt+0xC4 and its short at +0x54 below dt+0xBD4, looks up that value on the unit (10871) and its
// level (10680): level < 2 gives the short at +0x48, otherwise min(level, 3) selects the short at +0x46 + 2*level.
// Anything else gives the short at +0x48.
int D2Game_91340_GetRecordScaledShort(const D2Game8E540Env* e, int index, A unit)
{
    A dt = e->dataTables;
    if (index < 0 || index >= (int)r32(dt, 0xBA0)) return -1;
    A rec = (A)index * 0x23C + r32(dt, 0xB98);
    if (rec == 0) return -1;
    if (((int)e->bitMasks[8] & (r32(rec, 4) & 0xFF)) != 0) {
        int16_t s80 = r16s(rec, 0x80);
        if (s80 >= 0 && s80 < (int)r32(dt, 0xC4) && r16s(rec, 0x54) >= 0 && r16s(rec, 0x54) < (int)r32(dt, 0xBD4)) {
            A stat = e->o10871(unit, s80);
            if (stat != 0) {
                int lvl = e->o10680(stat, r16s(rec, 0x54), 0);
                if (lvl < 2) return r16s(rec, 0x48);
                if (lvl > 2) lvl = 3;
                return r16s(rec, 0x46 + lvl * 2);
            }
        }
    }
    return r16s(rec, 0x48);
}

// 1.13c D2Game+0x93040 (193 bytes). ECX = ctx (dword 0 a, 1 b, 3 -> request record), EDX/`target` = the second argument.
// Copies the 112-byte request record at *(request+0); flag word is the short at request+8. Without bit 1 it asks 0xE5A0
// (a, request+4, 1) and returns 1 untouched when that gives bit 1 clear, else ORs the answer in. With bit 1 set: ORs
// request+0xC into record dword 0; when the byte at request+0x10 is non-zero calls 0xD450 with it; then 0xDDDE0(0x80,...)
// and 0xED30. Always returns 1.
uint32_t D2Game_93040_RunRecordSkillEffectOnUnit(const D2Game8E540Env* e, A ctx, A target)
{
    A request = r32(ctx, 12);
    uint8_t rec[0x70];
    std::memcpy(rec, (const void*)(A)r32(request, 0), 0x70);
    uint16_t flags = r16(request, 8);
    uint16_t cur = flags;
    if ((flags & 1) == 0) {
        uint16_t r = e->fE5A0(r32(ctx, 0), r32(request, 4), 1);
        if ((r & 1) == 0) return 1;
        cur = (uint16_t)(r | flags);
    }
    if (cur & 1) {
        uint32_t d0; std::memcpy(&d0, rec, 4);
        d0 |= r32(request, 0xC);
        std::memcpy(rec, &d0, 4);
        uint8_t b = ((const uint8_t*)request)[0x10];
        if (b != 0) e->fE450(r32(ctx, 0), r32(ctx, 4), target, (A)rec, 0, b);
        e->fDDDE0(0x80, r32(ctx, 0), target);
        e->fED30(r32(ctx, 0), r32(ctx, 4), target);
    }
    return 1;
}

// 1.13c D2Game+0x9E680 (35 bytes): calls the function pointer it is handed (register arguments pass through untouched).
void D2Game_9E680_CallThrough(void (*fn)()) { fn(); }
