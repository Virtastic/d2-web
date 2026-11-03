// src/core/d2client/d2client_2f0a0.cpp -- D2Client+0x2F0A0 (437 B), retail-faithful (Host style; IAT helpers by IAT RVA, D2Win text calls by thunk RVA).
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace d2client_2f0a0 {
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
static int32_t thousands(int32_t v) { return (v + 500) / 1000; }   // retail: imul 0x10624DD3, sar 6, add sign bit (truncating /1000)

// 1.13c D2Client+0x2F0A0: EAX=lo, ECX=hi (EDX unused), stack s0 (draw mode), s1 (left), s2 (y), s3 (right), ret 0x10. Formats "lo-hi" (narrow sprintf via IAT 0xCF114,
// values >= 10000 shown in thousands with the 0xD359F suffix string), converts it to the wide buffer (0xCEAC4), measures it (0xD3B4), picks font 6 when the 11/7-scaled
// width exceeds s3-s1 (and moves y up one), else font 1 (0xD3A8), centres it in [s1, s3] when narrower and draws it with 0xD37E at (x, 0xDBC4C + [0x11B9A4] + y - 0x1E0).
uint32_t sub_2F0A0(const Host& h, int32_t eax, int32_t ecx, uint32_t s0, int32_t s1, int32_t s2, int32_t s3) {
    int32_t edi = eax, esi = ecx;
    if (!(esi < edi)) edi = esi + 1;
    char16_t wide[32];
    for (auto& c : wide) h.call(0xCEB00, {out(&c)});
    char narrow[32];
    const uintptr_t str = base + 0xD359F;
    if (esi == edi) h.call(0xCF114, {out(narrow), base + 0xD057C, u(esi)});
    else if (esi < 10000) {
        if (edi < 10000) h.call(0xCF114, {out(narrow), base + 0xD45C0, u(esi), u(edi)});
        else h.call(0xCF114, {out(narrow), base + 0xD6A18, str, u(esi), u(thousands(edi))});
    } else h.call(0xCF114, {out(narrow), base + 0xD6A24, str, u(thousands(esi)), u(thousands(edi))});
    h.call(0xCEAC4, {out(wide), out(narrow), 0x20});
    int32_t dim[2] = {0, 0};
    h.call(0xD3B4, {out(wide), out(&dim[0]), out(&dim[1])});
    const int32_t v = static_cast<int32_t>((static_cast<int64_t>(dim[0]) * 11) / 7);
    if (v > s3 - s1) { h.call(0xD3A8, {6}); --s2; }
    else h.call(0xD3A8, {1});
    const int32_t y = h.read<int32_t>(base + 0xDBC4C) + h.read<int32_t>(base + 0x11B9A4) + s2 - 0x1e0;
    const int32_t left = h.read<int32_t>(base + 0x11B9A0);
    int32_t x = s1 + left;
    int32_t room = (left - x) + s3 + 1;
    const int32_t w = static_cast<int32_t>(h.call(0xD35A, {out(wide)}));
    if (w < room) x += (room - w) >> 1;
    return static_cast<uint32_t>(h.call(0xD37E, {out(wide), u(x), u(y), s0, 0}));
}
} // namespace d2client_2f0a0
