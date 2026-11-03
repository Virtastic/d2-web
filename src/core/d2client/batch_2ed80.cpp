// src/core/d2client/batch_2ed80.cpp -- damage-range line emitters, D2Client+0x2ED80..0x2EF80, retail-faithful (callee calls
// carry retail's register and stack words; D2Common imports are the thunk RVAs 0xC880/0xC96A/0xCEC2/0xCECE/0xCEAA/0xC868).
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
static uintptr_t u(int32_t v) { return static_cast<uintptr_t>(static_cast<uint32_t>(v)); }
static uintptr_t out(const void* p) { return reinterpret_cast<uintptr_t>(p); }

// 1.13c D2Client+0x2ED80 (65 B): EAX, EDI regs, stack p1, p2 (=EBX dst), ret 8. (CEC2, CECE)(p1, eax, edi, 1) * 75 >> 8 into 0x2CF20(x, y, 0x10A1).
uint32_t sub_2ED80(const Host& h, uint32_t eax, uint32_t edi, uint32_t p1, Address p2) {
    int32_t x = static_cast<int32_t>(h.call(0xCEC2, {p1, eax, edi, 1})) * 0x4b >> 8;
    int32_t y = static_cast<int32_t>(h.call(0xCECE, {p1, eax, edi, 1})) * 0x4b >> 8;
    return static_cast<uint32_t>(h.call(0x2CF20, {p2, u(x), u(y), 0x10A1}));   // EBX = p2
}

// 1.13c D2Client+0x2EDD0 (254 B): EAX=edi, ECX=ebx, ESI regs; stack p1 (=EBX dst), p2 (this for 0x2E2D0/0x2E270), ret 8.
uint32_t sub_2EDD0(const Host& h, uint32_t eax, uint32_t ecx, uint32_t esi, Address p1, Address p2) {
    int32_t lohi[2];   // lo = [p2 slot], hi = local
    lohi[0] = static_cast<int32_t>(h.call(0xC880, {0, ecx, esi, eax})) * 0x4b;
    lohi[1] = static_cast<int32_t>(h.call(0xC96A, {0, ecx, esi, eax})) * 0x4b;
    h.call(0x2E2D0, {p2, out(&lohi[1]), out(&lohi[0])});     // ECX=this, EBX=&hi, stack &lo
    h.call(0x2E270, {p2, out(&lohi[1]), out(&lohi[0])});
    int32_t lo = lohi[0] >> 8, hi = lohi[1] >> 8;
    uint32_t k = static_cast<uint8_t>(h.call(0xC868, {esi}));
    static const uint32_t key[7] = {0x10A1, 0x10A1, 0x10A3, 0x10C3, 0x10A2, 0x10A4, 0xDC4};
    return static_cast<uint32_t>(h.call(0x2CF20, {p1, u(lo), u(hi), k >= 1 && k <= 6 ? key[k] : 0x10A1}));
}

// 1.13c D2Client+0x2EEF0 (42 B): EAX, ECX, EDX regs, stack p1 (=EBX dst), ret 4.
uint32_t sub_2EEF0(const Host& h, uint32_t eax, uint32_t ecx, uint32_t edx, Address p1) {
    uint32_t id = static_cast<uint32_t>(h.call(0xCEAA, {edx, ecx, eax}));
    if (!id) return 0;
    return static_cast<uint32_t>(h.call(0x2D9B0, {p1, 0x10A6, 0x10B3, id, 1}));   // EBX = p1
}

// 1.13c D2Client+0x2EF20 (87 B): EAX=edi, EBX regs, stack p1,p2,p3 (=EAX dst of 0x2D220), ret 0xC.
uint32_t sub_2EF20(const Host& h, uint32_t eax, uint32_t ebx, uint32_t p1, uint32_t p2, Address p3) {
    int32_t lo = static_cast<int32_t>(h.call(0xC880, {0, p1, ebx, eax})) >> 8;
    int32_t hi = static_cast<int32_t>(h.call(0xC96A, {0, p1, ebx, eax})) >> 8;
    if (!lo && !hi) return 0;
    uint32_t kind = static_cast<uint8_t>(h.call(0xC868, {ebx}));
    return static_cast<uint32_t>(h.call(0x2D220, {p3, p2, u(lo), u(hi), kind}));
}

// 1.13c D2Client+0x2EF80 (80 B): EBX, EDI regs, stack p1, p2, p3 (item rec, mode byte +0x1DC), p4 (=EAX dst of 0x2D220), ret 0x10.
uint32_t sub_2EF80(const Host& h, uint32_t ebx, uint32_t edi, uint32_t p1, uint32_t p2, Address p3, Address p4) {
    int32_t x = static_cast<int32_t>(h.call(0xCEC2, {p1, ebx, edi, 1}));
    int32_t y = static_cast<int32_t>(h.call(0xCECE, {p1, ebx, edi, 1}));
    if (!x && !y) return 0;
    uint32_t mode = h.read<uint8_t>(p3 + 0x1DC);
    return static_cast<uint32_t>(h.call(0x2D220, {p4, p2, u(x >> 8), u(y >> 8), mode}));
}
} // namespace d2client_batch_2ed80
