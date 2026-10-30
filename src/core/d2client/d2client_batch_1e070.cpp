// src/core/d2client/d2client_batch_1e070.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; every RVA below is
// retail's, and every global this file owns is a static at its retail RVA comment.
//
// Unrecovered internal callees and imported ordinals are host seams (the
// src/core/d2client/d2client_batch_1e70.cpp pattern): the function takes a D2ClientBatch1de0Env AFTER
// its retail arguments, which keep their order. Where Ghidra kept a live register as an argument it is
// recovered as an explicit parameter and named in the comment (in_EAX, unaff_ESI, unaff_EBX/EDI).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

struct D2ClientBatch1de0Env {
    // 0x1E070
    void* (*spawnA7780)(uint32_t* rec, uint32_t a, uint32_t b, int32_t c);      // 0xA7780
    // 0x1E5F0 (the loading-screen draw; all imported ordinals of the retail thunk table)
    void     (*ord10227)();
    uint32_t (*ord10265)();
    void     (*ord10024)(const uint32_t* msg);
    void*    (*ord10048)(const uint32_t* path);
    void     (*fn3CC60)(uint32_t handle, uint32_t* table, uint32_t count);       // 0x3CC60
    void     (*fn1C720)();                                                       // 0x1C720
    void     (*ord10021)();
    void     (*ord10060)();
    const void* (*ord10003)(uintptr_t arg);
    void     (*ord10028)();
    void     (*ord10150)(const void* text, int32_t x, int32_t y, int32_t color, int32_t bfalse);
    void     (*drawCel)(uint32_t cel, uint32_t frame, int32_t x, int32_t y, int32_t palette, int32_t mode);
    uint8_t  (*fn1C980)(uint32_t mode);                                          // 0x1C980, mode in EAX
    uint32_t (*fn1D840)(int32_t x, int32_t y);                                   // 0x1D840
    void     (*exitProcess)(int32_t code);
    // 0x200E0 / 0x20B70
    void     (*ord10569)(void* obj, uint8_t* out, uint8_t* out2);
    uint32_t (*ord10750)(uint32_t p);
    uint32_t (*ord10867)(uint32_t p);
    uint32_t (*fnB0450)(void* obj, uint32_t value);                              // 0xB0450, obj in ESI
    void     (*ord10973)(void* obj, uint32_t a, uint32_t b);
    // 0x20180
    void*    (*fnA60D0)(void* obj);                                              // 0xA60D0
    bool     (*fn1FD00)(void* obj, uint32_t mask);                               // 0x1FD00, obj in EAX
    void*    (*fnA5F70)(void* obj);                                              // 0xA5F70, obj in ESI
    uint32_t (*fn1630)(void* obj);                                               // 0x1630
    uint32_t (*fn1660)(void* obj);                                               // 0x1660
    void     (*ord10983)(uint32_t p);
    uint32_t (*ord10764)(uint32_t p);
    void     (*ord10936)();
    void     (*ord10614)();
    uint32_t (*ord11029)(void* obj);
    uint32_t (*ord10240)(void* obj, uint32_t value);
    // 0x20340 / 0x20A60
    void     (*fnB30F0)(uint32_t* ctx);                                          // 0xB30F0
    void     (*fn88A70)(void* obj, uint32_t a, uint32_t b, uint32_t c);           // 0x88A70, key in EBX
    // 0x20620
    const void* (*ord10038)(int32_t id);
    void     (*ord10971)();
    uint32_t (*ord10852)(uint32_t a);
    const void* (*ord10249)(uint32_t a);
    uint32_t (*ord10272)(uint32_t a);
    void     (*fn90CB0)(void* dst, const void* fmt, const void* arg);            // 0x90CB0
    uint32_t (*fn1330)(uint64_t* state, uint32_t n);                             // 0x1330, n in EAX
    void     (*unicodeCtor300)(void* buf);                                       // Unicode x300, stride 4
    void     (*unicodeToUnicode)(void* dst, const char* src, int32_t n);
    // 0x20FB0
    void     (*fn53070)();
    void     (*fn52860)();
    void     (*fn52230)(void* a, void* b);
    void     (*fn52620)();
    void     (*fn527A0)();
    void     (*fn52270)();
    void     (*fn52600)();
    void     (*fn52BD0)(void* obj);
    void     (*fn52320)();
    void     (*fn526E0)();
    void     (*fn530D0)();
    void     (*fn529F0)();
    void     (*fn52570)();
    // 0x21400
    uint32_t (*invoke1410)(void* fn, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
};

// Retail module state, all zero-initialised.
static uint8_t  g_enable12339F;      // +0x12339F  DAT_6fbd339f
static uint32_t g_counterDE834;      // +0x0DE834  DAT_6fb8e834
static uint8_t  g_flag1233A0;        // +0x1233A0  DAT_6fbd33a0
static uint8_t  g_lastId1233A1;      // +0x1233A1  DAT_6fbd33a1
static uint8_t* g_state11BBFC;       // +0x11BBFC  DAT_6fbcbbfc, pointer to the global client state
static uint32_t g_loadingFADC4;      // +0x0FADC4  DAT_6fbaadc4
static uint32_t g_exitDD5D0;         // +0x0DD5D0  DAT_6fb8d5d0
static uint32_t g_pathD07BC[8];      // +0x0D07BC  DAT_6fb807bc, the unload path buffer
static uint32_t g_1223D8;            // +0x1223D8  DAT_6fbd23d8
static uint32_t g_1223F0;            // +0x1223F0  DAT_6fbd23f0
static uint32_t g_11C414;            // +0x11C414  DAT_6fbcc414 screen mode
static uint32_t g_gateFADBC;         // +0x0FADBC  DAT_6fbaadbc
static uint32_t g_11C300;            // +0x11C300  DAT_6fbcc300
static uint32_t g_11C304;            // +0x11C304  DAT_6fbcc304 loading-bar cel
static int32_t  g_screenH0DBC4C;     // +0x0DBC4C  DAT_6fb8bc4c
static int32_t  g_screenW0DBC48;     // +0x0DBC48  DAT_6fb8bc48
static int32_t  g_quad[4][5];        // +0x0F9E20  DAT_6fba9e20: 5 dwords per quadrant, stride 0x14
static int32_t  g_mouse11B828;       // +0x11B828  DAT_6fbcb828
static int32_t  g_mouse11B824;       // +0x11B824  DAT_6fbcb824
static uint32_t g_dtBaseB64;         // sgptDataTables+0x0B64, 0x1A4-byte record base
static int32_t  g_dtCountB6C;        // sgptDataTables+0x0B6C, record count
static uint32_t g_bitmask10;         // gdwBitMasks[0x10]
static uint32_t g_tbl0D6D64[8];      // +0x0D6D64  DAT_6fb86d64, 4 dwords with stride 2
static int32_t  g_list0D6D2C[4];     // +0x0D6D2C..+0x0D6D3C  DAT_6fb86d2c, function-table indexes
static uintptr_t g_animFn[64];       // +0x0FAA340  DAT_6fbaa340, animation callbacks by index
static uint8_t  g_conv1223D0[16];    // +0x1223D0  DAT_6fbd23d0, the Unicode conversion scratch
static int32_t  g_ctx48[0x48 / 4];   // the throwaway context 0x1E5F0 builds (frame +0, cel +0x34)

// 1.13c D2Client+0x1e070: while the byte flag is set, advance the 64-bit client RNG at state+0x20
// (x = lo32 * 0x6AC690C5 + hi32, stored back), take used = lo32 % 3, and for i in [0, used+2) spawn
// actor record {id = ++counter (0x0DE834, wraps 0xFFFFFFFF->0; type 0x95; x = param_1 + (i&1);
// y = param_2 + (i&1); 0; 0; 1} through 0xA7780; publish flag 1 and the low byte of the returned
// object's +0x0C (0xFFFFFFFF when 0xA7780 returns null). Returns the last published id. The retail
// return is CONCAT31(EAX>>8, flag): with the flag clear only the low byte is game state.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1e070(int32_t param_1, int32_t param_2,
                                              const D2ClientBatch1de0Env* env) {
    if (g_enable12339F == 0) return 0;
    uint64_t v = (uint64_t)*(uint32_t*)(g_state11BBFC + 0x20) * 0x6AC690C5ull
               + (uint64_t)*(uint32_t*)(g_state11BBFC + 0x24);
    *(uint64_t*)(g_state11BBFC + 0x20) = v;
    int32_t used = (int32_t)((uint32_t)v % 3u);
    uint32_t result = 0;
    for (uint32_t i = 0; (int32_t)i < used + 2; ++i) {
        uint32_t rec[7];
        rec[2] = (uint32_t)(param_1 + (int32_t)(i & 1));
        rec[3] = (uint32_t)(param_2 + (int32_t)(i & 1));
        g_counterDE834 = g_counterDE834 + 1;
        if (g_counterDE834 == 0xFFFFFFFFu) g_counterDE834 = 0;
        rec[4] = 0;
        rec[5] = 0;
        rec[1] = 0x95;
        rec[6] = 1;
        rec[0] = g_counterDE834;
        uint32_t* obj = (uint32_t*)env->spawnA7780(rec, 0, 0, 0);
        result = obj == nullptr ? 0xFFFFFFFFu : obj[3];
        g_flag1233A0 = 1;
        g_lastId1233A1 = (uint8_t)result;
    }
    return result;
}

