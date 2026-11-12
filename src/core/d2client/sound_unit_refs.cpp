// 1.13c D2Client+0x22B20 on the client unit-insert / remove path (callers 0xA6500, 0xA6D60, 0x23630): when a client unit enters its
// hash it takes a reference on the Sounds rows its sound profile names (0x5DE90, +1 over the row's child run); when it leaves
// (0xA6D60 passes -1) it gives them back. A monster (unit type 1) names six MonSounds rows through 0x22A70, at MonSounds offsets
// +0x04, +0x1C, +0x34, +0x3C, +0x78, +0x58; an object (type 2) names the eight rows of the table at D2Client+0xF6C58, indexed by
// its class (<= 0x23C). The Sounds sweep (0x5ED90) later loads every row whose count is non-zero, which is where the A records
// for rows 0x37A.. come from. Nothing is referenced unless the Sounds table is loaded (the replay arms it; the play path does not).
// The table below is the static data of D2Client+0xF6C58 (573 pointers to 8-row arrays), as {class, slot, Sounds row}; absent = 0.
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
#include "d2client_5dfd0.h"
#include "sound_unit_refs.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Units/Monster.h"
#include "../d2moo/include/D2DataTbls.h"

extern "C" uint32_t d2_client_22a70(const D2UnitStrc* pUnit, int32_t monStatsCount, uint32_t (*cb)(uint32_t op, uint32_t arg));

