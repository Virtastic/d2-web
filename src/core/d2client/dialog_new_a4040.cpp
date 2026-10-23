// 1.13c D2Client+0xA4040 (663 B): modal dialog-box node constructor, from the cached decompile.
// Node = 0x2C4 dwords (retail 32-bit layout, byte-addressed); imports/globals are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientDlgA4040Env {
    uint32_t* g_ba46c4; uint32_t* g_ba46c8;        // 0x6FBA46C4/C8 (set 0x10)
    uint32_t* g_90ea0;                             // 0x6FB90EA0 (=0)
    uint32_t* g_90ea8_ebc;                         // 0x6FB90EA8..EBC, 6 dwords (=-1)
    uint32_t* g_bcbc28_2c_38_3c;                   // 0x6FBCBC28,2C,38,3C (4 dwords =0)
    uint32_t* g_bcc3dc_e0_f8_fc;                   // 0x6FBCC3DC,E0,F8,FC (4 dwords =0)
    uint32_t* g_bcc2f8_f4;                         // 0x6FBCC2F8,F4 (2 dwords =0)
    uint32_t* g_bcbc08;                            // dialog count
    uint32_t* g_bcbc00;                            // list head (node ptr as uint32)
    uint32_t* g_bcbc04;                            // =1
    uint32_t* g_bbb208;                            // ++
    uint32_t  g_baada8;                            // 0x6FBAADA8
    void (*fn_ecc60)(uint32_t, uint32_t);          // D2Client+0x3CC60 (release)
    void (*fn_eccc0)(uint32_t, uint32_t);          // D2Client+0x3CCC0
    uint32_t (*ord10048)();
    uint8_t* (*ord10042)(uint32_t id, uint32_t z); // Fog alloc(0x399,0)
    uint32_t (*getTick)();
    void (*fmtPath)(char* buf, const char* name);  // sprintf("%s\\ui\\menu\\%s","DATA\\GLOBAL",name)
    const char* const* nameTbl;                    // PTR table 0x6FB8E920
    uint32_t (*sub2b420)(uint32_t zero);           // returns char* handle
    uint32_t (*ord10021)(uint32_t* frame18);
    uint32_t (*ord10060)(uint32_t);
    uint32_t (*ord10000)(uint32_t);
    void (*sub53a20)(uint32_t);                    // D2Client+0xA3A20
    uint32_t (*sub42090)();                        // D2Client+0x42090
    void (*fatal)(uint32_t id);                    // Ordinal_10265 + 10024 + _exit(-1)
    uint32_t datB80780, datB8E858, datB8E900;
};

// 1.13c D2Client+0xA4040
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* d2_client_a4040(
    uint32_t p1, uint32_t p2, int32_t p3, uint32_t p4, int32_t p5, D2ClientDlgA4040Env* e) {
    if (e->g_baada8) e->fn_ecc60(e->ord10048(), e->datB80780);
    *e->g_ba46c8 = 0x10; *e->g_ba46c4 = 0x10; *e->g_90ea0 = 0;
    for (int i = 0; i < 6; i++) e->g_90ea8_ebc[i] = 0xffffffffu;
    for (int i = 0; i < 4; i++) e->g_bcbc28_2c_38_3c[i] = 0;
    for (int i = 0; i < 4; i++) e->g_bcc3dc_e0_f8_fc[i] = 0;
    e->g_bcc2f8_f4[0] = 0; e->g_bcc2f8_f4[1] = 0;
    if (p3 == 0 || p5 > 0) return 0;
    ++*e->g_bcbc08;
    uint32_t* n = (uint32_t*)e->ord10042(0x399, 0);
    memset(n, 0, 0x2c4 * 4);
    n[0x2c3] = *e->g_bcbc00;
    memset(n, 0, 0x2c4 * 4);                       // retail clears twice; [0x2c3] ends 0
    *e->g_bcbc00 = (uint32_t)(uintptr_t)n;
    n[0xd] = 0; n[0xe] = 0; n[0x14] = 0; n[0x13] = 0; n[0x11] = 0xffffffffu;
    n[0x17] = 2; n[0xb] = 0; n[0xc] = 0; n[0x2c2] = 0;
    n[0x18] = p1; n[0x19] = p2; n[0x16] = 0; n[5] = 0;
    n[0] = e->getTick();
    n[6] = p4; n[0x12] = 0xffffffffu; n[4] = 0; n[0x15] = 0; n[3] = 1;
    char buf[252];
    e->fmtPath(buf, e->nameTbl[p3]);
    n[2] = e->sub2b420(0);
    uint32_t f[0x12] = {};                         // f[0xc]=DStack_10, f[0xd]=DStack_c after the call
    uint32_t h = e->ord10021(f);
    if (!h) { e->fatal(0x3b6); return 0; }
    n[0xd] = e->ord10060(h);
    n[0xe] = e->ord10000(h);
    n[9] = f[0xc]; n[10] = f[0xd]; n[0xb] = f[0xc]; n[0xc] = f[0xd];
    *e->g_bcbc04 = 1;
    e->sub53a20(f[0xc]);
    e->fn_eccc0(e->ord10048(), e->datB8E858);
    if (e->sub42090() & 0xff) e->fn_eccc0(e->ord10048(), e->datB8E900);
    ++*e->g_bbb208;
    if (*e->g_bcbc08 != 1) { e->fatal(0x3ce); return 0; }
    return n;
}