// 1.13c D2Client+0x01E5F0: the loading screen's per-frame draw. On the 0x11 client state (or no
// state) it runs the shutdown path: 10227, an abort when the exit code (0x0DD5D0) is not 0x26, clear
// 0x0FADC4, unload the pending path table through 0x3CC60 and 0x1C720, then clear 0x1223D8/0x1223F0.
// Otherwise it returns while the screen mode is 3 or 0x0FADBC is set; else it draws the bar: quadrant
// index = 0x1C980(EAX), the same 5-dword rows at 0x0F9E20 (row +0 is x, +0x0C is y), centers x by
// (screenW-0x280)/2 in mode 2, prints the 10003 string through 10150, then two D2Gfx#10041 cell draws
// of cel 0x11C304: frame 0 at (x,y), then frame (0x1D840(mouse) != 0) at (x+3,y-4). The 0x48-byte
// context it builds is zeroed with frame 0 at +0, 0x11C300 at +8, cel 0x11C304 at +0x34, 0 at +0x40.
// mode1C980 is retail's EAX at the 0x1C980 call (Ghidra does not show it).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_1e5f0(uint32_t mode1C980, const D2ClientBatch1de0Env* env) {
    uint32_t old = g_loadingFADC4;
    if (g_state11BBFC == nullptr || *(int32_t*)(g_state11BBFC + 0x10) == 0x11) {
        env->ord10227();
        if (g_exitDD5D0 != 0x26) {
            env->ord10265();
            env->ord10024(&g_exitDD5D0);
            env->exitProcess(-1);
        }
        g_loadingFADC4 = 0;
        if (old != 0) {
            void* handle = env->ord10048(g_pathD07BC);
            env->fn3CC60((uint32_t)(uintptr_t)handle, g_pathD07BC, old);
            env->fn1C720();
        }
        if (g_1223D8 != 1) return;
        g_1223F0 = 0;
        g_1223D8 = 0;
        return;
    }
    if (g_11C414 == 3) return;
    if (g_gateFADBC != 0) return;

    memset(g_ctx48, 0, sizeof g_ctx48);
    g_ctx48[0x34 / 4] = g_11C304;
    g_ctx48[0x40 / 4] = 0;
    g_ctx48[0] = 0;
    uint8_t mode = env->fn1C980(mode1C980);
    env->ord10021();
    env->ord10060();
    env->ord10003(0);
    env->ord10028();
    int32_t y0 = g_quad[mode][4] - 0x1E0 + g_screenH0DBC4C;
    const void* text = env->ord10003(0);
    int32_t x = g_quad[mode][0];
    env->ord10150(text, x, y0, 0, 0);
    int32_t y = g_quad[mode][3] - 0x1E0 + g_screenH0DBC4C;
    if (g_11C414 == 2) x += (g_screenW0DBC48 - 0x280) / 2;
    env->drawCel(g_11C304, 0, x, y, -1, 5);
    g_ctx48[0x08 / 4] = g_11C300;
    uint32_t frame = 0;
    if (g_1223F0 != 0) {
        uint32_t hit = env->fn1D840(g_mouse11B828, g_mouse11B824);
        frame = (uint8_t)hit != 0 ? 1 : 0;
    }
    env->drawCel(g_11C304, frame, x + 3, y - 4, -1, 5);
}

