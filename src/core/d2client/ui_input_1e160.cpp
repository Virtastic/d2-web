// D2Client rva 0x1E160: input-message dispatcher, thiscall(this, pMsg), ret 4.
// Callees are injected (D2Client internals by RVA, D2 ordinals by their D2Client import thunk); `base` is the module base the
// callers' code addresses (string, handler pointers) are relative to. g = the pinned-globals block (layout below).
#include <cstdint>
#include <cstring>
struct Client1E160Callees {
    uint32_t (*o10174)(uint32_t esi, uint32_t a, uint32_t b);   // thunk 0xC58C, stdcall
    void (*o10227)();                                           // 0xBF8C
    uint32_t (*o10003)(uint32_t ecx, uint32_t a);               // 0xD1EC, leaves its stack word
    uint32_t (*o10048)(uint32_t a);                             // 0xD0B4, leaves its stack word
    void (*c7d850)(uint32_t a, uint32_t b);                     // ret 8
    void (*a53a0)(uint32_t eax);
    uint32_t (*a5440)(uint32_t eax);
    void (*c88a70)(uint32_t ebx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // ret 0x10
    void (*c6a0c0)(uint32_t ebx, uint32_t a);                   // ret 4
    void (*c43830)();
    void (*c1c940)(uint32_t edx);
    int32_t (*c1c890)(uint32_t edx);
    void (*c521a0)(uint32_t esi);
    void (*c42bd0)();
    void (*c47b20)(uint32_t edi);
    void (*c2790)(uint32_t ecx, uint32_t edx, uint32_t a);      // fastcall + 1 stack word
    void (*c1c720)();
    void (*c3ccc0)(uint32_t eax, uint32_t a, uint32_t b);       // ret 8
    void (*c3cc60)(uint32_t eax, uint32_t a, uint32_t b);
    void (*fatal)();                                            // the _exit(-1) path (ordinals 10265/10024 + _exit): not vectored
};
// g layout (offset: module rva, len)
enum { O_C973B = 0, O_D23E4 = 4, O_D23EC = 8, O_D23D8 = 12, O_D23F3 = 16, O_D23F9 = 20, O_D3395 = 24, O_FLAGS = 32 /*0xFAD80, 38 dwords*/,
       O_PTRS = 184 /*0xDD538*/, O_COUNT = 336 /*0xDD5D0*/, O_C3A4 = 340, O_C3AC = 344, O_C9854 = 348, O_BF2F0 = 352 /*+4: 0xBF2F4*/,
       O_C9721 = 360, O_C971D = 364, O_UNITP = 368 /*0x11BBFC*/, O_D2496 = 372, O_F8188 = 416 /*41 x 16*/ };
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void wr(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }
static int32_t sx(const uint8_t* m) { return (int16_t)(m[4] | m[5] << 8); }
// 1.13c D2Client+0x1E160
extern "C" void d2_client_1e160(const uint8_t* m, uint8_t* g, uint32_t base, const Client1E160Callees& f) {
    const uint32_t kStr = base + 0xD07BC;   // 0x6FB807BC: the string handed to ordinal 10048
    if (m) {
        const uint8_t fl = m[2], code = m[1];
        if (fl & 1) {                                                   // 1E174
            switch (code) {
            case 3: f.c7d850(f.o10003(0xe7c, 0), 0); break;
            case 0xf: f.c7d850(f.o10003(0xe7e, 0), 0); break;
            case 4: f.a53a0(base + 0x1C920); break;
            case 10: f.c43830(); break;
            case 0xd: f.c6a0c0(0xd3, 1); break;
            case 0x17: wr(g, O_C3A4, 1); break;
            case 0x21: f.c88a70(0xed, 0, 0, 0, 0); break;
            }
            return;
        }
        if (fl & 2) {                                                   // 1E21D
            switch (code) {
            case 4: f.c88a70(0xf1, 0, 0, 0, 0); break;
            case 8: case 0xf: case 0x12: case 0x16: case 0x23: f.c88a70(7, 0, 0, 0, 0); break;
            case 0x17: if (rd(g, O_C9854)) g[O_BF2F0 + 4] = 1; else { f.c42bd0(); g[O_BF2F0] = 1; } break;
            case 0x20: f.c88a70(0xd9, 0, 0, 0, 0); break;
            case 0x21: f.c88a70(0xf3, 0, 0, 0, 0); break;
            case 0x24: wr(g, O_C3AC, 1); break;
            }
            return;
        }
        if (fl == 0x10) {                                               // 1E2DB
            if (code == 10) { f.c88a70(0x998, 0, 0, 0, 0); f.c88a70(0x9aa, 0, 0, 0, 0); }
            else if (code == 0x21) f.c88a70((uint32_t)sx(m), 0, 0, 0, 0);
            return;
        }
        if (fl & 0x20) {                                                // 1E32F
            if (code == 0x20) wr(g, O_D23EC, (uint32_t)sx(m));
            else if (code == 0x21) { f.a53a0(base + 0x1C900); return; }
            else {
                const uint32_t esi = rd(g, O_C973B);
                if (esi && f.o10174(esi, 1, 0)) return;
                if (f.o10174(esi, 1, 1)) return;
                const int32_t v = sx(m);
                wr(g, O_D23E4, (uint32_t)v);
                if (v > 5 || v < 0) wr(g, O_D23E4, 0x29a);
                else if (v == 5) {
                    if (code == 0x17) { f.c521a0(5); return; }          // esi = max(1, v) = 5
                    goto L344;
                }
                g[O_D2496 + f.c1c890(1)] = m[3]; return;
            }
        }
    }
L344:
    if (rd(g, O_D23D8) == 0) {                                          // 1E351: first-run init
        const uint32_t old = rd(g, O_FLAGS + 17 * 4);                   // [0xFADC4]
        f.o10227();
        if (rd(g, O_COUNT) != 0x26) { f.fatal(); return; }
        for (uint32_t i = 0; (int32_t)i < (int32_t)rd(g, O_COUNT); ++i) {
            if (rd(g, O_FLAGS + 4 * i) != 1) continue;
            const uint32_t mode = rd((const uint8_t*)(uintptr_t)rd(g, O_PTRS + 4 * i), 0x44);
            if (mode == 0) continue;
            if (mode == 1) f.c2790(i, 1, 0);
            else if (mode == 4) { if (rd(g, O_C9721)) f.c47b20(rd(g, O_C971D)); }
            else return;                                                // modes 2, 3, >4 leave the function
        }
        const uint32_t up = rd(g, O_UNITP);
        if (!up || (rd((const uint8_t*)(uintptr_t)up, 0) == 0 && !f.a5440(up))) wr(g, O_FLAGS + 17 * 4, 1);
        const uint32_t cur = rd(g, O_FLAGS + 17 * 4);
        if (!old) { if (cur) f.c3ccc0(4, f.o10048(kStr), kStr); }
        else if (!cur) { f.c3cc60(4, f.o10048(kStr), kStr); f.c1c720(); }
        f.c1c940(m[1]);
        wr(g, O_D23D8, 1);
    } else if (rd(g, O_FLAGS + 15 * 4)) {                               // 1E503
        const int32_t i = f.c1c890(m[1]);
        if (i == -1) return;
        g[O_D2496 + i] = m[3];
        if (g[O_D23F3]) return;
        if ((uint32_t)g[O_F8188 + i * 16 + 3] == rd(g, O_D3395)) wr(g, O_D23F9, g[O_F8188 + i * 16 + 2]);
    }
}
