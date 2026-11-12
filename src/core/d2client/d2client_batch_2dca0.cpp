// Prior art: none for D2Client. Bodies follow the cached 1.13c decompiles
// 0x2DCA0..0x32E20 kept outside the repository. Guest addresses only; callees go through Host.
// Register-passed arguments are named after the register. Import thunks are named by their
// D2Client RVA (callee lists); ordinal-to-RVA pairings for 0xC5BC/0xC5CE/0xCEB6.. follow call order.
#include <cstdint>
#include <cstring>
#include <string>
#include <cstdio>
#include <initializer_list>

namespace d2client_batch_2dca0 {
using Address = uint32_t;
struct Host {
    void* context;
    uint8_t* (*memory)(void*, Address);
    uintptr_t (*invoke)(void*, uint32_t, const uintptr_t*, size_t);
    template<class T> T read(Address a) const { T v{}; std::memcpy(&v, memory(context, a), sizeof v); return v; }
    template<class T> void write(Address a, T v) const { std::memcpy(memory(context, a), &v, sizeof v); }
    uintptr_t call(uint32_t rva, std::initializer_list<uintptr_t> args = {}) const {
        return invoke(context, rva, args.begin(), args.size());
    }
};
static uintptr_t out(const void* p) { return reinterpret_cast<uintptr_t>(p); }

// D2Lang #10003 (thunk +0xD1EC) text appended to the guest string at dst. key<0: key is in a register.
static void ref(const Host& h, Address dst, int64_t key = -1) {
    Address s = key < 0 ? (Address)h.call(0xD1EC) : (Address)h.call(0xD1EC, {(uintptr_t)key});
    if (!s || !dst) return;
    Address e = dst;
    while (h.read<uint16_t>(e)) e += 2;
    for (; h.read<uint16_t>(s); s += 2, e += 2) h.write<uint16_t>(e, h.read<uint16_t>(s));
    h.write<uint16_t>(e, 0);
}
static void local(const Host& h, Address dst, const char16_t* s) {
    Address e = dst;
    while (h.read<uint16_t>(e)) e += 2;
    for (; *s; ++s, e += 2) h.write<uint16_t>(e, (uint16_t)*s);
    h.write<uint16_t>(e, 0);
}
static void ascii(const Host& h, Address dst, const char* a) {
    char16_t b[32]{};
    for (int i = 0; a[i] && i < 31; ++i) b[i] = (unsigned char)a[i];
    local(h, dst, b);
}

// 1.13c D2Client+0x2DCA0 (168 B): EBX=dest. value 0 -> 0; "+%d" if plus and value>0 else "%d".
uint32_t sub_2DCA0(const Host& h, Address ebx, int32_t value, int32_t plus) {
    if (!value) return 0;
    char a[32];
    std::snprintf(a, sizeof a, (plus && value >= 0) ? "+%d" : "%d", value);
    ref(h, ebx); ascii(h, ebx, a); ref(h, ebx);
    return 1;
}

// 1.13c D2Client+0x2DD50 (173 B): EBX=dest. 0x1506 -> two refs; else sprintf(ref(strId)) + ref -> dest.
uint32_t sub_2DD50(const Host& h, Address ebx, int16_t sel, uint32_t strId) {
    if (sel == 0x1506) { ref(h, ebx); ref(h, ebx); return 1; }
    // Unicode::sprintf(0x100, buf, ref(strId)): the string is used as a format with no arguments.
    Address f = (Address)h.call(0xD1EC, {strId});
    char16_t buf[256]{};
    for (int i = 0; f && i < 255 && h.read<uint16_t>(f + i * 2); ++i) buf[i] = h.read<uint16_t>(f + i * 2);
    Address t = (Address)h.call(0xD1EC);
    for (int i = 0, n = (int)std::char_traits<char16_t>::length(buf); t && n + i < 255; ++i) {
        uint16_t c = h.read<uint16_t>(t + i * 2);
        if (!(buf[n + i] = c)) break;
    }
    local(h, ebx, buf);
    return 1;
}

// (host-style body, superseded by d2client_2de00.cpp) D2Client+0x2DE00 (326 B): EBX=scale. 24.8 fixed-point range "lo-hi" or "lo.f-hi.f".
uint32_t sub_2DE00(const Host& h, Address dst, int16_t selA, int16_t selB, uint32_t p4, uint32_t p5,
                   int32_t ebx) {
    int32_t a = (int32_t)p4, b = (int32_t)p5;
    int32_t ai = a / 256, bi = b / 256;
    int32_t af = ((a % 256) * ebx) / 256, bf = ((b % 256) * ebx) / 256;
    char s[32];
    if (!ai && !bi) {
        if (!af && !bf) return 0;
        std::snprintf(s, sizeof s, "%d.%d-%d.%d", ai, af, bi, bf);
    } else if (!af && !bf) std::snprintf(s, sizeof s, "%d-%d", ai, bi);
    else std::snprintf(s, sizeof s, "%d.%d-%d.%d", ai, af, bi, bf);
    if (selA != 0x1506) ref(h, dst);
    ascii(h, dst, s);
    if (selB != 0x1506) ref(h, dst);
    ref(h, dst);
    return 1;
}

// (host-style body, superseded by d2client_2df50.cpp) D2Client+0x2DF50 (268 B): EBX=dest; frames/25 range with one decimal.
uint32_t sub_2DF50(const Host& h, Address ebx, int32_t lo, int32_t hi) {
    int32_t a = lo / 25, af = ((lo % 25) * 10) / 25, b = hi / 25, bf = ((hi % 25) * 10) / 25;
    char s[48];
    if (a == 0 && af == 0 && b == 0) return 0;
    if (af == 0 && bf == 0)
        std::snprintf(s, sizeof s, "%d-%d", a, b);
    else std::snprintf(s, sizeof s, "%d.%d-%d.%d", a, af, b, bf);
    ref(h, ebx); ascii(h, ebx, s); ref(h, ebx); ref(h, ebx);
    return 1;
}

// 1.13c D2Client+0x2E060 (218 B): stdcall(dst, key, frames), EAX=record. whole = frames/25, tenth = (frames%25)*10/25; both 0 returns 0.
// Tenth 0: 0x2C7E0(eax=whole, ecx=rec, [buf]); else 0x2C790(eax=whole, ecx=tenth, edx=rec, [buf]), both ret 4.
// Appends D2Lang(key), the number, D2Lang(whole==1 && tenth==0 ? 0x10AB : 0x10AC), D2Lang(0xF9E) to dst.
uint32_t sub_2E060(const Host& h, Address dst, Address eax, uint32_t key, int32_t p3) {
    int32_t whole = p3 / 25, f = ((p3 % 25) * 10) / 25;
    char16_t buf[64]{};
    if (!whole && !f) return 0;
    if (f) h.call(0x2C790, {(uintptr_t)whole, (uintptr_t)f, eax, out(buf)});
    else h.call(0x2C7E0, {(uintptr_t)whole, eax, out(buf)});
    ref(h, dst, key); local(h, dst, buf);
    ref(h, dst, (whole == 1 && !f) ? 0x10AB : 0x10AC);
    ref(h, dst, 0xF9E);
    return 1;
}

// 1.13c D2Client+0x2E140 (45 B), fastcall: ECX, EDX forwarded; EAX compared with p3.
uint32_t sub_2E140(const Host& h, uintptr_t ecx, uintptr_t edx, int32_t p3, int32_t eax) {
    if (p3 == eax) h.call(0x2C7E0, {(uintptr_t)p3, ecx, edx});
    else h.call(0x2C6A0, {(uintptr_t)eax, ecx, 1, edx});
    return 1;
}

// 1.13c D2Client+0x2E190 (195 B). EAX=record (id int16 at +0, mode byte at +0x1dc), EBX=register arg, stack p1,outX,outY,kindOut (ret 0x10).
// D2Common thunks 0xCEB6/0xCEC2/0xCECE take (p1, id, ebx, 1). Mode 5 scales both by CEB6 (>>8), then kindOut by mode, then *outX+=x, *outY+=y.
// EAX at the return still holds y (retail adds it to *outY through EDX), so y is the result.
int32_t sub_2E190(const Host& h, Address eax, Address ebx, int32_t p1, Address outX, Address outY, Address kindOut) {
    uint8_t mode = h.read<uint8_t>(eax + 0x1dc);
    uintptr_t id = (uintptr_t)(int32_t)h.read<int16_t>(eax);
    auto d2 = [&](uint32_t t) { return (int32_t)h.call(t, {(uintptr_t)p1, id, (uintptr_t)ebx, 1}); };
    int32_t x, y;
    if (mode == 5) {
        int32_t k = d2(0xCEB6);
        h.write<uint32_t>(kindOut, 2);
        x = (int32_t)((int64_t)k * d2(0xCEC2)) >> 8;
        y = (int32_t)((int64_t)d2(0xCECE) * k) >> 8;
    } else {
        x = d2(0xCEC2) >> 8;
        y = d2(0xCECE) >> 8;
        if (kindOut) {
            static const uint32_t kind[6] = {0, 1, 9, 3, 3, 2};
            if (mode >= 1 && mode <= 5) h.write<uint32_t>(kindOut, kind[mode]);
        }
    }
    h.write<int32_t>(outX, h.read<int32_t>(outX) + x);
    h.write<int32_t>(outY, h.read<int32_t>(outY) + y);
    return y;
}

// 1.13c D2Client+0x2E2D0 (90 B), thiscall: when this+0x1dc==1 and #10973(global 0x11BBFC, 0x149)!=0,
// each of the two values grows by FUN +0x14A0(100, v) (percent of v).
// EAX at the return: the player word on a failed gate (loaded before the test), 0 for a zero stat, else the new *EBX.
uint32_t sub_2E2D0(const Host& h, Address self, Address p1, Address ebx, Address player) {
    if (!self || h.read<uint8_t>(self + 0x1dc) != 1) return player;
    uintptr_t v = h.call(0xC1C6, {player, 0x149, 0});
    if (!v) return 0;
    uint32_t last = 0;
    for (Address p : {p1, ebx}) {
        int32_t x = h.read<int32_t>(p);
        last = (uint32_t)((int32_t)h.call(0x14A0, {v, 100, (uintptr_t)(uint32_t)x}) + x);
        h.write<uint32_t>(p, last);
    }
    return last;
}

// 1.13c D2Client+0x2E350 (187 B, ret 8): stack unit, mode. Unit 0 returns 0. a = mode ? 0xC5BC(unit) : 0xC5CE(unit); b = 0xC640(ECX=a), 0 returns 0;
// rec = 0x17F0(EAX=int16 [b]), 0 returns 0. Retail zeroes its own unit and mode argument slots and uses them as out-variables: when word rec+0x14 (< 0x48)
// has a non-null entry in the table at 0xF52A0, c = 0xC718(unit, a, 1) and the entry runs with ECX=unit, EDX=a, stack b, c, &v1, &lb, &v2, &la (ret 0x18);
// the result is v1 (the unit slot, zeroed before the call).
uint32_t sub_2E350(const Host& h, uint32_t unit, int32_t mode) {
    if (!unit) return 0;
    const uintptr_t a = mode ? h.call(0xC5BC, {unit}) : h.call(0xC5CE, {unit});
    const Address b = static_cast<Address>(h.call(0xC640, {a}));
    if (!b) return 0;
    const Address rec = static_cast<Address>(h.call(0x17F0, {static_cast<uintptr_t>(static_cast<uint32_t>(static_cast<int32_t>(h.read<int16_t>(b))))}));
    if (!rec) return 0;
    uint32_t v1 = 0, v2 = 0, la = 0, lb = 0;
    const uint16_t idx = h.read<uint16_t>(rec + 0x14);
    if (idx < 0x48) {
        const Address fn = h.read<uint32_t>(0x6FAB0000 + 0xF52A0 + idx * 4);
        if (fn) {
            const uintptr_t c = h.call(0xC718, {unit, a, 1});
            h.call(fn - 0x6FAB0000, {unit, a, b, c, reinterpret_cast<uintptr_t>(&v1), reinterpret_cast<uintptr_t>(&lb), reinterpret_cast<uintptr_t>(&v2), reinterpret_cast<uintptr_t>(&la)});
        }
    }
    return v1;
}

// 1.13c D2Client+0x2FD30 (1317 B): SkillDesc tooltip layout; the 1317-byte body is mostly stack
// plumbing around the callbacks at 0xF5210 (0x90 entries) and 0xF52A0 (0x48 entries) and the
// 0xF52E8 layout rows (stride 12). Only the dispatch preconditions are reproduced.
// ponytail: layout drawing (0xBE800/0xBC3E0/0xBC4E0 text calls) left to its own verified row.
uint32_t sub_2FD30(const Host& h, Address skillRec, uint32_t flagsOut) {
    (void)flagsOut;
    if (!h.call(0xC640)) return 0;
    uint16_t cb1 = h.read<uint16_t>(skillRec + 0x12), cb2 = h.read<uint16_t>(skillRec + 0x14);
    return cb1 < 0x90 && cb2 < 0x48;
}
} // namespace d2client_batch_2dca0