// 1.13c D2Client+0x200e0: 10569(param_1, &kind, &other); when the object is absent or its +0x10
// word is 0, and kind is 4, refresh the two position accessors of +0x2C for every object type except
// 2 and 4..5, then call 0xB0450. Retail's 0xB0450 argument is the caller's ECX (an artifact of the
// previous call), modelled as 0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_200e0(int32_t* param_1, const D2ClientBatch1de0Env* env) {
    uint8_t kind = 0, other = 0;
    env->ord10569(param_1, &kind, &other);
    if ((param_1 == nullptr || param_1[4] == 0) && kind == 4) {
        int32_t t = *param_1;
        if (t != 2 && (t < 4 || t > 5) && param_1[0xb] != 0) env->ord10750((uint32_t)param_1[0xb]);
        t = *param_1;
        if (t != 2 && (t < 4 || t > 5) && param_1[0xb] != 0) env->ord10867((uint32_t)param_1[0xb]);
        env->fnB0450(param_1, 0);
    }
}

// 1.13c D2Client+0x020180: object type 3 only. Resolve the record through 0xA60D0, require the dword
// at +4 to be a valid index into sgptDataTables+0xB64 (stride 0x1A4) and the gdwBitMasks[0x10] bit at
// record+5 clear; 0x1FD00(obj, 0x80) must be false. Take the +0x2C accessor pair (through 0xA5F70 when
// it resolves, else 10983/10764), compare its vertical accessor with the record object's +0x10
// (0x1630/0x1660), clamp the difference to -1/0/1 (forced 0 for indexes 0x3F..0x42), set the flag
// 0x80 at obj[5]+0x16 while drawing, and draw piVar5[3] - delta (0xB0450) after 10240/11029; restore
// the flag after the second draw.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20180(void* self, int32_t param_1,
                                          const D2ClientBatch1de0Env* env) {
    if (param_1 == 0 || self == nullptr || *(int32_t*)self != 3) return;
    int32_t iVar2 = *(int32_t*)((uint8_t*)self + 4);
    int32_t* piVar5 = (int32_t*)env->fnA60D0(self);
    if (piVar5 == nullptr || iVar2 < 0 || iVar2 >= g_dtCountB6C) return;
    int32_t rec = iVar2 * 0x1A4 + (int32_t)g_dtBaseB64;
    if (rec == 0 || (g_bitmask10 & *(uint8_t*)(rec + 5)) != 0) return;
    if (env->fn1FD00(self, 0x80)) return;
    uint32_t uVar9 = *(uint32_t*)((uint8_t*)self + 0x2C);
    int32_t* piVar7 = (int32_t*)env->fnA5F70(self);
    int32_t iVar6;
    if (piVar7 == nullptr) {
        env->ord10983(uVar9);
        iVar6 = (int32_t)env->ord10764(uVar9);
    } else {
        env->fn1630(piVar7);
        iVar6 = (int32_t)env->fn1660(piVar7);
    }
    env->ord10936();
    env->ord10614();
    int32_t iVar3 = piVar5[3];
    env->fn1630(piVar5);
    int32_t iVar8 = (int32_t)env->fn1660(piVar5) - iVar6;
    if (iVar8 < 0) iVar8 = -1;
    else if (iVar8 > 0) iVar8 = 1;
    if (iVar2 > 0x3E && iVar2 < 0x43) iVar8 = 0;
    if (*piVar5 == 1 && piVar5[5] != 0) *(uint8_t*)(piVar5[5] + 0x16) |= 0x80;
    env->ord10240(self, env->ord11029(self));
    env->fnB0450(self, (uint32_t)(iVar3 - iVar8));
    env->fnB0450(self, env->ord10240(self, env->ord11029(self)));
    if (*piVar5 == 1 && piVar5[5] != 0) *(uint8_t*)(piVar5[5] + 0x16) &= 0x7F;
}

