// BNClient batch b145: 0x11780 0x11960 0x11B00 0x11CB0 0x11DC0 0x11E60 0x11F80 0x12060 0x185C0 0x185E8.
// Win32/ordinal callees are injected as callbacks (garbled decompile; not byte-vectored).
#include <cstdint>

// 1.13c BNClient+0x11780
// Patch-status reply. Order as retail: onBadStatus for 1..3, then #424("Patch","Patches",0x82,"",1) unconditionally,
// then the status switch. Status 1: lock-copy the payload into f670, f508=0, #424 write of the payload, E120, log,
// eef4=0. Status 2: eef4=1, log. Else: fail(1), log, eef4=0.
extern "C" uint32_t d2_bnclient_11780(const uint32_t* p, int len, uint32_t* patchFlag,
                                      void (*onBadStatus)(), void (*patch424)(const char*, int),
                                      void (*download)(const uint32_t*, int), void (*e120)(const char*, int),
                                      void (*fail)(int), void (*log)(const char*)) {
    uint32_t st = p[0];
    if (st != 0 && st < 4) onBadStatus();
    patch424("", 1);
    const char* msg;
    if (st == 1) {
        download(p + 1, len - 4);
        patch424((const char*)(p + 1), len - 4);
        e120((const char*)(p + 1), len - 4);
        msg = "downloading upgrade"; *patchFlag = 0;
    }
    else if (st == 2) { *patchFlag = 1; msg = "user has latest version"; }
    else { fail(1); msg = st == 3 ? "User must reinstall and patch" : "user has unknown version"; *patchFlag = 0; }
    log(msg);
    return 1;
}

// 1.13c BNClient+0x11960: src/core/bnclient/bnclient_11960.cpp

// 1.13c BNClient+0x11B00: src/core/bnclient/bnclient_11b00.cpp

// The three login frame builders below (0x11CB0, 0x11DC0, 0x11E60) are straight-line: every callee is a seam. Addresses are 32-bit words
// (a stack/blob address, or 0xB2000000|rva for a module global: the seam records them, nothing dereferences a module word here).
struct BnPacketEnv {
    uint32_t (*scopy)(uint32_t dst, uint32_t src, uint32_t max);                    // 0x804E  Storm #501 SStrCopy, answers a string pointer
    void (*zero)(uint32_t dst, uint32_t n);                                          // 0x806C  #494
    void (*lower)(uint32_t s);                                                       // 0x8078  #510
    void (*mcopy)(uint32_t dst, uint32_t src, uint32_t n);                           // 0x807E  #491
    void (*hash_a)(uint32_t eax, uint32_t ecx, uint32_t ebx, uint32_t s0, uint32_t s1);   // 0xAED0 (eax/ecx/ebx + 2 stack words)
    void (*hash_b)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0);            // 0xAE10 (eax/ecx/edx + 1 stack word)
    void (*send)(uint32_t ecx, uint32_t edi, uint32_t id);                           // 0xEB00 (ecx = payload, edi = block, id on the stack)
};
static inline uint32_t bn_a(const void* p) { return (uint32_t)(uintptr_t)p; }
#define BN_MOD(rva) (0xB2000000u | (rva))

// 1.13c BNClient+0x11CB0 (stdcall, ret 8: two string pointers; ESI = a third string)
// Clears DAT+0x1EB50, zeroes 0xBC bytes at DAT+0x1EC98, copies ESI / a0 / a1 into three 0x100 buffers (lowercased), runs 0xAED0 over (b1, b3) and 0xAE10 over b1/b2,
// copies 0x20 bytes from DAT+0x1ECF8, copies ESI once more and sends frame 0x55 with ECX = that copy + 0x21. Returns 0.
extern "C" uint32_t d2_bnclient_11cb0(uint32_t a0, uint32_t a1, uint32_t esi, uint32_t* flag, const BnPacketEnv* e) {
    char b1[0x100], b3[0x100], b2[0x100], m[0x20], b4[0x140];
    *flag = 0;
    e->zero(BN_MOD(0x1EC98), 0xBC);
    e->scopy(bn_a(b1), esi, 0x100); e->lower(bn_a(b1));
    e->scopy(bn_a(b2), a0, 0x100);  e->lower(bn_a(b2));
    e->scopy(bn_a(b3), a1, 0x100);  e->lower(bn_a(b3));
    e->hash_a(BN_MOD(0x1EC58), BN_MOD(0x1F8E4), BN_MOD(0x1EC78), bn_a(b1), bn_a(b3));
    e->hash_b(bn_a(b1), BN_MOD(0x1EC98), BN_MOD(0x1F8E4), bn_a(b2));
    e->mcopy(bn_a(m), BN_MOD(0x1ECF8), 0x20);
    uint32_t r = e->scopy(bn_a(b4), esi, 0x100);
    e->send(r + 0x21, bn_a(m), 0x55);
    return 0;
}

