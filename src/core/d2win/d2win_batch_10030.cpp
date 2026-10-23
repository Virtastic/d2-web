// src/core/d2win/d2win_batch_10030.cpp -- 1.13c D2Win.dll control-object functions transcribed from
// the cached Ghidra decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c) and checked against the
// retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base 0x6F8E0000). Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Every function here takes the control object as retail does (ECX for the __fastcall entries, the
// first stack word for the others) and checks the type word at +0x00 == 2; a mismatch is retail's
// Fog #10265(id) -> #10024("") -> _exit(-1), folded into d2win_fatal. The imported D2CMP/Fog/Win32
// callees and the control callbacks the constructors publish are seams on D2WinBatch10030Env, passed
// after the retail arguments, which keep their order. 32-bit pointer fields keep the retail encoding
// through (uint32_t)(uintptr_t) -- the objects live in the low wasm heap, as in the earlier batches.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win (control layouts only; no function bodies).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

[[noreturn]] static void d2win_fatal(uint32_t) { _exit(-1); }

// Retail module state, one symbol per DLL address so a probe can seed and read it.
extern "C" {
uint32_t d2win_g_6f8fb17c;   // 0x6F8FB17C gdwBitMasks[0]
uint32_t d2win_g_6f8fb184;   // 0x6F8FB184 gdwBitMasks[2]
uint32_t d2win_g_6f901488;   // 0x6F901488
uint32_t d2win_g_6f90148c;   // 0x6F90148C
uint32_t d2win_g_6f9014a0;   // 0x6F9014A0, head of the control list (linked through +0x3C)
}

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2WinBatch10030Env {
    // D2CMP #10086 (D2CMP+0x11610): *p must be 6; returns p[5].
    int32_t  (*d2cmp10086)(void* ctx);
    // Fog #10042, ECX = size, EDX = file, stack = (line, unused).
    void*    (*alloc)(uint32_t size, const char* file, int32_t line, int32_t n0);
    // Control callbacks the constructors publish into the object; ECX = control.
    uint32_t (*renderFEE0)(void* self);      // D2Win+0xFEE0
    uint32_t (*render104B0)(void* self);     // D2Win+0x104B0
    // 0x107C0: the D2CMP cel-context imports. #10021 is __thiscall in retail; its ECX is 0 at this
    // call site, so only the stack argument is modelled.
    void*    (*getCelFromCelContext)(void* ctx);   // D2CMP #10021
    uint32_t (*cel4)(void* cel);                   // D2CMP #10060, returns cel[1]
    uint32_t (*cel8)(void* cel);                   // D2CMP #10000, returns cel[2]
    uint32_t (*celC)(void* cel);                   // D2CMP #10002, returns cel[3]
    uint32_t (*cel10)(void* cel);                  // D2CMP #10069, returns cel[4]
    uint32_t (*getTickCount)(void);
};

// Retail source-file literal passed to FOG_Alloc (read from the DLL's .rdata at 0x6F8FC9D8).
static const char kD2WinImageCpp[] = "..\\Source\\D2Win\\Src\\D2WinImage.cpp";

// Append a freshly built control to the global list at 0x6F9014A0 (the link is +0x3C).
static void* d2win_control_append(void* obj) {
    if (d2win_g_6f9014a0 == 0) {
        d2win_g_6f9014a0 = (uint32_t)(uintptr_t)obj;
        return obj;
    }
    uint32_t node = d2win_g_6f9014a0;
    while (rd32((void*)(uintptr_t)node, 0x3C) != 0) node = rd32((void*)(uintptr_t)node, 0x3C);
    wr32((void*)(uintptr_t)node, 0x3C, (uint32_t)(uintptr_t)obj);
    return obj;
}

// 1.13c D2Win+0x10030 (52 bytes): the type-2 object's slot +0x04 is handed to D2CMP #10086 and its
// result returned. A type other than 2 is fatal 0xA9. The retail code is a tail jump after replacing
// its own argument, so the return value is D2CMP #10086's.
// 1.13c D2Win+0x10030
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10030(void* obj, const D2WinBatch10030Env* env) {
    if (rd32(obj, 0x00) != 2) d2win_fatal(0xA9);
    return env->d2cmp10086((void*)(uintptr_t)rd32(obj, 0x04));
}

// 1.13c D2Win+0x10070 (55 bytes): a type-2 object gets 5 at +0x44 and the call returns 1. Retail
// pops one extra stack word (ret 8), unused here.
// 1.13c D2Win+0x10070
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10070(void* obj, uint32_t unused) {
    (void)unused;
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x9D);
    wr32(obj, 0x44, 5);
    return 1;
}

// 1.13c D2Win+0x100B0 (81 bytes): the caller's cap is clamped to D2CMP #10086(obj[1]) - 1, then to
// zero from below (signed), and stored at +0x40. Returns 1; a type other than 2 is fatal 0x8B.
// 1.13c D2Win+0x100b0
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_100B0(void* obj, int32_t cap, const D2WinBatch10030Env* env) {
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x8B);
    const int32_t limit = env->d2cmp10086((void*)(uintptr_t)rd32(obj, 0x04)) - 1;
    int32_t v = cap <= limit ? cap : limit;
    if (v < 0) v = 0;
    wr32(obj, 0x40, (uint32_t)v);
    return 1;
}

