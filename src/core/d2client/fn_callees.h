// src/core/d2client/fn_callees.h -- the callee table of the seam-ported D2Client rows (the d2client_batch_* ports rewritten
// to retail's call sites). Every callee retail reaches that is not ported in the same file is one call: h(rva, the registers
// it reads, its stack words), returning eax. mod(rva) is the address of a module datum (a file-name string, a callback);
// g(rva) is a module global's word. The FN probe binds this to HookEnv (src/core/fn/fnseam.h); a game binding supplies
// the real callees.
#pragma once
#include <cstdint>
#include <cstdlib>
struct FnCallees {
    virtual ~FnCallees() = default;
    virtual uint32_t h(uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0, uint32_t esi = 0,
                       uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) = 0;
    virtual uint32_t mod(uint32_t rva) = 0;
    virtual uint32_t* g(uint32_t rva) = 0;
    virtual void blk(uint32_t rva, const void* p, uint32_t n) { (void)rva; (void)p; (void)n; }   // hookblk: n dwords at p
    virtual void edi(uint32_t rva, uint32_t v) { (void)rva; (void)v; }                             // REGS J: EDI at the call
    virtual void ed(uint32_t v) { (void)v; }                                                       // EDI for the next h() (a callee's EDI out-pointer)
    // retail's fatal-message tail: Ordinal_10265(code) via 0xBF92, Ordinal_10024(file, msg) via 0xBF80, _exit(-1) via 0x3664
    [[noreturn]] void fatal(uint32_t code) {
        uint32_t m = h(0xbf92, 0, 0, 0, 0, 0, code);
        h(0xbf80, 0, 0, 0, 0, 0, mod(0xd359f), m);
        h(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        abort();
    }
    uint32_t& gw(uint32_t rva) { return *g(rva); }
};
template <class T> static inline T& fnAt(uintptr_t b, uint32_t o) { return *(T*)(b + o); }
static inline uintptr_t fnPtr(uint32_t v) { return (uintptr_t)v; }   // a 32-bit pointer field of a retail struct
