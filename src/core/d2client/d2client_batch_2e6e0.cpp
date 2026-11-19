// src/core/d2client/d2client_batch_2e6e0.cpp -- item/skill tooltip line builders, D2Client+0x2E6E0..0x2EFD0, retail-faithful: callees carry retail's
// register-then-stack words (EAX/ECX/EDX/EBX first, then the stack words), D2Common imports are the thunk RVAs, D2Lang strings are 0xD1EC (ECX=key).
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>

namespace d2client_batch_2e6e0 {
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
// sgptDataTables value (cell 0xCE474 double deref) and the globals the class-record readers use: byte 0x11C390 (cls), dword 0x11C394 (gmode), dword 0x119858 (g2).
struct Env { Address dt; uint32_t cls; int32_t gmode; int32_t g2; };
static uintptr_t u(int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); }
static uintptr_t out(const void* p) { return reinterpret_cast<uintptr_t>(p); }

static void ref(const Host& h, Address dst, uint32_t key) {   // D2Lang string, strcat'd onto dst
    const Address src = static_cast<Address>(h.call(0xD1EC, {key}));
    if (!src || !dst) return;
    Address end = dst;
    while (h.read<uint16_t>(end)) end += 2;
    for (Address p = src; h.read<uint16_t>(p); p += 2, end += 2) h.write<uint16_t>(end, h.read<uint16_t>(p));
    h.write<uint16_t>(end, 0);
}
static void appendA(const Host& h, Address dst, const char* a) {   // ASCII text widened and strcat'd (toUnicode of the wsprintfA text)
    Address end = dst;
    while (h.read<uint16_t>(end)) end += 2;
    for (; *a; ++a, end += 2) h.write<uint16_t>(end, static_cast<uint8_t>(*a));
    h.write<uint16_t>(end, 0);
}
static void rangeText(const Host& h, Address dst, int32_t lo, int32_t hi) {   // "%d" when lo == hi else "%d-%d"
    char a[48];
    if (lo == hi) std::snprintf(a, sizeof a, "%d", lo); else std::snprintf(a, sizeof a, "%d-%d", lo, hi);
    appendA(h, dst, a);
}
static Address classRec(const Host& h, const Env& e, int32_t idx) {   // 0x1A8-stride table at tab+0xA78, count tab+0xA80
    if (idx < 0 || idx >= h.read<int32_t>(e.dt + 0xA80)) return 0;
    return static_cast<Address>(idx) * 0x1A8 + h.read<Address>(e.dt + 0xA78);
}
static int32_t gArg(const Env& e) { return (e.gmode < 2 || e.gmode > 3) ? 1 : e.g2; }
static int32_t mul32(int32_t a, int32_t b) { return static_cast<int32_t>(static_cast<uint32_t>(a) * static_cast<uint32_t>(b)); }
// v*s/100 as 2E900 computes it (the 32-bit product or _alldiv branches chosen by magnitude).
// 1.13c D2Client+0x2E6E0 (133 B): EAX = skill row id, ECX = level, EDX = dst; plain ret. level > 1: v = (level-2)*row[+0x14C] + row[+0x148] (0 when the row is
// out of range or level-1 <= 0); 0x2DBF0(EBX=dst; 0x10D5, v, 0), then D2Lang(0x110D), D2Lang(0xF9E). Returns 1.
uint32_t sub_2E6E0(const Host& h, const Env& e, int32_t eax, int32_t ecx, Address dst) {
    if (ecx <= 1) return 1;
    int32_t edx = ecx - 1, v = 0;
    Address row = 0;
    if (eax >= 0 && eax < h.read<int32_t>(e.dt + 0xBA0)) row = static_cast<Address>(eax) * 0x23C + h.read<Address>(e.dt + 0xB98);
    if (row && edx > 0) v = mul32(edx - 1, h.read<int32_t>(row + 0x14C)) + h.read<int32_t>(row + 0x148);
    h.call(0x2DBF0, {dst, 0x10D5, u(v), 0});
    ref(h, dst, 0x110D);
    ref(h, dst, 0xF9E);
    return 1;
}

