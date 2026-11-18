// src/core/d2client/batch_34960.cpp -- skill-description layout helpers, D2Client+0x34960..0x34D70.
// Source: cached 1.13c decompiles. Same Host seam as batch_2ed80.cpp; `scratch` is a host address with
// 16 free bytes standing in for the decompile's two stack out-params.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include "batch_34960.h"
extern "C" int32_t d2_client_14a0(int32_t a, int32_t b, int32_t d);

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

static uint32_t rd32(const uint8_t* p, uint32_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; }

// 1.13c D2Client+0x34960 (ecx=p1, edx=p2, 5 stack words, ret 0x14): the pair from 0x32F30 (gated by 0x2C230) goes to
// 0x2F0A0 with the a0 slot reused as the third out word. eax0 is the caller's eax, left as the result on the first exit.
uint32_t sub_34960(Ops& o, uint32_t eax0, uint32_t p1, uint32_t p2, uint32_t a0, uint32_t, uint32_t a2, uint32_t a3, uint32_t a4) {
    if (!p2) return eax0;
    if (!a0) return 0;
    int32_t la = 0, lb = 0, a = 0;
    uint32_t r = o.c2C230();
    if (!r) return 0;
    o.c32F30(p1, &la, &lb, &a, 0, 0, p2, 0, r);
    o.c2C190();
    return o.c2F0A0(uint32_t(lb), uint32_t(la), uint32_t(a), uint32_t(a), a2, a3, a4);
}

// 1.13c D2Client+0x349F0 (ecx=p1, edx=p2, 5 stack words, ret 0x14): as 0x34960 without the gate; the pair is scaled by the
// byte at unit+0x1a5 over 128, then 0x2E190 (eax=unit, ebx=a1) adjusts all three words before 0x2F0A0.
uint32_t sub_349F0(Ops& o, uint32_t eax0, uint32_t p1, uint32_t p2, const uint8_t* unit, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
    if (!p2 || !unit) return eax0;
    int32_t b = unit[0x1a5];
    int32_t la = 0, lb = 0, a = 0;
    o.c32F30(p1, &la, &lb, &a, 0, 0, p2, 0, 0);
    la = div128(la * b);
    lb = div128(lb * b);
    o.c2E190(unit, a1, p1, &la, &lb, &a);
    return o.c2F0A0(uint32_t(lb), uint32_t(la), uint32_t(a), uint32_t(a), a2, a3, a4);
}

// 1.13c D2Client+0x34AB0 (ecx=p1, edx=p2, 5 stack words, ret 0x14): skill record from 0x17F0(*(int16*)a0); its +0x18/+0x1c
// values are resolved through #10601(p2, file, line) (lines 0xa02/0xa03, or 0xa0c/0xa0d) and #10435(p1, v, that, a1). With #10747(p1) set the pair goes to
// 0x30B40 with the 0x33240 callback; otherwise 0x33240 fills the pair and 0x2F0A0 emits it.
uint32_t sub_34AB0(Ops& o, uint32_t eax0, uint32_t p1, uint32_t p2, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
    if (!p2) return eax0;
    if (!a0) return 0;
    int16_t id; std::memcpy(&id, a0, 2);
    const uint8_t* rec = o.c17F0(id);
    if (!rec) return 0;
    if (o.c10747(p1)) {
        uint32_t r1 = o.c10435(p1, rd32(rec, 0x18), o.c10601(p2, o.srcFile, 0xa02), a1);
        uint32_t r2 = o.c10435(p1, rd32(rec, 0x1c), o.c10601(p2, o.srcFile, 0xa03), a1);
        return o.c30B40(a0, p2, a2, p1, a1, o.pairCb, r1, r2, a2, a3, a4, 1);
    }
    int32_t la = 0, lb = 0, a = 0;
    uint32_t r1 = o.c10435(p1, rd32(rec, 0x18), o.c10601(p2, o.srcFile, 0xa0c), a1);
    uint32_t r2 = o.c10435(p1, rd32(rec, 0x1c), o.c10601(p2, o.srcFile, 0xa0d), a1);
    o.c33240(p1, &la, &lb, &a, r1, r2, p2, 0, 0);
    return o.c2F0A0(uint32_t(lb), uint32_t(la), a2, uint32_t(a), a2, a3, a4);
}

// 1.13c D2Client+0x34C00 (ecx=p1, edx=p2, 5 stack words, ret 0x14): as the plain branch of 0x34AB0 (lines 0x98c/0x98d) but
// 0x33240 gets (r1, r2, p2, 0x80, 0) and fills x (edx), the a0 slot and a local z. Unless the unit's byte +0x1a5 is 0x80 both x
// and the a0 word are rescaled by 0x14A0(eax=byte, ecx=0x80, edx=v). Then #10567/#10121 (>>8) add to x and #10297/#11091 to
// the a0 word, all with (p1, skill, a1, 0/1); 0x2F0A0 gets eax=a0 word, ecx=x, edx=z and stack (z, a2, a3, a4).
uint32_t sub_34C00(Ops& o, uint32_t eax0, uint32_t p1, uint32_t p2, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
    if (!p2 || !a0) return eax0;
    int16_t skill; std::memcpy(&skill, a0, 2);
    const uint8_t* rec = o.c17F0(skill);
    if (!rec) return 0;
    int32_t x = 0, w = 0, z = 0;
    uint32_t r1 = o.c10435(p1, rd32(rec, 0x18), o.c10601(p2, o.srcFile, 0x98c), a1);
    uint32_t r2 = o.c10435(p1, rd32(rec, 0x1c), o.c10601(p2, o.srcFile, 0x98d), a1);
    o.c33240(p1, &x, &w, &z, r1, r2, p2, 0x80, 0);
    uint8_t b = a0[0x1a5];
    if (b != 0x80) {
        x = d2_client_14a0(b, x, 0x80);
        w = d2_client_14a0(b, w, 0x80);
    }
    x += o.c10567(p1, skill, a1, 0) >> 8;
    w += o.c10297(p1, skill, a1, 0) >> 8;
    x += o.c10121(p1, skill, a1, 1) >> 8;
    w += o.c11091(p1, skill, a1, 1) >> 8;
    return o.c2F0A0(uint32_t(w), uint32_t(x), uint32_t(z), uint32_t(z), a2, a3, a4);
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
