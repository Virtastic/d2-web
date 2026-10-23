// BNClient batch b145: 0x11780 0x11960 0x11B00 0x11CB0 0x11DC0 0x11E60 0x11F80 0x12060 0x185C0 0x185E8.
// Win32/ordinal callees are injected as callbacks (garbled decompile; not byte-vectored).
#include <cstdint>

// 1.13c BNClient+0x11780
// Patch-status reply: status 0 or >3 aside, 1=download (copy name under CS, clear flag), 2=latest (flag=1), 3/else=reinstall/unknown (flag=0). Always 1.
extern "C" uint32_t d2_bnclient_11780(const uint32_t* p, int len, uint32_t* patchFlag,
                                      void (*onBadStatus)(), void (*download)(const uint32_t*, int), void (*fail)(int), void (*log)(const char*)) {
    uint32_t st = p[0];
    if (st != 0 && st < 4) onBadStatus();
    const char* msg;
    if (st == 1) { download(p + 1, len - 4); msg = "downloading upgrade"; *patchFlag = 0; }
    else if (st == 2) { *patchFlag = 1; msg = "user has latest version"; }
    else { fail(1); msg = st == 3 ? "User must reinstall and patch" : "user has unknown version"; *patchFlag = 0; }
    log(msg);
    return 1;
}

// 1.13c BNClient+0x11960
// Builds 0x128-byte client-info block (arch 'X86I', product 'VD2D'/'PD2D'-style +0x13fa, tz bias minutes, LCID, UI lang, locale strings), sends 0x50. Always 1.
extern "C" uint32_t d2_bnclient_11960(void (*build)(), void (*send)()) { build(); send(); return 1; }

// 1.13c BNClient+0x11B00
// Tick + lowercased 0x100 string hash + seed hash, rotate buffer DAT+0x3EC58 (0x40), send 0x58. Returns 0.
extern "C" uint32_t d2_bnclient_11b00(void (*gather)(), void (*hash)(), void (*send)()) { gather(); hash(); send(); return 0; }

// 1.13c BNClient+0x11CB0
// Clears DAT+0x3EB50, lowercases/hashes three strings, copies 0x20 from DAT+0x3ECF8, sends 0x55. Returns 0.
extern "C" uint32_t d2_bnclient_11cb0(uint32_t* flag, void (*work)(), void (*send)()) { *flag = 0; work(); send(); return 0; }

// 1.13c BNClient+0x11DC0
// Two lowercased strings hashed, then send 0x52. Returns 1.
extern "C" uint32_t d2_bnclient_11dc0(void (*work)(), void (*send)()) { work(); send(); return 1; }

// 1.13c BNClient+0x11E60
// As 11CB0 (flag clear, strings, copy 0x20) but send 0x53. Returns 1.
extern "C" uint32_t d2_bnclient_11e60(uint32_t* flag, void (*work)(), void (*send)()) { *flag = 0; work(); send(); return 1; }

// 1.13c BNClient+0x11F80
// r = FUN_b3a0(DAT+0x3F8E4, arg); zero DAT+0x3ECB8 (0x20); if r: copy 0x14 and 0x40 buffers, zero 0x40, send 0x56, return 1; else 0.
extern "C" uint32_t d2_bnclient_11f80(int r, void (*clear)(), void (*sendBlock)()) {
    clear();
    if (!r) return 0;
    sendBlock();
    return 1;
}

// 1.13c BNClient+0x12060
// Seed = time + tick + prior; spin (Sleep 10) until DAT+0x3F510 set (return 0 if DAT+0x3F644); if lookup fails set DAT+0x3F1F4=1 under CS, return 0;
// else build 1 or 2 hashed blocks, free handles (Ordinal_280), send 0x51, return 1.
extern "C" uint32_t d2_bnclient_12060(uint32_t* seed, uint32_t timeNow, uint32_t tick, const volatile char* ready, const uint32_t* abort,
                                      void* (*lookup)(), void (*markFail)(), void (*buildAndSend)()) {
    *seed += tick + timeNow;
    while (!*ready) { if (*abort) return 0; }
    if (!lookup()) { markFail(); return 0; }
    buildAndSend();
    return 1;
}

// 1.13c BNClient+0x185C0
// SEH unwind funclet: FUN_21110(*(frame+4)).
extern "C" void d2_bnclient_185c0(uint32_t frame4, void (*f)(uint32_t)) { f(frame4); }

// 1.13c BNClient+0x185E8
// SEH unwind funclet, same as 185C0.
extern "C" void d2_bnclient_185e8(uint32_t frame4, void (*f)(uint32_t)) { f(frame4); }
