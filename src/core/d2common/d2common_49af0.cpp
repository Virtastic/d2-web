// 1.13c D2Common+0x49AF0: unit stat writes for an item source. __fastcall: ECX=p1, EDX=p2, p3..p8 on the stack (retail ret 0x1C).
// Two shapes, chosen by a flag in the unit's ext block (unit[5] + 0x30): the flagged shape calls 47EF0 four times; the other
// runs four H-and-write steps (H = 464D0, then the Fog link for p1 == 1, then 47020 + 3A330 when H is nonzero).
// Every callee and module global goes through D2Common49AF0Env, so the FN probe (src/core/fn/d2common_49af0.cpp) can mirror each call.
#include <cstdint>

extern "C" {
struct D2Common49AF0Env {
    int32_t (*callerValue)(int32_t eax, int32_t ecx, int32_t* unit);            // 0x464D0: plain ret; EAX=*(p4+0xC), ECX=*(p4+8), EDX=unit
    const uint8_t* (*itemRow)(uint32_t txtId);                                  // 0x719A0: stack txt id, ret 4
    int32_t (*linkIndex)(const uint8_t* linker, uint32_t key, uint32_t zero);  // Fog ordinal 10213 (IAT 0x890BC via thunk): ret 12
    uint32_t* (*statUnit)(int32_t* unit, void* self, int32_t p7, uint32_t p8);  // 0x47020: EAX=unit, ECX=self, stack p7 p8; ret 8
    int32_t (*calc38B00)(int32_t base);                                         // 0x38B00: stack base, ret 4
    void (*setStat)(uint32_t* unit, int32_t stat, int32_t value, int32_t layer); // 0x3A330: stack 4 words, ret 0x10
    void (*extStat)(int32_t stat, int32_t* unit, int32_t p1, void* p2, int32_t v, int32_t p6, int32_t p7, uint32_t p8); // 0x47EF0: EAX=0, ECX=unit, EDI=stat; stack 6 words, ret 0x18
    void (*fatal)(uint32_t msg);                                                // report and _exit(-1): does not return
    const uint8_t* linker;                                                      // DAT_6FDEFF6C (RVA 0x8EF6C)
    const uint8_t* dataTables;                                                  // sgptDataTables (RVA 0x99E1C)
};
uint32_t D2Common_49AF0_ApplyUnitStats(const D2Common49AF0Env* e, int32_t p1, void* p2, int32_t* p3,
                                       const uint8_t* p4, uint32_t p5, int32_t p6, int32_t p7, uint32_t p8);
}

namespace {
struct Step { int32_t stat, need, off; };
constexpr Step kSteps[4] = { {0x28, 0x29, 0x32a0}, {0x2a, 0x2b, 0x3528}, {0x2c, 0x2d, 0x37b0}, {0x2e, 0x2f, 0x3a38} };
constexpr int32_t kExtStat[4] = { 0x28, 0x2a, 0x2c, 0x2e };
}

extern "C" uint32_t D2Common_49AF0_ApplyUnitStats(const D2Common49AF0Env* e, int32_t p1, void* p2, int32_t* p3,
                                                  const uint8_t* p4, uint32_t p5, int32_t p6, int32_t p7, uint32_t p8) {
    if (!p3) e->fatal(0x1505);
    if (p3[0] != 4) e->fatal(0x1506);
    if (!p3[5]) e->fatal(0x1508);
    const uint8_t* ext = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(p3[5])));
    if (*reinterpret_cast<const int16_t*>(ext + 0x30) != 0) {
        if (!p4) e->fatal(0x364);
        const int32_t v = e->callerValue(*reinterpret_cast<const int32_t*>(p4 + 0xC), *reinterpret_cast<const int32_t*>(p4 + 8), p3);
        for (int i = 0; i < 4; ++i) e->extStat(kExtStat[i], p3, p1, p2, v, p6, p7, p8);
        return p5;
    }
    for (int i = 0; i < 4; ++i) {
        const Step& s = kSteps[i];
        int32_t v = e->callerValue(*reinterpret_cast<const int32_t*>(p4 + 0xC), *reinterpret_cast<const int32_t*>(p4 + 8), p3);
        if (p1 == 1) {
            if (p3[0] != 4) e->fatal(0x61c);
            const uint8_t* row = e->itemRow(static_cast<uint32_t>(p3[1]));
            if (!row) e->fatal(0x61f);
            (void)e->linkIndex(e->linker, *reinterpret_cast<const uint32_t*>(row + 0x80), 0);
        }
        if (!v) continue;
        uint32_t* h = e->statUnit(p3, p2, p7, p8);
        if (!h) e->fatal(0x29d);
        if (p6) {
            const int32_t cnt = *reinterpret_cast<const int32_t*>(e->dataTables + 0xbd4);
            const int32_t base = *reinterpret_cast<const int32_t*>(e->dataTables + 0xbcc) + s.off;
            v = -((cnt < s.need || base == 0) ? 0 : e->calc38B00(base));
        }
        e->setStat(h, s.stat, v, 0);
    }
    return p5;
}
