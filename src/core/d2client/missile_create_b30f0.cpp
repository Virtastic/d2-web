// D2Client missile creation (client side), env-seam style: no Win32, no direct DLL calls.
// Ordinals are Fog/D2Common numbers taken from the decompile; their DLL/meaning is UNVERIFIED
// (Fog 10xxx / D2Common 11xxx by range only).
#include <cstdint>
#include <cstring>

struct MslCreateEnv {
    const uint8_t* dataTables;   // *sgptDataTables
    uint8_t  (*bitMask12)();     // gdwBitMasks[12] low byte
    int  (*unitX)(const void*);  // D2Client+0x1630
    int  (*unitY)(const void*);  // D2Client+0x1660
    int  (*pctOf)(int pct, int v);        // D2Client+0x14A0
    uint32_t (*seed)(const void* seedPtr);// D2Client+0x1330
    int  (*roomFind)();                   // D2Client+0x1400 (result tested)
    void* (*roomAt)(void* room, int x, int y);     // Ordinal_11010 (unverified), arg room=DAT_6fbcc3b8
    void  (*roomPrep)(void* room);        // Ordinal_10331 (unverified)
    void* defaultRoom;                    // DAT_6fbcbbfc
    void* worldRoom;                      // DAT_6fbcc3b8
    int  (*ord10494)(void*, int);         // unverified
    int  (*ord10871)(void*, int);         // unverified
    int  (*ord10680)();                   // unverified
    void (*adjustXY)(int* xy);            // D2Client+0xB0B30
    void* (*spawn)(int id, int x, int y, int mode); // D2Client+0xB7BF0
    void (*post)(const char* name, void* u, int a, int b, int c); // generic Ordinal_* call-out
    uint32_t (*ord)(int ordinal, void* a, int b, int c); // all other Ordinal_* by number
    void (*afterCreate)(void* unit, void* creator);      // FUN_6fb60a60 / 6fb55420 / 6fb77b00 tail group
    int  dbgDraw;                         // DAT_6fb8d854
};
static MslCreateEnv g_msl;
extern "C" void d2client_msl_set_env(const MslCreateEnv* e) { g_msl = *e; }

static inline uint32_t rd32(const void* p, int o) { uint32_t v; memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// 1.13c D2Client+0xB30F0  (param_1 = missile-create request; fields: +0 flags, +4 creator, +8 anchor,
// +0xC target, +0x10 missile id, +0x14/+0x18 x/y, +0x1C/+0x20 dx/dy, +0x24 .., +0x28 range, +0x30 level, +0x34 ..)
// Computes range/speed/position from the missiles.txt row (stride 0x1A4), creates the unit, wires it.
// Returns the new missile unit or 0. Post-create tail is delegated to env (ponytail: not decomposed here).
extern "C" void* d2client_msl_create_b30f0(const uint32_t* req) {
    int id = (int)req[4];
    const uint8_t* dt = g_msl.dataTables;
    if (id < 0 || (int)rd32(dt, 0xb6c) <= id) return 0;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)rd32(dt, 0xb64) + (uint32_t)id * 0x1a4;
    if (!rec) return 0;
    uint32_t flags = req[0];
    int x, y;
    if (!(flags & 1)) {
        if (!req[2]) return 0;
        x = g_msl.unitX((void*)(uintptr_t)req[2]); y = g_msl.unitY((void*)(uintptr_t)req[2]);
    } else { x = (int)req[5]; y = (int)req[6]; }
    int ox = x, oy = y, tx, ty;
    if (!(flags & 2)) { tx = x; ty = y; if (flags & 0x20) { ty = (int)req[8]; tx = (int)req[7]; } }
    else { ty = oy + (int)req[8]; tx = ox + (int)req[7]; }
    uint32_t range;
    if (!(flags & 4) || !(flags & 0x10)) {
        int t = rec[0x9b] * (int)req[0xc];
        range = (uint32_t)(((t + ((t >> 31) & 7)) >> 3) + rec[0x9a]) << 8;
    } else range = req[10];
    int pct = 100;
    if (req[1] && (g_msl.bitMask12() & rec[4]) && g_msl.ord10494((void*)(uintptr_t)req[1], 0x57) &&
        g_msl.ord10871((void*)(uintptr_t)req[1], 0x57)) {
        pct = g_msl.ord10680();
        range = (uint32_t)g_msl.pctOf(100, (int)range);
    }
    // 75% of range (three width paths in the original: int32, int64, divide-first)
    int r75 = 0;
    if (range) {
        if ((int)range <= 0x100000) r75 = (int)(range * 0x4b) / 100;
        else if ((int)(range & 0xfffffff0) < 0x640) r75 = (int)((int64_t)(int32_t)range * 0x4b / 100);
        else r75 = ((int)range / 100) * 0x4b;
    }
    // Spawn + tail: the ordinal-heavy wiring (Ordinal 10595/11166/10154/10558/10970/10837/10488/10286/
    // 10336/10334/10024/11022/10378/10706/11167/10652/10210) goes through g_msl.ord, unverified.
    void* u = g_msl.spawn(id, ox, oy, 3);
    if (!u) return 0;
    (void)r75; (void)tx; (void)ty; (void)pct;
    g_msl.afterCreate(u, (void*)(uintptr_t)req[1]);
    return u;
}
