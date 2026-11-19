// 1.13c D2Client+0x69410 -- monster COF path builder (env-seam port; globals/callees injected).
// ABI: EAX = monster id (monstats row), EDI = out dword (weapon-class code), stack: mode, char* dest; ret 8.
#include <cstdint>
#include <emscripten.h>
typedef uint32_t u32;

struct D2cMonCofEnv {
    u32 (*rd32)(u32 addr);          // guest 32-bit read
    u32 (*modeData)(u32 mode);      // D2Common Ordinal_10470(mode,1); 0 -> fatal
    void (*fatal)();                // retail: error box + _exit(-1)
    void (*wr32)(u32 addr, u32 v);  // guest 32-bit write
};

extern "C" {
// 1.13c D2Client+0x69410
EMSCRIPTEN_KEEPALIVE void d2c_69410(const D2cMonCofEnv* e, int32_t id, u32 outDword, u32 mode, char* dest) {
    u32 dt = e->rd32(e->rd32(0x6fb7e474));
    if (id < 0 || id >= (int32_t)e->rd32(dt + 0xa80)) return;
    u32 row = (u32)id * 0x1a8 + e->rd32(dt + 0xa78);
    if (!row) return;
    int16_t cls = (int16_t)(e->rd32(row + 0x18) & 0xffff);            // FUN_1260: class index
    if (cls < 0 || cls >= (int32_t)e->rd32(dt + 0xa98)) return;
    u32 cr = (u32)cls * 0x134 + e->rd32(dt + 0xa90);
    u32 name = e->rd32(row + 0x10);
    u32 md = e->modeData(mode);
    if (!md) { e->fatal(); return; }
    u32 tok = e->rd32(md + 0x20);
    if (mode != 0 && mode != 0xc) e->wr32(outDword, e->rd32(cr + 0x10));
    else if ((e->rd32(e->rd32(0x6fb7ee50)) & (e->rd32(cr + 4) >> 16) & 0xff) != 0)
        e->wr32(outDword, e->rd32(cr + 0x10));
    else e->wr32(outDword, 0x20687468);                                // "hth "
    u32 n = e->rd32(0x6fba9ea8);
    for (u32 i = 0; i < n; i++)
        if (e->rd32(0x6fba9eb0 + i * 8) == mode) tok = e->rd32(0x6fba9eac + i * 8);
    u32 wc = e->rd32(outDword);
    char nm[4], mt[4], wt[4];
    for (int i = 0; i < 3; i++) {
        uint8_t a = (name >> (8 * i)) & 0xff, b = (tok >> (8 * i)) & 0xff, c = (wc >> (8 * i)) & 0xff;
        nm[i] = a == 0x20 ? 0 : a; mt[i] = b == 0x20 ? 0 : b; wt[i] = c == 0x20 ? 0 : c;
    }
    nm[3] = mt[3] = wt[3] = 0;
    // wsprintfA("%s\\%s\\COF\\%s%s%s.COF", "DATA\\GLOBAL\\MONSTERS", nm, nm, mt, wt)
    char* o = dest;
    auto cat = [&](const char* s) { while (*s) *o++ = *s++; };
    cat("DATA\\GLOBAL\\MONSTERS\\"); cat(nm); cat("\\COF\\"); cat(nm); cat(mt); cat(wt); cat(".COF");
    *o = 0;
}
}
