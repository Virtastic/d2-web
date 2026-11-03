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

// 1.13c D2Client+0x2F2E0 (82 B): EAX, ECX, EDX regs (CEB6 stdcall (edx, ecx, eax, 0)), stack p1 (dst), p2 (item, kind byte +0x1DC): kind 4 uses key 0x10A9, kind 5 key 0x10AA.
uint32_t sub_2F2E0(const Host& h, uint32_t eax, uint32_t ecx, uint32_t edx, Address p1, Address p2) {
    uint32_t v = static_cast<uint32_t>(h.call(0xCEB6, {edx, ecx, eax, 0}));
    if (!v) return 0;
    uint8_t k = h.read<uint8_t>(p2 + 0x1DC);
    if (k != 4 && k != 5) return 0;
    return static_cast<uint32_t>(h.call(0x2E060, {0, p1, k == 4 ? 0x10A9u : 0x10AAu, v}));   // EAX = 0
}

static void ref(const Host& h, Address dst, uint32_t key) { // D2Lang string (0xD1EC thunk, ECX=key) strcat'd onto dst
    const Address src = static_cast<Address>(h.call(0xD1EC, {key}));
    if (!src || !dst) return;
    Address end = dst;
    while (h.read<uint16_t>(end)) end += 2;
    for (Address p = src; h.read<uint16_t>(p); p += 2, end += 2) h.write<uint16_t>(end, h.read<uint16_t>(p));
    h.write<uint16_t>(end, 0);
}

// Shared body of 2F340/2F430 (retail: EAX=value, stack dst, key, ret 8): t = value*mul/3 (trunc), whole = t/10 clamped to >=1,
// frac = t%10; text "%d" or "%d.%d"; D2Lang(key), number, D2Lang(whole==1 ? 0x10C0 : 0x10B6), D2Lang(0xF9E).
static uint32_t tenths(const Host& h, int32_t eax, Address dst, uint32_t key, int32_t mul) {
    int32_t t = static_cast<int32_t>(static_cast<uint32_t>(eax) * static_cast<uint32_t>(mul)) / 3;
    int32_t whole = t / 10, frac = t % 10;
    if (whole < 1) whole = 1;
    char a[48];
    if (frac == 0) std::snprintf(a, sizeof a, "%d", whole); else std::snprintf(a, sizeof a, "%d.%d", whole, frac);
    std::u16string w = widen(a);
    ref(h, dst, key); appendLocal(h, dst, w.c_str());
    ref(h, dst, whole == 1 ? 0x10C0u : 0x10B6u);
    ref(h, dst, 0xF9E);
    return 1;
}
// 1.13c D2Client+0x2F340 (234 B): EAX=value, stack dst, key (ret 8); value*10/3.
uint32_t sub_2F340(const Host& h, int32_t eax, Address dst, uint32_t key) { return tenths(h, eax, dst, key, 10); }
// 1.13c D2Client+0x2F430 (235 B): same with value*20/3.
uint32_t sub_2F430(const Host& h, int32_t eax, Address dst, uint32_t key) { return tenths(h, eax, dst, key, 20); }

static std::u16string rangeText(int32_t lo, int32_t hi, int32_t plus) {
    char a[48]; std::snprintf(a, sizeof a, plus ? "+%d-%d" : "%d-%d", lo, hi);
    return widen(a);
}

// 1.13c D2Client+0x2F520 (223 B): EAX=dst (kept in EBX), stack key, lo, hi, plus, extra (ret 0x14).
// lo==hi: 0x2DCA0(EBX=dst; key, lo, plus). Else D2Lang(key), [D2Lang(0xF9B) if extra], "[+]lo-hi", D2Lang(0xF9E).
uint32_t sub_2F520(const Host& h, Address eaxDst, uint32_t key, int32_t lo, int32_t hi, int32_t plus, int32_t extra) {
    if (lo == hi) return static_cast<uint32_t>(h.call(0x2DCA0, {eaxDst, key, static_cast<uintptr_t>(static_cast<uint32_t>(lo)), static_cast<uintptr_t>(static_cast<uint32_t>(plus))}));
    std::u16string w = rangeText(lo, hi, plus);
    ref(h, eaxDst, key);
    if (extra) ref(h, eaxDst, 0xF9B);
    appendLocal(h, eaxDst, w.c_str());
    ref(h, eaxDst, 0xF9E);
    return 1;
}