namespace {
struct Ent { uint16_t cls, slot, row; };
const Ent kObj[] = {
{1,1,2545},{2,1,2604},{3,1,2545},{4,1,2636},{5,2,2547},{6,2,2547},{7,1,2645},{8,1,2632},{9,1,2636},{11,1,2534},{13,0,2564},{13,2,2563},{14,0,2564},{14,2,2563},{15,0,2569},{15,2,2568},{16,0,2569},{16,2,2568},{17,1,4},{18,1,4},{19,1,4},{20,1,4},{21,1,4},{22,1,4},{23,0,2560},{23,2,2559},{24,0,2560},{24,2,2559},{25,0,2560},{25,2,2559},{26,1,2584},{26,3,2585},{27,0,2569},{27,1,2568},{28,1,2593},{46,1,2645},{47,0,2560},{47,2,2559},{50,1,2545},{51,1,2545},{52,1,2636},{53,1,2545},{54,1,2554},{55,1,2554},{56,1,2554},{57,1,2552},{58,1,2552},{59,1,2633},{60,1,2633},{61,6,418},{62,0,2569},{62,2,2568},{63,0,2569},{63,2,2568},{64,0,2569},{64,2,2568},{74,0,2569},{74,2,2568},{75,0,2569},{75,2,2568},{77,1,2603},{79,1,2545},{83,1,2604},{84,1,2603},{85,1,2603},{86,1,2604},{87,1,2547},{88,1,2547},{89,1,2545},{91,0,2567},{91,1,2567},{92,0,2567},{92,1,2567},{93,1,2603},{94,1,2636},{95,1,2636},{96,1,2603},{97,1,2605},{98,0,2567},{98,1,2567},{99,0,2567},{99,1,2567},{100,1,2558},{104,2,2533},{105,2,2533},{106,2,2643},{107,2,2643},{108,2,2594},{109,1,2603},{110,3,525},{111,1,2644},{111,2,2644},{112,3,526},{113,1,2644},{113,2,2644},{114,3,528},{115,1,2644},{115,2,2644},{116,1,2610},{119,1,2642},{120,1,2603},{123,1,2606},{124,1,2607},{125,1,2619},{126,1,2612},{127,1,2621},{128,1,2620},{129,1,2565},{129,3,2565},{130,1,2644},{130,2,2644},{131,2,46},{132,1,2644},{132,2,2644},{133,1,2606},{137,1,2644},{137,2,2644},{138,1,2644},{138,2,2644},{139,1,2547},{140,1,2546},{141,1,2546},{142,1,2636},{143,1,2636},{144,1,2547},{145,1,2642},{146,1,2546},{147,1,2547},{148,1,2547},{149,1,2630},{153,1,2558},{154,1,2554},{155,1,2629},{156,1,2642},{157,1,2642},{158,1,2554},{159,1,2629},{163,1,2606},{164,1,2603},{165,1,2603},{166,1,2603},{167,1,2603},{168,1,2603},{169,1,1786},{170,1,2603},{171,1,2554},{172,1,2603},{173,1,2603},{174,1,2628},{175,1,2628},{176,1,2546},{177,1,2546},{178,1,2552},{179,2,2542},{180,2,2542},{181,1,2547},{182,1,2548},{183,1,2546},{184,1,2604},{185,1,2622},{186,1,2623},{187,1,2624},{188,1,2625},{189,1,2642},{190,1,2604},{191,1,2604},{193,1,2632},{194,1,2617},{195,1,2617},{196,1,2635},{197,1,2604},{198,1,2546},{199,1,2604},{200,1,2604},{202,1,2603},{203,1,2626},{204,1,2627},{205,1,2622},{206,1,2603},{207,0,2567},{207,1,2567},{208,1,2535},{209,1,2535},{222,1,2619},{223,1,2549},{224,1,2549},{225,1,2612},{226,1,2606},{228,1,2566},{228,3,2566},{229,1,2566},{229,3,2566},{230,1,2566},{230,3,2566},{231,1,2606},{232,1,2607},{233,1,2618},{234,1,2566},{234,3,2566},{235,1,2566},{235,3,2566},{236,1,2603},{237,1,2642},{238,1,2642},{240,1,2547},{241,1,2546},{242,1,2546},{243,1,2546},{244,1,2596},{246,1,2596},{247,1,2540},{248,1,2540},{249,1,2603},{250,1,407},{259,1,2579},{260,1,2608},{261,1,2635},{266,1,2590},{270,1,2554},{271,1,2554},{272,1,2554},{275,1,2603},{276,1,2603},{277,1,2604},{278,1,2603},{279,1,2604},{280,1,2603},{281,1,2603},{284,1,2545},{288,1,2642},{289,1,2540},{290,0,2564},{290,2,2563},{291,0,2564},{291,2,2563},{292,0,2564},{292,2,2563},{293,0,2564},{293,2,2563},{294,0,2569},{294,2,2568},{295,0,2569},{295,2,2568},{298,1,2556},{299,1,2606},{300,1,2606},{301,1,2603},{302,1,2603},{303,1,2604},{314,1,2554},{315,1,2554},{316,1,2554},{317,1,2554},{318,1,2554},{319,1,2603},{320,1,2603},{321,1,2633},{322,1,2644},{322,2,2644},{323,1,2642},{324,1,2642},{325,1,2604},{326,1,2554},{329,1,2547},{330,1,2547},{331,1,2547},{332,1,2547},{333,1,2547},{334,1,2547},{335,1,2547},{336,1,2547},{341,1,2543},{342,1,2561},{343,1,2603},{344,1,2603},{354,1,2548},{355,1,2548},{356,1,2548},{357,1,2632},{360,1,2600},{362,1,2570},{363,1,2554},{364,1,2571},{365,1,2545},{366,1,2617},{367,1,2601},{369,1,1981},{369,5,1981},{371,1,2547},{372,1,2612},{373,1,2579},{375,1,2547},{376,1,34},{377,1,2633},{380,1,1981},{380,5,1981},{381,1,1981},{381,5,1981},{383,1,1981},{383,5,1981},{384,1,1981},{384,5,1981},{386,1,2617},{387,1,2547},{388,1,2547},{389,1,2547},{390,1,2547},{391,1,2547},{392,1,2555},{393,1,2555},{394,1,2555},{395,1,2555},{396,1,2555},{397,1,2547},{398,1,2642},{399,1,2579},{402,1,2642},{405,1,2547},{406,1,2547},{407,1,2547},{413,1,2546},{414,1,2603},{415,1,2603},{416,1,2657},{416,5,2657},{418,1,2655},{419,1,2655},{420,1,2663},{421,1,2603},{422,1,2603},{423,1,2603},{424,1,2651},{425,1,2651},{426,1,2644},{426,2,2644},{427,1,2603},{428,1,2603},{429,1,2642},{430,1,2600},{431,1,2663},{432,1,2600},{433,1,2600},{443,1,2636},{444,1,2636},{445,1,2636},{448,1,2653},{449,1,2662},{454,1,2534},{455,1,2548},{463,1,2622},{464,1,2603},{465,1,2603},{466,1,2548},{467,1,2636},{468,1,2636},{469,1,2636},{470,1,2636},{471,1,2636},{472,1,2603},{474,3,30},{475,3,30},{476,3,30},{477,1,2554},{479,1,2603},{483,1,2603},{484,1,2603},{485,1,2545},{486,1,2545},{487,1,2545},{488,1,2603},{491,1,2603},{492,1,2603},{493,1,2644},{493,2,2644},{494,1,2642},{495,1,2603},{496,1,2642},{497,1,2603},{498,1,2644},{498,2,2644},{499,1,2603},{500,1,2657},{500,5,2657},{501,1,2547},{502,1,2547},{503,1,2603},{504,1,2546},{505,1,2547},{508,1,2652},{509,1,2603},{511,1,2642},{512,1,2603},{513,1,2644},{513,2,2644},{516,1,2622},{517,1,2622},{518,1,2645},{519,1,2644},{519,2,2644},{520,1,2603},{521,1,2603},{522,1,2603},{524,1,2545},{525,1,2545},{526,1,2545},{529,1,2545},{530,1,2545},{531,1,2545},{532,1,2545},{533,1,2545},{534,1,2545},{535,1,2645},{539,1,2642},{540,1,2658},{541,1,2600},{544,1,2600},{545,1,2659},{547,1,2650},{548,2,2643},{549,2,2643},{550,2,2533},{551,2,2533},{556,1,2659},{565,1,2633},{566,1,2633}
};

// 1.13c D2Client+0x5DE90 (ECX = row, EBX = delta): add delta to +0x8A of every record of the row's child run.
void bump(int32_t i, int32_t delta) {
    uint8_t* b = d2client_sounds::records(); const int32_t n = d2client_sounds::recordCount();
    if (!b || i <= 0 || i >= n) return;
    uint8_t* p = b + size_t(i) * 0x8e;
    int32_t cnt; memcpy(&cnt, p + 0x8a, 4);
    if (cnt == 0 && delta == -1) return;
    int32_t first; memcpy(&first, p + 0x60, 4);
    for (int32_t k = first; k < int32_t(p[0x3d]) + first; ++k) {
        if (k < 0 || k >= n) continue;
        uint8_t* c = b + size_t(k) * 0x8e; int32_t v; memcpy(&v, c + 0x8a, 4);
        if (v == 0 && delta == -1) continue;   // retail aborts here (fatal 0x1dc); not reachable with balanced insert / remove
        v += delta; memcpy(c + 0x8a, &v, 4);
    }
}
const D2UnitStrc* g_cur = nullptr;
uint32_t cbProfile(uint32_t op, uint32_t arg) {
    switch (op) {
    case 0: return (uint32_t)(uintptr_t)DATATBLS_GetSuperUniquesTxtRecord((int32_t)arg);
    case 1: return (uint32_t)(uintptr_t)DATATBLS_GetMonSoundsTxtRecordFromSoundId((int32_t)arg);
    case 2: return (uint32_t)(uintptr_t)DATATBLS_GetMonSoundsTxtRecordFromMonsterId((int32_t)arg);
    case 3: return g_cur->dwClassId;
    case 4: return (uint32_t)(uintptr_t)DATATBLS_GetMonStatsTxtRecord((int32_t)arg);
    case 5: return g_cur->pMonsterData && (g_cur->pMonsterData->nTypeFlag & 0x10) ? 1u : 0u;   // 0x4C0D0
    }
    return 0;
}
// The rows 0x22B20 would reference for the unit (empty = none).
std::vector<int32_t> rowsOf(const D2SndUnit& u) {
    std::vector<int32_t> r;
    if (u.type == 1 && u.unit) {
        g_cur = u.unit;
        const D2MonSoundsTxt* p = (const D2MonSoundsTxt*)(uintptr_t)d2_client_22a70(u.unit, sgptDataTables ? sgptDataTables->nMonStatsTxtRecordCount : 0, cbProfile);
        if (p) for (uint32_t v : { p->dwAttack1, p->dwWeapon1, p->dwHitSound, p->dwDeathSound, p->dwFlee, p->dwFootstepLayer }) r.push_back((int32_t)v);
    } else if (u.type == 2 && u.cls <= 0x23c) {
        for (const Ent& e : kObj) if (e.cls == u.cls) r.push_back(e.row);
    }
    return r;
}
std::map<uint64_t, std::vector<int32_t>> g_held;
}

