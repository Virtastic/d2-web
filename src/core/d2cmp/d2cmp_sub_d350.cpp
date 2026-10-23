#include <stdint.h>
#include <string.h>
#include <unistd.h>

// Callees are seams (other rows / Fog ordinals), passed in like d2cmp_def0.
struct D2cmpD350Callees {
    int (*c670)(void* self, void* p1);               // 0xC670
    uint8_t* (*lookup)(const char* name);            // 0x12B20(&tbl 6fe76240, name) -> entry
    void (*e273a0)();                                // 0x173A0
    int (*c2d0)(char* out);                          // 0xC2D0
    uint32_t* (*slot)();                             // 0x12AA0(tbl) -> new slot
    int32_t* (*alloc)();                             // 0x174B0 -> block ptr holder
    int (*d23010)();                                 // 0x13010
    void (*fog10102)(); void (*fog10103)(); void (*fog10104)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
    uint32_t (*fog10105)();
    const uint32_t* tbl883a0;                        // DAT_6fe883a0
    void (*fatal)(uint32_t line);                    // Ordinal_10265+10024, then _exit(-1)
};

// 1.13c D2CMP+0x0D350 (eax=name, ecx=self, stack p1, outName)
extern "C" uint8_t* d2cmp_sub_d350(void* self, const char* name, uint8_t* p1, char* outName, const D2cmpD350Callees& c) {
    *outName = 0;
    if (!c.c670(self, p1)) return 0;
    uint8_t* r;
    if (uint8_t* e = c.lookup(name)) {
        c.e273a0();
        r = (uint8_t*)(uintptr_t)**(uint32_t**)(e + 0x24);
    } else {
        if (!c.c2d0(outName)) return 0;
        uint32_t* s = c.slot();
        if (!s) { c.fatal(0x41c); _exit(-1); }
        int32_t* b = c.alloc();
        s[9] = (uint32_t)(uintptr_t)b;
        if (!b) { c.fatal(0x421); _exit(-1); }
        r = (uint8_t*)(uintptr_t)*b;
        memset(r, 0, 0xc0);
        strcpy((char*)r + 0xa0, name);
        if (!(p1[0x44] & 1)) {
            c.fog10102();
            if (!outName) { r[0] |= 1; }
            else {
                c.fog10104(0x18, 0, 0, 0, 0);
                uint32_t v = c.fog10105();
                memset(r + 4, 0, 0x10); memset(r + 0x14, 0, 8);   // 0x4-0xf, 0x14-0x1b
                c.fog10104(0, 0, 0, 0, 0);
                c.fog10103();
                uint32_t n; memcpy(&n, r + 0x14, 4);
                if (n > 0x20) { c.fatal(0x452); _exit(-1); }
                uint32_t k; memcpy(&k, r + 0x18, 4);
                for (uint32_t i = 0; i < n; i++) memcpy(r + 0x1c + 4 * i, &c.tbl883a0[k * i], 4);
                memcpy(r + 0x1c + 4 * n, &v, 4);
            }
        } else if (!c.d23010()) r[0] |= 1;
    }
    if (!r) { c.fatal(0x465); _exit(-1); }
    return r;
}
