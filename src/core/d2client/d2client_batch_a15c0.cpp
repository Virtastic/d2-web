// src/core/d2client/d2client_batch_a15c0.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (0xA15C0..0xA2F60,
// the unit-targeting / skill-cast-effect cluster) reimplemented from the decompiles in
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000.
//
// Every unrecovered callee, Fog/D2Common ordinal and data table is a host seam on one env, called in
// retail's call order after the retail arguments (the d2client_batch_bf80.cpp pattern). Several bodies
// take their arguments in registers (EAX/EBX/ESI/EDI unaffected-register reads in the decompile); those
// are named extra parameters here, in decompile order. Guest 32-bit pointers held inside structs are
// widened through uintptr_t, never cast to uint32_t.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatchA15c0Env {
    // 0xA1340: fills out[0..1] (client context pair), returns nonzero on success.
    int32_t  (*getClientCtx)(int32_t* out);                              // 0xA1340
    uint32_t (*f60450)(int32_t a);                                       // 0xB0450
    uint32_t (*f630f0)(uint32_t* particle);                              // 0xB30F0
    void     (*f69a80)(uintptr_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t, uintptr_t, uint32_t, uint32_t); // 0xB9A80
    uint32_t (*f515c0)();                                                // 0xA15C0
    void     (*fatal)(uint32_t line);                                    // ord10265+ord10024+_exit(-1), noreturn
    void     (*fda0b0)();                                                // 0x2A0B0
    // unit accessors
    int32_t  (*ord10331)(uintptr_t unit);                                // room of unit
    int32_t  (*ord10057)(int32_t a);
    int32_t  (*ord10239)();
    int32_t  (*ord10064)(int32_t a);
    int32_t  (*ord10278)();
    int32_t  (*ord10494)();
    int32_t  (*ord11138)(uintptr_t unit);
    int32_t  (*ord11076)(int32_t a, const uint32_t* p, const uint32_t* q, int32_t n);
    uint32_t (*f1630)(uintptr_t unit);                                   // 0x1630 x
    uint32_t (*f1660)(uintptr_t unit);                                   // 0x1660 y
    uintptr_t (*f1580)();                                                // 0x1580
    uintptr_t (*f1230)();                                                // 0x1230
    uint32_t (*f62a0)(uintptr_t unit);                                   // 0xA62A0
    int32_t  (*f50ee0)(uintptr_t unit);                                  // 0xA0EE0
    int32_t  (*f50750)(uint32_t room, uintptr_t ctx);                    // 0xA0750
    uint32_t (*f1760)(uint32_t ecx, uint32_t id);                        // 0x1760
    uint32_t gdwBitMask0, gdwBitMask4, gdwBitMask0x1c;                   // gdwBitMasks[0], [4], [0x1c]
    int32_t  (*ord10383)(int32_t room, uint32_t** list, uint32_t* count);
    int32_t  (*ord10750)(uint32_t p);
    int32_t  (*ord10867)(uint32_t p);
    int32_t  (*ord10769)(int32_t a, int32_t b, int32_t c, int32_t d);
    int32_t  (*scanCb)(uintptr_t unit, uintptr_t ctx);                   // (*param_2)() in 0xA1860
    // data tables
    uint32_t (*dtI32)(uint32_t off);                                       // *(sgptDataTables) + off, i32
    uint8_t* (*dtPtr)(uint32_t off);                                     // pointer field at that offset
    int32_t  (*rodata)(uint32_t addr);                                   // i32 at 0x6fb84xxx
    int32_t  (*handlerCount)();                                          // DAT_6fb8ec50
    int32_t  (*callHandler)(int32_t idx, int32_t arg);                   // DAT_6fb8ea48[idx]; -1 if null
    int32_t  (*ord10286)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    int32_t  (*ord11129)();
    uint32_t (*ord10392)(uint32_t a);
    int32_t  (*f38a70)(uintptr_t u, int, int, int);                      // 0x88A70
    uintptr_t (*f55f70)();                                               // 0xA5F70
    int32_t  (*f1b300)(uintptr_t u, int, int, int, int);                 // 0x6B300
    int32_t  (*ord10889)(uintptr_t unit);
    int32_t  (*ord10709)(uintptr_t unit, uintptr_t arg);
    int32_t  (*ord10871)();
    int32_t  (*ord11013)(int, int, int, uint32_t, uint32_t);
    void     (*ord10785)(int32_t, int32_t);
    int32_t  (*ord10807)();
    void     (*ord10188)(int32_t, int32_t, int32_t, int32_t);
    int32_t  (*ord10641)(uintptr_t unit);
    uint32_t (*ord10321)(int32_t a);
    int32_t  (*ord11088)(int32_t a);
    int32_t  (*ord10614)();
    uint32_t cb50650;                                                    // LAB_6fb50650
    uintptr_t cb509f0, cb50740;                                          // scan filters passed by 0xA1A50/0xA1AD0
};
typedef D2ClientBatchA15c0Env Env;

static inline uint32_t rd32(uintptr_t b, uint32_t o) { return *(const uint32_t*)(b + o); }
static inline void     wr32(uintptr_t b, uint32_t o, uint32_t v) { *(uint32_t*)(b + o) = v; }
static inline int16_t  rd16(const uint8_t* b, uint32_t o) { return *(const int16_t*)(b + o); }

// Retail's 64-bit LCG, state in two dwords at +0x20 (lo) / +0x24 (hi): s = lo*0x6AC690C5 + hi.
static uint32_t seedRoll(uintptr_t h, uint32_t n) {
    if ((int32_t)n < 1) return 0;
    uint64_t s = (uint64_t)rd32(h, 0x20) * 0x6AC690C5ull + rd32(h, 0x24);
    wr32(h, 0x20, (uint32_t)s); wr32(h, 0x24, (uint32_t)(s >> 32));
    return (n & (n - 1)) == 0 ? ((n - 1) & (uint32_t)s) : (uint32_t)s % n;
}

// unit width/height pair used by 0xA1860 and 0xA27B0: types 2,4,5 read the info record directly.
static void unitSize(const Env* e, uintptr_t u, int32_t* w, int32_t* h) {
    int32_t t = (int32_t)rd32(u, 0); uint32_t info = rd32(u, 0x2C);
    bool direct = t == 2 || (t > 3 && t < 6);
    *w = direct ? (int32_t)rd32(info, 0x10) : (info ? e->ord10750(info) : 0);
    *h = direct ? (int32_t)rd32(info, 0x0C) : (info ? e->ord10867(info) : 0);
}

// 1.13c D2Client+0xA15C0 (86 B): the player-owned context object, or 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a15c0(const Env* e) {
    int32_t out[2] = {0, 0};  // out[0] = local_8, out[1] = local_4
    if (e->getClientCtx(out) && out[0] != 0 && out[1] != 0) return e->f60450(out[0]);
    return 0;
}

// 0xA1A50 / 0xA1AD0: real register conventions live in d2client_a1a50.cpp / d2client_a1ad0.cpp (the old ports here followed a wrong decompile reading).

// retired, see d2client_a27b0.cpp: D2Client 0xA27B0.