// The units the client holds now (monsters and objects), whole set each call: those not seen before take their references (the
// insert path, 0xA6500 -> 0x22B20 with a positive EAX), those gone give them back (0xA6D60 -> 0x22B20 with -1).
void d2client_sound_unit_sync(const D2SndUnit* u, uint32_t n) {
    if (!d2client_sounds::records()) return;
    std::map<uint64_t, const D2SndUnit*> now;
    for (uint32_t i = 0; i < n; ++i) if (u[i].type == 1 || u[i].type == 2) now[(uint64_t(u[i].type) << 32) | u[i].id] = &u[i];
    for (auto it = g_held.begin(); it != g_held.end();) {
        if (now.count(it->first)) { ++it; continue; }
        for (int32_t r : it->second) bump(r, -1);
        it = g_held.erase(it);
    }
    for (uint32_t i = 0; i < n; ++i) {
        if (u[i].type != 1 && u[i].type != 2) continue;
        const uint64_t k = (uint64_t(u[i].type) << 32) | u[i].id;
        if (g_held.count(k)) continue;
        std::vector<int32_t> r = rowsOf(u[i]);
        for (int32_t v : r) bump(v, 1);
        g_held[k] = std::move(r);
    }
}
void d2client_sound_unit_reset() { g_held.clear(); }
// Slot `slot` (0..7) of the object class's D2Client+0xF6C58 array (0x22570's read), 0 when absent.
int32_t d2client_object_slot_row(uint32_t cls, uint32_t slot) {
    for (const Ent& e : kObj) if (e.cls == cls && e.slot == slot) return e.row;
    return 0;
}