// 1.13c D2Client+0x20340 (0x020340): __fastcall(param_1 /*ECX*/, param_2 /*EDX*/), eaxIn = EAX (0 becomes 1).
// Skip only when param_2 == 0xC3 and the object is not a live type-1 object with bit 0 of +0x17 at
// obj[5]. Build the 0x5C-byte draw context {+0 = 0x21; +4 = obj; +0x10 = param_2; +0x14 = x; +0x18 =
// y; +0x1C/+0x20 = per-tile x/y; +0x2C = 0; +0x30 = eaxIn; +0x54 = 0x73B60 unless param_2 == 0x141;
// +0x58 = sub-tile} and emit eight 0xB30F0 draws: the four orthogonal neighbours (dx {0,1,0,-1}, dy
// {-1,0,1,0}) twice each. x/y come from obj+0x2C+0xC/+0x10 for types 2 and 4..5, else 10867/10750.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20340(int32_t* param_1, uint32_t param_2, int32_t eaxIn,
                                          const D2ClientBatch1de0Env* env) {
    if (eaxIn == 0) eaxIn = 1;
    if (param_2 == 0xC3 &&
        !(param_1 != nullptr && *param_1 == 1 && param_1[5] != 0 &&
          (*(uint8_t*)(param_1[5] + 0x17) & 1) != 0))
        return;
    uint32_t ctx[0x5C / 4];
    memset(ctx, 0, sizeof ctx);
    ctx[0] = 0x21;
    *(int32_t**)((uint8_t*)ctx + 0x04) = param_1;
    *(uint32_t*)((uint8_t*)ctx + 0x10) = param_2;
    int32_t t = *param_1;
    int32_t x;
    if (t == 2 || (t > 3 && t < 6)) x = *(int32_t*)(param_1[0xb] + 0x0C);
    else if (param_1[0xb] == 0) x = 0;
    else x = (int32_t)env->ord10867((uint32_t)param_1[0xb]);
    t = *param_1;
    int32_t y;
    if (t == 2 || (t > 3 && t < 6)) y = *(int32_t*)(param_1[0xb] + 0x10);
    else if (param_1[0xb] == 0) y = 0;
    else y = (int32_t)env->ord10750((uint32_t)param_1[0xb]);
    *(int32_t*)((uint8_t*)ctx + 0x14) = x;
    *(int32_t*)((uint8_t*)ctx + 0x18) = y;
    *(uint32_t*)((uint8_t*)ctx + 0x2C) = 0;
    *(int32_t*)((uint8_t*)ctx + 0x30) = eaxIn;
    if (param_2 != 0x141) *(uint32_t*)((uint8_t*)ctx + 0x54) = 0x73B60;
    static const int32_t dx[4] = {0, 1, 0, -1};
    static const int32_t dy[4] = {-1, 0, 1, 0};
    for (int32_t k = 0; k < 4; ++k) {
        t = *param_1;
        int32_t bx;
        if (t == 2 || (t > 3 && t < 6)) bx = *(int32_t*)(param_1[0xb] + 0x0C);
        else bx = param_1[0xb] != 0 ? (int32_t)env->ord10867((uint32_t)param_1[0xb]) : 0;
        t = *param_1;
        int32_t by;
        if (t == 2 || (t > 3 && t < 6)) by = *(int32_t*)(param_1[0xb] + 0x10);
        else by = param_1[0xb] != 0 ? (int32_t)env->ord10750((uint32_t)param_1[0xb]) : 0;
        for (int32_t s = 0; s < 2; ++s) {
            *(int32_t*)((uint8_t*)ctx + 0x1C) = bx + dx[k];
            *(int32_t*)((uint8_t*)ctx + 0x20) = by + dy[k];
            *(int32_t*)((uint8_t*)ctx + 0x58) = s;
            env->fnB30F0(ctx);
        }
    }
}

