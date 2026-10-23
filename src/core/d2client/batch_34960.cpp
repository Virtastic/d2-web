// src/core/d2client/batch_34960.cpp -- skill-description layout helpers, D2Client+0x34960..0x34D70.
// Source: cached 1.13c decompiles. Same Host seam as batch_2ed80.cpp; `scratch` is a host address with
// 16 free bytes standing in for the decompile's two stack out-params.
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace d2client_batch_34960 {
using Address = uint32_t;
struct Host {
    void* context;
    uint8_t* (*memory)(void*, Address);
    uintptr_t (*invoke)(void*, uint32_t, const uintptr_t*, size_t);
    Address scratch;
    template<class T> T read(Address at) const { T v{}; std::memcpy(&v, memory(context, at), sizeof(v)); return v; }
    template<class T> void write(Address at, T v) const { std::memcpy(memory(context, at), &v, sizeof(v)); }
    uintptr_t call(uint32_t rva, std::initializer_list<uintptr_t> args = {}) const {
        return invoke(context, rva, args.begin(), args.size());
    }
};
constexpr uint32_t ord(uint32_t n) { return 0x10000000u + n; }
static uintptr_t u(int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); }
static int32_t div128(int32_t x) { return (x + ((x >> 31) & 0x7f)) >> 7; } // signed x/128 as the compiler emits it

// 1.13c D2Client+0x34960 (129 B): when p2,p3 set and 0x2C230 succeeds, fetch p1's pair (0x32F30), 0x2C190, emit first of pair.
void sub_34960(const Host& h, Address p1, int p2, int p3, int, uintptr_t p5, uintptr_t p6) {
    if (!p2 || !p3) return;
    h.write<int32_t>(h.scratch, 0); h.write<int32_t>(h.scratch + 4, 0);
    if (!h.call(0x2C230)) return;
    h.call(0x32F30, {p1, h.scratch, h.scratch + 4});
    h.call(0x2C190);
    h.call(0x2F0A0, {h.read<uint32_t>(h.scratch), 0, p5, p6});
}

// 1.13c D2Client+0x349F0 (182 B): as 0x34960 but pair is scaled by byte p3+0x1a5 /128 and 0x2E190(p1) replaces 0x2C190.
void sub_349F0(const Host& h, Address p1, int p2, Address p3, int, uintptr_t p5, uintptr_t p6) {
    if (!p2 || !p3) return;
    int32_t b = h.read<uint8_t>(p3 + 0x1a5);
    h.call(0x32F30, {p1, h.scratch, h.scratch + 4});
    int32_t a = div128(h.read<int32_t>(h.scratch) * b);
    h.write<int32_t>(h.scratch, a);
    h.write<int32_t>(h.scratch + 4, div128(h.read<int32_t>(h.scratch + 4) * b));
    h.call(0x2E190, {p1});
    h.call(0x2F0A0, {u(a), 0, p5, p6});
}

// 1.13c D2Client+0x34AB0 (327 B): looks up the skill record (0x17F0); if #10747(p1) is set lays out via 0x30B40
// with callback 0x33240, else computes the pair (0x33240) and emits it through 0x2F0A0.
// ponytail: the decompile's stack-built argument lists are ambiguous; arg order follows the visible pushes.
void sub_34AB0(const Host& h, Address p1, Address p2, int p3) {
    if (!p2 || !p3) return;
    uint32_t rec = static_cast<uint32_t>(h.call(0x17F0));
    if (!rec) return;
    bool alt = h.call(ord(10747), {p1}) != 0;
    uintptr_t s = h.call(ord(10601), {0xa02u + (alt ? 0 : 0xa), 0, u(p3)}); // line 0xa02/0xa0c
    uintptr_t d = h.call(ord(10435), {p1, h.read<uint32_t>(rec + 0x18)});
    uintptr_t t = h.call(ord(10601), {p2, 0xa03u + (alt ? 0 : 0xa), u(p3)});
    uintptr_t v = h.call(ord(10435), {p1, h.read<uint32_t>(rec + 0x1c), t});
    if (alt) {
        h.call(0x30B40, {p2, p1, u(p3), 0x33240, h.read<uint32_t>(rec + 0x18), v, s, 1});
        (void)d;
        return;
    }
    h.call(0x33240, {p1, h.scratch, h.scratch + 4, h.scratch + 8});
    h.call(0x2F0A0, {h.read<uint32_t>(h.scratch), h.read<uint32_t>(h.scratch + 8), s, p2});
}

// 1.13c D2Client+0x34C00 (356 B): skill icon line; pair from 0x33240 is rescaled (0x14A0, x/128) when p3+0x1a5 != 0x80,
// then #10567/#10121/#11091 (>>8) of skill *p3 are added and emitted via 0x2F0A0.
void sub_34C00(const Host& h, Address p1, int p2, Address p3, int p4) {
    if (!p2 || !p3) return;
    if (!h.call(0x17F0)) return;
    h.call(ord(10601), {u(p2), 0x98c, u(p4)});
    h.call(ord(10435), {p1});
    h.call(ord(10601), {u(p2), 0x98d, u(p4)});
    h.call(ord(10435), {p1});
    h.call(0x33240, {p1, h.scratch, h.scratch + 4, h.scratch + 8});
    int32_t x = h.read<int32_t>(h.scratch), y = h.read<int32_t>(h.scratch + 4);
    uint8_t sc = h.read<uint8_t>(p3 + 0x1a5);
    if (sc != 0x80) {
        x = static_cast<int32_t>(h.call(0x14A0, {0x80, u(x)}));
        y = static_cast<int32_t>(h.call(0x14A0, {0x80, u(y)}));
    }
    int16_t sk = h.read<int16_t>(p3);
    int32_t a = static_cast<int32_t>(h.call(ord(10567), {p1, u(sk), u(p4), 0}));
    h.call(ord(10297), {p1, u(sk)});
    int32_t b = static_cast<int32_t>(h.call(ord(10121), {p1, u(sk), u(p4), 1}));
    h.call(ord(11091), {p1, u(sk), u(p4), 1});
    h.call(0x2F0A0, {u(x + (b >> 8) + (a >> 8)), u(p4), u(p4), 0});
}

// 1.13c D2Client+0x34D70 (134 B): register-arg helper (ESI = scale). With scale != 0, 0x31220(p1) then 0x30E30 fill a pair;
// each is scaled by esi/128 and added into *p2 and *p3.
void sub_34D70(const Host& h, Address p1, Address p2, Address p3, int32_t esi, uintptr_t eax) {
    if (!esi) return;
    h.write<int32_t>(h.scratch, 0); h.write<int32_t>(h.scratch + 4, 0);
    h.call(0x31220, {p1});
    h.call(0x30E30, {p1, h.scratch, h.scratch + 4, eax});
    h.write<int32_t>(p2, h.read<int32_t>(p2) + div128(h.read<int32_t>(h.scratch) * esi));
    h.write<int32_t>(p3, h.read<int32_t>(p3) + div128(h.read<int32_t>(h.scratch + 4) * esi));
}
} // namespace d2client_batch_34960
