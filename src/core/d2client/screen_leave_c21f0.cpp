// D2Client+0xC21F0: per-screen-state teardown/notify switch (state in EAX); env-seam style.
// Ordinal_10048 (unverified ordinal) + FUN_6faecc60/6faeccc0 are folded into sendA/sendB(string RVA).
#include <cstdint>
#include <emscripten.h>
struct D2c21f0Env {
    uint32_t state;                       // EAX on entry
    void (*sendA)(uint32_t strRva);       // FUN_6faecc60(Ordinal_10048(), &str)
    void (*sendB)(uint32_t strRva);       // FUN_6faeccc0(Ordinal_10048(), &str)
    void (*call)(uint32_t rva, uint32_t arg); // plain callee by RVA (arg = ECX/stack arg, 0 if none)
    uint32_t (*test)(uint32_t rva);       // bool/int-returning callee (FUN_6faf2090 / 6faf20e0 / 6fb09830)
    void (*ord10006)(void);               // Ordinal_10006
    uint32_t g_aadb0, g_aaddc, g_aade4, g_aade8, g_aadec, g_aadf0, g_aadf4, g_aadf8, g_aae00, g_aada8; // DAT_6fbaadXX
    uint32_t *g_cc434, *g_d349c, *g_d3498, *g_cc2f8, *g_cc2f4, *g_b9ac, *g_cc418, *g_cc414;
    uint32_t g_a9e14;                     // DAT_6fba9e14 (arg of FUN_6fb3a4d0)
};
#define A(x) ((x) - 0x6FAB0000u)
static void tail(const D2c21f0Env* e) {
    *e->g_cc434 = 1;
    if (*e->g_d349c == 0) {
        *e->g_d349c = 1;
        if (*e->g_d3498) { e->sendA(A(0x6fba51ec)); *e->g_d3498 = 0; }
        e->sendB(A(0x6fba51bc));
    }
}
static void reset3a4d0(const D2c21f0Env* e) {
    *e->g_cc418 = 0; *e->g_cc414 = 0;
    e->ord10006();
    e->call(A(0x6fb3a4d0), e->g_a9e14);
}
extern "C" {
// 1.13c D2Client+0xC21F0
EMSCRIPTEN_KEEPALIVE void d2c_c21f0(const D2c21f0Env* e) {
    auto a = [&](uint32_t d) { e->sendA(A(d)); };
    auto c = [&](uint32_t f, uint32_t x = 0) { e->call(A(f), x); };
    switch (e->state) {
    case 0: a(0x6fb80580); return;
    case 1: c(0x6fb6f720); a(0x6fb805b0); c(0x6fb6e2e0); return;
    case 2: c(0x6fb6f720); a(0x6fb80628); c(0x6fb6e2e0); return;
    case 3: a(0x6fb8061c); return;
    case 4: a(0x6fb80604); c(0x6fb6e2e0); c(0x6fb27da0); return;
    case 5:
        c(0x6fb1f4a0);
        if (!e->g_aadb0 && !e->g_aaddc && !e->g_aade4 && !e->g_aade8 && !e->g_aadec && !e->g_aadf0 &&
            !e->g_aadf4 && !e->g_aadf8 && !e->g_aae00) tail(e);
        if (e->g_aada8) e->sendB(A(0x6fb80780));
        return;
    case 6: a(0x6fb80640); return;
    case 7: a(0x6fb80664); return;
    case 8: case 0xe: c(0x6fb6e2e0); c(0x6faf92f0); return;
    case 9: a(0x6fb80680); *e->g_cc2f8 = 0; *e->g_cc2f4 = 0; return;
    case 10: a(0x6fb80780); *e->g_b9ac = 0; return;
    case 0xb:
        if (e->test(A(0x6faf2090)) || e->test(A(0x6faf20e0))) a(0x6fb80774);
        a(0x6fb806d8); c(0x6fac2dd0, 0); return;
    case 0xc: a(0x6fb8bc60); c(0x6faf92f0); tail(e); return;
    case 0xf: a(0x6fb8bcc0); c(0x6facd8f0); c(0x6fb6e2e0); return;  // ECX arg of 6facd8f0 is caller-supplied (unverified)
    case 0x10: a(0x6fb80810); c(0x6fb6e2e0); return;
    case 0x11: a(0x6fb807bc); c(0x6facc720); return;
    case 0x12: c(0x6fb6e2e0); return;
    case 0x13: a(0x6fb80658); return;
    case 0x14: a(0x6fb80828); c(0x6fb5a380); c(0x6fb6e2e0); return;
    case 0x15: a(0x6fb80864); return;
    case 0x16: a(0x6fb8087c); c(0x6fb24f90); c(0x6fb6e2e0); return;
    case 0x17:
        if (e->test(A(0x6fb09830)) == 0) {
            reset3a4d0(e); c(0x6fb6f720); a(0x6fb8bd18); c(0x6fb6e2e0); c(0x6fb09740, 0); tail(e);
        }
        return;
    case 0x18: c(0x6fb2c730); return;
    case 0x19: c(0x6fb6f720); reset3a4d0(e); c(0x6fb3f140); a(0x6fb8bdb8); c(0x6fb6e2e0); tail(e); return;
    case 0x1a: reset3a4d0(e); c(0x6fb47840); a(0x6fb8be18); c(0x6fb6e2e0); tail(e); return;
    case 0x1f: a(0x6fb8be6c); return;
    case 0x21: c(0x6fae58e0); a(0x6fb80924); return;
    case 0x22: a(0x6fb80960); return;
    case 0x23: a(0x6fb80978); return;
    case 0x24: a(0x6fb8099c); c(0x6fb3bbf0); return;
    case 0x25: a(0x6fb809c0); return;
    default: return;  // 0xd,0x1b-0x1e,0x20 and out-of-range: no-op
    }
}
}
