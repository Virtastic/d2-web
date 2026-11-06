#include <stdint.h>
#include <string.h>
#include <unistd.h>

// Callees are seams (other rows / Fog ordinals), passed in like d2cmp_9070.
struct D2cmpD350Callees {
    int (*c670)(char* name, void* p1);               // 0xC670 (EAX = name out, EDX = p1)
    uint8_t* (*lookup)(const char* name);            // 0x12B20(&tbl 6fe76240, name): the entry
    void (*e173a0)();                                // 0x173A0 (ESI = 6fe7727c)
    int (*c2d0)(char* out);                          // 0xC2D0
    uint32_t* (*slot)();                             // 0x12AA0(tbl, 0): the new slot
    int32_t* (*alloc)(uint32_t* slot);               // 0x174B0 (EAX = 0xC0, ESI = 6fe7727c, stack slot): the block holder
    int (*d13010)(char* path);                       // 0x13010 (ECX = path)
    uint32_t (*open)(char* path);                    // Fog 10102 (ECX = path, EDX = &handle): the handle written back
    void (*read)(uint32_t h, void* dst, uint32_t n); // Fog 10104 (ECX = handle, EDX = dst, stack n,0,0,0,0)
    uint32_t (*get)(uint32_t h);                     // Fog 10105 (ECX = handle, EDX = 0)
    void (*close)(uint32_t h);                       // Fog 10103 (ECX = handle)
    uint32_t* tbl883a0;                              // DAT_6fe883a0 (the second read fills it)
    void (*fatal)(uint32_t line);                    // Ordinal_10265 + 10024, then _exit(-1)
};

// 1.13c D2CMP+0x0D350 (eax = name, stack p1, outName; ret 8)
extern "C" uint8_t* d2cmp_d350(char* name, uint8_t* p1, char* outName, const D2cmpD350Callees& c) {
    *outName = 0;
    if (!c.c670(name, p1)) return 0;
    uint8_t* r;
    if (uint8_t* e = c.lookup(name)) {
        c.e173a0();
        r = (uint8_t*)(uintptr_t)**(uint32_t**)(e + 0x24);
    } else {
        if (!c.c2d0(outName)) return 0;
        uint32_t* s = c.slot();
        if (!s) { c.fatal(0x41c); _exit(-1); }
        int32_t* b = c.alloc(s);
        s[9] = (uint32_t)(uintptr_t)b;
        if (!b) { c.fatal(0x421); _exit(-1); }
        r = (uint8_t*)(uintptr_t)*b;
        memset(r, 0, 0xc0);
        strcpy((char*)r + 0xa0, name);
        if (!(p1[0x44] & 1)) {
            uint32_t h = c.open(outName);
            if (!h) r[0] |= 1;
            else {
                uint32_t l[6] = {0, 0, 0, 0, 0, 0};
                c.read(h, l, 0x18);
                uint32_t v = c.get(h);
                memcpy(r + 4, &l[0], 4); memcpy(r + 8, &l[1], 4); memcpy(r + 0xc, &l[2], 4);
                memcpy(r + 0x14, &l[4], 4); memcpy(r + 0x18, &l[5], 4);
                uint32_t cnt = l[5] * l[4];
                if (cnt >= 0x4000) { c.fatal(0x44d); _exit(-1); }
                c.read(h, c.tbl883a0, cnt * 4);
                c.close(h);
                uint32_t n = l[4];
                if (n > 0x20) { c.fatal(0x452); _exit(-1); }
                uint32_t k = l[5];
                for (uint32_t i = 0; i < n; i++) memcpy(r + 0x1c + 4 * i, &c.tbl883a0[k * i], 4);
                memcpy(r + 0x1c + 4 * n, &v, 4);
            }
        } else if (!c.d13010(outName)) r[0] |= 1;
    }
    if (!r) { c.fatal(0x465); _exit(-1); }
    return r;
}
