// 1.13c D2Client+0xC9BE0  cltactivefunc 90 (Hurricane per-tick handler). ECX = unit, EDX = skill id, one stack
// argument (ret 4). Returns 1 after the spawn loop (or when the count is <= 0), 0 when the skill row is out of range
// or the town branch ends the call.
// Thunks C1F0, C208, C316 are stdcall D2Common imports (#10331, #10057, #10786).
// their convention is the one the pushes show. 0x225D0 takes EAX = sound id, ECX = unit and returns eax.
// 0x88C20 is ret 8, 0x88A70 is ret 0x10, 0xC97D0 is ret 0xC (D2Client functions).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// One callee call. s[] are the stack words in parameter order (s[0] is the top of the stack).
struct D2cHurrCall { uint32_t rva; uint32_t eax; uint32_t ecx; uint32_t ebx; uint32_t n; uint32_t s[4]; };
typedef uint32_t (*D2cHurrHost)(D2cHurrCall*);

struct D2cHurrEnv {
    D2cHurrHost host;
    uint32_t baseGuest;       // [table+0xB98]
    const uint8_t* recBase;   // host view of the row array, row stride 0x23C
    int32_t countBA0;         // [table+0xBA0]
};

static_assert(sizeof(D2cHurrCall) == 4 * 5 + 4 * 4, "hurricane call record");

namespace {
uint32_t hurrCall(const D2cHurrEnv* e, uint32_t rva, uint32_t n, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0,
                  uint32_t s3 = 0) {
    D2cHurrCall c{rva, 0, 0, 0, n, {s0, s1, s2, s3}};
    return e->host(&c);
}
uint32_t hurrCallRegs(const D2cHurrEnv* e, uint32_t rva, uint32_t eax, uint32_t ecx) {
    D2cHurrCall c{rva, eax, ecx, 0, 0, {0, 0, 0, 0}};
    return e->host(&c);
}
}  // namespace

extern "C" {
// unitRef is the guest unit pointer (ECX); skill is EDX (signed); arg is the stack argument.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_cltactivefunc_c9be0(const D2cHurrEnv* e, uint32_t unitRef, int32_t skill,
                                                      uint32_t arg) {
    if (skill < 0 || skill >= e->countBA0) return 0;
    const uint32_t recGuest = e->baseGuest + uint32_t(skill) * 0x23Cu;
    if (recGuest == 0) return 0;
    const uint8_t* rec = e->recBase + size_t(skill) * 0x23C;

    // Gate: EBX = 1 unless (C1F0(unit) != 0 and C208(room) == 0).
    bool flag = true;
    const uint32_t room = hurrCall(e, 0xC1F0, 1, unitRef);
    if (room != 0) {
        const uint32_t inTown = hurrCall(e, 0xC208, 1, room);
        flag = inTown != 0;
    }

    int16_t word = 0;
    std::memcpy(&word, rec + 0x10E, 2);
    uint32_t v = 0;
    if (word > 0) v = hurrCallRegs(e, 0x225D0, uint32_t(int32_t(word)), unitRef);

    if (flag) {
        if (word > 0 && v != 0) hurrCall(e, 0x88C20, 2, unitRef, 1);
        return 0;
    }
    if (word > 0 && v == 0) {   // 0x88A70: EBX = sound id (movsx ebx,[row+0x10E] at 0x9C69), four zero stack words
        D2cHurrCall snd{0x88A70, 0, 0, uint32_t(int32_t(word)), 4, {unitRef, 0, 0, 0}};
        e->host(&snd);
    }

    // Common tail: the count is C316(unit, [row+0x114], skill, arg); each positive count runs three C97D0 spawns.
    uint32_t calc = 0;
    std::memcpy(&calc, rec + 0x114, 4);
    const int32_t count = int32_t(hurrCall(e, 0xC316, 4, unitRef, calc, uint32_t(skill), arg));
    if (count <= 0) return 1;
    for (int32_t i = count; i > 0; --i) {
        for (int k = 0; k < 3; ++k) {
            D2cHurrCall spawn{0xC97D0, unitRef, 0, 0, 3, {recGuest, uint32_t(skill), arg, 0}};  // EAX = unit
            e->host(&spawn);
        }
    }
    return 1;
}
}
