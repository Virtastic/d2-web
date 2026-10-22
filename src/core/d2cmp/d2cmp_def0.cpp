#include <stdint.h>
#include <unistd.h>

// Callees (sub_D350, sub_C2D0, sub_DC80, sub_17020, GetCelFromCelContext) are still UNMAPPED/other-file;
// they are passed in until mapped. Fields are u32 slots of the context struct (param_1[0xd], [0xf], [0x11]).
struct D2cmpDef0Callees {
    uint32_t (*cel)(void*, uint32_t*);               // 0x122E0
    uint8_t* (*path)(void*, uint32_t*, char*);       // 0xD350
    uint8_t* (*cache)(const void*);                  // 0x17020
    int (*fallback)(char*);                          // 0xC2D0
    uint32_t (*load)(uint32_t*, uint8_t*, int);      // 0xDC80
};

// 1.13c D2CMP+0xDEF0
extern "C" uint32_t d2cmp_def0(void* self, uint32_t* p, const D2cmpDef0Callees& c, const void* cacheKey) {
    char name[520];
    if (!p) _exit(-1);
    if (p[0xd]) { p[0xf] = c.cel(self, p); return 1; }
    uint8_t* r = c.path(self, p, name);
    if (!r) return 0;
    uint8_t* e = c.cache(cacheKey);
    if (!e) {
        if (!name[0] && !c.fallback(name)) return 0;
        return c.load(p, r, 0);
    }
    if (*(uint8_t*)(*(uintptr_t*)(e + 0x20) + 0x20) & 1) p[0x11] |= 4;
    return 1;
}
