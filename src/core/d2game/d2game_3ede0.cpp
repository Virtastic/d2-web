// D2Game+0x3EDE0 -- monster attack/hit resolution used by the monster-mode "do" handlers.
// Stack args (ret 0xC): game, target unit (may be null), force flag; EDI = the acting monster (unit).
// Written from the 1.13c decompile and disassembly. Unit and records are read by byte offset; the callees
// are taken by retail RVA through an env (same seam style as d2game_batch_3fed0.cpp).
//   monstats row: sgptDataTables+0xB64 (+0xB6C count), stride 0x1A4, indexed by unit+4.
//   bit masks: gdwBitMasks[] (byte array) at the pointer 0x6FD18BE0.
//   hook tables: DAT_6FD2DA48 (count at 0x6FD2DB64) and DAT_6FD2DB68 (count at 0x6FD2DBE4).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game3EDE0Env {
    const uint8_t* dataTables;                      // *sgptDataTables_exref
    const uint8_t* bitMasks;                        // gdwBitMasks
    int32_t hook1Count, hook2Count;                 // DAT_6fd2db64 / DAT_6fd2dbe4
    uint32_t (*hook1)(int32_t idx, uint32_t game, uint32_t unit, uint32_t target);                // ecx=game edx=unit stack=target
    void     (*hook2)(int32_t idx, uint32_t game, uint32_t unit, uint32_t target, uint32_t buf);  // + stack buf
    uint32_t (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);   // 0xA42A
    uint32_t (*o10910)(uint32_t unit, uint32_t stat, uint32_t layer);   // 0xA802
    uint32_t (*o10494)(uint32_t unit, uint32_t state);                  // 0xA68E
    uint32_t (*o10331)(uint32_t unit);                                  // 0xA478
    uint32_t (*sub_B27C)(uint32_t unit, uint32_t target);               // nonzero -> return 1
    void     (*sub_B0F0)(uint32_t unit, uint32_t target);               // result unused
    uint32_t (*sub_A71E)(uint32_t unit, uint32_t what);                 // (unit, 0x40)
    void     (*sub_B258)(uint32_t room, uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*sub_21130)(uint32_t unit);
    uint32_t (*sub_21100)(uint32_t unit);
    uint32_t (*sub_58FB0)(uint32_t target, uint32_t mode);              // eax = target, ecx = unit mode
    uint32_t (*sub_58F40)(uint32_t unit);                               // esi = unit
    uint32_t (*sub_E03A0)(uint32_t unit94, uint32_t unit98, uint32_t game);   // eax, edx, ecx -> "this"
    uint32_t (*sub_E0A60)(uint32_t self, uint32_t target, uint32_t game);     // ecx = self, eax = target, stack game
    uint32_t (*sub_DDE90)(uint32_t self, uint32_t target, uint32_t v, uint32_t one);  // eax = self, stack (target, v, 1)
    void     (*sub_3E590)(uint32_t unit, uint32_t target);              // ecx = unit, stack target
    void     (*sub_37030)(uint32_t game, uint32_t zero0, uint32_t unit, uint32_t zero1, uint32_t target); // stack game,0,unit,0; ebx = target
    void     (*sub_3A4E0)(uint32_t target, uint32_t buf, uint32_t unit); // stack target, buf; ebx = unit
    void     (*sub_3AFF0)(uint32_t game, uint32_t buf, uint32_t target, uint32_t unit); // eax game, ecx buf, esi target, ebx unit
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint8_t  rd8(const uint8_t* p, uint32_t o) { return p[o]; }
static inline int16_t  rd16s(const uint8_t* p, uint32_t o) { int16_t v; std::memcpy(&v, p + o, 2); return v; }

// 1.13c D2Game+0x3EDE0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_3ede0(uint32_t game, uint32_t target, uint32_t force, uint32_t unit, const D2Game3EDE0Env* e)
{
    const uint8_t* rec = nullptr;
    if (unit) {
        const int32_t id = (int32_t)rd32(unit + 4);
        if (id >= 0 && id < (int32_t)rd32((uint32_t)(uintptr_t)e->dataTables + 0xB6C))
            rec = (const uint8_t*)(uintptr_t)(rd32((uint32_t)(uintptr_t)e->dataTables + 0xB64) + (uint32_t)id * 0x1A4);
    }
    uint32_t self = 0, local74 = 3, local78 = 2;
    if (unit && ((rd32(unit + 0xC8) >> 10) & 1)) self = e->sub_E03A0(rd32(unit + 0x94), rd32(unit + 0x98), game);
    if (e->bitMasks[4] & rec[4]) local78 = 0;

    bool toMask = false;                              // LAB_6fc5eef1 reached
    if (target == 0) {
        toMask = true;
    } else {
        if (rec[0x188] && e->o10494(target, 0x56)) return 1;
        if (e->sub_B27C(unit, target)) return 1;
        e->sub_B0F0(unit, target);
        if (e->sub_58FB0(target, unit ? rd32(unit + 0x10) : 0) == 0) return 1;
        if (self) {
            if (e->sub_E0A60(self, target, game) == 0 && rec[0x187] == 0) return 1;
            toMask = true;
        }
    }
    if (toMask && self && target && (e->bitMasks[8] & rec[4]) &&
        (e->o10973(self, 0xa6, 0) != 0 || e->o10910(self, 0x9c, 0) != 0))
        local74 = e->sub_58F40(unit);

    if (rec[0x186]) local78 |= 1;
    bool skipTo_f0b4 = false;
    if (target == 0) {
        if (force == 0 && rec[0x18c] == 0) skipTo_f0b4 = true;
    } else if (rec[0x18b]) {
        const uint32_t v = unit ? e->o10973(unit, 0x13, 0) : 0;
        if (e->sub_DDE90(self, target, v, 1) == 0) {
            e->sub_3E590(unit, target);
            e->sub_37030(game, 0, unit, 0, target);
            const int16_t s = rd16s(rec, 0xe);
            if (rec[0x18c] && s > 0 && s < e->hook1Count) {
                if (e->hook1(s, game, unit, target) & 4) return 1;
            }
            return 2;
        }
    }
    if (!skipTo_f0b4) {
        if (local74 & 2) {
            e->sub_3E590(unit, target);
            e->sub_37030(game, 0, unit, 0, target);
            const int16_t s = rd16s(rec, 0xe);
            if (s > 0 && s < e->hook1Count) {
                local78 = e->hook1(s, game, unit, target);
                if (local78 & 4) return 1;
            }
        }
        if (target && (local78 & 2)) {
            uint32_t buf[28];
            e->sub_3A4E0(target, (uint32_t)(uintptr_t)buf, unit);
            const int16_t s = rd16s(rec, 0x10);
            if (s > 0 && s < e->hook2Count) e->hook2(s, game, unit, target, (uint32_t)(uintptr_t)buf);
            e->sub_3AFF0(game, (uint32_t)(uintptr_t)buf, target, unit);
        }
    }
    if (((local78 & 1) || force) && (local74 & 1)) {
        const uint32_t room = e->o10331(unit);
        if (room) {
            const uint32_t a = e->sub_A71E(unit, 0x40);
            const uint32_t b = e->sub_21130(unit);
            const uint32_t c = e->sub_21100(unit);
            e->sub_B258(room, c, b, a);
        }
        return 2;
    }
    return 1;
}
