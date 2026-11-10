// d2cmp+0x12e60 sub_12E60: FUNCTION-mode probe.
// The vector records retail's layout: 128 four-byte bucket heads; entries {key18, key1c, key14, tiles, next} and links
// {tile, next} as 4-byte fields. The port's index is native width. The probe converts the retail index into native
// entries in its own pools, calls the port, and writes the output tiles back as retail dwords.
#include <emscripten.h>
#include <cstdint>

extern "C" uint32_t d2cmp_12e60(const void* index, uint32_t key1c, const void** out,
                                uint32_t cap, uint32_t key14, uint32_t key18);

// Native mirror of the port's structs (src/core/d2cmp/d2cmp_tile_index.cpp).
struct PLink { const void* tile; PLink* next; };
struct PEntry { uint32_t key18, key1c, key14; PLink* tiles; PEntry* next; };
struct PIndex { PEntry* buckets[128]; };

static PEntry pe[1024];
static uint32_t npe;
static PLink pl[2048];
static uint32_t npl;

static uint32_t rd(uint32_t addr) { return *(const uint32_t*)(uintptr_t)addr; }

static PLink* conv_link(uint32_t a) {
    if (!a) return nullptr;
    PLink* l = &pl[npl++];
    l->tile = (const void*)(uintptr_t)rd(a);
    l->next = conv_link(rd(a + 4));
    return l;
}

static PEntry* conv_entry(uint32_t a) {
    if (!a) return nullptr;
    PEntry* e = &pe[npe++];
    e->key18 = rd(a);
    e->key1c = rd(a + 4);
    e->key14 = rd(a + 8);
    e->tiles = conv_link(rd(a + 0xc));
    e->next = conv_entry(rd(a + 0x10));
    return e;
}

// r[0] = index (EAX), r[1] = key1c (EBX), r[2] = out (stack 1), r[3] = cap (stack 2),
// r[5] = key14 (stack 3), r[6] = key18 (stack 4); r[4] = result count
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_12e60(uint32_t* r) {
    const uint32_t tab = r[0];
    const uint32_t out = r[2];
    const uint32_t cap = r[3];
    npe = 0;
    npl = 0;
    PIndex idx;
    for (uint32_t i = 0; i < 128; ++i) idx.buckets[i] = conv_entry(rd(tab + 4 * i));
    const void* nout[16] = {};
    uint32_t n = d2cmp_12e60(&idx, r[1], nout, cap, r[5], r[6]);
    for (uint32_t i = 0; i < n; ++i) *(uint32_t*)(uintptr_t)(out + 4 * i) = (uint32_t)(uintptr_t)nout[i];
    r[4] = n;
}
