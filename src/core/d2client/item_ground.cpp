// 1.13c D2Client+0x82EA0: the ITEM branch of the client unit pass.
// Dispatched from FUN_6FADB1E0 (0x2B1E0) case 4, i.e. for `*pUnit == UNIT_ITEM`, once per frame.
// No parameters in retail -- the unit arrives in ESI and the function returns void.
//
// The whole function is a switch on dwItemMode (D2C_ItemModes, unit+0x10):
//   IMODE_STORED/EQUIP/INBELT/ONCURSOR/SOCKETED ... clear UNITFLAG_TARGETABLE (bit 1) in dwFlags
//   IMODE_ONGROUND  (3) ........................ set   UNITFLAG_TARGETABLE, then submit the cel
//   IMODE_DROPPING  (5) ........................ the drop clock: D2Common #10426 (the end-of-cycle
//                                                predicate on the unit's own frame/speed/count) says
//                                                "still dropping" -> #10759 advances it one step and
//                                                the frame ends here, flags untouched; "done" -> the
//                                                unit LANDS at IMODE_ONGROUND and the cel is submitted.
// Any other mode is a client defect: retail asserts (message 0x362) and _exit(-1), so no such mode
// reaches the branch and this implementation refuses it the same way.
//
// The four deep callees are a parameter, exactly as in the other D2Client recoveries (unit_1c150):
// `c69130` is FUN_6fb19130, the search of the unit's own COF list (pGfxData, +0x54) for component
// 0x46 (EAX = unit, ECX = 0x46), whose non-zero answer means the submit has already happened this
// mode; `c6b300` is FUN_6fb1b300, the COF component submit that builds the frame the item is DRAWN
// from (EAX = 0x46, stack unit, 6, 8, 0, 0, 8, 0); `c10426`/`c10759` are the D2Common drop-clock
// pair. It is the caller's job to point them at the real functions; the FN capture hooks all four (a
// synthetic unit has a null pGfxData and no c48 component pool, and D2Common's own clock would run on
// those synthetic clocks) and the probe injects the same four, so both sides run the same branch.
#include <cstdint>
#include <cstring>
#include <cstddef>
#include "../d2moo/include/Units/Units.h"

static_assert((uint32_t)UNITFLAG_TARGETABLE == 0x2, "bit 1 of dwFlags is UNITFLAG_TARGETABLE");

// This function addresses the unit by 1.13c byte offsets (like the other D2Client unit leaves), because
// the FN capture's blob IS the 32-bit image: retail's 0x82EA0 reads and writes those exact bytes. The
// tree's wasm64 D2UnitStrc widens pointer fields, so the same fields sit 4 bytes further along per
// pointer field ahead of them (the convention at src/core/d2moo/src/Units/Units.cpp:2127); asserting the
// tree layout catches a drift that would make these offsets wrong for a real engine unit.
static_assert(offsetof(D2UnitStrc, dwItemMode) == 0x10 + 1 * (sizeof(void*) - 4), "unit+0x10 is the item mode");
static_assert(offsetof(D2UnitStrc, pGfxData) == 0x54 + 6 * (sizeof(void*) - 4), "unit+0x54 is the COF list 0x69130 walks");
static_assert(offsetof(D2UnitStrc, nSeqCurrentFramePrecise) == 0x44 + 5 * (sizeof(void*) - 4), "the drop clock");
static_assert(offsetof(D2UnitStrc, dwFrameCountPrecise) == 0x48 + 5 * (sizeof(void*) - 4), "the drop clock's period");
static_assert(offsetof(D2UnitStrc, wAnimSpeed) == 0x4C + 5 * (sizeof(void*) - 4), "the drop clock's step");
static_assert(offsetof(D2UnitStrc, dwFlags) == 0xC4 + 18 * (sizeof(void*) - 4), "unit+0xC4 is dwFlags");

// The seam's four callees and their real addresses (1.13c: FUN_6fb19130 = 0x69130, 73 bytes;
// FUN_6fb1b300 = 0x6B300, 651 bytes; D2Common #10426 = thunk 0xC46C; D2Common #10759 = thunk 0xC3A6).
// Spelled without a marker comment: none of them is implemented here.
struct ClientItemCallees {
    uint32_t (*c69130)(uint32_t unit, uint32_t key);                  // FUN_6fb19130(EAX=unit, ECX=key)
    void (*c6b300)(uint32_t sel, uint32_t unit, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
                   uint32_t s5, uint32_t s6);                         // FUN_6fb1b300(EAX=sel, stack 7 words)
    uint32_t (*c10426)(uint32_t unit);                                // D2Common #10426 (stdcall, 1 arg)
    void (*c10759)(uint32_t unit);                                    // D2Common #10759 (stdcall, 1 arg)
};

// dwItemMode (D2C_ItemModes, D2Items.h): the five modes whose arm clears the flag.
static bool item_mode_clears_targetable(uint32_t mode) {
    return mode == 0 /*STORED*/ || mode == 1 /*EQUIP*/ || mode == 2 /*INBELT*/ ||
           mode == 4 /*ONCURSOR*/ || mode == 6 /*SOCKETED*/;
}

static uint32_t rd32(const uint8_t* u, uint32_t off) { uint32_t v; memcpy(&v, u + off, 4); return v; }
static void wr32(uint8_t* u, uint32_t off, uint32_t v) { memcpy(u + off, &v, 4); }

static const uint32_t UNIT_MODE = 0x10;   // dwItemMode / dwAnimMode (1.13c offset; the tree's is 0x14)
static const uint32_t UNIT_FLAGS = 0xC4;  // dwFlags (1.13c offset; the tree's is 0x10C)

// uint8_t* unit: the unit in memory (the blob in the FN capture, a real unit in the engine).
extern "C" void d2_client_item_pass(uint8_t* unit, const ClientItemCallees& f) {
    const uint32_t mode = rd32(unit, UNIT_MODE);
    const uint32_t flags = rd32(unit, UNIT_FLAGS);
    if (item_mode_clears_targetable(mode)) {                 // 0x82EA0: case 0/1/2/4/6
        wr32(unit, UNIT_FLAGS, flags & ~(uint32_t)UNITFLAG_TARGETABLE);
        return;
    }
    if (mode == 3) {                                         // case 3: ONGROUND
        wr32(unit, UNIT_FLAGS, flags | (uint32_t)UNITFLAG_TARGETABLE);
        if (f.c69130((uint32_t)(uintptr_t)unit, 0x46) != 0) return;
    } else if (mode == 5) {                                  // case 5: DROPPING
        if (f.c10426((uint32_t)(uintptr_t)unit) == 0) {
            f.c10759((uint32_t)(uintptr_t)unit);
            return;
        }
        wr32(unit, UNIT_MODE, 3);                            // the drop is over: land ONGROUND
    } else {
        return;                                              // retail asserts 0x362 and _exit(-1)
    }
    f.c6b300(0x46, (uint32_t)(uintptr_t)unit, 6, 8, 0, 0, 8, 0);   // the cel the item is drawn from
}