// 1.13c D2Client+0x2F600 (219 B): EAX=dst, stack k1, k2, lo, hi, plus (ret 0x14).
// lo==hi: 0x2D9B0(EBX=dst; k1, k2, lo, 0). Else D2Lang(k1), D2Lang(k2), "[+]lo-hi", D2Lang(0xF9E).
uint32_t sub_2F600(const Host& h, Address eaxDst, uint32_t k1, uint32_t k2, int32_t lo, int32_t hi, int32_t plus) {
    if (lo == hi) return static_cast<uint32_t>(h.call(0x2D9B0, {eaxDst, k1, k2, static_cast<uintptr_t>(static_cast<uint32_t>(lo)), 0}));
    std::u16string w = rangeText(lo, hi, plus);
    ref(h, eaxDst, k1); ref(h, eaxDst, k2);
    appendLocal(h, eaxDst, w.c_str());
    ref(h, eaxDst, 0xF9E);
    return 1;
}

// 1.13c D2Client+0x2F6E0 (233 B): as 2F600, but D2Lang(k1) comes first and D2Lang(k2) after the number; each is skipped when its low word is 0x1506.
uint32_t sub_2F6E0(const Host& h, Address eaxDst, uint32_t k1, uint32_t k2, int32_t lo, int32_t hi, int32_t plus) {
    if (lo == hi) return static_cast<uint32_t>(h.call(0x2D9B0, {eaxDst, k1, k2, static_cast<uintptr_t>(static_cast<uint32_t>(lo)), 0}));
    std::u16string w = rangeText(lo, hi, plus);
    if (static_cast<uint16_t>(k1) != 0x1506) ref(h, eaxDst, k1);
    appendLocal(h, eaxDst, w.c_str());
    if (static_cast<uint16_t>(k2) != 0x1506) ref(h, eaxDst, k2);
    ref(h, eaxDst, 0xF9E);
    return 1;
}

// 1.13c D2Client+0x2F7D0 (155 B): EBX=value, stack dst, key, mode, rec (ret 0x10).
// mode==0 needs value>0 (else returns 0). value!=0: 0x2C790(eax=mode, ecx=value, edx=rec, [buf]); value==0: 0x2C7E0(eax=mode, ecx=rec, [buf]).
// Text: D2Lang(key), buf, D2Lang(0xF9E).
uint32_t sub_2F7D0(const Host& h, Address dst, uint32_t key, int32_t mode, Address rec, int32_t ebx) {
    char16_t buf[32]{};
    if (mode == 0 && ebx <= 0) return 0;
    if (ebx != 0) h.call(0x2C790, {static_cast<uintptr_t>(static_cast<uint32_t>(mode)), static_cast<uintptr_t>(static_cast<uint32_t>(ebx)), rec, out(buf)});
    else h.call(0x2C7E0, {static_cast<uintptr_t>(static_cast<uint32_t>(mode)), rec, out(buf)});
    ref(h, dst, key); appendLocal(h, dst, buf); ref(h, dst, 0xF9E);
    return 1;
}

// 1.13c D2Client+0x2F870 (243 B): EBX=packed 24.8 value, stack dst, k1, k2, mul, rec (ret 0x14).
// iv = (((ebx & 0x800000FF) sign-fixed) * mul) >> 8 (round toward zero), whole = ebx >> 8 (toward zero).
// whole==0: iv!=0 ? 0x2C790(whole, iv, rec, [buf]) : nothing. whole!=0: iv==0 ? 0x2C7E0(whole, rec, [buf]) : 0x2C790(whole, iv, rec, [buf]).
uint32_t sub_2F870(const Host& h, Address dst, uint32_t k1, uint32_t k2, int32_t mul, Address rec, uint32_t ebx) {
    char16_t buf[32]{};
    if (ebx == 0) return 0;
    uint32_t u = ebx & 0x800000ffu;
    if (static_cast<int32_t>(u) < 0) u = ((u - 1) | 0xffffff00u) + 1;
    int32_t prod = static_cast<int32_t>(u * static_cast<uint32_t>(mul));
    int32_t iv = (prod + ((prod >> 31) & 0xff)) >> 8;
    int32_t eb = static_cast<int32_t>(ebx);
    int32_t whole = (eb + ((eb >> 31) & 0xff)) >> 8;
    auto w32 = [](int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); };
    if (whole == 0) { if (iv != 0) h.call(0x2C790, {w32(whole), w32(iv), rec, out(buf)}); }
    else if (iv == 0) h.call(0x2C7E0, {w32(whole), rec, out(buf)});
    else h.call(0x2C790, {w32(whole), w32(iv), rec, out(buf)});
    if (static_cast<uint16_t>(k1) != 0x1506) ref(h, dst, k1);
    appendLocal(h, dst, buf);
    if (static_cast<uint16_t>(k2) != 0x1506) ref(h, dst, k2);
    ref(h, dst, 0xF9E);
    return 1;
}

