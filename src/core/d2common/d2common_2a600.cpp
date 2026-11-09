// 1.13c D2Common+0x2A600: the full item bitstream decoder (the 1.13c successor of D2MOO's
// ITEMS_DecodeItemBitstreamComplete). stdcall (unit, buf, checkHeader, gamble, pSocketed, version), ret 0x18;
// returns 1, or -1 when the stream named an unknown quality, a bad unique/set record or a doubled zero stat id.
// Every callee is a seam (register-convention callees take their registers as arguments), so the function runs on
// 32-bit addresses with the item unit, its item data and its stat lists in their i686 layouts.
#include <cstdint>
#include <cstring>

struct D2Common2A600Env {
    uint32_t (*readBits)(uint32_t buf, int32_t n);                          // Fog 10130 (thunk 0x92CA)
    int32_t  (*lookup)(uint32_t link, uint32_t code, int32_t zero);         // Fog 10213 (thunk 0x9240)
    void     (*f2f2c0)(uint32_t unit, uint32_t v);
    void     (*f2f270)(uint32_t unit, uint32_t v);
    void     (*setMode)(uint32_t unit, uint32_t mode);                       // 0x33920
    void     (*f225f0)(uint32_t unit, int32_t v);
    int32_t  (*isType)(uint32_t unit, int32_t type);                         // 0x24430
    uint32_t (*f238e0)(uint32_t unit);
    void     (*f23ac0)(uint32_t unit, uint32_t v, int32_t z);
    void     (*f23a10)(uint32_t unit, uint32_t v, int32_t z);
    void     (*f239c0)(uint32_t unit, uint32_t v);
    void     (*f23970)(uint32_t unit, uint32_t v);
    int32_t  (*setListStat)(uint32_t list, int32_t stat, int32_t v, int32_t layer);   // 0x3A280
    void     (*setStat)(uint32_t unit, int32_t stat, int32_t v, int32_t layer);       // 0x3A740
    void     (*f38550)(uint32_t key, uint32_t list);                         // EAX = key, EBX = list
    uint32_t (*itemTxt)(uint32_t cls);                                       // 0x719A0
    int32_t  (*f23e00)(uint32_t unit);
    int32_t  (*getStat)(uint32_t unit, int32_t stat, int32_t layer);         // 0x38C20
    int32_t  (*f22e20)(uint32_t unit);
    void     (*f2a3d0)(uint32_t unit, uint32_t v);
    uint32_t (*f37f80)(uint32_t sl, uint32_t state);                         // ECX = sl, EDX = state
    uint32_t (*f37f40)(uint32_t sl, uint32_t lf, uint32_t z);                // EAX = sl, ECX = lf, EDX = 0
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t z);  // thunk 0x9222: ECX, EDX, stack 3
    void     (*addList)(uint32_t unit, uint32_t list, int32_t f);            // 0x3A7D0
    void     (*f29f30)(uint32_t unit, int32_t stat);                         // ESI = unit
    void     (*decodeStat)(uint32_t b109, uint32_t version, uint32_t row, uint32_t buf, uint32_t list, int32_t s);  // 0x29D90: EAX, ECX, ESI, stack 3
    int32_t  (*find)(uint32_t arr, uint32_t key);                            // 0x382B0: EAX = arr, EDI = key
    uint32_t (*insert)(uint32_t key, uint32_t arr, uint32_t pool);           // 0x38640: EAX = key, ESI = arr, stack 1
    void     (*f39f70)(uint32_t list, uint32_t key, int32_t diff, int32_t z); // EAX = list, EDI = key, stack 2
    int32_t  (*f38b00)(uint32_t list, uint32_t key, uint32_t row);           // EBX = list, EDI = key, stack 1
    int32_t  (*f39140)(uint32_t list, int32_t stat, int32_t layer);
    void     (*remove)(uint32_t entry, uint32_t arr, uint32_t pool);         // 0x381A0: EDX = entry, ESI = arr, stack 1
    void     (*f45780)(uint32_t v, int32_t* a, int32_t* b, int32_t* c);
    void     (*fatal)(uint32_t line);                                        // assert + exit; does not return in retail
    uint32_t linkTable;   // [0x9FF6C]
    uint32_t dataTables;  // [0x99E1C]
    uint32_t g0, g1, g2;  // [0x9FBBC], [0x9FBC4], [0x9FBC8]
    uint32_t allocFile;   // the source-file string pushed to the allocator (0x940C0)
};