// 1.13c D2Win+0x10110 (59 bytes): store the two arguments at +0x0C and +0x10, return 1. A type other
// than 2 is fatal 0x6C.
// 1.13c D2Win+0x10110
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10110(void* obj, uint32_t a, uint32_t b) {
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x6C);
    wr32(obj, 0x0C, a);
    wr32(obj, 0x10, b);
    return 1;
}

// 1.13c D2Win+0x10190 (238 bytes): hit-test a control. The caller passes a pointer to the control
// pointer. Unless the control is a type-2 object fatal 0x159. The +0x08 word must intersect both
// gdwBitMasks[2] and gdwBitMasks[0]. Then, with x = gdwMasks? no: xRange = 0x6F901488 - obj[3] and
// yBase = (obj[6] - obj[4]) + 0x6F90148C, the list at obj[0x12] (nodes of five dwords: left, top,
// right, bottom, callback) is scanned in order; the first node with xRange >= left,
// yBase >= top - bottom, xRange < right + left and yBase < top has its callback called with no
// arguments and that result returned. Otherwise the control's own +0x34 callback is called and its
// result returned; with neither, 0.
// 1.13c D2Win+0x10190
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_10190(uint32_t* controlPtr) {
    void* obj = (void*)(uintptr_t)*controlPtr;
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x159);
    if ((d2win_g_6f8fb184 & rd32(obj, 0x08)) == 0) return 0;
    if ((d2win_g_6f8fb17c & rd32(obj, 0x08)) == 0) return 0;

    const int32_t xRange = (int32_t)d2win_g_6f901488 - (int32_t)rd32(obj, 0x0C);
    const int32_t yBase = (int32_t)rd32(obj, 0x18) - (int32_t)rd32(obj, 0x10) +
                          (int32_t)d2win_g_6f90148c;
    uint32_t node = rd32(obj, 0x48);
    if (node != 0) {
        while (rd32((void*)(uintptr_t)node, 0x10) != 0) {
            const int32_t left   = (int32_t)rd32((void*)(uintptr_t)node, 0x00);
            const int32_t top    = (int32_t)rd32((void*)(uintptr_t)node, 0x04);
            const int32_t right  = (int32_t)rd32((void*)(uintptr_t)node, 0x08);
            const int32_t bottom = (int32_t)rd32((void*)(uintptr_t)node, 0x0C);
            if (xRange >= left && yBase >= top - bottom && xRange < right + left && yBase < top)
                return ((uint32_t (*)(void))(uintptr_t)rd32((void*)(uintptr_t)node, 0x10))();
            node += 0x14;
        }
    }
    const uint32_t own = rd32(obj, 0x34);
    if (own != 0) return ((uint32_t (*)(void))(uintptr_t)own)();
    return 0;
}

// 1.13c D2Win+0x103D0 (212 bytes): construct and link a 0x4C-byte type-2 control. __fastcall: ECX
// and EDX land at +0x0C and +0x10; six stack words are taken in the order arg1..arg6, stored at
// +0x18, +0x14, +0x04, +0x34, +0x48 and +0x30. The +0x08 word is gdwBitMasks[0] | gdwBitMasks[2];
// +0x1C gets the render callback D2Win+0xFEE0, +0x24 the hit-test D2Win+0x10190, +0x44 the value 5,
// and the zeroed +0x3C link is appended after the current 0x6F9014A0 tail (or becomes the head).
// Returns the new object. FOG_Alloc is called with size 0x4C and line 0x1D.
// 1.13c D2Win+0x103D0
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2win_sub_103D0(uint32_t ecx, uint32_t edx,
                                           uint32_t s1, uint32_t s2, uint32_t s3,
                                           uint32_t s4, uint32_t s5, uint32_t s6,
                                           const D2WinBatch10030Env* env) {
    void* obj = env->alloc(0x4C, kD2WinImageCpp, 0x1D, 0);
    std::memset(obj, 0, 0x4C);
    wr32(obj, 0x00, 2);
    wr32(obj, 0x08, d2win_g_6f8fb17c | d2win_g_6f8fb184);
    wr32(obj, 0x04, s3);
    wr32(obj, 0x0C, ecx);
    wr32(obj, 0x10, edx);
    wr32(obj, 0x14, s2);
    wr32(obj, 0x18, s1);
    wr32(obj, 0x1C, (uint32_t)(uintptr_t)env->renderFEE0);
    wr32(obj, 0x24, (uint32_t)(uintptr_t)&d2win_sub_10190);
    wr32(obj, 0x30, s6);
    wr32(obj, 0x34, s4);
    wr32(obj, 0x44, 5);
    wr32(obj, 0x48, s5);
    return d2win_control_append(obj);
}