// Shared skill-bonus scan for 2F980/2FB50. sgptDataTables = *(kBase+?) is host-resolved via dataTables().
// ponytail: tables are read through h.read at guest addresses supplied by the caller.
struct Tables { Address dt; }; // D2Client sgptDataTables value

// 1.13c D2Client+0x2F980 (452 B), stdcall(unit, outA, outB, outC) ret 0x10: for each skill of the class skill list (+0x16C, count short +0x170) the unit knows
// (C1FC, C1C0), skill row id (C4B4 0x15E) and level (0x15F) and value (row +0x54): with no 0x17F0 record (EAX=id) *outA += CEC2(unit,id,lvl,1)>>8, *outB += CECE(...)>>8;
// else k = clamp(value,1,3): *outA += CEE0(unit, rec[+0x20+k*4], id, lvl), *outB += CEE0(unit, rec[+0x2C+k*4], id, lvl), and a flag byte rec[+0x1F+k] in 1..0xC sets *outC = 0x2C830(EAX=flag).
uint32_t sub_2F980(const Host& h, Tables t, Address unit, Address outA, Address outB, Address outC) {
    auto w32 = [](int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); };
    for (int i = 0; i < h.read<int16_t>(t.dt + 0x170); ++i) {
        int16_t skill = h.read<int16_t>(h.read<Address>(t.dt + 0x16C) + i * 2);
        if (!h.call(ord(10494), {unit, w32(skill)})) continue;
        skill = h.read<int16_t>(h.read<Address>(t.dt + 0x16C) + i * 2);
        uint32_t st = static_cast<uint32_t>(h.call(ord(10871), {unit, w32(skill)}));
        if (!st) continue;
        int32_t id = static_cast<int32_t>(h.call(ord(10680), {st, 0x15e, 0}));
        if (id <= 0 || id >= h.read<int32_t>(t.dt + 0xBA0)) continue;
        Address row = static_cast<Address>(id) * 0x23C + h.read<Address>(t.dt + 0xB98);
        if (!row) continue;
        int32_t lvl = static_cast<int32_t>(h.call(ord(10680), {st, 0x15f, 0}));
        if (lvl <= 0) continue;
        int16_t sv = h.read<int16_t>(row + 0x54);
        if (sv < 0 || sv >= h.read<int32_t>(t.dt + 0xBD4)) continue;
        int32_t cnt = static_cast<int32_t>(h.call(ord(10680), {st, w32(sv), 0}));
        if (cnt <= 0) continue;
        Address rec = static_cast<Address>(h.call(0x17F0, {w32(id)}));
        if (!rec) {
            int32_t a = static_cast<int32_t>(h.call(ord(10121), {unit, w32(id), w32(lvl), 1}));
            h.write<int32_t>(outA, h.read<int32_t>(outA) + (a >> 8));
            int32_t b = static_cast<int32_t>(h.call(ord(11091), {unit, w32(id), w32(lvl), 1}));
            h.write<int32_t>(outB, h.read<int32_t>(outB) + (b >> 8));
        } else {
            int32_t k = cnt < 2 ? 1 : (cnt > 2 ? 3 : cnt);
            int32_t a = static_cast<int32_t>(h.call(ord(10435), {unit, h.read<uint32_t>(rec + 0x20 + k * 4), w32(id), w32(lvl)}));
            h.write<int32_t>(outA, h.read<int32_t>(outA) + a);
            int32_t b = static_cast<int32_t>(h.call(ord(10435), {unit, h.read<uint32_t>(rec + 0x2c + k * 4), w32(id), w32(lvl)}));
            h.write<int32_t>(outB, h.read<int32_t>(outB) + b);
            uint8_t flag = h.read<uint8_t>(rec + 0x1f + k);
            if (flag != 0 && flag < 0xd) h.write<uint32_t>(outC, static_cast<uint32_t>(h.call(0x2C830, {flag})));
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
        sum += static_cast<int32_t>(h.call(0x2CD0, {static_cast<uintptr_t>(id), static_cast<uintptr_t>(lvl)})) * cnt;
    }
    return sum;
}

// dup of D2Client+0x2FD10 (28 B): skill-desc record (0x17F0) -> u16 at +8+idx*2, or 0x1506 when none.
uint16_t sub_2FD10(const Host& h, int32_t idx) {
    Address rec = static_cast<Address>(h.call(0x17F0));
    return rec ? h.read<uint16_t>(rec + 8 + idx * 2) : 0x1506;
}
} // namespace d2client_batch_2f2e0
