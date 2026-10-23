// Fog batch s-fog-next-15. Win32/CPU seams are function pointers, no platform calls.
#include <stdint.h>
#include <string.h>

// 1.13c Fog+0x1C5B0: vendor from the 12-char cpuid string (+NUL): Intel 1, AMD 2, Cyrix 3, else 0.
uint8_t fog_cpuVendor(const char vendor12[12]) {
    char b[13];
    memcpy(b, vendor12, 12);
    b[12] = 0;
    if (!memcmp(b, "GenuineIntel", 13)) return 1;
    if (!memcmp(b, "AuthenticAMD", 13)) return 2;
    return !memcmp(b, "CyrixInstead", 13) ? 3 : 0;
}

// 1.13c Fog+0xFD80: write the line then the terminator string (DAT_6ff78704) to the sink (Ordinal_503).
void fog_writeLineThenTail(void (*sink)(void*, const char*, int), void* h, const char* line, const char* tail) {
    sink(h, line, 0x7fffffff);
    sink(h, tail, 0x7fffffff);
}

// 1.13c Fog+0xD9E0: bucket lookup under the bucket's lock. Table: mask @+0xC, buckets @+0x14, locks @+0x1C (0x18 each).
// Hit: lock stays held and *cs is set. Miss: lock released, null.
struct fog_HashEnv { void (*initLock)(void*); void (*enter)(void*); void (*leave)(void*); };
uint32_t* fog_hashLookup(const uint8_t* tbl, uint32_t key, void** cs, const fog_HashEnv* e) {
    uint32_t i = *(const uint32_t*)(tbl + 0xC) & key;
    uint8_t* lock = *(uint8_t* const*)(tbl + 0x1C) + i * 0x18;
    e->initLock(lock);
    e->enter(lock);
    uint32_t* p = (*(uint32_t* const* const*)(tbl + 0x14))[i];
    while (p && *p != key) p = (uint32_t*)(uintptr_t)p[1];
    if (!p) { e->leave(lock); return 0; }
    *cs = lock;
    return p;
}

// 1.13c Fog+0x19CB0: pop from the stack struct (count @+0x100, slots from +0); 0 when empty.
uint32_t fog_stackPop(uint8_t* s) {
    int32_t* n = (int32_t*)(s + 0x100);
    if (*n < 1) return 0;
    return ((uint32_t*)s)[--*n];
}

// 1.13c Fog+0x13520: SHA-1 compression of one 64-byte big-endian block into st[0..4].
void fog_sha1Block(uint32_t* st, const uint8_t* in) {
    uint32_t w[80];
    for (int i = 0; i < 16; i++) w[i] = in[4*i] << 24 | in[4*i+1] << 16 | in[4*i+2] << 8 | in[4*i+3];
    for (int i = 16; i < 80; i++) { uint32_t x = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]; w[i] = x << 1 | x >> 31; }
    uint32_t a = st[0], b = st[1], c = st[2], d = st[3], e = st[4];
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
        else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
        else { f = b ^ c ^ d; k = 0xca62c1d6; }
        uint32_t t = (a << 5 | a >> 27) + f + e + k + w[i];
        e = d; d = c; c = b << 30 | b >> 2; b = a; a = t;
    }
    st[0] += a; st[1] += b; st[2] += c; st[3] += d; st[4] += e;
}
