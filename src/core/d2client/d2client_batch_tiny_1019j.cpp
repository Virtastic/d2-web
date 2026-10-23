// src/core/d2client/d2client_batch_tiny_1019j.cpp -- five UNMAPPED 1.13c D2Client.dll rows reimplemented
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, batch 1019j). Base 0x6FAB0000; every
// global this file names is at its retail RVA. Register-argument functions keep retail's order in the
// parameter list. No spec, no vector yet.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

// Host seams. Retail reaches these through import thunks (0x0C09A = Fog.dll!#10055, 0x0D27C/0x0D26A/
// 0x0D270 = D2sound.dll!#10067/#10011/#10001). A null seam is the unlinked case: the call site keeps
// retail's control flow and the stored observable is still produced.
struct D2ClientTiny1019jEnv {
    int32_t (*tick)(void);        // 0x0C09A Fog.dll!#10055: millisecond clock (the 0x52D90 deadline base)
    void    (*sound_10067)(void); // 0x0D27C D2sound.dll!#10067
    void    (*sound_10011)(void); // 0x0D26A D2sound.dll!#10011
    void    (*sound_10001)(void); // 0x0D270 D2sound.dll!#10001
};

// 0x17F0 (src/core/d2client/d2client_batch_1760.cpp) is the skills.txt -> skilldesc.txt lookup.
extern "C" void* d2_client_skilldesc_for_skill(int32_t nSkillId);

// 1.13c D2Client+0x2FD10
// stdcall, one stack word = skilldesc column; EAX = skill id (the ABI the caller 0x54E10 uses: `push 0;
// call`). The 0x17F0 lookup gives the skilldesc row; a null lookup returns the 0x1506 sentinel, else the
// little-endian word at row + 8 + index * 2. The index is not range-checked in retail.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2fd10(int32_t index, int32_t skillId) {
    const uint8_t* desc = reinterpret_cast<const uint8_t*>(d2_client_skilldesc_for_skill(skillId));
    if (desc == nullptr) return 0x1506;
    uint16_t v = 0;
    std::memcpy(&v, desc + 8 + index * 2, sizeof(v));
    return v;
}

// 1.13c D2Client+0x521A0
// ESI = span. When the span is positive and the deadline global (+0x11C2E8 DAT_6fbcc2e8) is zero, arm it
// to tick() + span; otherwise leave it alone. Only caller 0x1E160 passes 5.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_521a0(int32_t span, uint32_t* deadline, const D2ClientTiny1019jEnv* env) {
    if (span > 0 && *deadline == 0) {
        const int32_t now = (env && env->tick) ? env->tick() : 0;
        *deadline = (uint32_t)(now + span);
    }
}

// 0x636A0 is its own function, not this batch's: body transcribed here only as the callee the three
// setters below use. fastcall ECX, EDX; one stack word = the object. It reads the unsigned dword at
// object + 0x120, forms (obj+0x120 - 1) * (ecx - edx + 1) / (ecx - edx) with the x87 unsigned-conversion
// sequence (fild; fadds 4294967296.0f when the top bit is set), then rounds to the nearest even integer
// (0xCACC4 = fistpll under the default FPU mode). The DAT_6fbd3710 != 0 branch (0xC9FF7 opcode 0x0D) is
// the CRT's software x87 divide and produces the same quotient. Integer arithmetic below is exact:
// every uint32 operand is exact in x87 and an exact .5 quotient has a finite binary representation.
// 1.13c D2Client+0x636A0
static uint64_t d2_client_636a0_body(const void* self, int32_t ecx, int32_t edx) {
    const uint32_t n = *reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(self) + 0x120);
    const uint64_t num = (uint64_t)(uint32_t)(n - 1) * (uint64_t)(uint32_t)(ecx - edx + 1);
    const uint64_t den = (uint32_t)(ecx - edx);
    if (den == 0) return 0;                                  // no caller reaches it; x87 would fault
    uint64_t q = num / den;
    const uint64_t r = num % den;
    if (r * 2 > den || (r * 2 == den && (q & 1) != 0)) ++q;  // round half to even
    return q;
}

// 1.13c D2Client+0x64330
// fastcall ECX = the object. Calls D2sound #10067 for its side effect, then stores the low dword of
// 0x636A0(object, 0x64, 0) at object + 0x124. Reached from the data table at 0x0E7130.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_64330(void* self, const D2ClientTiny1019jEnv* env) {
    if (env && env->sound_10067) env->sound_10067();
    const uint64_t v = d2_client_636a0_body(self, 0x64, 0);
    *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(self) + 0x124) = (int32_t)(uint32_t)v;
}

// 1.13c D2Client+0x64400
// Same body as 0x64330 with D2sound #10011 (thunk 0x0D26A); low dword of 0x636A0(object, 0x64, 0) to
// object + 0x124. Reached from the data table at 0x0E6140.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_64400(void* self, const D2ClientTiny1019jEnv* env) {
    if (env && env->sound_10011) env->sound_10011();
    const uint64_t v = d2_client_636a0_body(self, 0x64, 0);
    *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(self) + 0x124) = (int32_t)(uint32_t)v;
}

// 1.13c D2Client+0x64440
// Same body as 0x64330 with D2sound #10001 (thunk 0x0D270); low dword of 0x636A0(object, 0x64, 0) to
// object + 0x124. Reached from the data table at 0x0E5BF0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_64440(void* self, const D2ClientTiny1019jEnv* env) {
    if (env && env->sound_10001) env->sound_10001();
    const uint64_t v = d2_client_636a0_body(self, 0x64, 0);
    *reinterpret_cast<int32_t*>(static_cast<uint8_t*>(self) + 0x124) = (int32_t)(uint32_t)v;
}
