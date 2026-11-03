// 1.13c D2Common+0x48BD0: stat pair writes for an item or stat source. __fastcall: ECX=p1, EDX=p2, p3..p8 on the stack (retail ret 0x1C).
// p5 == 0x15 takes the item-row path: the row (719A0 of p2[1], or of p3[1] when p2 is not a type-4 record) and its flags decide
// four 47EF0 writes (0x15..0x18), and when p3 is a type-4 unit whose 230F0 index maps a row with a nonzero byte at +0x10, two more (0x9F, 0xA0).
// Any other p5 takes the stat-unit path: 47020 gives the unit, p5 and p5+1 get 3A330 writes (39140 values, negated, when p6 != 0), and 0x39 also writes 0x146.
// 45640 is a local helper here: it shifts a value left 8 for a fixed set of stat indexes.
// Every callee and module global goes through D2Common48BD0Env, so the FN probe (src/core/fn/d2common_48bd0.cpp) can mirror each call.
#include <cstdint>
#include <cstring>

extern "C" {
struct D2Common48BD0Env {
    uint32_t (*hasFlag)(int32_t* unit, uint32_t id);                              // 0x24430: stack unit, id; ret 8
    const uint8_t* (*itemRow)(uint32_t txtId);                                    // 0x719A0: stack txt id, ret 4
    int32_t (*unitIndex)(int32_t* unit);                                          // 0x230F0: stack unit, ret 4
    int32_t (*statValue)(uint32_t* unit, int32_t stat, int32_t layer);           // 0x39140: stack unit, stat, layer; ret 0xC
    uint32_t* (*statUnit)(int32_t* unit, void* self, int32_t p7, uint32_t p8);    // 0x47020: EAX=unit, ECX=self, stack p7 p8; ret 8
    void (*extStat)(int32_t stat, int32_t* unit, int32_t p1, void* p2, int32_t v, int32_t p6, int32_t p7, uint32_t p8); // 0x47EF0: EAX=0, ECX=unit, EDI=stat; ret 0x18
    void (*setStat)(uint32_t* unit, int32_t stat, int32_t value, int32_t layer);  // 0x3A330: stack 4 words, ret 0x10
    void (*fatal)(uint32_t msg);                                                  // report and _exit(-1): does not return
    const uint8_t* dataTables;                                                    // sgptDataTables (RVA 0x99E1C)
};
uint32_t D2Common_48BD0_ApplyStatPair(const D2Common48BD0Env* e, int32_t p1, int32_t* p2, int32_t* p3,
                                       const uint8_t* p4, int32_t p5, int32_t p6, int32_t p7, uint32_t p8);
}

namespace {
uint32_t Load32(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

// 0x45640: EAX = stat index, ECX = value pointer. Shifts the value left 8 for the listed stats.
void ScaleByStat(int32_t stat, int32_t* v) {
    switch (stat) {
        case 6: case 7: case 8: case 9: case 10: case 0xb: case 0xd8: case 0xd9:
            *v = int32_t(uint32_t(*v) << 8);
            break;
        default:
            break;
    }
}
}

extern "C" uint32_t D2Common_48BD0_ApplyStatPair(const D2Common48BD0Env* e, int32_t p1, int32_t* p2, int32_t* p3,
                                                  const uint8_t* p4, int32_t p5, int32_t p6, int32_t p7, uint32_t p8) {
    int32_t v4 = int32_t(Load32(p4 + 8));
    int32_t vl = int32_t(Load32(p4 + 0xC));
    ScaleByStat(p5, &v4);
    ScaleByStat(p5 + 1, &vl);

    if (p5 == 0x15) {
        const uint8_t* row = (p2 == nullptr || p2[0] != 4)
            ? e->itemRow(p3 == nullptr ? 0xffffffffu : uint32_t(p3[1]))
            : e->itemRow(uint32_t(p2[1]));

        const uint32_t fA = e->hasFlag(p3, 0x2d);
        if (fA == 0 || row[0xff] != 0 || row[0x103] == 0) {
            e->extStat(0x15, p3, p1, p2, v4, p6, p7, p8);
            e->extStat(0x16, p3, p1, p2, vl, p6, p7, p8);
        }
        const uint32_t fB = e->hasFlag(p3, 0x2d);
        if (fB == 0 || row[0x103] != 0 || row[0xfe] == 0) {
            e->extStat(0x17, p3, p1, p2, v4, p6, p7, p8);
            e->extStat(0x18, p3, p1, p2, vl, p6, p7, p8);
        }

        if (p3 != nullptr && p3[0] == 4) {
            const int32_t idx = e->unitIndex(p3);
            const uint8_t* dt = e->dataTables;
            if (idx >= 0 && idx < int32_t(Load32(dt + 0xbfc))) {
                const uint32_t rowAddr = Load32(dt + 0xbf8) + uint32_t(idx) * 0xe4u;
                if (rowAddr != 0 && reinterpret_cast<const uint8_t*>(uintptr_t(rowAddr))[0x10] != 0) {
                    e->extStat(0x9f, p3, p1, p2, v4, p6, p7, p8);
                    e->extStat(0xa0, p3, p1, p2, vl, p6, p7, p8);
                    return 1;
                }
            }
        }
        return 1;
    }

    uint32_t* h = e->statUnit(p3, p2, p7, p8);
    if (!h) {
        e->fatal(0x485);
        return 0;
    }
    int32_t a = v4;
    int32_t b = vl;
    if (p6 != 0) {
        a = -e->statValue(h, p5, 0);
        b = -e->statValue(h, p5 + 1, 0);
    }
    e->setStat(h, p5, a, 0);
    e->setStat(h, p5 + 1, b, 0);
    if (p5 == 0x39) e->setStat(h, 0x146, 1, 0);
    return 1;
}