// 1.13c BNClient+0x11DC0 (stdcall, ret 4: one string pointer; ESI = a second string)
// Two lowercased 0x100 copies (ESI, a0), 0xAED0 over them, a third copy of ESI (limit 0x7FFFFFFF) and frame 0x52 with ECX = that copy + 0x41. Returns 1.
extern "C" uint32_t d2_bnclient_11dc0(uint32_t a0, uint32_t esi, const BnPacketEnv* e) {
    char b1[0x100], b2[0x100], b3[0x20], b4[0x20], b5[0x100], b6[0x10];
    e->scopy(bn_a(b1), esi, 0x100); e->lower(bn_a(b1));
    e->scopy(bn_a(b2), a0, 0x100);  e->lower(bn_a(b2));
    e->hash_a(bn_a(b3), BN_MOD(0x1F8E4), bn_a(b4), bn_a(b1), bn_a(b2));
    uint32_t r = e->scopy(bn_a(b5), esi, 0x7FFFFFFF);
    e->send(r + 0x41, bn_a(b6), 0x52);
    return 1;
}

// 1.13c BNClient+0x11E60 (stdcall, ret 4: one string pointer; ESI = a second string)
// Clears DAT+0x1EB50, copies ESI into DAT+0x1EB58, zeroes 0xBC bytes at DAT+0x1EC98, two lowercased copies (ESI, a0), 0xAE10, copies 0x20 bytes from DAT+0x1ECF8,
// copies ESI once more and sends frame 0x53 with ECX = that copy + 0x21. Returns 1.
extern "C" uint32_t d2_bnclient_11e60(uint32_t a0, uint32_t esi, uint32_t* flag, const BnPacketEnv* e) {
    char b1[0x100], b2[0x100], m[0x20], b4[0x140];
    *flag = 0;
    e->scopy(BN_MOD(0x1EB58), esi, 0x100);
    e->zero(BN_MOD(0x1EC98), 0xBC);
    e->scopy(bn_a(b1), esi, 0x100); e->lower(bn_a(b1));
    e->scopy(bn_a(b2), a0, 0x100);  e->lower(bn_a(b2));
    e->hash_b(bn_a(b1), BN_MOD(0x1EC98), BN_MOD(0x1F8E4), bn_a(b2));
    e->mcopy(bn_a(m), BN_MOD(0x1ECF8), 0x20);
    uint32_t r = e->scopy(bn_a(b4), esi, 0x100);
    e->send(r + 0x21, bn_a(m), 0x53);
    return 1;
}

// 1.13c BNClient+0x11F80
// r = FUN_b3a0(DAT+0x3F8E4, arg); zero DAT+0x3ECB8 (0x20); if r: copy 0x14 and 0x40 buffers, zero 0x40, send 0x56, return 1; else 0.
extern "C" uint32_t d2_bnclient_11f80(int r, void (*clear)(), void (*sendBlock)()) {
    clear();
    if (!r) return 0;
    sendBlock();
    return 1;
}

// 1.13c BNClient+0x12060: src/core/bnclient/bnclient_12060.cpp

// 1.13c BNClient+0x185C0
// SEH unwind funclet: FUN_21110(*(frame+4)).
extern "C" void d2_bnclient_185c0(uint32_t frame4, void (*f)(uint32_t)) { f(frame4); }

// 1.13c BNClient+0x185E8
// SEH unwind funclet, same as 185C0.
extern "C" void d2_bnclient_185e8(uint32_t frame4, void (*f)(uint32_t)) { f(frame4); }