// 1.13c D2Win+0x10610 (210 bytes): construct and link a 0x54-byte type-2 control; seven stack words
// arg1..arg7 go to +0x0C, +0x10, +0x14, +0x18, +0x04, +0x4C and +0x50. The +0x08 word is
// gdwBitMasks[0] | gdwBitMasks[2]; +0x1C gets D2Win+0x104B0, +0x24 D2Win+0x10190, +0x44 the value 5,
// and the zeroed +0x3C link is appended after 0x6F9014A0's tail (or becomes the head). Returns the
// new object. FOG_Alloc is called with size 0x54 and line 0x3B.
// 1.13c D2Win+0x10610
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2win_sub_10610(uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
                                           uint32_t s5, uint32_t s6, uint32_t s7,
                                           const D2WinBatch10030Env* env) {
    void* obj = env->alloc(0x54, kD2WinImageCpp, 0x3B, 0);
    std::memset(obj, 0, 0x54);
    wr32(obj, 0x00, 2);
    wr32(obj, 0x08, d2win_g_6f8fb17c | d2win_g_6f8fb184);
    wr32(obj, 0x04, s5);
    wr32(obj, 0x0C, s1);
    wr32(obj, 0x10, s2);
    wr32(obj, 0x14, s3);
    wr32(obj, 0x18, s4);
    wr32(obj, 0x1C, (uint32_t)(uintptr_t)env->render104B0);
    wr32(obj, 0x24, (uint32_t)(uintptr_t)&d2win_sub_10190);
    wr32(obj, 0x44, 5);
    wr32(obj, 0x4C, s6);
    wr32(obj, 0x50, s7);
    return d2win_control_append(obj);
}

// 1.13c D2Win+0x10700 (55 bytes): store the argument at +0x5C and return 1; a type other than 2 is
// fatal 0x183. Retail pops one extra stack word (ret 8).
// 1.13c D2Win+0x10700
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_10700(void* obj, uint32_t v) {
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x183);
    wr32(obj, 0x5C, v);
    return 1;
}

// 1.13c D2Win+0x107C0 (215 bytes): does the control's current image overlap the screen rectangle.
// __fastcall: ECX = control. Index +0x50 (scaled by 0x10) into the array at +0x40 gives an entry; its
// first word (or zero) becomes the cel-header pointer in a zeroed 0x48-byte cel context at +0x34,
// with frame 0 at +0x00 and layer 0 at +0x40. D2CMP #10021 resolves the cel; #10060/#10000/#10002/
// #10069 give its +0x04/+0x08/+0x0C/+0x10 words. With x = +0x0C, y = +0x10, a = cel[1], b = cel[2],
// c = cel[3], d = cel[4]:
//   sx = 0x6F901488 - c, sy = 0x6F90148C - d
// returns 1 iff sx >= x, sy >= y - b, sx < x + a and sy < y; else 0. A type other than 2 is fatal 0x73.
// 1.13c D2Win+0x107C0
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_107C0(void* obj, const D2WinBatch10030Env* env) {
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x73);

    const uint32_t entryPtr = rd32(obj, 0x40) + (rd32(obj, 0x50) << 4);
    const uint32_t entry = rd32((void*)(uintptr_t)entryPtr, 0x00);
    const uint32_t header = entry != 0 ? rd32((void*)(uintptr_t)entry, 0x00) : 0;

    uint8_t ctx[0x48];
    std::memset(ctx, 0, sizeof ctx);
    wr32(ctx, 0x00, 0);
    wr32(ctx, 0x34, header);
    wr32(ctx, 0x40, 0);

    void* cel = env->getCelFromCelContext(ctx);
    const int32_t x = (int32_t)rd32(obj, 0x0C);
    const int32_t y = (int32_t)rd32(obj, 0x10);
    const int32_t a = (int32_t)env->cel4(cel);
    const int32_t b = (int32_t)env->cel8(cel);
    const int32_t c = (int32_t)env->celC(cel);
    const int32_t d = (int32_t)env->cel10(cel);

    const int32_t sx = (int32_t)d2win_g_6f901488 - c;
    const int32_t sy = (int32_t)d2win_g_6f90148c - d;
    if (sx < x) return 0;
    if (sy < y - b) return 0;
    if (sx >= x + a) return 0;
    if (sy >= y) return 0;
    return 1;
}

// 1.13c D2Win+0x108A0 (95 bytes): advance the control's state word at +0x50. State 3 becomes 4, the
// word at +0x4C is cleared, the tick count is stored at +0x48 and 1 is returned. State 2 or 4 returns
// 0; any other state returns 1. A type other than 2 is fatal 0x5C.
// 1.13c D2Win+0x108A0
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_108A0(void* obj, const D2WinBatch10030Env* env) {
    if (rd32(obj, 0x00) != 2) d2win_fatal(0x5C);
    const uint32_t state = rd32(obj, 0x50);
    if (state == 3) {
        wr32(obj, 0x50, 4);
        wr32(obj, 0x4C, 0);
        wr32(obj, 0x48, env->getTickCount());
        return 1;
    }
    if (state != 2 && state != 4) return 1;
    return 0;
}