namespace {
uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint8_t r8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
void w32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
void w16(uint32_t a, uint16_t v) { std::memcpy((void*)(uintptr_t)a, &v, 2); }
void w8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
uint32_t idata(uint32_t it) { return r32(it) == 4 ? r32(it + 0x14) : 0; }
bool missileOrItem(uint32_t it) { const int32_t t = (int32_t)r32(it); return t == 2 || (t >= 4 && t <= 5); }
uint32_t row(uint32_t dt, int32_t s) { return (int32_t)r32(dt + 0xBD4) > s ? r32(dt + 0xBCC) + (uint32_t)s * 0x144u : 0; }
// The module's own table lookups (0x17D0 set row, 0x1800 set-item row, 0x18F0 skill check): plain reads of the data
// tables, not seams, so they run against the same tables the decoder was handed.
uint32_t setRow(uint32_t dt, int32_t id) {   // 1.13c D2Common+0x17D0
    return id >= 0 && id < (int32_t)r32(dt + 0xC10) ? (uint32_t)id * 0x128u + r32(dt + 0xC0C) : 0;
}
uint32_t setItemRow(uint32_t dt, int32_t id) {   // 1.13c D2Common+0x1800
    return id >= 0 && id < (int32_t)r32(dt + 0xC1C) ? (uint32_t)id * 0x1B8u + r32(dt + 0xC18) : 0;
}
int32_t skillValid(uint32_t dt, int32_t sk) {   // 1.13c D2Common+0x18F0
    if (sk < 0 || sk >= (int32_t)r32(dt + 0xBA0)) return 0;
    const uint32_t r = (uint32_t)sk * 0x23Cu + r32(dt + 0xB98);
    if (!r) return 0;
    int16_t d; std::memcpy(&d, (const void*)(uintptr_t)(r + 0x194), 2);
    return d >= 0 && d < (int32_t)r32(dt + 0xB94);
}
const uint32_t kSetStates[5] = { 0xA5, 0xA6, 0xA7, 0xA8, 0xA9 };

struct Ctx {
    const D2Common2A600Env* e; uint32_t unit, buf, version, b;
    uint32_t rd(uint32_t r, uint32_t bits) const { return e->readBits(buf, (int32_t)bits); }
    uint32_t bitsOf(uint32_t r) const { return r8(r + b + 0x19); }
    int32_t addOf(uint32_t r) const { return (int32_t)r32(r + b * 4 + 0x1C); }
    // read a stat by its own row and hand it to the list (skipped without a list)
    void put(uint32_t list, int32_t s, uint32_t r, uint32_t bits) const {
        const uint32_t v = e->readBits(buf, (int32_t)bits);
        if (list) e->setListStat(list, s, (int32_t)(v - (uint32_t)addOf(r)), 0);
    }
    void fixup(uint32_t list, uint32_t key) const {   // shared tail of the set-to-1 and the clear loop
        if ((int32_t)r32(list + 0x10) < 0 && !r32(list + 8)) e->f38550(key, list);
    }
    void setOne(uint32_t list, uint32_t key) const {
        const uint32_t arr = list + 0x24;
        const int32_t idx = e->find(arr, key);
        uint32_t ent = idx >= 0 ? r32(arr) + (uint32_t)idx * 8u : 0;
        if (!ent) ent = e->insert(key, arr, r32(list));
        const int32_t diff = 1 - (int32_t)r32(ent + 4);
        if (!diff) return;
        w32(ent + 4, 1);
        e->f39f70(list, key, diff, 0);
        fixup(list, key);
    }
};
}

