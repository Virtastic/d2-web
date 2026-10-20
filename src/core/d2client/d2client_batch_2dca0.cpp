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

// 1.13c D2Client+0x2DE00 (326 B): EBX=scale. 24.8 fixed-point range "lo-hi" or "lo.f-hi.f".
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

// 1.13c D2Client+0x2DF50 (268 B): EBX=dest; frames/25 range with one decimal.
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

// 1.13c D2Client+0x2E060 (218 B): EAX=record. value p3 frames/25; 0x2C7E0 whole, 0x2C790 fractional.
uint32_t sub_2E060(const Host& h, Address dst, Address eax, int32_t p3) {
    int32_t f = ((p3 % 25) * 10) / 25;
    char16_t buf[64]{};
    if (p3 / 25 == 0) { if (!f) return 0; h.call(0x2C790, {(uintptr_t)f, eax, out(buf)}); }
    else if (!f) h.call(0x2C7E0, {eax, out(buf)});
    else h.call(0x2C790, {(uintptr_t)f, eax, out(buf)});
    ref(h, dst); local(h, dst, buf); ref(h, dst); ref(h, dst);
    return 1;
}

// 1.13c D2Client+0x2E140 (45 B), fastcall: ECX, EDX forwarded; EAX compared with p3.
uint32_t sub_2E140(const Host& h, uintptr_t ecx, uintptr_t edx, int32_t p3, int32_t eax) {
    if (p3 == eax) h.call(0x2C7E0, {ecx, edx});
    else h.call(0x2C6A0, {ecx, 1, edx});
    return 1;
}

// 1.13c D2Client+0x2E190 (195 B). EAX=record (mode byte at +0x1dc, shorts at +0), EBX=out Y, outX=
// retail's [sp] slot. Mode 5 scales by param_1 first. Adds (x>>8)/(y>>8) into the two outputs.
void sub_2E190(const Host& h, Address eax, int32_t p1, Address outX, Address ebx, Address kindOut) {
    uint8_t mode = h.read<uint8_t>(eax + 0x1dc);
    int16_t id = h.read<int16_t>(eax);
    int32_t x, y;
    if (mode == 5) {
        int32_t k = (int32_t)h.call(0xCEB6, {(uintptr_t)p1, (uintptr_t)(int32_t)id});
        h.write<uint32_t>(kindOut, 2);
        x = k * (int32_t)h.call(0xCEC2, {(uintptr_t)p1, (uintptr_t)(int32_t)id}) ;
        y = (int32_t)h.call(0xCECE, {(uintptr_t)p1, (uintptr_t)(int32_t)id}) * p1;
    } else {
        x = (int32_t)h.call(0xCEC2, {(uintptr_t)p1, (uintptr_t)(int32_t)id});
        y = (int32_t)h.call(0xCECE, {(uintptr_t)p1});
        uint32_t kind = mode == 1 ? 1 : mode == 2 ? 9 : (mode == 3 || mode == 4) ? 3 : 0;
        if (kind) h.write<uint32_t>(kindOut, kind);
    }
    h.write<int32_t>(outX, h.read<int32_t>(outX) + (x >> 8));
    h.write<int32_t>(ebx, h.read<int32_t>(ebx) + (y >> 8));
}

// 1.13c D2Client+0x2E2D0 (90 B), thiscall: when this+0x1dc==1 and #10973(global 0xBBFC, 0x149)!=0,
// each of the two values grows by FUN +0x14A0(100, v) (percent of v).
void sub_2E2D0(const Host& h, Address self, Address p1, Address ebx, Address lang, uint32_t flagArg) {
    if (!self || h.read<uint8_t>(self + 0x1dc) != 1) return;
    if (!h.call(0xC1C6, {lang, 0x149, 0})) return;
    for (Address p : {p1, ebx}) {
        int32_t v = h.read<int32_t>(p);
        h.write<int32_t>(p, v + (int32_t)h.call(0x14A0, {100, (uintptr_t)v}));
    }
    (void)flagArg;
}

