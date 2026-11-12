// BNClient: what the small chat-leg frame composers share. Each one builds a payload on its stack and hands it to the
// module's send routine with a message id; some then wait for the answer to that id. The seams are the callees.
#pragma once
#include <cstdint>

struct BnSendEnv {
    uint32_t (*strcopy)(char* dst, const char* src, uint32_t cap);      // Storm SStrCopy: copies, terminates, returns the length
    int32_t (*send)(uint32_t len, uint8_t id, const uint8_t* payload);  // the module's frame sender
    void (*wait)(uint32_t id);                                         // blocks until the answer to id has arrived
    uint32_t (*expansion)();                                           // nonzero when the expansion is running
};

// Generic callee seam for the glue functions: the callee's entry, the five registers it may read and up to four stack
// words, returning its EAX. A pointer into the module is passed as BN_MOD(rva); a local is passed as its address.
struct BnCallEnv {
    void* ctx;
    uint32_t (*call)(void* ctx, uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
};
#define BN_MOD(rva) (0xB2000000u | (uint32_t)(rva))
static inline uint32_t bn_call(const BnCallEnv& e, uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0,
                               uint32_t s = 0, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    return e.call(e.ctx, rva, a, c, d, b, s, s0, s1, s2, s3);
}
static inline uint32_t bn_ptr(const void* p) { return (uint32_t)(uintptr_t)p; }
