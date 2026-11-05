// 1.13c D2Common+0x2C0C0: the compact item-record decoder (the tail read by the item bitstream reader when the
// item carries the compact flag). Retail register convention: EAX = item unit, EDI = bit buffer, then two stack
// words (the extended-fields flag, the save version), ret 8. Every callee is a seam so the function can run on
// 32-bit addresses (raw offsets; the item unit and its item data are i686 layouts).
#include <cstdint>
#include <cstring>

struct D2Common2C0C0Env {
    uint32_t (*readBits)(uint32_t buf, int32_t n);                       // Fog 10130 (thunk 0x92CA)
    int32_t  (*lookup)(uint32_t link, uint32_t code, int32_t zero);      // Fog 10213 (thunk 0x9240)
    void     (*setQuality)(uint32_t item, uint32_t v);                    // 0x2F2C0
    void     (*setFileIndex)(uint32_t item, uint32_t v);                  // 0x2F270
    void     (*setMode)(uint32_t item, uint32_t mode);                    // 0x33920
    void     (*setEarClass)(uint32_t item, uint32_t v);                   // 0x225F0
    int32_t  (*isType)(uint32_t item, int32_t type);                      // 0x24430
    void     (*setStat)(uint32_t unit, int32_t stat, int32_t v, int32_t layer);    // 0x3A740
    uint32_t (*itemCode)(uint32_t item);                                  // 0x23290
    uint32_t (*allocList)(uint32_t pool, uint32_t flags, uint32_t a, uint32_t b, uint32_t owner); // 0x38100
    void     (*addList)(uint32_t unit, uint32_t list, int32_t f);         // 0x3A7D0
    void     (*setListStat)(uint32_t list, int32_t stat, int32_t v, int32_t layer); // 0x3A280
    void     (*fatal)(uint32_t line);                                     // assert + exit; does not return in retail
    uint32_t linkTable;   // global 0x9FF6C
    uint32_t itemsTxt;    // global 0x9FB98
    uint32_t dataTables;  // sgptDataTables 0x99E1C
};

namespace {
uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint8_t r8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
void w32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
void w16(uint32_t a, uint16_t v) { std::memcpy((void*)(uintptr_t)a, &v, 2); }
void w8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
// The item data pointer when the unit is an item with data, else 0.
uint32_t idata(uint32_t it) { return r32(it) == 4 ? r32(it + 0x14) : 0; }
bool missileOrItem(uint32_t it) { const int32_t t = (int32_t)r32(it); return t == 2 || (t >= 4 && t <= 5); }
}

extern "C" int32_t D2Common_2C0C0_ReadCompact(const D2Common2C0C0Env* e, uint32_t it, uint32_t buf, int32_t bExt,
                                              uint32_t version) { // 1.13c D2Common+0x2C0C0
    const uint32_t v0 = e->readBits(buf, 10);
    if (!it) { e->fatal(0x1510); return 0; }
    if (r32(it) != 4) { e->fatal(0x1511); return 0; }
    if (!r32(it + 0x14)) { e->fatal(0x1513); return 0; }
    w16(r32(it + 0x14) + 0x30, (uint16_t)v0);
    const uint32_t mode = e->readBits(buf, 3);
    if (mode == 3 || mode == 5) {
        const uint32_t x = e->readBits(buf, 0x10);
        if (!missileOrItem(it)) { e->fatal(0x6FA); return 0; }
        w32(r32(it + 0x2C) + 0xC, x);
        const uint32_t y = e->readBits(buf, 0x10);
        if (!missileOrItem(it)) { e->fatal(0x711); return 0; }
        w32(r32(it + 0x2C) + 0x10, y);
    } else {
        const uint32_t a = e->readBits(buf, 4);
        if (uint32_t d = idata(it)) w8(d + 0x44, (uint8_t)a);
        e->setQuality(it, e->readBits(buf, 4));
        e->setFileIndex(it, e->readBits(buf, 4));
        const uint32_t b = e->readBits(buf, 3);
        if (uint32_t d = idata(it)) w8(d + 0x45, (uint8_t)(b - 1));
    }
    e->setMode(it, mode);
    const uint32_t d0 = idata(it);
    const uint32_t flags = d0 ? r32(d0 + 0x18) : 0;
    if (flags & 0x10000) {
        uint32_t p = idata(it) ? idata(it) + 0x4A : 0;
        e->setEarClass(it, e->readBits(buf, 3));
        const uint32_t lvl = e->readBits(buf, 7);
        if (uint32_t d = idata(it)) w8(d + 0x48, (uint8_t)lvl);
        uint8_t c;
        do { c = (uint8_t)e->readBits(buf, 7); w8(p, c); p++; } while (c);
    } else {
        const uint32_t code = e->readBits(buf, 0x20);
        w32(it + 4, (uint32_t)e->lookup(e->linkTable, code, 0));
        if (e->isType(it, 4)) {
            const int32_t n = e->readBits(buf, 1) ? 0x20 : 0xC;
            e->setStat(it, 0xE, (int32_t)e->readBits(buf, n), 0);
        }
        if (e->isType(it, 0x16)) {
            if (code == 0x20637374u) { if (uint32_t d = idata(it)) w16(d + 0x3E, 0); }
            else if (code == 0x20637369u) { if (uint32_t d = idata(it)) w16(d + 0x3E, 1); }
        }
    }
    if (version > 0x5C) {
        const int32_t idx = e->lookup(e->linkTable, e->itemCode(it), 0);
        if (idx < 0) return -1;
        const uint32_t rec = (uint32_t)idx * 0x1A8u + e->itemsTxt;
        if (!rec) return -1;
        if (r8(rec + 0x12A) && r8(rec + 0x12B)) {
            const uint32_t dt = e->dataTables;
            const uint32_t isc = r32(dt + 0xBCC) + 0x1C290u;
            if ((int32_t)r32(dt + 0xBD4) <= 0x164 || !isc) { e->fatal(0x1608); return 0; }
            const uint32_t raw = e->readBits(buf, r8(isc + 0x19));
            const int32_t val = (int32_t)((raw - r32(isc + 0x1C)) << (r8(isc + 0x18) & 31));
            uint32_t list = 0;
            const uint32_t sl = r32(it + 0x5C);
            if (sl && (int32_t)r32(sl + 0x10) < 0)
                for (uint32_t q = r32(sl + 0x3C); q; q = r32(q + 0x2C))
                    if (r8(q + 0x10) & 0x40) { list = q; break; }
            if (!list) {
                list = e->allocList(r32(it + 8), 0x40, 0, 4, r32(it + 0xC));
                e->addList(it, list, 1);
            }
            if (list) e->setListStat(list, 0x164, val, 0);
        }
    }
    if (bExt && version > 0x56 && e->readBits(buf, 1)) {
        const uint32_t a = e->readBits(buf, 0x20);
        const uint32_t b = e->readBits(buf, 0x20);
        if (uint32_t d = idata(it)) { w32(d + 0x1C, a); w32(d + 0x20, b); }
        if (version > 0x5D) e->readBits(buf, 0x20);
    }
    w32(it + 0x28, 0);
    w32(it + 0x20, 0);
    w32(it + 0x24, 0x29A);
    if (r32(it) == 4) {
        if (r32(it + 0x14)) w32(r32(it + 0x14) + 0x2C, 1);
        if (uint32_t d = idata(it)) w32(d, 2);
    }
    return 1;
}