// 1.13c D2Client+0x2E350 (187 B): dispatch through the 0x48-entry callback table at 0xF52A0.
uint32_t sub_2E350(const Host& h, int32_t unit, int32_t mode, Address table) {
    if (!unit) return 0;
    uintptr_t a = mode ? h.call(0xC5CE) : h.call(0xC5BC);
    uintptr_t b = h.call(0xC640);
    if (!b) return 0;
    Address rec = (Address)h.call(0x17F0);
    if (!rec) return 0;
    uint16_t idx = h.read<uint16_t>(rec + 0x14);
    if (idx >= 0x48 || !h.read<uint32_t>(table + idx * 4)) return 0;
    uintptr_t c = h.call(0xC718, {(uintptr_t)unit, a, 1});
    // Retail invokes the table entry (b, c, scratch...); ours reports it through the same seam.
    h.call(h.read<uint32_t>(table + idx * 4) - 0x6FAB0000, {b, c});
    return 0;
}

// 1.13c D2Client+0x2E410 (713 B): aggregates two stat lists (6+5 entries) for skill 0x19 / 0x6F
// bonuses, applies the percentage to both totals, then emits via +0x2D220. a/b/c: stat helper args.
static int32_t pct(int32_t base, int32_t p) {
    if (base <= 0x100000) {
        if (p <= 0x10000) return (int32_t)((uint32_t)p * (uint32_t)base) / 100;
        if ((p & ~0xF) < 0x640) return (int32_t)((int64_t)p * base / 100);
        return (p / 100) * base;
    }
    if ((base & ~0xF) < 0x640) return (int32_t)((int64_t)p * base / 100);
    return (base / 100) * p;
}
uint32_t sub_2E410(const Host& h, Address dst, Address unit, Address dt, Address stats, uint32_t a,
                   uint32_t b, uint32_t c, uint32_t cls, uint32_t lvl, uint32_t gA, uint32_t gB) {
    int16_t rec = h.read<int16_t>(unit + 0xbc);
    Address tbl = h.read<uint32_t>(dt);
    if (rec < 0 || rec >= h.read<int32_t>(tbl + 0xa80)) return 0;
    Address row = h.read<uint32_t>(tbl + 0xa78) + rec * 0x1a8;
    uint32_t t[14]{};
    h.call(0xC244, {(uintptr_t)rec, gA < 2 || gA > 3 ? 1u : gB, cls, (uintptr_t)h.read<int16_t>(row + 0xaa + cls * 2), 8, out(t)});
    int32_t sum = 0, t0 = (int32_t)t[0], tot = t[0];
    (void)lvl;
    auto scan = [&](Address kinds, Address vals, int n) {
        for (int i = 0; i < n; ++i) {
            int16_t k = h.read<int16_t>(kinds + i * 2);
            if (k != 0x19 && k != 0x6f) continue;
            int32_t v = (int32_t)h.call(0xC316, {a, h.read<uint32_t>(vals + i * 4), b, c});
            if (k == 0x19) sum += v; else { t0 += v; tot += v; }
        }
    };
    scan(stats + 0x54, stats + 0x68, 6);
    scan(stats + 0x98, stats + 0xa4, 5);
    if (sum) {
        if (sum < -0x59) sum = -0x5A;
        t0 += pct(t0, sum);
        tot += pct(tot, sum);
    }
    h.call(0x2D220, {dst, (uintptr_t)t0, (uintptr_t)tot});
    return 1;
}

// 1.13c D2Client+0x32E20 (261 B), fastcall: p2/p3 nonzero. Gets the skill row (0x23C stride, +0x158
// percent) and scales the first value by it, then forwards via +0x30E30 and +0x2F0A0.
void sub_32E20(const Host& h, Address ecx, Address p2, Address p3, Address dt, int32_t esi,
               int32_t edi, uint32_t ret) {
    if (!p2 || !p3) return;
    int32_t v[2]{};
    h.call(0x31220, {ecx});
    int32_t id = (int32_t)h.call(0xC42A, {p2, 0, 0xd40});
    Address tbl = h.read<uint32_t>(dt);
    int32_t pc = 0;
    if (id >= 0 && id < h.read<int32_t>(tbl + 0xba0)) {
        Address row = h.read<uint32_t>(tbl + 0xb98) + id * 0x23c;
        if (row) pc = h.read<int32_t>(row + 0x158);
    }
    v[0] += (pc * v[0]) / 100;
    h.call(0x30E30, {ecx, out(&v[0]), out(&v[1])});
    h.call(0x2F0A0, {(uintptr_t)(edi + (pc * edi) / 100), (uintptr_t)esi, ret, 0});
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
