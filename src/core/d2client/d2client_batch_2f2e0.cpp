// src/core/d2client/d2client_batch_2f2e0.cpp -- item-description string builders, D2Client+0x2F2E0..0x30270.
// Source: cached 1.13c decompiles build-wasm/ghidra-d2client/decomp/0x2F2E0.c .. 0x30270.c (no prior art).
// Same Host seam as d2client_batch_2c8c0.cpp; unrecovered callees go through h.call(rva) / h.call(ord(n)).
// Hidden register arguments (EAX/EBX/ESI/EDI) are explicit parameters, named for the register.
// NOT done: D2Client+0x2FD30 (1317 B, jump table + stack-mangled decompile), left UNMAPPED.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <string>

namespace d2client_batch_2f2e0 {
using Address = uint32_t;

struct Host {
    void* context;
    uint8_t* (*memory)(void*, Address);
    uintptr_t (*invoke)(void*, uint32_t, const uintptr_t*, size_t);
    template<class T> T read(Address at) const { T v{}; std::memcpy(&v, memory(context, at), sizeof(v)); return v; }
    template<class T> void write(Address at, T v) const { std::memcpy(memory(context, at), &v, sizeof(v)); }
    uintptr_t call(uint32_t rva, std::initializer_list<uintptr_t> args = {}) const {
        return invoke(context, rva, args.begin(), args.size());
    }
};

constexpr Address kBase = 0x6FAB0000;
constexpr uint32_t ord(uint32_t n) { return 0x10000000u + n; } // import ordinal pseudo-rva
static uintptr_t out(const void* p) { return reinterpret_cast<uintptr_t>(p); }

static void appendCurrentRef(const Host& h, Address dst) { // Ordinal_10003() + Unicode::strcat
    const Address src = static_cast<Address>(h.call(0xD1EC));
    if (!src || !dst) return;
    Address end = dst;
    while (h.read<uint16_t>(end)) end += 2;
    for (Address p = src; h.read<uint16_t>(p); p += 2, end += 2) h.write<uint16_t>(end, h.read<uint16_t>(p));
    h.write<uint16_t>(end, 0);
}
static void appendLocal(const Host& h, Address dst, const char16_t* s) {
    if (!dst) return;
    Address end = dst;
    while (h.read<uint16_t>(end)) end += 2;
    for (size_t i = 0; s[i]; ++i, end += 2) h.write<uint16_t>(end, static_cast<uint16_t>(s[i]));
    h.write<uint16_t>(end, 0);
}
static std::u16string widen(const char* a) { std::u16string w; while (*a) w.push_back(static_cast<unsigned char>(*a++)); return w; }

// 1.13c D2Client+0x2F2E0 (82 B): EAX=item unit, ECX=dst; kind byte +0x1DC 4 -> string 0x10A9, 5 -> 0x10AA.
uint32_t sub_2F2E0(const Host& h, Address eaxUnit, Address dst, uint32_t param2) {
    uint32_t v = static_cast<uint32_t>(h.call(ord(10510), {param2}));
    if (v != 0) {
        uint8_t k = h.read<uint8_t>(eaxUnit + 0x1DC);
        if (k == 4) return static_cast<uint32_t>(h.call(0x2E060, {dst, 0x10A9, v}));
        if (k == 5) return static_cast<uint32_t>(h.call(0x2E060, {dst, 0x10AA, v}));
    }
    return 0;
}

// Shared body of 2F340/2F430: EAX*mul/3 as tenths; "%d" or "%d.%d", clamped to >=1.
static uint32_t tenths(const Host& h, Address dst, int32_t eax, int32_t mul) {
    char16_t num[32]{};
    int32_t t = (eax * mul) / 3, whole = t / 10, frac = t % 10;
    bool have = true;
    char a[32];
    if (whole < 1) { whole = 1; if (frac == 0) std::snprintf(a, sizeof a, "%d", whole); else std::snprintf(a, sizeof a, "%d.%d", whole, frac); }
    else if (whole != 0 && frac == 0) std::snprintf(a, sizeof a, "%d", whole);
    else std::snprintf(a, sizeof a, "%d.%d", whole, frac);
    if (have) { std::u16string w = widen(a); std::memcpy(num, w.c_str(), (w.size() + 1) * 2); }
    appendCurrentRef(h, dst); appendLocal(h, dst, num); appendCurrentRef(h, dst); return 1;
}
// 1.13c D2Client+0x2F340 (234 B): EAX*10/3.
uint32_t sub_2F340(const Host& h, int32_t eax, Address dst) { uint32_t r = tenths(h, dst, eax, 10); appendCurrentRef(h, dst); return r; }
// 1.13c D2Client+0x2F430 (235 B): EAX*20/3.
uint32_t sub_2F430(const Host& h, int32_t eax, Address dst) { uint32_t r = tenths(h, dst, eax, 20); appendCurrentRef(h, dst); return r; }

// 1.13c D2Client+0x2F520 (223 B): EAX=dst. lo==hi -> 0x2DCA0(p1,lo,p4); else "[+]lo-hi".
uint32_t sub_2F520(const Host& h, Address eaxDst, uint32_t p1, int32_t lo, int32_t hi, int32_t plus, int32_t extra) {
    if (lo == hi) return static_cast<uint32_t>(h.call(0x2DCA0, {eaxDst, p1, static_cast<uintptr_t>(lo), static_cast<uintptr_t>(plus)}));
    char a[48]; std::snprintf(a, sizeof a, plus ? "+%d-%d" : "%d-%d", lo, hi);
    std::u16string w = widen(a);
    appendCurrentRef(h, eaxDst);
    if (extra) appendCurrentRef(h, eaxDst);
    appendLocal(h, eaxDst, w.c_str());
    appendCurrentRef(h, eaxDst);
    return 1;
}

// 1.13c D2Client+0x2F600 (219 B): EAX=dst.
uint32_t sub_2F600(const Host& h, Address eaxDst, uint32_t p1, uint32_t p2, int32_t lo, int32_t hi, int32_t plus) {
    if (lo == hi) return static_cast<uint32_t>(h.call(0x2D9B0, {eaxDst, p1, p2, static_cast<uintptr_t>(lo), 0}));
    char a[48]; std::snprintf(a, sizeof a, plus ? "+%d-%d" : "%d-%d", lo, hi);
    std::u16string w = widen(a);
    appendCurrentRef(h, eaxDst); appendCurrentRef(h, eaxDst);
    appendLocal(h, eaxDst, w.c_str());
    appendCurrentRef(h, eaxDst);
    return 1;
}

// 1.13c D2Client+0x2F6E0 (233 B): as 2F600, but lead/trail refs are skipped when the id low word is 0x1506.
uint32_t sub_2F6E0(const Host& h, Address eaxDst, uint32_t p1, uint32_t p2, int32_t lo, int32_t hi, int32_t plus) {
    if (lo == hi) return static_cast<uint32_t>(h.call(0x2D9B0, {eaxDst, p1, p2, static_cast<uintptr_t>(lo), 0}));
    char a[48]; std::snprintf(a, sizeof a, plus ? "+%d-%d" : "%d-%d", lo, hi);
    std::u16string w = widen(a);
    if (static_cast<int16_t>(p1) != 0x1506) appendCurrentRef(h, eaxDst);
    appendLocal(h, eaxDst, w.c_str());
    if (static_cast<int16_t>(p2) != 0x1506) appendCurrentRef(h, eaxDst);
    appendCurrentRef(h, eaxDst);
    return 1;
}

// 1.13c D2Client+0x2F7D0 (155 B): EBX=value; param_3 selects the 2C7E0 (signed) vs 2C790 (range) formatter.
uint32_t sub_2F7D0(const Host& h, Address dst, uint32_t p2, int32_t p3, Address p4, int32_t ebx) {
    char16_t buf[64]{};
    if (p3 == 0) { if (ebx < 1) return 0; }
    else if (ebx == 0) { h.call(0x2C7E0, {p4, out(buf)}); goto done; }
    h.call(0x2C790, {static_cast<uintptr_t>(ebx), p4, out(buf)});
done:
    (void)p2;
    appendCurrentRef(h, dst); appendLocal(h, dst, buf); appendCurrentRef(h, dst);
    return 1;
}

// 1.13c D2Client+0x2F870 (243 B): EBX=packed value (low byte fraction, >>8 whole), p4 multiplier.
uint32_t sub_2F870(const Host& h, Address dst, int16_t p2, int16_t p3, int32_t p4, Address p5, uint32_t ebx) {
    char16_t buf[64]{};
    if (ebx == 0) return 0;
    uint32_t u = ebx & 0x800000ffu;
    if (static_cast<int32_t>(u) < 0) u = ((u - 1) | 0xffffff00u) + 1;
    int32_t prod = static_cast<int32_t>(u * static_cast<uint32_t>(p4));
    int32_t iv = (prod + ((prod >> 31) & 0xff)) >> 8;
    int32_t eb = static_cast<int32_t>(ebx);
    bool whole0 = ((eb + ((eb >> 31) & 0xff)) >> 8) == 0;
    if (whole0) { if (iv != 0) h.call(0x2C790, {static_cast<uintptr_t>(iv), p5, out(buf)}); }
    else if (iv == 0) h.call(0x2C7E0, {p5, out(buf)});
    else h.call(0x2C790, {static_cast<uintptr_t>(iv), p5, out(buf)});
    if (p2 != 0x1506) appendCurrentRef(h, dst);
    appendLocal(h, dst, buf);
    if (p3 != 0x1506) appendCurrentRef(h, dst);
    appendCurrentRef(h, dst);
    return 1;
}

// Shared skill-bonus scan for 2F980/2FB50. sgptDataTables = *(kBase+?) is host-resolved via dataTables().
// ponytail: tables are read through h.read at guest addresses supplied by the caller.
struct Tables { Address dt; }; // D2Client sgptDataTables value

// 1.13c D2Client+0x2F980 (452 B): sum per-skill bonuses over the class skill list (+0x16C, count short +0x170).
// outA = EDI target, outB = param_3 target, outC = param_4 (set from 0x2C830 on the item-type path).
uint32_t sub_2F980(const Host& h, Tables t, Address unit, Address ebxUnit, Address outB, Address outC, Address ediOutA) {
    int16_t n = h.read<int16_t>(t.dt + 0x170);
    for (int i = 0; i < n; ++i) {
        Address list = h.read<Address>(t.dt + 0x16C);
        int16_t skill = h.read<int16_t>(list + i * 2);
        uint32_t u1 = static_cast<uint32_t>(h.call(ord(10494), {unit, static_cast<uintptr_t>(skill)}));
        if (!u1) continue;
        uint32_t st = static_cast<uint32_t>(h.call(ord(10871), {unit, static_cast<uintptr_t>(h.read<int16_t>(h.read<Address>(t.dt + 0x16C) + i * 2))}));
        if (!st) continue;
        int32_t id = static_cast<int32_t>(h.call(ord(10680), {st, 0x15e, 0}));
        if (id <= 0 || id >= h.read<int32_t>(t.dt + 0xBA0)) continue;
        Address row = static_cast<Address>(id) * 0x23C + h.read<Address>(t.dt + 0xB98);
        if (!row) continue;
        int32_t lvl = static_cast<int32_t>(h.call(ord(10680), {st, 0x15f, 0}));
        if (lvl <= 0) continue;
        int16_t sv = h.read<int16_t>(row + 0x54);
        if (sv < 0 || sv >= h.read<int32_t>(t.dt + 0xBD4)) continue;
        int32_t cnt = static_cast<int32_t>(h.call(ord(10680), {st, static_cast<uintptr_t>(sv), 0}));
        if (cnt <= 0) continue;
        Address rec = static_cast<Address>(h.call(0x17F0));
        if (!rec) {
            int32_t a = static_cast<int32_t>(h.call(ord(10121), {unit, static_cast<uintptr_t>(id), static_cast<uintptr_t>(lvl), 1}));
            h.write<int32_t>(ediOutA, h.read<int32_t>(ediOutA) + (a >> 8));
            int32_t b = static_cast<int32_t>(h.call(ord(11091), {unit, static_cast<uintptr_t>(id), static_cast<uintptr_t>(lvl), 1}));
            h.write<int32_t>(outB, h.read<int32_t>(outB) + (b >> 8));
        } else {
            int32_t k = cnt < 2 ? 1 : (cnt > 2 ? 3 : cnt);
            int32_t a = static_cast<int32_t>(h.call(ord(10435), {unit, h.read<uint32_t>(rec + 0x20 + k * 4), static_cast<uintptr_t>(id), static_cast<uintptr_t>(lvl)}));
            h.write<int32_t>(ediOutA, h.read<int32_t>(ediOutA) + a);
            int32_t b = static_cast<int32_t>(h.call(ord(10435), {ebxUnit, h.read<uint32_t>(rec + 0x2c + k * 4), static_cast<uintptr_t>(id), static_cast<uintptr_t>(lvl)}));
            h.write<int32_t>(outB, h.read<int32_t>(outB) + b);
            uint8_t flag = h.read<uint8_t>(rec + 0x1f + k);
            if (flag != 0 && flag < 0xd) h.write<uint32_t>(outC, static_cast<uint32_t>(h.call(0x2C830)));
        }
    }
    return 1;
}

// 1.13c D2Client+0x2FB50 (307 B): sum of 0x2CD0(lvl)*count over skills whose row +0x44 byte == 1.
// player = the DAT_6fbcbbfc (+0xFBCBBFC-0x6FAB0000 = 0x11BBFC) client unit, passed in.
int32_t sub_2FB50(const Host& h, Tables t, Address player) {
    if (!player) return 0;
    int32_t sum = 0;
    int16_t n = h.read<int16_t>(t.dt + 0x170);
    for (int i = 0; i < n; ++i) {
        int16_t skill = h.read<int16_t>(h.read<Address>(t.dt + 0x16C) + i * 2);
        if (!h.call(ord(10494), {player, static_cast<uintptr_t>(skill)})) continue;
        uint32_t st = static_cast<uint32_t>(h.call(ord(10871), {player, static_cast<uintptr_t>(h.read<int16_t>(h.read<Address>(t.dt + 0x16C) + i * 2))}));
        if (!st) continue;
        int32_t id = static_cast<int32_t>(h.call(ord(10680), {st, 0x15e, 0}));
        if (id <= 0 || id >= h.read<int32_t>(t.dt + 0xBA0)) continue;
        Address row = static_cast<Address>(id) * 0x23C + h.read<Address>(t.dt + 0xB98);
        if (!row) continue;
        int32_t lvl = static_cast<int32_t>(h.call(ord(10680), {st, 0x15f, 0}));
        if (lvl <= 0 || h.read<uint8_t>(row + 0x44) != 1) continue;
        int16_t sv = h.read<int16_t>(row + 0x54);
        if (sv < 0 || sv >= h.read<int32_t>(t.dt + 0xBD4)) continue;
        int32_t cnt = static_cast<int32_t>(h.call(ord(10680), {st, static_cast<uintptr_t>(sv), 0}));
        if (cnt <= 0) continue;
        sum += static_cast<int32_t>(h.call(0x2CD0, {0, static_cast<uintptr_t>(lvl)})) * cnt;
    }
    return sum;
}

// 1.13c D2Client+0x30270 (198 B): ESI=text buffer, param_1, param_2=unit/ECX arg. flag = DAT_6fbc9854 (RVA 0x119854).
uint32_t sub_30270(const Host& h, Address esiBuf, uint32_t p1, Address p2) {
    if (h.read<int32_t>(kBase + 0x119854) == 0) {
        h.call(0x2E190, {esiBuf});
        h.call(0x2F520, {esiBuf, 0x10a0, 0, 0, 0, 0});
    } else {
        sub_2F2E0(h, esiBuf, p2, 0);
        h.call(0x2EF80, {esiBuf});
        h.call(0x2D000, {esiBuf, p1, p2});
        uint32_t v = static_cast<uint32_t>(h.call(ord(10653)));
        if (v) { h.call(0x2D9B0, {0x10a6, 0x10b3, v, 1}); return 1; }
    }
    return 1;
}

// 1.13c D2Client+0x2FD10 (28 B): skill-desc record (0x17F0) -> u16 at +8+idx*2, or 0x1506 when none.
uint16_t sub_2FD10(const Host& h, int32_t idx) {
    Address rec = static_cast<Address>(h.call(0x17F0));
    return rec ? h.read<uint16_t>(rec + 8 + idx * 2) : 0x1506;
}
} // namespace d2client_batch_2f2e0
