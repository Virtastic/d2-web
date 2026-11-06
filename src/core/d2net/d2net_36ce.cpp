// 1.13c D2Net+0x36CE __setmbcp: select a multibyte codepage under lock 0xD.
// -2 / -3 / -4 pick GetOEMCP / GetACP / __lc_codepage and set the system flag (0xB0D0); the current codepage returns 0.
// Otherwise the snapshot block at __ptmbcinfo (0xB458) is reused when its first dword is 0, else a fresh 0x220-byte
// block is allocated; the table builder 0x353A runs, and on 0 the block takes the new state and becomes __ptmbcinfo.
// A -1 from the builder (or a failed allocation) frees the block when it is not __ptmbcinfo.
#include <cstdint>
#include <cstring>
#include "d2net_mbwin.h"
namespace d2net_36ce {
struct Host {
    void (*lock)(int);
    uint32_t (*oemcp)();
    uint32_t (*acp)();
    void* (*malloc_)(uint32_t);
    int (*build)(uint32_t cp);     // 0x353A, on the same window
    void (*free_)(void*);
    void (*unlock)(int);
};
// 1.13c D2Net+0x36CE
int setmbcp(const Host& h, d2net_mb::Window& w, uint32_t& fSystemSet, uint32_t lcCodepage, int cpArg) {
    int r = -1;
    h.lock(0xd);
    fSystemSet = 0;
    uint32_t cp = (uint32_t)cpArg;
    if (cpArg == -2) { fSystemSet = 1; cp = h.oemcp(); }
    else if (cpArg == -3) { fSystemSet = 1; cp = h.acp(); }
    else if (cpArg == -4) { fSystemSet = 1; cp = lcCodepage; }
    if (cp == w.mbcodepage) {
        r = 0;
    } else {
        uint8_t* mem = (uint8_t*)(uintptr_t)w.mbinfo;
        uint32_t first = 0;
        if (mem) memcpy(&first, mem, 4);
        if (!mem || first != 0) mem = (uint8_t*)h.malloc_(0x220);
        if (mem) {
            r = h.build(cp);
            if (r == 0) {
                const uint32_t hdr[4] = { 0, w.mbcodepage, w.ismbcodepage, w.lcid };
                memcpy(mem, hdr, 16);
                memcpy(mem + 0x10, w.mbulinfo, 10);
                memcpy(mem + 0x1c, w.mbctype, 0x101);
                memcpy(mem + 0x11d, w.mbcasemap, 0x100);
                w.mbinfo = (uint32_t)(uintptr_t)mem;
            }
        }
        if (r == -1 && (uint32_t)(uintptr_t)mem != w.mbinfo) h.free_(mem);
    }
    h.unlock(0xd);
    return r;
}
}
