// 1.13c D2Client+0xC42B0  row_apply. ECX = unit, EDX = skill id, one stack argument (ret 4).
// Returns 1 when the chain reached the apply step or ran past it, 0 when a lookup failed or the skill is out of range.
// Callees: C310, CB68, CB6E, C3EE and AFF10. C310 through C3EE are import thunks (jmp [IAT]); their stack convention is
// the one the pushes show (one callee-saved register pushed before each argument group, no add esp after the call).
// AFF10 is a D2Client function with ret 0x14: five stack words, EDX = unit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// One callee call. s[] are the stack words in parameter order (s[0] is the top of the stack, pushed last).
struct D2cRowCall { uint32_t rva; uint32_t edx; uint32_t n; uint32_t s[5]; };
typedef uint32_t (*D2cRowHost)(D2cRowCall*);

// Skill table view. baseGuest is [table+0xB98] (guest address of the row array), countBA0 is [table+0xBA0],
// countB6C points at [table+0xB6C] (read after the calls, as retail does), recBase is the host view of the rows.
struct D2cRowEnv {
    D2cRowHost host;
    uint32_t baseGuest;
    const uint8_t* recBase;
    int32_t countBA0;
    const int32_t* countB6C;
};

static_assert(sizeof(D2cRowCall) == 4 + 4 + 4 + 4 * 5, "row call record");

namespace {
uint32_t rowCall(const D2cRowEnv* e, uint32_t rva, uint32_t n, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0,
                 uint32_t s3 = 0, uint32_t s4 = 0, uint32_t edx = 0) {
    D2cRowCall c{rva, edx, n, {s0, s1, s2, s3, s4}};
    return e->host(&c);
}
}  // namespace

extern "C" {
// unitRef is the guest unit pointer (ECX); unitHost is its host view, read at +0x2C after the first three calls.
// skill is EDX (signed); arg is the stack argument.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_row_apply_c42b0(const D2cRowEnv* e, uint32_t unitRef, const uint8_t* unitHost,
                                                  int32_t skill, uint32_t arg) {
    if (skill < 0 || skill >= e->countBA0) return 0;
    const uint32_t recGuest = e->baseGuest + uint32_t(skill) * 0x23Cu;
    if (recGuest == 0) return 0;
    const uint8_t* rec = e->recBase + size_t(skill) * 0x23C;

    const uint32_t c = rowCall(e, 0xC310, 1, unitRef);  // 0xC310: stack word = unit
    if (c == 0) return 0;
    const uint32_t d = rowCall(e, 0xCB68, 1, c);       // EDI
    const uint32_t b = rowCall(e, 0xCB6E, 1, c);       // EBX
    if (d == 0 || b == 0) return 0;

    uint32_t unit2c = 0;
    std::memcpy(&unit2c, unitHost + 0x2C, 4);
    rowCall(e, 0xC3EE, 3, unit2c, d, b);

    int16_t word = 0;
    std::memcpy(&word, rec + 0xEA, 2);
    if (word < 0) return 1;
    if (int32_t(word) >= *e->countB6C) return 1;
    // Stack, top first: word, EDI, EBX, skill (the local at E-4), arg (the caller's stack word); EDX = unit.
    rowCall(e, 0xAFF10, 5, uint32_t(int32_t(word)), d, b, uint32_t(skill), arg, unitRef);
    return 1;
}
}
