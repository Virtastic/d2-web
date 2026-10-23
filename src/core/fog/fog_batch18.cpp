// Fog batch 18: hacklist match, hacklist size log throttle, Blizzard SHA-1 block.
#include <cstdint>

// 1.13c Fog+0x0CEB0 -- hacklist match. `self` is the object retail receives in EAX: entry count at +0x848,
// entries of 8 bytes from +0x48 (value, mask-table index). `maskTable` is the table at Fog+0x7FEF8.
bool fog_hacklist_match_ceb0(const uint8_t* self, const uint32_t* maskTable, uint32_t key) {
    const int32_t n = *(const int32_t*)(self + 0x848);
    for (int32_t i = 0; i < n; i++) {
        const uint32_t* e = (const uint32_t*)(self + 0x48 + i * 8);
        if ((maskTable[e[1]] & key) == e[0]) return true;
    }
    return false;
}

// 1.13c Fog+0x0D230 -- at most once per 2000 ms, log "[HACKLIST]  Hacklist size is %d" via Fog+0x190A0.
// Returns true when it logged (caller supplies the tick and the stored last-log tick).
bool fog_hacklist_log_d230(uint32_t now, uint32_t* lastTick) {
    if (now - *lastTick <= 2000) return false;
    *lastTick = now;
    return true;
}

// 1.13c Fog+0x1D2A0 -- Blizzard "broken" SHA-1 block: state[0..4], 16 input words follow at [5..20].
// Differs from SHA-1 in the schedule: w[t] = ROL(1, (w[t-3]^w[t-8]^w[t-14]^w[t-16]) & 31) instead of ROL(x,1).
static inline uint32_t rol(uint32_t x, unsigned n) { n &= 31; return n ? (x << n) | (x >> (32 - n)) : x; }
void fog_sha1_block_1d2a0(uint32_t* p) {
    uint32_t w[80];
    for (int i = 0; i < 16; i++) w[i] = p[5 + i];
    for (int t = 16; t < 80; t++) w[t] = rol(1, (w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16]) & 31);
    uint32_t a = p[0], b = p[1], c = p[2], d = p[3], e = p[4];
    for (int t = 0; t < 80; t++) {
        uint32_t f, k;
        if (t < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
        else if (t < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
        else if (t < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
        else { f = b ^ c ^ d; k = 0xca62c1d6; }
        uint32_t tmp = rol(a, 5) + f + e + w[t] + k;
        e = d; d = c; c = rol(b, 30); b = a; a = tmp;
    }
    p[0] += a; p[1] += b; p[2] += c; p[3] += d; p[4] += e;
}
