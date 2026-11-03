// 1.13c Fog+0x95B5 __setmbcp (MSVC 2003 Release CRT). cdecl(int codepage); returns 0 on success, -1 when the fill fails (the scratch is freed), otherwise the nonzero sub_9421 value.
// The Fog layout of the locale block (0x220 bytes): +0 is not written (the reuse test reads it), +4 cp, +8 DAT_6ffa6eec, +0xC DAT_6ffa6ee4,
// +0x10 five halfwords (DAT_6ffa7010), +0x1C ctype (257 bytes from DAT_6ffa6f00), +0x11D casemap (256 bytes from DAT_6ffa7020).
// The block is kept in DAT_6ffa6ee8 (fog+0x56EE8) after a successful fill; a block whose dword 0 is zero is reused.
#include <cstdint>
#include <cstring>

struct Fog95b5Env {
    void (*lock)(int32_t);             // __lock(0xD)            (fog+0x3CF4)
    void (*unlock)(void);              // sub_96FC: unlock 0xD   (fog+0x96FC)
    uint32_t (*getACP)(void);          // GetACP   (IAT 0x2518C)
    uint32_t (*getOEMCP)(void);        // GetOEMCP (IAT 0x25190)
    void* (*malloc_)(uint32_t);        // _malloc  (fog+0x20C5)
    void (*free_)(void*);              // _free    (fog+0x1FAD)
    int32_t (*sub9421)(int32_t cp);    // sub_9421 (fog+0x9421)
};

struct Fog95b5Glob {
    uint32_t* sbcsFlag;                // DAT_6ff80570 (fog+0x30570)
    const uint32_t* mbcp510;           // DAT_6ff80510 (fog+0x30510)
    uint32_t* cp;                      // DAT_6ffa7004 (fog+0x57004)
    uint32_t* ee8;                     // DAT_6ffa6ee8 (fog+0x56EE8): holds the block address
    const uint32_t* ee4;               // DAT_6ffa6ee4 (fog+0x56EE4)
    const uint32_t* eec;               // DAT_6ffa6eec (fog+0x56EEC)
    const uint16_t* tbl5;              // DAT_6ffa7010 (fog+0x57010): five halfwords
    const uint8_t* ctype;              // DAT_6ffa6f00 (fog+0x56F00): 257 bytes
    const uint8_t* casemap;            // DAT_6ffa7020 (fog+0x57020): 256 bytes
};

// 1.13c Fog+0x95B5
extern "C" int32_t fog_setmbcp_95b5(int32_t cp, const Fog95b5Env* e, const Fog95b5Glob* g) {
    int32_t local24 = -1;
    e->lock(0xd);
    *g->sbcsFlag = 0;
    if (cp == -2) {
        *g->sbcsFlag = 1;
        cp = (int32_t)e->getOEMCP();
    } else if (cp == -3) {
        *g->sbcsFlag = 1;
        cp = (int32_t)e->getACP();
    } else if (cp == -4) {
        *g->sbcsFlag = 1;
        cp = (int32_t)*g->mbcp510;
    }
    if (cp == (int32_t)*g->cp) {
        local24 = 0;
    } else {
        uint8_t* mem = nullptr;
        uint32_t cur = *g->ee8;
        if (cur == 0 || (mem = (uint8_t*)(uintptr_t)cur, *(const uint32_t*)mem != 0))
            mem = (uint8_t*)e->malloc_(0x220);
        if (mem != nullptr && (local24 = e->sub9421(cp)) == 0) {
            uint32_t* w = (uint32_t*)mem;

            w[1] = *g->cp;
            w[2] = *g->eec;
            w[3] = *g->ee4;
            for (uint32_t i = 0; i < 5; i++) memcpy(mem + 0x10 + 2 * i, &g->tbl5[i], 2);
            for (uint32_t i = 0; i < 0x101; i++) mem[0x1c + i] = g->ctype[i];
            *g->ee8 = (uint32_t)(uintptr_t)mem;
            for (uint32_t i = 0; i < 0x100; i++) mem[0x11d + i] = g->casemap[i];
        }
        if (local24 == -1 && mem != (uint8_t*)(uintptr_t)*g->ee8) e->free_(mem);
    }
    e->unlock();
    return local24;
}