// 1.13c D2Client+0x2E770 (394 B): stdcall(dst, item, addLo, addHi) ret 0x10. Class record = tab+0xA78[item word +0xBC]; 0xC244(cnt > 0x124 ? 0x124 : -1, g, cls,
// word, 8, &local); lo = local[5] + addLo, hi = local[6] + addHi; key = 0x2C870(EAX = item byte +0x1DC); key low word != 0: D2Lang(key), "%d"/"%d-%d", D2Lang(0xF9E).
uint32_t sub_2E770(const Host& h, const Env& e, Address dst, Address item, int32_t addLo, int32_t addHi) {
    Address rec = classRec(h, e, h.read<int16_t>(item + 0xBC));
    if (!rec) return 0;
    int32_t loc[14] = {};
    const int32_t first = h.read<int32_t>(e.dt + 0xA80) > 0x124 ? 0x124 : -1;
    h.call(0xC244, {u(first), u(gArg(e)), e.cls, u(h.read<int16_t>(rec + e.cls * 2 + 0xAA)), 8, out(loc)});
    const int32_t lo = loc[5] + addLo, hi = loc[6] + addHi;
    const uint32_t key = static_cast<uint32_t>(h.call(0x2C870, {h.read<uint8_t>(item + 0x1DC)}));
    if (static_cast<uint16_t>(key)) {
        ref(h, dst, key);
        rangeText(h, dst, lo, hi);
        ref(h, dst, 0xF9E);
    }
    return 1;
}

static uint32_t modeKey(uint8_t mode) {   // jump table at 0x2EC44: mode 1..6, anything else the default
    static const uint32_t k[6] = {0x10A1, 0x10A3, 0x10C3, 0x10A2, 0x10A4, 0xDC4};
    return (mode >= 1 && mode <= 6) ? k[mode - 1] : 0x10A1;
}

// 1.13c D2Client+0x2EB80 (194 B): EAX, EDI, ESI regs, stack dst (EBX for 0x2CF20), item (mode byte +0x1DC), ret 8.
// x = CEC2(EAX, EDI, ESI, 1) * 25 >> 8, y = CECE(...) * 25 >> 8; 0x2CF20(EBX=dst; x, y, key by mode).
uint32_t sub_2EB80(const Host& h, uint32_t eax, uint32_t edi, uint32_t esi, Address dst, Address item) {
    const int32_t x = mul32(static_cast<int32_t>(h.call(0xCEC2, {eax, edi, esi, 1})), 0x19) >> 8;
    const int32_t y = mul32(static_cast<int32_t>(h.call(0xCECE, {eax, edi, esi, 1})), 0x19) >> 8;
    return static_cast<uint32_t>(h.call(0x2CF20, {dst, u(x), u(y), modeKey(h.read<uint8_t>(item + 0x1DC))}));
}

// 1.13c D2Client+0x2EC60 (277 B): EAX, ECX regs, stack unit, dst, item, ret 0xC. f = CEB6(unit, ECX, EAX, 0); x = CEC2(unit, ECX, EAX, 1) * f >> 8, y = CECE(...) * f >> 8;
// 0x2E060(EAX=0; dst, 0x2B0E, f); key = 0x2C870(EAX = item byte +0x1DC); key low word != 0: D2Lang(key), "%d"/"%d-%d", D2Lang(0xF9E). Returns 1.
uint32_t sub_2EC60(const Host& h, uint32_t eax, uint32_t ecx, Address unit, Address dst, Address item) {
    const int32_t f = static_cast<int32_t>(h.call(0xCEB6, {unit, ecx, eax, 0}));
    const int32_t x = mul32(static_cast<int32_t>(h.call(0xCEC2, {unit, ecx, eax, 1})), f) >> 8;
    const int32_t y = mul32(static_cast<int32_t>(h.call(0xCECE, {unit, ecx, eax, 1})), f) >> 8;
    h.call(0x2E060, {0, dst, 0x2B0E, u(f)});
    const uint32_t key = static_cast<uint32_t>(h.call(0x2C870, {h.read<uint8_t>(item + 0x1DC)}));
    if (static_cast<uint16_t>(key)) {
        ref(h, dst, key);
        rangeText(h, dst, x, y);
        ref(h, dst, 0xF9E);
    }
    return 1;
}

} // namespace d2client_batch_2e6e0
