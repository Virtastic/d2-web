// shared helpers for the D2Game FN probes that call a packet-builder port over a HookEnv: wlog records for the 0x8A3E0 send hook
// (`hook 0x8a3e0 2 aB`: eax = client, bytes at stack word 0, length stack word 1) and the blob tagging the capture applies to
// recorded pointers (0xB1<blob index><offset>), using the blob table the harness hands the probe (r[13] = count, r[14+2k] = address, r[15+2k] = size).
#pragma once
#include "hookenv.h"
struct HkWlog {
    uint8_t* base = nullptr; uint8_t* p = nullptr; uint32_t n = 0;
    void init(uint32_t addr) { base = (uint8_t*)(uintptr_t)addr; n = 0; p = base + 4; std::memcpy(base, &n, 4); }
    void add(const void* b, uint32_t len) {
        std::memcpy(p, &len, 4); std::memcpy(p + 4, b, len); p += 4 + ((len + 3) & ~3u);
        ++n; std::memcpy(base, &n, 4);
    }
};
// log a call to 0x8A3E0 (EAX = client, stack: buf, len) and record the bytes
// `mask` is the descriptor's `bytemask` (64 bits): a clear bit zeroes that byte of the logged packet, the stack residue retail never wrote
static inline void hk_send(HookEnv& e, HkWlog& w, const void* client, const void* buf, uint32_t len, uint64_t mask = ~0ull) {
    e.h(0x8A3E0, (uint32_t)(uintptr_t)client, 0, 0, 0, 0, (uint32_t)(uintptr_t)buf, len);
    uint8_t m[4096]; if (len > sizeof m) len = sizeof m;
    std::memcpy(m, buf, len); for (uint32_t i = 0; i < len && i < 64; ++i) if (!(mask >> i & 1)) m[i] = 0;
    w.add(m, len);
}
static inline void hk_tag(const uint32_t* r, uint32_t* L) {
    for (uint32_t i = 0; i < L[0]; i++) {
        if (L[1 + i * 10] & 0x40000000u) continue;   // hookblk data words are not pointers
        for (int w = 1; w < 10; w++) {
            uint32_t& v = L[1 + i * 10 + w];
            for (uint32_t k = 0; k < r[13]; k++) if (v >= r[14 + 2 * k] && v < r[14 + 2 * k] + r[15 + 2 * k]) { v = 0xB1000000u | (k << 16) | (v - r[14 + 2 * k]); break; }
        }
    }
}
