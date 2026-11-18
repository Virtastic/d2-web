// D2Client rva 0x1E940: per-frame panel-state / viewport-shift tick, stdcall(param), ret 4.
// Callees are injected (D2Client internals by RVA, ordinals by their import thunk); g = pinned-globals block (layout below).
#include <cstdint>
#include <cstring>
struct Client1E940Callees {
    void (*o10227)();                                                           // 0xBF8C
    void (*c2790)(uint32_t ecx, uint32_t edx, uint32_t a);                      // fastcall + 1 stack word
    void (*c47b20)();                                                           // edi = [0x11971D] (not recorded)
    uint32_t (*a5440)(uint32_t eax);
    void (*bfa10)();                                                            // edi = const
    uint32_t (*o10048)(uint32_t a);                                             // 0xD0B4, leaves its stack word
    void (*c3ccc0)(uint32_t eax, uint32_t a, uint32_t b);                       // ret 8
    void (*c3cc60)(uint32_t eax, uint32_t a, uint32_t b);
    void (*d8f0)();
    void (*be2e0)();
    void (*d1b0)(uint32_t ecx);                                                 // ordinal 10006 thunk
    void (*a4d0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t a);   // 0x8A4D0, ret 4
    void (*c61b0)(uint32_t ebx);                                                // edi = [0x11B824] (not recorded)
    void (*d930)();
    void (*dcc0)();
    void (*da90)(uint32_t a, uint32_t b);                                       // ret 8
    void (*a4a30)(uint32_t eax);
    void (*c7e7e0)(uint32_t a);                                                 // ret 4
    void (*c7e6e0)();
    void (*fatal)();                                                            // ordinals 10265/10024 + _exit(-1): not vectored
};
// g layout (offset: module rva, len)
enum { O_STATE = 0 /*0x1223D8*/, O_SEL = 4 /*0x123395*/, O_LAST = 8 /*0x123399*/, O_UNIT = 12 /*0x11BBFC*/, O_F3 = 16 /*0x10F2F3 byte*/,
       O_CNT = 20 /*0xDD5D0*/, O_A68 = 24 /*0x113A68*/, O_FLAGS = 28 /*0xFAD80, 38 dwords*/, O_PTRS = 180 /*0xDD538, 38 dwords*/,
       O_C9721 = 332, O_C971D = 336, O_WH = 340 /*0x11B824, +4 = 0x11B828*/, O_SCR = 348 /*0xDBC48*/, O_F9E = 352 /*0xF9E14, +4 = 0xF9E18*/,
       O_ESI = 360 /*0x11C3C4*/, O_S414 = 364 /*0x11C414, +4 = 0x11C418*/, O_S3DC = 372 /*0x11C3DC, +4*/, O_S3F8 = 380 /*0x11C3F8, +4*/, O_LEN = 388 };
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }
static int32_t q4(int32_t v) { return (v + ((v >> 31) & 3)) >> 2; }   // v/4 rounded toward zero
// 1.13c D2Client+0x1E940
extern "C" void d2_client_1e940(uint32_t param, uint32_t now, uint8_t* g, uint32_t base, const Client1E940Callees& f) {
    const uint32_t kStr = base + 0xDBCC0;   // 0x6FB8BCC0: the string handed to ordinal 10048
    const uint32_t st = rd(g, O_STATE);
    if (st == 0) {
        const uint32_t up = rd(g, O_UNIT);
        if (now - rd(g, O_LAST) < 0xFA || !up) return;
        if (rd((const uint8_t*)(uintptr_t)up, 0x10) == 0x11 || g[O_F3]) return;
    } else if (st == 1) {
        wr(g, O_STATE, 0);
        if (g[O_F3]) return;
    } else {
        f.c7e7e0(param); wr(g, O_LAST, now); f.c7e6e0(); return;
    }
    const uint32_t old = rd(g, O_FLAGS + 15 * 4);   // [0xFADBC]
    f.o10227();
    if (rd(g, O_CNT) != 0x26) { f.fatal(); return; }
    if (rd(g, O_A68)) return;
    for (uint32_t i = 0; (int32_t)i < (int32_t)rd(g, O_CNT); ++i) {
        if (rd(g, O_FLAGS + i * 4) != 1) continue;
        const uint32_t mode = rd((const uint8_t*)(uintptr_t)rd(g, O_PTRS + i * 4), 0x3C);
        if (mode == 1) f.c2790(i, 1, 0);
        else if (mode == 4) { if (rd(g, O_C9721)) f.c47b20(); }
        else if (mode != 0) return;
    }
    const uint32_t up = rd(g, O_UNIT);
    if (!up || (rd((const uint8_t*)(uintptr_t)up, 0) == 0 && f.a5440(up) == 0)) wr(g, O_FLAGS + 15 * 4, 1);
    const uint32_t cur = rd(g, O_FLAGS + 15 * 4);
    if (old == 0) {
        if (cur) { f.bfa10(); f.c3ccc0(7, f.o10048(kStr), kStr); }
    } else if (cur == 0) {
        f.c3cc60(7, f.o10048(kStr), kStr); f.d8f0(); f.be2e0();
    }
    const int32_t W = (int32_t)rd(g, O_SCR), H = (int32_t)rd(g, O_F9E), edi = (int32_t)rd(g, O_WH + 4);
    const uint32_t eax = rd(g, O_F9E + 4), esi = rd(g, O_ESI);
    if (rd(g, O_FLAGS + 4) == 0 && rd(g, O_FLAGS + 16) == 0) {
        if (cur) {                                                      // panel open: shift right by W/4
            wr(g, O_S414 + 4, (uint32_t)q4(W)); wr(g, O_S414, 2); f.d1b0((uint32_t)q4(W));
            f.a4d0(eax, (uint32_t)(H + q4(H)), 0, esi, (uint32_t)q4(H));
            if (param && edi < W - q4(W)) f.c61b0((uint32_t)(q4(W) + edi));
        } else {
            wr(g, O_S414 + 4, 0); wr(g, O_S414, 0); f.d1b0(0);
            f.a4d0(eax, (uint32_t)H, 0, esi, 0);
            if (param && edi > (W - (W >> 31)) >> 1) f.c61b0((uint32_t)(edi - q4(W)));
        }
    } else if (cur == 0) {
        wr(g, O_S414 + 4, (uint32_t)-q4(W)); wr(g, O_S414, 1); f.d1b0((uint32_t)-q4(W));
        f.a4d0(eax, (uint32_t)(H - q4(H)), 0, esi, (uint32_t)-q4(H));
    } else {
        wr(g, O_S414 + 4, 0); wr(g, O_S414, 3); f.d1b0(0);
        f.a4d0(eax, (uint32_t)H, 0, esi, 0);
        wr(g, O_S3DC, 0); wr(g, O_S3DC + 4, 0); wr(g, O_S3F8, 0); wr(g, O_S3F8 + 4, 0);
    }
    f.d930(); wr(g, O_STATE, 2); f.dcc0(); f.da90(rd(g, O_SEL), 0); f.a4a30(0x40);
}
