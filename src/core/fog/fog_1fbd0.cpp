// 1.13c Fog+0x1FBD0 and 0x1FDC0: the handle pool.
// Pool fields: +0x100 slot count, +0x104 slot size, +0x108 table-present flag, +0x10c next-free hint, +0x110 highest used,
// +0x114 used count, +0x118 next 16-bit id, +0x11c slot table. A slot's first u16 is its id; 0 means free.
#include <cstdint>
#include <cstdlib>
// 1.13c Fog+0x1FBD0: returns id | index << 16, or 0xFFFFFFFF when the pool is full.
uint32_t fog_sub_1fbd0(uint8_t* p) {
    auto D = [&](int o) { return (int32_t*)(p + o); };
    if (!*D(0x108)) abort();
    uint32_t sz = *D(0x104); int32_t i = *D(0x10c);
    uint8_t* base = (uint8_t*)(uintptr_t)*(uint32_t*)D(0x11c);
    uint16_t* s = (uint16_t*)(base + sz * i);
    for (; i < *D(0x100); i++, s = (uint16_t*)((uint8_t*)s + sz)) {
        if (*s) continue;
        for (uint32_t k = 0; k < sz; k++) ((uint8_t*)s)[k] = 0;
        uint16_t* c = (uint16_t*)D(0x118);
        *s = *c; if (++*c == 0) *c = 0x8000;
        ++*D(0x114); *D(0x10c) = i + 1;
        if (*D(0x110) < i) *D(0x110) = i;
        return *s | (uint32_t)i << 16;
    }
    return 0xFFFFFFFF;
}
// 1.13c Fog+0x1FDC0: frees handle h; returns the new used count (the vector pins EAX). A bad handle aborts.
uint32_t fog_sub_1fdc0(uint8_t* p, uint32_t h) {
    auto D = [&](int o) { return (int32_t*)(p + o); };
    int32_t i = (int16_t)(h >> 16);
    uint8_t* base = (uint8_t*)(uintptr_t)*(uint32_t*)D(0x11c);
    if (h == 0xFFFFFFFF) abort();
    if ((int32_t)h < 0 || i >= *D(0x100) || !*D(0x108)) abort();
    uint16_t* s = (uint16_t*)(base + *D(0x104) * i);
    if (!*s || ((int16_t)h != 0 && *s != (uint16_t)h)) abort();
    *s = 0;
    if (i < *D(0x10c)) *D(0x10c) = i;
    if (i == *D(0x110)) do {
        s = (uint16_t*)((uint8_t*)s - *D(0x104));
        if (--*D(0x110) < 0) break;
    } while (!*s);
    return (uint32_t)--*D(0x114);
}