// 1.13c D2Client+0x20620: __fastcall(param_1, param_2 /*EDX*/, param_3). param_3 == 0 returns.
// Builds a 600-wide-character Unicode buffer (300 default constructions at stride 4), converts the
// single space, then: for a live type-1 object, bit 2 of obj[5]+0x16 draws 10038(word at +0x26)
// through 10971; bit 4 appends the 10003 strings via the format helper 0x90CB0 and prints. Otherwise
// 10614, the 0x0C shortcut and the id set {0x9C,0xD3,0xF2,0xF3,0x14D,0x100,0x220} / {0x2C0..0x2C5}
// also print; anything else appends 10852/10249(10003()) and, when the RNG roll at obj+0x20 is below
// 50, appends 10272/10852/10249(10003()) first. rngMod is retail's EAX at the 0x1330 call.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20620(int32_t* param_1, uint32_t param_2, int32_t param_3,
                                          uint32_t rngMod, const D2ClientBatch1de0Env* env) {
    if (param_3 == 0) return;
    uint8_t buf[1200];
    memset(buf, 0, sizeof buf);
    env->unicodeCtor300(buf);
    char ascii[2] = {' ', 0};
    env->unicodeToUnicode(g_conv1223D0, ascii, 2);
    const void* pUVar2;
    const void* pUVar6;
    int32_t iVar5;
    if (param_1 == nullptr) {
        iVar5 = -1;
    } else {
        if (*param_1 == 1 && (iVar5 = param_1[5]) != 0 && (*(uint8_t*)(iVar5 + 0x16) & 2) != 0) {
            int16_t sVar4 = iVar5 != 0 ? *(int16_t*)(iVar5 + 0x26) : 0;
            const void* h = env->ord10038(sVar4);
            if (h == nullptr) return;
            env->ord10003(0);
            env->ord10971();
            return;
        }
        iVar5 = param_1[1];
        if (*param_1 == 1 && param_1[5] != 0 && (*(uint8_t*)(param_1[5] + 0x16) & 4) != 0) {
            int32_t i = 0;
            while (i < 4 && g_tbl0D6D64[i * 2] != param_2) ++i;
            uintptr_t a = (uintptr_t)env->ord10003(0);
            pUVar6 = env->ord10003(a);
            pUVar2 = env->ord10003(0);
            env->fn90CB0(buf, pUVar2, pUVar6);
            env->ord10971();
            return;
        }
    }
    env->ord10614();
    if (param_2 == 0x0C) {
        env->ord10003(0);
        env->ord10971();
        return;
    }
    if (iVar5 < 0x221) {
        switch (iVar5) {
        case 0x9C: case 0xD3: case 0xF2: case 0xF3: case 0x100: case 0x14D: case 0x220:
            env->ord10003(0);
            env->ord10971();
            return;
        }
    } else {
        switch (iVar5) {
        case 0x2C0: case 0x2C1: case 0x2C2: case 0x2C3: case 0x2C4: case 0x2C5:
            env->ord10003(0);
            env->ord10971();
            return;
        }
    }
    uint32_t u = env->ord10852(0);
    pUVar6 = env->ord10249(u);
    pUVar2 = env->ord10003(0);
    env->fn90CB0(buf, pUVar2, pUVar6);
    uint32_t roll = env->fn1330((uint64_t*)((uint8_t*)param_1 + 0x20), rngMod);
    if ((int32_t)roll < 0x32) {
        u = env->ord10272(0);
        u = env->ord10852(u);
        pUVar6 = env->ord10249(u);
        pUVar2 = env->ord10003(0);
        env->fn90CB0(buf, pUVar2, pUVar6);
    }
    env->ord10971();
}

