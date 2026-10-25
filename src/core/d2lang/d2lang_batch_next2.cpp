// 1.13c D2Lang implementation batch next-2: string-table lookups. Table = loaded .tbl image
// (u16@2 n, i32@4 count, u32@0xd used, u32@0x11 size, u16 index[n] @0x15, 0x11-byte nodes after).
// `cache` is the per-node resolved-pointer array (i32 per node). Retail failure paths _exit(-1).
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" uint32_t d2lang_8fe0(const char* s, uint32_t mod);

static uint16_t rd16(const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; }
static uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }

// 1.13c D2Lang+0x09050: id >= n falls back to 500; node must be in range, inside the image and
// flagged 1, with a non-NULL cache slot. Returns the cache slot value.
extern "C" uint32_t d2lang_9050_lookup(const uint8_t* tbl, uint16_t id, const uint32_t* cache)
{
    uint32_t i = id;
    if (rd16(tbl + 2) <= id) i = 500;
    i = rd16(tbl + 0x15 + i * 2);
    if ((int32_t)i >= (int32_t)rd32(tbl + 4)) exit(-1);
    const uint8_t* node = tbl + 0x15 + (uint32_t)rd16(tbl + 2) * 2 + i * 0x11;
    if (node >= tbl + rd32(tbl + 0x11) || *node != 1 || cache[i] == 0) exit(-1);
    return cache[i];
}

// 1.13c D2Lang+0x09360: probe-hash lookup of a narrow key; returns node u16@+1 and *out = cache slot,
// or 0xffff (out untouched) after `used` probes without a match. NULL cache slot is fatal.
extern "C" uint32_t d2lang_9360_find(const uint8_t* tbl, const uint32_t* cache, const uint8_t* key, uint32_t* out)
{
    const uint8_t* nodes = tbl + 0x15 + (uint32_t)rd16(tbl + 2) * 2;
    uint32_t count = rd32(tbl + 4), used = rd32(tbl + 0xd), i = d2lang_8fe0((const char*)key, count);
    for (uint32_t n = 0; n < used; n++) {
        const uint8_t* nd = nodes + i * 0x11;
        if (*nd == 1) {
            const uint8_t* a = tbl + rd32(nd + 7);
            const uint8_t* b = key;
            while (*a == *b && *a) { a++; b++; }       // retail walks the same two bytes at a time
            if (*a == *b) {
                if (!cache[i]) exit(-1);
                *out = cache[i];
                return rd16(nodes + i * 0x10 + 1);
            }
        }
        i = (i + 1) % count;
    }
    return 0xffff;
}

// 1.13c D2Lang+0x09590: load the four suffix chars (DAT 10558/1055c/10560/10564) from string ids
// 0x14d0..0x14d3: first wide char narrowed if the string is one char long, else defaults 'K','M','B','T'.
extern "C" void d2lang_9590_suffixes(const uint8_t* tbl, const uint32_t* cache, int16_t out[4])
{
    static const int16_t def[4] = {0x4b, 0x4d, 0x42, 0x54};
    if (!tbl) exit(-1);
    for (int k = 0; k < 4; k++) {
        const uint16_t* w = (const uint16_t*)(uintptr_t)d2lang_9050_lookup(tbl, 0x14d0 + k, cache);
        int n = 0;                                      // narrowed copy is capped at 4 chars
        while (n < 4 && w[n]) n++;
        out[k] = n < 2 ? (int16_t)(int8_t)(n ? (char)w[0] : 0) : def[k];
    }
}
