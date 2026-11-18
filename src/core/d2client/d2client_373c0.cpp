// src/core/d2client/d2client_373c0.cpp -- D2Client+0x373C0 (362 B) and +0x37740 (342 B), retail-faithful (Host style: guest addresses, retail's
// register-then-stack words per callee, D2Common/D2Win imports as thunk RVAs, IAT Unicode helpers as their IAT RVAs 0xCEAB4/0xCEAB8/0xCEABC/0xCEB00).
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace d2client_373c0 {
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
static constexpr Address base = 0x6fab0000;
static uintptr_t u(int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); }
static uintptr_t out(const void* p) { return reinterpret_cast<uintptr_t>(p); }
static int32_t gi(const Host& h, uint32_t rva) { return h.read<int32_t>(base + rva); }

// Finds the first 10-byte record of the table 0x11A2E0..0x11A75A (kind dword @0, id word @4, flag dword @6) with kind 6 and the given flag; -1 if none.
static int32_t find(const Host& h, uint32_t flag) {
    int32_t idx = 0;
    for (Address p = base + 0x11A2E6; p < base + 0x11A75A; p += 10, ++idx)
        if (h.read<uint32_t>(p - 6) == 6 && h.read<uint32_t>(p) == flag) return idx;
    return -1;
}

// 1.13c D2Client+0x373C0: no args. Builds "<D2Lang 0x1051>" then, for the flag-1 and flag-0 kind-6 table records whose id word is not 0xFFFF,
// appends sprintf(30, D2Lang(0x1052), 0x39F30(eax=6, edx=flag)); draws the text at (0xDBC48 - width/2 - 0x3A, 0xDBC4C - 0xC5) via 0xD37E.
void sub_373C0(const Host& h) {
    char16_t big[200];
    char16_t small[30];
    for (auto& c : big) h.call(0xCEB00, {out(&c)});
    for (auto& c : small) h.call(0xCEB00, {out(&c)});
    h.call(0xCEAB8, {out(big), h.call(0xD1EC, {0x1051})});
    for (uint32_t flag = 1;; flag = 0) {
        const int32_t i = find(h, flag);
        if (i >= 0 && h.read<uint16_t>(base + 0x11A2E4 + i * 10) != 0xFFFF) {
            std::memset(small, 0, sizeof small);
            const uintptr_t s = h.call(0x39F30, {6, flag});
            h.call(0xCEAB4, {0x1e, out(small), h.call(0xD1EC, {0x1052}), s});
            h.call(0xCEABC, {out(big), out(small)});
        }
        if (!flag) break;
    }
    const int32_t w = static_cast<int32_t>(h.call(0xD35A, {out(big)}));
    const int32_t x = gi(h, 0xDBC48) - ((w - (w >> 31)) >> 1) - 0x3a;
    h.call(0xD37E, {out(big), u(x), u(gi(h, 0xDBC4C) - 0xc5), 0, 0});
}

// 1.13c D2Client+0x37740: no args. HUD gate: if any of the six busy flags (0xFAD90, AD8C, AD84, ADB0, ADDC, ADE4) is set, 0x11C454 = 1 and return.
// Else 0x11C454 = 0; 0x36450() == 0 draws 0x373C0 plus two 0xD096 records (stack: &local, x, y, -1, 5, 0); otherwise 0xBF8C, the 0xDD5D0 != 0x26 fatal
// (0xBF92(0x139B), 0xBF80(file 0xD359F, msg, 0x139B), 0x3664(-1)), and if 0xFAE08 was set, clear it and 0x3CC60(eax=2; D0B4(0xD0960)).
void sub_37740(const Host& h) {
    static const uint32_t busy[6] = {0xFAD90, 0xFAD8C, 0xFAD84, 0xFADB0, 0xFADDC, 0xFADE4};
    for (uint32_t r : busy) if (gi(h, r)) { h.write<int32_t>(base + 0x11C454, 1); return; }
    h.write<int32_t>(base + 0x11C454, 0);
    if (!h.call(0x36450)) {
        int32_t l[18] = {};
        h.call(0x373C0);
        l[13] = gi(h, 0x11C304);
        h.call(0xD096, {out(l), u(gi(h, 0xDBC48) - 0x4b), u(gi(h, 0xDBC4C) - 0xa0), 0xffffffffu, 5, 0});
        const int32_t c450 = gi(h, 0x11C450), c300 = gi(h, 0x11C300);
        l[13] = c300;
        l[0] = c450 != 0;
        h.call(0xD096, {out(l), u(gi(h, 0xDBC48) - 0x48), u(gi(h, 0xDBC4C) - 0xa4), 0xffffffffu, 5, 0});
        return;
    }
    const int32_t old = gi(h, 0xFAE08);
    h.call(0xBF8C);
    if (gi(h, 0xDD5D0) != 0x26) {
        const uintptr_t msg = h.call(0xBF92, {0x139b});
        h.call(0xBF80, {base + 0xD359F, msg, 0x139b});
        h.call(0x3664, {0xffffffffu});
    }
    h.write<int32_t>(base + 0xFAE08, 0);
    if (old) h.call(0x3CC60, {2, h.call(0xD0B4, {base + 0xD0960})});
}
} // namespace d2client_373c0