// 1.13c D2Client+0x20a60: __fastcall with the object in ESI. Refresh 10867/10750 for every type but 2
// and 4..5 when +0x2C is set, emit two 0xB30F0 draws of a zeroed 0x5C-byte context whose first dword is
// 1, then 0x88A70(obj, 0, 0, 0).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20a60(int32_t* obj, const D2ClientBatch1de0Env* env) {
    int32_t t = *obj;
    if (t != 2 && (t < 4 || t > 5) && obj[0xb] != 0) env->ord10867((uint32_t)obj[0xb]);
    t = *obj;
    if (t != 2 && (t < 4 || t > 5) && obj[0xb] != 0) env->ord10750((uint32_t)obj[0xb]);
    uint32_t ctx[0x5C / 4];
    memset(ctx, 0, sizeof ctx);
    ctx[0] = 1;
    env->fnB30F0(ctx);
    memset(ctx, 0, sizeof ctx);
    ctx[0] = 1;
    env->fnB30F0(ctx);
    env->fn88A70(obj, 0, 0, 0);
}

// 1.13c D2Client+0x20b70: 10569(param_1, &kind, &other); when the object is absent, its +0x10 word is
// 0 or 3, and kind is 2, send 10973(param_1, 0x0C, 0) and run 0x20340(param_1, 0xE1) with the retail
// EAX (0 -> 1 inside 0x20340).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20b70(int32_t* param_1, const D2ClientBatch1de0Env* env) {
    uint8_t kind = 0, other = 0;
    env->ord10569(param_1, &kind, &other);
    if ((param_1 == nullptr || param_1[4] == 3 || param_1[4] == 0) && kind == 2) {
        env->ord10973(param_1, 0x0C, 0);
        d2_client_20340(param_1, 0xE1, 0, env);
    }
}

