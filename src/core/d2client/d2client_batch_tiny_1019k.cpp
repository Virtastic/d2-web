// src/core/d2client/d2client_batch_tiny_1019k.cpp -- five UNMAPPED 1.13c D2Client.dll rows reimplemented
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c plus the retail bytes (alpha lane, batch 1019k).
// D2Client image base 0x6FAB0000; every global named below is at its retail RVA. Retail passes the first
// arguments in registers (ECX/EAX/EDX/ESI/EBX, noted per function); the exports take them in the same
// order. No spec, no vector yet.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Host seams for the four cross-module import thunks these rows reach. A null seam is the unlinked case:
// the stored observable is still produced.
struct D2ClientTiny1019kEnv {
    // 0xD0BA D2gfx.dll!#10014 (D2Gfx/D2FillArea): six stack words.
    uint32_t (*fillArea)(int32_t left, int32_t top, int32_t right, int32_t bottom, uint32_t color, uint32_t trans);
    // 0xBF02 Storm.dll!#571 (SStrChr): first occurrence of c in s, or null.
    const char* (*sstrChr)(const char* s, int32_t c);
    // 0xBF86 Fog.dll!#10043 (D2FogMemDeAlloc): ecx = block, edx = file, stack line, zero.
    void (*fogFree)(void* block, const char* file, int32_t line, int32_t zero);
    // 0xCA5A D2Common.dll!#10332: weapon class id of an item, AX.
    uint32_t (*weaponClassId)(void* pItem);
};

// The Fog#10043 source string at 0x6FB84BDC (RVA 0x0D4BDC).
static const char kFogFile[] = "..\\Source\\D2Client\\UI\\text.cpp";

// 1.13c D2Client+0x68970
// ECX = x, EAX = y, EDX = transparency table; stack = width, height, color. Fills the rectangle
// (x, y)-(x+width, y+height) by forwarding to D2Gfx#10014; its EAX is returned untouched (the wrapper's
// `ret 0xc` only drops the three stack words).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_68970(int32_t x, int32_t y, int32_t width, int32_t height,
                                              uint32_t color, uint32_t trans,
                                              const D2ClientTiny1019kEnv* env) {
    if (env == nullptr || env->fillArea == nullptr) return 0;
    return env->fillArea(x, y, x + width, y + height, color, trans);
}

// 1.13c D2Client+0x6EB50
// Clears the 0x202-byte table at 0x11EC80 (0x80 dwords, then one word) and the two state dwords at
// 0x11C028/0x11C02C. The buffer and the two state words are supplied by the caller.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_6eb50(uint8_t* table, uint32_t* state1, uint32_t* state2) {
    if (table != nullptr) std::memset(table, 0, 0x202);
    if (state1 != nullptr) *state1 = 0;
    if (state2 != nullptr) *state2 = 0;
}

// 1.13c D2Client+0x6EBB0
// ESI = string. Returns the first character after the first '*' when that character exists and is not
// NUL; otherwise returns the input pointer. The '*' is found through Storm#571 (SStrChr), a seam.
extern "C"
EMSCRIPTEN_KEEPALIVE const char* d2_client_6ebb0(const char* s, const D2ClientTiny1019kEnv* env) {
    if (s == nullptr) return nullptr;
    const char* star = (env != nullptr && env->sstrChr != nullptr) ? env->sstrChr(s, '*')
                                                                   : std::strchr(s, '*');
    if (star != nullptr && star[1] != '\0') return star + 1;
    return s;
}

// 0x7B3D0 is its own function, not this batch's: modelled here only as the callee 0x7BF40 calls first.
// ESI = list head; each node is {+0 value, +4 next}. For every node it frees the value (Fog#10043 line
// 0x60B) then the node itself (line 0x60C), walking via the next pointer read before either free.
static void d2_client_7b3d0_body(void* head, const D2ClientTiny1019kEnv* env) {
    while (head != nullptr) {
        void*  value = *reinterpret_cast<void**>(head);
        void*  next  = *reinterpret_cast<void**>(static_cast<uint8_t*>(head) + 4);
        if (env != nullptr && env->fogFree != nullptr) {
            env->fogFree(value, kFogFile, 0x60B, 0);
            env->fogFree(head, kFogFile, 0x60C, 0);
        }
        head = next;
    }
}

// 1.13c D2Client+0x7BF40
// ESI = record whose first dword is a list head. Null does nothing. Otherwise frees the list (0x7B3D0),
// then frees the record itself through Fog#10043 with line 0x68A.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_7bf40(void* self, const D2ClientTiny1019kEnv* env) {
    if (self == nullptr) return;
    d2_client_7b3d0_body(*reinterpret_cast<void**>(self), env);
    if (env != nullptr && env->fogFree != nullptr) env->fogFree(self, kFogFile, 0x68A, 0);
}

// 1.13c D2Client+0x81B70
// EAX = unit, BL = selector. Calls D2Common#10332 once (result used as AX) and, for selector 4, stores
// the 16-bit result at 0x11BF58; for selector 5, at 0x11BF5C. Any other selector still performs the call
// but stores nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_81b70(void* unit, uint8_t selector, uint16_t* slot4, uint16_t* slot5,
                                          const D2ClientTiny1019kEnv* env) {
    const uint16_t id = (env != nullptr && env->weaponClassId != nullptr)
                            ? static_cast<uint16_t>(env->weaponClassId(unit))
                            : 0;
    if (selector == 4) { if (slot4 != nullptr) *slot4 = id; }
    else if (selector == 5) { if (slot5 != nullptr) *slot5 = id; }
}
