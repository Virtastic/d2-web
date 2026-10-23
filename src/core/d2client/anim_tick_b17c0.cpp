// D2Client per-unit animation/mode-frame advance, derived from the 1.13c decompile.
// Ordinal names are UNVERIFIED guesses from call shape; each seam is the D2Common ordinal in its comment.
#include <cstdint>
#include <cstring>
struct ClientB17C0Env {
    const uint8_t* (*monStats)(int32_t idx);         // sgptDataTables+0xb64 table, stride 0x1a4, count at +0xb6c
    const void*    (*localPlayer)();                 // FUN_6fb560d0
    const void*    localPlayerAtBBFC;                // DAT_6fbcbbfc
    int32_t screenState;                             // DAT_6fbcc394
    int32_t (*o10130)(void*); int32_t (*o10737)(void*); int32_t (*o10985)(void*);   // cur frame / frame count / ?
    int32_t (*o10426)(void*); void (*o10759)(void*); void (*o10023)(void*, uint8_t a, uint8_t b);
    int32_t (*o10777)(uint32_t mode); void (*o10342)(void*, int32_t); int32_t dat9824;
    void (*o10970)(void*, int32_t); uint16_t (*o11009)(uint32_t mode);
    int32_t (*o10214)(uint32_t mode, int32_t* cnt); uint32_t (*o10331)(void*); int32_t (*o10589)(void*);
    int32_t (*o10389)(uint32_t, uint16_t, uint16_t, uint32_t, int32_t*, int32_t); int32_t (*f55440)();
    void (*f61420)(void*, int32_t); int32_t (*o10331p)(const void*); int32_t (*o10057)(int32_t);
    uint32_t (*tbl8d858)(int32_t i);
};
static uint32_t rdu32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
// 1.13c D2Client+0xB17C0  fastcall(unit)
extern "C" void d2_client_b17c0(uint8_t* u, const ClientB17C0Env& e) {
    if (!u) return;
    int32_t idx = (int32_t)rdu32(u + 4);
    const uint8_t* ms = idx < 0 ? nullptr : e.monStats(idx);
    if (!ms) return;
    int32_t local = 0;
    if (e.screenState == 0 || e.screenState == 1 || e.screenState == 6 || e.screenState == 8)
        if (e.localPlayer() == e.localPlayerAtBBFC) local = 1;
    for (;;) {
        int32_t cur = e.o10130(u), cnt = e.o10737(u);
        bool atEnd = cur < cnt;   // decompile name bVar5: true while frames remain
        if (!atEnd && cnt >= 0 && (int32_t)ms[0x135] < e.o10985(u)) { uint32_t f = rdu32(u + 200) & 0xfffbffffu; memcpy(u + 200, &f, 4); }
        uint32_t pd = rdu32(u + 0x54);
        if (pd) { uint32_t fp = rdu32((const uint8_t*)(uintptr_t)pd + 0x30);
            if (fp) { uint32_t fl = rdu32((const uint8_t*)(uintptr_t)fp); if ((fl & 1) && !(fl & 8)) { e.f61420(u, 1); return; } } }
        if (!atEnd) {
            bool skip = false;
            if (ms[0x137] == 0) { if (e.o10426(u)) skip = true; }
            else if (ms[0x180] && (int32_t)(ms[0x178] - ms[0x182] + 1) < e.o10737(u)) { e.o10023(u, ms[0x181], ms[0x182]); skip = true; }
            if (!skip) e.o10759(u);
        }
        uint32_t mode = rdu32(u + 0x2c);
        if (e.o10777(mode)) e.o10342(u, e.dat9824);
        e.o10970(u, e.o10737(u) - 1);
        if (e.o10737(u) < 1) { e.f61420(u, 0); return; }
        if (atEnd) return;
        if (ms[300]) {
            const void* lp = e.localPlayer();
            int32_t t;
            if (!lp || ((t = e.o10331p(lp)) != 0 && e.o10057(t) != 0)) {
                int32_t n = e.o10737(u);
                if (ms[300] <= n) { e.o10970(u, ms[300]);
                    if (ms[0x137] == 0) { int32_t d = ms[0x178] - ms[300]; if (d > 0) { int32_t v = d * 0x100; memcpy(u + 0x44, &v, 4); } } }
            }
        }
        int32_t st = (int32_t)rdu32(u + 0x10);
        if (st == 0) return;
        uint16_t fl = e.o11009(mode);
        if (fl & 5) { e.f61420(u, 1); return; }
        if (st == 6) return;
        if (ms[0x185] && fl) {
            int32_t n = 0; int32_t arr = e.o10214(mode, &n); uint32_t owner = e.o10331(u);
            for (int32_t i = 0; i < n; i++) {
                int32_t o = e.o10389(owner, (uint16_t)(arr + i * 4), (uint16_t)((arr + i * 4) >> 16), e.tbl8d858(arr), &n, e.o10589(u));
                if (o && !e.f55440()) { e.f61420(u, 0); return; }
            }
        }
        // ponytail: loop-again gate uses the 'local' flag (decompile unaff_ESI), frame==1 and o10777(state)
        if (!local || e.o10985(u) != 1 || !e.o10777((uint32_t)st)) return;
        local = 0;
    }
}