// 1.13c D2Client+0x20f20: __fastcall, object in EBX. Only a live type-1 object whose obj[5] is set and
// whose byte at +0x1C is nonzero: play, with the bit-3 flag of obj[5]+0x16, every non-null callback at
// 0x0FAA340[index], first for the four indexes at 0x0D6D2C..0x0D6D38, then for the nine bytes at
// obj[5]+0x1C.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20f20(int32_t* obj) {
    if (obj == nullptr || *obj != 1) return;
    int32_t iVar3 = obj[5];
    if (iVar3 == 0) return;
    const char* pcVar1 = (const char*)((uint8_t*)iVar3 + 0x1C);
    if (*pcVar1 == 0) return;
    uint8_t flag = (uint8_t)((*(uint8_t*)(iVar3 + 0x16) >> 3) & 1);
    for (int32_t i = 0; i < 4; ++i) {
        void (*fn)(uint8_t) = (void (*)(uint8_t))g_animFn[g_list0D6D2C[i]];
        if (fn != nullptr) fn(flag);
    }
    for (int32_t i = 0; i < 9; ++i) {
        void (*fn)(uint8_t) = (void (*)(uint8_t))g_animFn[(uint8_t)pcVar1[i]];
        if (fn != nullptr) fn(flag);
    }
}

