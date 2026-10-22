// BNClient batch b48: 0xB5F0 0xB700 0xB7F0 0xB9B0 0xBA00 0xBB40 0xC1B0 0xC220.
// Register-convention inputs of retail are explicit parameters; Storm/ordinal calls are injected.
#include <cstdint>
#include <cstring>

typedef void* (*BnAllocFn)(uint32_t line, uint32_t flags);   // Ordinal_10042
typedef void (*BnFreeFn)(void* p, uint32_t line, uint32_t flags); // Ordinal_10043
typedef int (*BnSendFn)(uint32_t len, uint32_t id);          // 0xEB00 (<0 on failure)
typedef void (*BnFatalFn)(uint32_t code);                    // Ordinal_10265/10024/_exit(-1); no return

static void bn_replace_blob(void** slot, const void* src, uint32_t n, uint32_t freeLine, uint32_t allocLine,
                            BnAllocFn a, BnFreeFn f) {
    if (*slot) f(*slot, freeLine, 0);
    *slot = 0;
    *slot = a(allocLine, 0);
    memcpy(*slot, src, n);
}

// 1.13c BNClient+0x0B5F0
// Replace the blob in global DAT+0x3F888: free old (line 0x39A), alloc (line 0x356), copy n bytes.
extern "C" void d2_bnclient_b5f0(void** slot, const void* src, uint32_t n, BnAllocFn a, BnFreeFn f) {
    bn_replace_blob(slot, src, n, 0x39A, 0x356, a, f);
}
// 1.13c BNClient+0x0B700
// Same, global DAT+0x3F880: free line 0x224, alloc line 0x21B.
extern "C" void d2_bnclient_b700(void** slot, const void* src, uint32_t n, BnAllocFn a, BnFreeFn f) {
    bn_replace_blob(slot, src, n, 0x224, 0x21B, a, f);
}
// 1.13c BNClient+0x0B7F0
// Same, global DAT+0x3F884: free line 0x1CF, alloc line 0x1A0.
extern "C" void d2_bnclient_b7f0(void** slot, const void* src, uint32_t n, BnAllocFn a, BnFreeFn f) {
    bn_replace_blob(slot, src, n, 0x1CF, 0x1A0, a, f);
}

// 1.13c BNClient+0x0B9B0
// Send message 0x5DC (ordinal 10263 gate must be non-zero, else false) with length 0x5D.
extern "C" bool d2_bnclient_b9b0(int gate, BnSendFn send) {
    if (gate == 0) return false;
    return send(0x5DC, 0x5D) >= 0;
}

// Pack up to three NUL-terminated strings (each < 0x101 incl NUL) into buf; fatal(code) on overflow.
static bool bn_pack(const char* const* s, int n, const uint32_t* nullErr, const uint32_t* lenErr,
                    uint8_t* buf, uint32_t* total, BnFatalFn fatal) {
    uint32_t off = 0;
    for (int i = 0; i < n; i++) {
        if (!s[i]) { if (i == 0) fatal(nullErr[0]); return false; }
        if (!*s[i]) { if (i == 0) fatal(nullErr[1]); return false; }
        uint32_t l = (uint32_t)strlen(s[i]) + 1;
        if (l >= 0x101) fatal(lenErr[i]);
        memcpy(buf + off, s[i], l);
        off += l;
    }
    *total = off;
    return true;
}

// 1.13c BNClient+0x0BA00
// Three strings packed back-to-back, sent as id 0x5B. First arg null/empty is fatal (0x5CD/0x5CE);
// later null/empty returns false; over-long fatal 0x5D8/0x5E3/0x5EE.
extern "C" bool d2_bnclient_ba00(const char* a, const char* b, const char* c, BnSendFn send, BnFatalFn fatal) {
    static const uint32_t ne[2] = {0x5CD, 0x5CE}, le[3] = {0x5D8, 0x5E3, 0x5EE};
    uint8_t buf[0x303]; uint32_t n; const char* s[3] = {a, b, c};
    if (!bn_pack(s, 3, ne, le, buf, &n, fatal)) return false;
    return send(n, 0x5B) >= 0;
}

// 1.13c BNClient+0x0BB40
// Two strings packed, sent as id 0x5A. Fatal 0x5A6/0x5A7 on bad first, 0x5AF/0x5B9 on over-long.
extern "C" bool d2_bnclient_bb40(const char* a, const char* b, BnSendFn send, BnFatalFn fatal) {
    static const uint32_t ne[2] = {0x5A6, 0x5A7}, le[2] = {0x5AF, 0x5B9};
    uint8_t buf[0x203]; uint32_t n; const char* s[2] = {a, b};
    if (!bn_pack(s, 2, ne, le, buf, &n, fatal)) return false;
    return send(n, 0x5A) >= 0;
}

typedef void (*BnQueueFn)(const uint8_t* pkt, uint32_t len); // Ordinal_491 + Ordinal_10007(4)

static uint32_t bn_ff_cmd(uint8_t cmd, int connected, int busy, int* pending, BnQueueFn q) {
    const uint8_t pkt[4] = {0xFF, cmd, 4, 0};
    if (connected != 0 && busy == 0) q(pkt, 4);
    else *pending = 1; // DAT+0x3F644
    return 1;
}
// 1.13c BNClient+0x0C1B0
// BNCS packet 0xFF 0x1F, length 4: queue if connected (DAT+0x3F654) and not busy (+0x3F660), else set pending.
extern "C" uint32_t d2_bnclient_c1b0(int connected, int busy, int* pending, BnQueueFn q) {
    return bn_ff_cmd(0x1F, connected, busy, pending, q);
}
// 1.13c BNClient+0x0C220
// BNCS packet 0xFF 0x02, length 4; same gating as 0xC1B0.
extern "C" uint32_t d2_bnclient_c220(int connected, int busy, int* pending, BnQueueFn q) {
    return bn_ff_cmd(0x02, connected, busy, pending, q);
}