extern "C" int32_t D2Common_2A600_DecodeItem(const D2Common2A600Env* e, uint32_t unit, uint32_t buf, int32_t checkHeader,
                                             int32_t gamble, uint32_t pSocketed, uint32_t version) { // 1.13c D2Common+0x2A600
    int32_t error = 0;
    const uint32_t dt = e->dataTables;
    const uint32_t v0 = e->readBits(buf, 10);
    if (!unit) { e->fatal(0x1510); return 0; }
    if (r32(unit) != 4) { e->fatal(0x1511); return 0; }
    if (!r32(unit + 0x14)) { e->fatal(0x1513); return 0; }
    w16(r32(unit + 0x14) + 0x30, (uint16_t)v0);
    const uint32_t mode = e->readBits(buf, 3);
    if (mode == 3 || mode == 5) {
        const uint32_t x = e->readBits(buf, 0x10);
        if (!missileOrItem(unit)) { e->fatal(0x6FA); return 0; }
        w32(r32(unit + 0x2C) + 0xC, x);
        const uint32_t y = e->readBits(buf, 0x10);
        if (!missileOrItem(unit)) { e->fatal(0x711); return 0; }
        w32(r32(unit + 0x2C) + 0x10, y);
    } else {
        const uint32_t a = e->readBits(buf, 4);
        if (uint32_t d = idata(unit)) w8(d + 0x44, (uint8_t)a);
        e->f2f2c0(unit, e->readBits(buf, 4));
        e->f2f270(unit, e->readBits(buf, 4));
        const uint32_t c = e->readBits(buf, 3);
        if (uint32_t d = idata(unit)) w8(d + 0x45, (uint8_t)(c - 1));
    }
    e->setMode(unit, mode);
    const uint32_t code = e->readBits(buf, 0x20);
    const int32_t cls = e->lookup(e->linkTable, code, 0);
    if (cls >= 0) w32(unit + 4, (uint32_t)cls);
    if (gamble) {
        if (uint32_t d = idata(unit)) w32(d + 0x2C, 1);
        if (uint32_t d = idata(unit)) w32(d, 1);
        return 1;
    }
    const uint32_t sock = e->readBits(buf, 3);
    if (pSocketed) w32(pSocketed, sock);
    if (checkHeader) {
        const uint32_t seed = e->readBits(buf, 0x20);
        w32(unit + 0x28, seed); w32(unit + 0x20, seed); w32(unit + 0x24, 0x29A);
    }
    int32_t lvl = (int32_t)e->readBits(buf, 7);
    if (lvl < 0) lvl = 1;
    if (uint32_t d = idata(unit)) w32(d + 0x2C, (uint32_t)(lvl < 1 ? 1 : lvl));
    const uint32_t q0 = e->readBits(buf, 4);
    if (uint32_t d = idata(unit)) w32(d, q0);
    if (e->readBits(buf, 1)) {
        const uint32_t v = e->readBits(buf, 3);
        if (uint32_t d = idata(unit)) w8(d + 0x49, (uint8_t)v);
    }
    int32_t pre = ((int32_t)(e->g1 - e->g0)) / 144, am = ((int32_t)(e->g2 - e->g0)) / 144;
    switch (version) {
    case 0x48: case 0x49: case 0x50: case 0x51: case 0x52: pre -= 0x295; am -= 0x4EE; break;
    case 0x53: case 0x54: pre -= 0x2A2; am -= 0x4F1; break;
    case 0x55: case 0x56: case 0x57: pre -= 0x2A3; am -= 0x4F2; break;
    case 0x58: pre -= 0x2D9; am -= 0x5A0; break;
    default: break;
    }
    if (e->readBits(buf, 1)) {
        uint32_t v = e->readBits(buf, 0xB);
        if (version <= 0x58) v = 0;
        else {
            if (version <= 0x59) v++;
            if ((uint16_t)v) v += (uint32_t)am;
        }
        if (uint32_t d = idata(unit)) w16(d + 0x36, (uint16_t)v);
    } else if (uint32_t d = idata(unit)) w16(d + 0x36, 0);

    auto hdr = [&]() { return checkHeader || (e->f238e0(unit) & 0x10); };
    const uint32_t qd = idata(unit);
    const uint32_t q = qd ? r32(qd) : 2;
    switch (q) {
    case 1: case 3: e->f225f0(unit, (int32_t)e->readBits(buf, 3)); break;
    case 2:
        if (e->isType(unit, 0xD) && hdr()) {
            if (e->readBits(buf, 1)) {
                uint32_t v = e->readBits(buf, 0xB);
                if ((uint16_t)v) v += (uint32_t)pre;
                e->f23ac0(unit, v, 0);
            } else e->f23a10(unit, e->readBits(buf, 0xB), 0);
        }
        if (e->isType(unit, 0x28) && !e->isType(unit, 7)) e->f225f0(unit, (int32_t)e->readBits(buf, 0xA));
        if (e->isType(unit, 0x16) || e->isType(unit, 0x12)) e->f23a10(unit, e->readBits(buf, 5), 0);
        break;
    case 4:
        if (hdr()) {
            uint32_t v = e->readBits(buf, 0xB);
            if ((uint16_t)v) v += (uint32_t)pre;
            e->f23ac0(unit, v, 0);
            e->f23a10(unit, e->readBits(buf, 0xB), 0);
        }
        break;
    case 5:
        if (hdr()) {
            if (version <= 0x5C) {
                const uint32_t rec = setRow(dt, (int32_t)e->readBits(buf, 0xC));
                int32_t i = 0;
                const int32_t n = rec ? (int32_t)r32(rec + 0xC) : 0;
                while (i < n && r32(r32(rec + 0x110 + 4u * (uint32_t)i) + 0x28) != code) i++;
                if (!rec || n <= 0 || i >= n) error = 1;
                else e->f225f0(unit, (int16_t)(r32(r32(rec + 0x110 + 4u * (uint32_t)i)) & 0xFFFF));
            } else {
                const uint32_t rec = setItemRow(dt, (int32_t)e->readBits(buf, 0xC));
                if (!rec) error = 1;
                else e->f225f0(unit, (int16_t)(r32(rec) & 0xFFFF));
            }
        }
        break;
    case 6: case 8:
        if (hdr()) {
            e->f239c0(unit, e->readBits(buf, 8));
            e->f23970(unit, e->readBits(buf, 8));
        }
        for (uint32_t o = 0x3E; o < 0x44; o += 2) {
            if (e->readBits(buf, 1)) {
                uint32_t v = e->readBits(buf, 0xB);
                if (version <= 0x59) v++;
                if ((uint16_t)v) v += (uint32_t)pre;
                if (uint32_t d = idata(unit)) w16(d + o - 6, (uint16_t)v);
            } else if (uint32_t d = idata(unit)) w16(d + o - 6, 0);
            if (e->readBits(buf, 1)) {
                uint32_t v = e->readBits(buf, 0xB);
                if (version <= 0x59) v++;
                if (uint32_t d = idata(unit)) w16(d + o, (uint16_t)v);
            } else if (uint32_t d = idata(unit)) w16(d + o, 0);
        }
        break;
    case 7:
        if (hdr()) {
            int32_t v = (int32_t)e->readBits(buf, 0xC);
            if (v < 0) v = -1;
            if (v >= (int32_t)r32(dt + 0xC28)) v = -1;
            e->f225f0(unit, v);
        }
        break;
    case 9:
        if (hdr()) {
            e->f239c0(unit, e->readBits(buf, 8));
            e->f23970(unit, e->readBits(buf, 8));
        }
        break;
    default: error = 1; break;
    }

    int32_t bRune = 0;
    auto flags = [&]() { const uint32_t d = idata(unit); return d ? r32(d + 0x18) : 0u; };
    if (flags() & 0x4000000) {
        bRune = 1;
        const uint32_t v = e->readBits(buf, 0x10);
        if (uint32_t d = idata(unit)) w16(d + 0x38, (uint16_t)v);
    }
    if (flags() & 0x10000) {
        uint32_t p = idata(unit) ? idata(unit) + 0x4A : 0;
        e->f225f0(unit, (int32_t)e->readBits(buf, 3));
        const uint32_t lv = e->readBits(buf, 7);
        if (uint32_t d = idata(unit)) w8(d + 0x48, (uint8_t)lv);
        uint8_t c;
        do { c = (uint8_t)e->readBits(buf, 7); w8(p, c); p++; } while (c);
    } else if (flags() & 0x1000000) {
        uint32_t p = idata(unit) ? idata(unit) + 0x4A : 0;
        uint8_t c;
        do { c = (uint8_t)e->readBits(buf, 7); w8(p, c); p++; } while (c);
    }
    if (checkHeader && version > 0x56 && e->readBits(buf, 1)) {
        const uint32_t a = e->readBits(buf, 0x20);
        const uint32_t b = e->readBits(buf, 0x20);
        if (version > 0x5D) e->readBits(buf, 0x20);
        if (uint32_t d = idata(unit)) { w32(d + 0x1C, a); w32(d + 0x20, b); }
    }
    const uint32_t b = version < 0x5D ? 1u : 0u;
    const Ctx k{ e, unit, buf, version, b };

    auto durability = [&]() {
        const uint32_t r49 = row(dt, 0x49);
        const int32_t dur = (int32_t)(e->readBits(buf, k.bitsOf(r49)) - (uint32_t)k.addOf(r49));
        e->setStat(unit, 0x49, dur, 0);
        if (dur) {
            const uint32_t r48 = row(dt, 0x48);
            const uint32_t bits = version > 0x5F ? k.bitsOf(r48) : 8u;
            e->setStat(unit, 0x48, (int32_t)(e->readBits(buf, bits) - (uint32_t)k.addOf(r48)), 0);
        }
    };
    if (e->isType(unit, 0x32)) {
        const uint32_t r1f = row(dt, 0x1F);
        const uint32_t v = e->readBits(buf, k.bitsOf(r1f)) - (uint32_t)k.addOf(r1f);
        if (e->setListStat(r32(unit + 0x5C), 0x1F, (int32_t)v, 0) && r32(unit) == 0) e->f38550(0x1F0000, r32(unit + 0x5C));
        const uint32_t txt = e->itemTxt(r32(unit + 4));
        if (!txt) { e->fatal(0x18B7); return 0; }
        e->setStat(unit, 0x14, r8(txt + 0x111), 0);
        e->setStat(unit, 0x43, -(int32_t)r32(txt + 0xD8), 0);
        durability();
    } else if (e->isType(unit, 0x2D)) {
        const uint32_t txt = e->itemTxt(r32(unit + 4));
        if (!txt) { e->fatal(0x18CE); return 0; }
        e->setStat(unit, 0x44, -(int32_t)r32(txt + 0xD8), 0);
        const uint32_t d = idata(unit);
        if (d && r32(d) == 1) {
            auto low = [&](uint32_t off, int32_t floor) { const int32_t v = (int32_t)r8(txt + off) * 3 / 4; return v < floor ? floor : v; };
            if (r8(txt + 0xFF)) e->setStat(unit, 0x16, low(0xFF, 2), 0);
            if (r8(txt + 0xFE)) e->setStat(unit, 0x15, low(0xFE, 1), 0);
            if (r8(txt + 0x103)) e->setStat(unit, 0x18, low(0x103, 2), 0);
            if (r8(txt + 0x102)) e->setStat(unit, 0x17, low(0x102, 1), 0);
            if (r8(txt + 0x101)) {
                e->setStat(unit, 0x9F, low(0x100, 1), 0);
                e->setStat(unit, 0xA0, low(0x101, 2), 0);
            }
        } else {
            e->setStat(unit, 0x16, r8(txt + 0xFF), 0);
            e->setStat(unit, 0x15, r8(txt + 0xFE), 0);
            e->setStat(unit, 0x18, r8(txt + 0x103), 0);
            e->setStat(unit, 0x17, r8(txt + 0x102), 0);
            if (r8(txt + 0x101)) {
                e->setStat(unit, 0x9F, r8(txt + 0x100), 0);
                e->setStat(unit, 0xA0, r8(txt + 0x101), 0);
            }
        }
        if (e->f23e00(unit)) {
            static const int32_t kEth[6] = { 0x15, 0x16, 0x17, 0x18, 0x9F, 0xA0 };
            for (int32_t s : kEth) {
                const int32_t v = e->getStat(unit, s, 0);
                e->setStat(unit, s, (int32_t)((uint32_t)v * 3u) / 2, 0);
            }
        }
        durability();
    } else if (e->isType(unit, 4)) {
        const int32_t n = e->readBits(buf, 1) ? 0x20 : 0xC;
        e->setStat(unit, 0xE, (int32_t)e->readBits(buf, n), 0);
    }
    if (e->f22e20(unit)) e->setStat(unit, 0x46, (int32_t)e->readBits(buf, version > 0x50 ? 9 : 8), 0);
    if (flags() & 0x800) e->f2a3d0(unit, e->readBits(buf, k.bitsOf(row(dt, 0xC2))));
    if (!checkHeader && !(flags() & 0x10)) return error ? -1 : 1;

    int32_t count = 0;
    uint32_t setbits = 0;
    if (version > 0x54) { const uint32_t d = idata(unit); if (d && r32(d) == 5) { count = 5; setbits = e->readBits(buf, 5); } }
    if (bRune) count++;
    for (int32_t i = -1; i < count; i++) {
        uint32_t state, lf;
        if (i < 0) { state = 0; lf = 0x40; }
        else if (bRune && i == count - 1) { state = 0xAB; lf = 0x40; }
        else {
            if (!(setbits & (1u << (i & 31)))) continue;
            state = kSetStates[i]; lf = 0x2040;
        }
        uint32_t list = 0;
        const uint32_t sl = r32(unit + 0x5C);
        if (sl && (int32_t)r32(sl + 0x10) < 0) list = state ? e->f37f80(sl, state) : e->f37f40(sl, lf, 0);
        if (!list) {
            const uint32_t pool = r32(unit + 8), owner = r32(unit + 0xC);
            list = e->alloc(pool, 0x3C, e->allocFile, 0x56A, 0);
            std::memset((void*)(uintptr_t)list, 0, 0x3C);
            w32(list, pool); w32(list + 8, 4); w32(list + 0xC, owner); w32(list + 0x10, lf); w32(list + 0x18, 0);
            e->addList(unit, list, 1);
            w32(list + 0x14, state);
        }
        int32_t prev = -1;
        uint32_t s = e->readBits(buf, 9);
        while (s != 0x1FF) {
            if (s == 0 && prev == 0) { error = 1; break; }
            prev = (int32_t)s;
            const int32_t si = (int32_t)s;
            if (si >= (int32_t)r32(dt + 0xBD4) || si < 0) break;
            const uint32_t r = r32(dt + 0xBCC) + s * 0x144u;
            if (!r) break;
            const bool old = version <= 0x5C;
            auto dflt = [&]() { e->decodeStat(b, version, r, buf, list, si); };
            switch (s) {
            case 0: case 2: k.put(list, si, r, k.bitsOf(r)); break;
            case 3:
                k.put(list, 3, r, k.bitsOf(r));
                if (version <= 0x55) { const uint32_t r7 = row(dt, 7); const uint32_t v = e->readBits(buf, k.bitsOf(r7));
                    if (list) e->setListStat(list, 7, (int32_t)((v - (uint32_t)k.addOf(r7)) << 8), 0); }
                break;
            case 1:
                k.put(list, 1, r, k.bitsOf(r));
                if (version <= 0x55) { const uint32_t r9 = row(dt, 9); k.put(list, 9, r9, k.bitsOf(r9)); }
                break;
            case 0x11:
                e->f29f30(unit, 0x11);
                k.put(list, 0x11, r, k.bitsOf(r));
                e->f29f30(unit, 0x12);
                { const uint32_t r12 = row(dt, 0x12); k.put(list, 0x12, r12, k.bitsOf(r12)); }
                break;
            case 0x30:
                k.put(list, 0x30, r, version > 0x51 ? k.bitsOf(r) : 6);
                { const uint32_t r2 = row(dt, 0x31); k.put(list, 0x31, r2, version > 0x51 ? k.bitsOf(r2) : 7); }
                break;
            case 0x32:
                k.put(list, 0x32, r, k.bitsOf(r));
                { const uint32_t r2 = row(dt, 0x33); k.put(list, 0x33, r2, version > 0x51 ? k.bitsOf(r2) : 7); }
                break;
            case 0x34:
                k.put(list, 0x34, r, k.bitsOf(r));
                { const uint32_t r2 = row(dt, 0x35); k.put(list, 0x35, r2, k.bitsOf(r2)); }
                break;
            case 0x36:
                k.put(list, 0x36, r, k.bitsOf(r));
                { const uint32_t r2 = row(dt, 0x37); k.put(list, 0x37, r2, version > 0x51 ? k.bitsOf(r2) : 7); }
                { const uint32_t r3 = row(dt, 0x38); k.put(list, 0x38, r3, k.bitsOf(r3)); }
                break;
            case 0x39:
                k.put(list, 0x39, r, version > 0x51 ? k.bitsOf(r) : 7);
                { const uint32_t r2 = row(dt, 0x3A); k.put(list, 0x3A, r2, version > 0x51 ? k.bitsOf(r2) : 8); }
                { const uint32_t r3 = row(dt, 0x3B); k.put(list, 0x3B, r3, k.bitsOf(r3)); }
                if (list) k.setOne(list, 0x1460000);
                break;
            case 0x86:
                if (version <= 0x59) { e->readBits(buf, 0x10); if (list) k.setOne(list, 0x860000); }
                else dflt();
                break;
            case 0x53: case 0x54: case 0x55: case 0x56: case 0x57: case 0xB3: case 0xB4:
                if (old) {
                    const int32_t layer = s == 0x53 ? 0 : s == 0x54 ? 3 : s == 0x55 ? 2 : s == 0x56 ? 1 : s == 0x57 ? 4 : s == 0xB3 ? 5 : 6;
                    e->setListStat(list, 0x53, (int32_t)e->readBits(buf, 3), layer);
                } else dflt();
                break;
            case 0xBC: case 0xBD: case 0xBE: case 0xBF: case 0xC0: case 0xC1:
                if (old) {
                    int32_t A = 0, B = 0, C = 0;
                    e->f45780(e->readBits(buf, 0xA), &A, &B, &C);
                    if (C > 7) C = 7;
                    e->setListStat(list, 0xBC, C, B + A * 8);
                } else dflt();
                break;
            case 0x7E:
                if (old) e->setListStat(list, 0x7E, (int32_t)e->readBits(buf, 4), 1);
                else dflt();
                break;
            case 0x6B: case 0x6C: case 0x6D: case 0xB5: case 0xB6: case 0xB7: case 0xB8: case 0xB9: case 0xBA: case 0xBB:
                if (old) {
                    const uint32_t v = e->readBits(buf, 0xE);
                    const uint32_t sk = v & 0x1FF;
                    const int32_t lv = (int32_t)((v >> 9) & 0x1F);
                    const uint32_t r6b = row(dt, 0x6B);
                    const int32_t lim = r6b ? (int32_t)((1u << (r8(r6b + 0x19) & 31)) - 1) : 7;
                    if (lv > 0 && lv <= lim && skillValid(dt, (int32_t)sk)) e->setListStat(list, 0x6B, lv, (int32_t)sk);
                } else dflt();
                break;
            case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7: case 0xC8: case 0xC9: case 0xCA: case 0xCB:
                if (old) {
                    const uint32_t v = e->readBits(buf, 0x15);
                    const int32_t base = s < 0xC6 ? 0xC3 : s < 0xC9 ? 0xC6 : 0xC9;
                    e->setListStat(list, base, (int32_t)((v >> 14) & 0x7F), (int32_t)(((v >> 9) & 0x1F) + ((v & 0x1FF) << 6)));
                } else dflt();
                break;
            case 0xCC: case 0xCD: case 0xCE: case 0xCF: case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5:
                if (old) {
                    const uint32_t v = e->readBits(buf, version <= 0x49 ? 0x1D : 0x1E);
                    const uint32_t param = (((v >> 9) & r8(dt + 0xC70)) & 0x1F) + ((v & 0x1FF) << (r32(dt + 0xC6C) & 31));
                    const uint32_t value = (((v >> 22) & 0xFF) << 8) + ((v >> 14) & 0xFF);
                    e->setListStat(list, 0xCC, (int32_t)value, (int32_t)param);
                } else dflt();
                break;
            default: dflt(); break;
            }
            s = e->readBits(buf, 9);
        }
        // sets of all-five resistances read in the pre-1.10 layout are folded into the single all-resist stat
        if (version > 0x5C || !list || (int32_t)r32(dt + 0xBD4) <= 0x53) continue;
        const uint32_t r53 = r32(dt + 0xBCC) + 0x690Cu;
        if (!r53) continue;
        const int32_t c0 = e->f38b00(list, 0x530000, r53);
        if (c0 <= 0) continue;
        if (e->f38b00(list, 0x530001, r53) != c0) continue;
        if (e->f38b00(list, 0x530002, r53) != c0) continue;
        if (e->f39140(list, 0x53, 3) != c0) continue;
        if (e->f39140(list, 0x53, 4) != c0) continue;
        for (uint32_t p = 0; p < 7; p++) {
            const uint32_t key = 0x530000u + p, arr = list + 0x24;
            const int32_t idx = e->find(arr, key);
            if (idx < 0) continue;
            const uint32_t ent = r32(arr) + (uint32_t)idx * 8u;
            if (!ent) continue;
            const int32_t neg = -(int32_t)r32(ent + 4);
            if (!neg) continue;
            e->remove(ent, arr, r32(list));
            e->f39f70(list, key, neg, 0);
            k.fixup(list, key);
        }
        e->setListStat(list, 0x7F, c0, 0);
    }
    return error ? -1 : 1;
}