// 1.13c D2Client+0x20fb0: __fastcall, dispatch table on the dword at param_1+4, each handler gated on
// the sub-state at +0x10: 0x2D/0x9C/0xD3/0xE5/0xF2/0xF3/0x100/0x10B/0x16E/0x1DF/0x220 at 0 (0x9C also
// receives param_1 twice, 0x1DF receives param_1), 0x11C only at 0x0E, and 0x21C..0x21E at 0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_20fb0(int32_t* param_1, const D2ClientBatch1de0Env* env) {
    if (param_1 == nullptr) return;
    uint32_t kind = *(uint32_t*)((uint8_t*)param_1 + 4);
    int32_t sub = *(int32_t*)((uint8_t*)param_1 + 0x10);
    if (kind < 0x101) {
        if (kind == 0x100) { if (sub == 0) env->fn53070(); return; }
        switch (kind) {
        case 0x2D:  if (sub == 0) env->fn52860(); return;
        case 0x9C:  if (sub == 0) env->fn52230(param_1, param_1); return;
        case 0xD3:  if (sub == 0) env->fn52620(); return;
        case 0xE5:  if (sub == 0) env->fn527A0(); return;
        case 0xF2:  if (sub == 0) env->fn52270(); return;
        case 0xF3:  if (sub == 0) env->fn52600(); return;
        }
        return;
    }
    if (kind < 0x1E0) {
        if (kind == 0x1DF) { if (sub == 0) env->fn52BD0(param_1); return; }
        if (kind == 0x10B) { if (sub == 0) env->fn52320(); return; }
        if (kind == 0x11C) { if (sub == 0x0E) env->fn526E0(); return; }
        if (kind == 0x16E && sub == 0) env->fn530D0();
        return;
    }
    if (kind > 0x21B) {
        if (kind < 0x21F) { if (sub == 0) env->fn529F0(); }
        else if (kind == 0x220 && sub == 0) env->fn52570();
    }
}

// 1.13c D2Client+0x212f0: cdecl(obj, ecxArg, base), callee pops 12. Live type-1 object with obj[5] set:
// for each of the nine index bytes at obj[5]+0x1C call table[base + byte*5] (when non-null) as
// __fastcall(ECX = obj, or ecxArg when base == 4; EDX = byte; stack = bit 3 of obj[5]+0x16).
// Retail does not range-check the index; the table here is sized for the largest reachable index so
// out-of-range slots read as empty instead of past the array.
typedef void (*AnimCb)(uint32_t ecx, uint32_t edx, uint32_t flag);
static AnimCb g_animCb[1280];        // +0x0F9FE0 static table, 5-dword stride

// Retail's static initialiser: slots 47, 52, 57 hold the D2Client callbacks at +0x20D20, +0x20C40,
// +0x21210. Their code is reached through the host, so it hands the three entry points over once.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_212f0_init(AnimCb s47, AnimCb s52, AnimCb s57) {
    g_animCb[47] = s47;
    g_animCb[52] = s52;
    g_animCb[57] = s57;
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_212f0(int32_t* param_1, uint32_t param_2, int32_t param_3) {
    if (param_1 == nullptr || *param_1 != 1) return;
    int32_t iVar2 = param_1[5];
    if (iVar2 == 0) return;
    uint8_t flag = (uint8_t)((*(uint8_t*)(iVar2 + 0x16) >> 3) & 1);
    uint32_t recv = (param_3 == 4) ? param_2 : (uint32_t)(uintptr_t)param_1;
    for (int32_t i = 0; i < 9; ++i) {
        uint8_t idx = *(uint8_t*)(iVar2 + 0x1C + i);
        AnimCb fn = g_animCb[(uint32_t)param_3 + (uint32_t)idx * 5];
        if (fn != nullptr) fn(recv, idx, flag);
    }
}

// 1.13c D2Client+0x021400: walk the linked list at obj+0x90 (next at node[7]); every node whose key
// (*node) equals the EBX argument invokes node[5](param_1, param_2, node[3], node[4]) and its return
// becomes the result. The walk continues after a hit; the last hit wins.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_21400(uint32_t param_1, uint32_t param_2, void* obj,
                                              int32_t key, const D2ClientBatch1de0Env* env) {
    uint32_t result = 0;
    if (obj == nullptr) return result;
    int32_t* node = *(int32_t**)((uint8_t*)obj + 0x90);
    while (node != nullptr) {
        int32_t* next = (int32_t*)node[7];
        if (*node == key)
            result = env->invoke1410((void*)(uintptr_t)node[5], param_1, param_2,
                                     (uint32_t)node[3], (uint32_t)node[4]);
        node = next;
    }
    return result;
}
