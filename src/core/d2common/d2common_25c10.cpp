// 1.13c D2Common+0x25C10: write one item into the save/network bit buffer. EBX = bit buffer; stack: item unit, server flag; ret 8.
// Unit layout used: +0 type (must be 4), +4 class id (ItemsTxt row), +0x10 anim mode, +0x14 item data, +0x2c path (x at +0xC, y at +0x10 for an item).
// Item data: +0x30 item format word (first field written), +0x18 flags (0x10000 = ear), +0x44 body location, +0x45 inventory page, +0x48 ear level, +0x4A ear name.
// Retail's callees are seams with their real convention (all stdcall unless noted):
//   put      0x92C4   (buffer, value, bits)                 earFile 0x22640 (item) -> ear file index
//   baseCode 0x23290  (item)                                isType 0x24430 (item, type id) -> bool
//   stat     0x38B70  (item, stat id, layer)                realm 0x22590 (item, &out0, &out1)
//   putAdj   0x23DD0  (EDX value, ESI bits, stack buffer)   fatal: the 0x921C/0x9216/0x1B0D path (noreturn in retail)
// The two item tables are module globals read through the env: ItemsTxt count/base (0x9FB94/98, row stride 0x1A8, format word at +0x30,
// quest flags at +0x12A/+0x12B) and the ItemStatCost table of the data-tables block (row stride 0x144; save-bits +0x19, shift +0x18, save-add +0x1C).
#include <cstdint>
#include <emscripten.h>

namespace {
const uint8_t* P(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t D(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(P(a) + o); }
int32_t clampi(int32_t v, int32_t hi) { return v > 0 ? (v < hi ? v : hi) : 0; }
}

extern "C" {
struct D2Common25C10Env {
    void (*put)(uint32_t buf, uint32_t value, uint32_t bits);
    uint32_t (*earFile)(uint32_t item);
    uint32_t (*baseCode)(uint32_t item);
    uint32_t (*isType)(uint32_t item, uint32_t type);
    int32_t (*stat)(uint32_t item, uint32_t stat, uint32_t layer);
    void (*realm)(uint32_t item, int32_t* out0, int32_t* out1);
    void (*putAdj)(uint32_t buf, uint32_t value, uint32_t bits);
    void (*fatal)(uint32_t code);
    uint32_t itemsCount, itemsBase;      // ItemsTxt: count and record array
    uint32_t statCount, statBase;        // ItemStatCost: record count and array (from the data-tables block)
};

// 1.13c D2Common+0x25C10
EMSCRIPTEN_KEEPALIVE void d2_port_d2common_25c10(uint32_t buf, uint32_t item, int32_t server, const D2Common25C10Env* E) {
    const uint32_t cls = item ? D(item, 4) : 0xffffffffu;
    if (cls >= E->itemsCount) { E->fatal(0x1c4d); return; }
    if (!E->itemsBase) { E->fatal(0x1ef); return; }
    const uint32_t row = E->itemsBase + cls * 0x1a8;
    if (D(item, 0) != 4) { E->fatal(0x1506); return; }
    const uint32_t idata = D(item, 0x14);
    if (!idata) { E->fatal(0x1508); return; }
    const uint32_t path = D(item, 0x2c);
    E->put(buf, clampi(*reinterpret_cast<const uint16_t*>(P(idata) + 0x30), 0x3ff), 10);
    const int32_t mode = static_cast<int32_t>(D(item, 0x10));
    E->put(buf, clampi(mode, 7), 3);
    if (mode == 3 || mode == 5) {
        E->put(buf, clampi(static_cast<int32_t>(D(path, 0xc)), 0xffff), 16);
        E->put(buf, clampi(static_cast<int32_t>(D(path, 0x10)), 0xffff), 16);
    } else {
        E->put(buf, clampi(P(idata)[0x44], 15), 4);
        E->put(buf, clampi(static_cast<int32_t>(D(path, 0xc)), 15), 4);
        E->put(buf, clampi(static_cast<int32_t>(D(path, 0x10)), 15), 4);
        E->put(buf, clampi(static_cast<int8_t>(static_cast<uint8_t>(P(idata)[0x45] + 1)), 7), 3);
    }
    if (D(idata, 0x18) & 0x10000) {
        const uint8_t* name = P(idata) + 0x4a;
        E->put(buf, clampi(static_cast<int32_t>(E->earFile(item)), 7), 3);
        E->put(buf, clampi(P(idata)[0x48], 0x7f), 7);
        do { E->put(buf, clampi(static_cast<int8_t>(*name), 0x7f), 7); } while (*name++);
    } else {
        E->put(buf, E->baseCode(item), 32);
        if (E->isType(item, 4)) {
            const int32_t gold = E->stat(item, 14, 0);
            E->put(buf, clampi(gold >= 0x1000, 1), 1);
            const uint32_t bits = gold < 0x1000 ? 12 : 32;
            uint32_t v = static_cast<uint32_t>(gold);
            if (bits != 32) {
                const int32_t top = static_cast<int32_t>((1u << bits) - 1);
                const int32_t g = gold > 0 ? gold : 0;
                v = static_cast<uint32_t>(g < top ? g : top);
            }
            E->put(buf, v, bits);
        }
    }
    if (P(row)[0x12a] && P(row)[0x12b]) {
        if (E->statCount <= 0x164 || !E->statBase) { E->fatal(0x1c7b); return; }
        const uint32_t sc = E->statBase + 0x164 * 0x144;
        const uint32_t bits = P(sc)[0x19];
        const int32_t v = E->stat(item, 0x164, 0);
        E->putAdj(buf, static_cast<uint32_t>((v >> (P(sc)[0x18] & 31)) + static_cast<int32_t>(D(sc, 0x1c))), bits);
    }
    if (server) {
        int32_t r0 = 0, r1 = 0;
        E->realm(item, &r0, &r1);
        E->put(buf, r1 != 0, 1);
        if (r1 != 0) { E->put(buf, r0, 32); E->put(buf, r1, 32); E->put(buf, 0, 32); }
    }
}
}
