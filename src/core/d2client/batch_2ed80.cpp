// src/core/d2client/batch_2ed80.cpp -- damage-range line emitters, D2Client+0x2ED80..0x2EF80.
// Source: cached 1.13c decompiles 0x2ED80.c..0x2EF80.c. Same Host seam as d2client_batch_2f2e0.cpp;
// the destination text buffer is a hidden register argument, passed through untouched.
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace d2client_batch_2ed80 {
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
constexpr uint32_t ord(uint32_t n) { return 0x10000000u + n; }
static uintptr_t u(int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); }

// 1.13c D2Client+0x2ED80 (65 B): stat 10121 and 11091 of the unit, each scaled by 75/256, emitted as a range.
void sub_2ED80(const Host& h, Address dst, Address unit) {
    int32_t a = static_cast<int32_t>(h.call(ord(10121), {unit}));
    int32_t b = static_cast<int32_t>(h.call(ord(11091), {unit}));
    h.call(0x2CF20, {dst, u(a * 0x4b >> 8), u(b * 0x4b >> 8)});
}

// 1.13c D2Client+0x2EDD0 (254 B): min/max (10205, 10532) get the 0x2E2D0 and 0x2E270 percent bonuses of
// param_2, then low>>8 and high*75>>8 are emitted. All six switch(10557) arms are the same call.
void sub_2EDD0(const Host& h, Address dst, Address unit, Address param2) {
    int32_t lo = static_cast<int32_t>(h.call(ord(10205), {0, unit}));
    int32_t hi = static_cast<int32_t>(h.call(ord(10532), {0, unit}));
    int32_t v[2] = {lo, hi}; // the decompile's stack0xffffffe8 pair
    // ponytail: ceiling -- v lives in host memory in the real call; wire via scratch address when vectored.
    (void)v;
    h.call(0x2E2D0, {param2});
    h.call(0x2E270, {param2});
    h.call(ord(10557));
    h.call(0x2CF20, {dst, u(lo >> 8), u(hi * 0x4b >> 8)});
}

// 1.13c D2Client+0x2EEF0 (42 B): if #10653 returns a nonzero string id, emit it via 0x2D9B0(0x10a6,0x10b3,id,1).
void sub_2EEF0(const Host& h, uintptr_t p1, uintptr_t p2) {
    uint32_t id = static_cast<uint32_t>(h.call(ord(10653), {p2, p1}));
    if (id) h.call(0x2D9B0, {0x10a6, 0x10b3, id, 1});
}

// 1.13c D2Client+0x2EF20 (87 B): (0,0) of #10205/#10532 (>>8) yields 0, else 0x2D220(null dst, lo, hi).
uint32_t sub_2EF20(const Host& h, Address unit) {
    int32_t lo = static_cast<int32_t>(h.call(ord(10205), {0, unit})) >> 8;
    int32_t hi = static_cast<int32_t>(h.call(ord(10532), {0, unit})) >> 8;
    if (lo == 0 && hi == 0) return 0;
    h.call(ord(10557));
    return static_cast<uint32_t>(h.call(0x2D220, {0, u(lo), u(hi)}));
}

// 1.13c D2Client+0x2EF80 (80 B): #10121/#11091 (>>8) as a range into param_1; nothing when both are 0.
void sub_2EF80(const Host& h, Address dst) {
    int32_t lo = static_cast<int32_t>(h.call(ord(10121)));
    int32_t hi = static_cast<int32_t>(h.call(ord(11091), {dst}));
    if (lo == 0 && hi == 0) return;
    h.call(0x2D220, {dst, u(lo >> 8), u(hi >> 8)});
}
} // namespace d2client_batch_2ed80
