// src/core/d2client/d2client_batch_8ca80.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals are pointers/parameters; ordinals and
// unrecovered callees are host callbacks. GUESSED ARGUMENTS (hidden registers): 8D430 (EBX=id),
// 8D4F0 (EAX=unit, EBX=suffix, retaddr slot=flag, ECX=out), 8D600 (EAX=item, ECX=out, ESI=out, EBP=tmp),
// 8D880 (ESI=out). Unicode/string helpers are callbacks taking the retail buffers.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

// 1.13c D2Client+0x8CA80 (178 B) fastcall(param_1)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8ca80(int32_t p, uint32_t unit_bc73b, int32_t* g_bcbc58, int32_t* g_bcbc5c, uint32_t* g_bcbc60,
                     char** g_bcbc64, int (*ord10174)(uint32_t, int, int), void (*ord10003)(uint32_t, int, int),
                     uint32_t (*tick)(void), void (*sprintf578)(char*, int, const char*, const char*),
                     char* (*load_b420)(int)) {
    char buf[260];
    if (p == 0x20747368) {
        if (!unit_bc73b) return;
        if (ord10174(unit_bc73b, 10, 0xb)) return;
        ord10003(unit_bc73b, 10, 0xb);
    } else if (p == 0x20326671) {
        if (!unit_bc73b) return;
        if (ord10174(unit_bc73b, 0x12, 0xb)) return;
        ord10003(unit_bc73b, 0x12, 0xb);
    }
    *g_bcbc58 = 1;
    *g_bcbc5c = 0;
    *g_bcbc60 = tick();
    if (!*g_bcbc64) {
        sprintf578(buf, 0x104, "%s\\ui\\menu\\horadric", "DATA\\GLOBAL");
        *g_bcbc64 = load_b420(0);
    }
}

// 1.13c D2Client+0x8CB40 (76 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8cb40(int32_t* g_bbb348, char** g_bcbc78, void (*sprintf578)(char*, int, const char*, const char*),
                     char* (*load_b420)(int)) {
    char buf[260];
    *g_bbb348 = 0;
    if (!*g_bcbc78) {
        sprintf578(buf, 0x104, "%s\\ui\\panel\\supertransmogrifier", "DATA\\GLOBAL");
        *g_bcbc78 = load_b420(0);
    }
}

// 1.13c D2Client+0x8CB90 (48 B): send packet {0x4F,0x18,0,0,0,0,0}.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8cb90(void (*send_43e0)(const uint8_t*)) {
    const uint8_t b[8] = {0x4f, 0x18, 0, 0, 0, 0, 0, 0};
    send_43e0(b);
}

// 1.13c D2Client+0x8CC50 (88 B) fastcall(param_1)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8cc50(uint32_t p, uint32_t unit_bbfc, uint32_t* g_90ec0, uint32_t (*ord10331)(uint32_t),
                     uint32_t (*ord10826)(uint32_t), int8_t (*ord10459)(uint32_t)) {
    if (!p || rd32(p + 4) != 0x10b) return;
    uint32_t a = ord10331(p);
    if (!a) return;
    uint32_t b = ord10331(unit_bbfc);
    if (!b) return;
    int8_t c1 = ord10459(ord10826(b));
    int8_t c2 = ord10459(ord10826(a));
    if (c1 == c2) *g_90ec0 = rd32(p + 0xc);
}

// 1.13c D2Client+0x8CCB0 (67 B): FUN_6fb72790(0x19,1,0) then send {0x4F,0x12,0,0,0,0,0}.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8ccb0(void (*f_72790)(int, int, int), void (*send_43e0)(const uint8_t*)) {
    const uint8_t b[8] = {0x4f, 0x12, 0, 0, 0, 0, 0, 0};
    f_72790(0x19, 1, 0);
    send_43e0(b);
}

// 1.13c D2Client+0x8CF60 (72 B)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8cf60(int32_t g_bcbc34, uint32_t unit_bbfc, int32_t g_bcb864, int (*ord11017)(uint32_t)) {
    if (g_bcbc34 == 1 || g_bcbc34 == 2 || g_bcbc34 == 4 || g_bcbc34 == 3)
        if (ord11017(rd32(unit_bbfc + 0x60)) == 0 && (g_bcb864 == 7 || g_bcb864 == 1 || g_bcb864 == 4)) return 1;
    return 0;
}

// 1.13c D2Client+0x8D300 (98 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8d300(int32_t* g_bb23c, int32_t* g_bb250, int32_t* g_bb264, int32_t* g_bb278, int32_t* g_bcbc34,
                     int32_t* g_bcb834, int32_t* g_bcb858, int32_t* g_bcb854, int32_t* g_bcb864,
                     uint32_t unit_bbfc, int (*ord11017)(uint32_t)) {
    *g_bb23c = 0; *g_bb250 = 0; *g_bb264 = 0; *g_bb278 = 0;
    *g_bcbc34 = 1;
    *g_bcb834 = ord11017(rd32(unit_bbfc + 0x60));
    *g_bcb858 = 0;
    *g_bcb854 = 5;
    *g_bcb864 = 1;
    if (*g_bcb834) *g_bcb864 = 4;
}

// 1.13c D2Client+0x8D370 (65 B) fastcall(param_1): g_90ea8[0..5] is 0x6FB90EA8..0x6FB90EBC.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8d370(int32_t p, int32_t* g_bcbc38, uint32_t* g_90ea8, int32_t* g_bcbc28, int32_t* g_bcbc2c,
                     int32_t* g_bcbc3c) {
    if (*g_bcbc38 != p) return;
    for (int i = 0; i < 6; i++) g_90ea8[i] = 0xffffffffu;
    *g_bcbc28 = 0; *g_bcbc2c = 0; *g_bcbc38 = 0; *g_bcbc3c = 0;
}

// 1.13c D2Client+0x8D430 (179 B): EBX = class id; walks the list from ord10460.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8d430(int16_t id, uint32_t data_tables, uint32_t (*ord10460)(void), uint32_t (*ord11147)(uint32_t),
                         int (*ord10106)(uint32_t), int (*ord10202)(uint32_t, int, int, const char*),
                         int8_t (*ord10020)(uint32_t), int (*ord10698)(uint32_t), int (*ord10620)(uint32_t),
                         uint32_t (*ord10464)(uint32_t)) {
    for (uint32_t n = ord10460(); n; n = ord10464(n)) {
        uint32_t u = ord11147(n);
        if (ord10106(u) != 5) continue;
        if (!ord10202(u, 0x10, 0xca1, "..\\Source\\D2Client\\UI\\inv.cpp")) continue;
        int8_t c = ord10020(u);
        if (!(c == 0 || c == 3 || c == 4 || c == -1)) continue;
        int s = ord10698(n);
        if (!(s == 1 || s == 3 || s == 4)) continue;
        int idx = ord10620(u);
        if (idx < 0 || idx >= (int)rd32(data_tables + 0xc1c)) continue;
        uint32_t rec = (uint32_t)idx * 0x1b8 + rd32(data_tables + 0xc18);
        if (rec && *(const int16_t*)(uintptr_t)rec == id) return u;
    }
    return 0;
}

// 1.13c D2Client+0x8D4F0 (269 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8d4f0(void* out, uint32_t unit, const void* suffix, int32_t flag, uint8_t* tmp /*>=24 B*/,
                     uint32_t (*ord10003)(void), uint32_t (*ord10460)(uint32_t), uint32_t (*ord11147)(uint32_t),
                     int (*ord10744)(uint32_t, int), uint32_t (*ord10695)(uint32_t), uint32_t (*ord10806)(int),
                     uint32_t (*ord10464)(uint32_t), void (*ustrcat)(void*, const void*),
                     void (*toUnicode)(void*, const char*, int)) {
    uint32_t list = rd32(unit + 0x60);
    if (!list) return;
    int first = 0;
    uint8_t* seg = tmp;       // local_c, 12 B
    uint8_t* quote = tmp + 12;
    uint32_t sep = ord10003();
    for (uint32_t n = ord10460(list); n; n = ord10464(n)) {
        uint32_t u = ord11147(n);
        if (!u) break;
        if (ord10744(u, 0x4a)) {
            uint32_t rec = ord10695(rd32(u + 4));
            int32_t k = (int32_t)rd32(rec + 0xf0);
            if (k > 0) {
                uint32_t r = ord10806(k);
                if (r) {
                    if (!first) { first = 1; ustrcat(out, (const void*)(uintptr_t)ord10003()); }
                    toUnicode(seg, (const char*)(uintptr_t)(r + 0x20), 6);
                    ustrcat(out, seg);
                }
            }
        }
    }
    (void)sep;
    if (flag) { toUnicode(quote, "'", 5); ustrcat(out, quote); ustrcat(out, suffix); }
}

// 1.13c D2Client+0x8D600 (536 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8d600(uint32_t param_1, uint32_t item, uint32_t unit_bbfc, void* out,
                     uint32_t (*ord10003)(void), uint32_t (*ord10428)(uint32_t, uint32_t, uint32_t),
                     uint32_t (*ord10540)(uint32_t, uint32_t), uint32_t (*ord10393)(uint32_t, uint32_t),
                     void (*ustrcpy)(void*, const void*), void (*ustrcat)(void*, const void*),
                     void (*usprintf)(int, void*, const void*), void (*sprintf578)(char*, int, const char*, uint32_t),
                     void (*win2Unicode)(void*, const char*, int), void* tmp_u /*>=24 B*/, void* tmp_big /*>=0x400 B*/) {
    if (!param_1 || !item || !*(const uint8_t*)(uintptr_t)(item + 0xb4) || !unit_bbfc) return;
    if (*(const int16_t*)(uintptr_t)(item + 0xb6) == 0x1506) return;
    uint32_t a = ord10003();
    uint32_t b = ord10003();
    uint32_t v;
    char num[16];
    switch (*(const uint8_t*)(uintptr_t)(item + 0xb4)) {
    case 1:
        ustrcpy(out, (const void*)(uintptr_t)ord10003());
        ustrcat(out, (const void*)(uintptr_t)b);
        return;
    case 2:
        v = ord10428(unit_bbfc, param_1, rd32(item + 0xa4));
        switch (*(const int16_t*)(uintptr_t)(item + 0x9e)) {
        case 6: case 0x4a: v = ord10540(unit_bbfc, v); break;
        case 8: case 0x1a: v = ord10393(unit_bbfc, v); break;
        }
        break;
    case 3:
        v = ord10428(unit_bbfc, param_1, rd32(item + 0xa4));
        break;
    case 4:
        v = ord10428(unit_bbfc, param_1, rd32(item + 0xa4));
        usprintf(0x200, tmp_big, (const void*)(uintptr_t)ord10003());
        ustrcat(out, tmp_big);
        ustrcat(out, (const void*)(uintptr_t)a);
        return;
    default:
        return;
    }
    ustrcpy(out, (const void*)(uintptr_t)ord10003());
    sprintf578(num, 10, "%d", v);
    win2Unicode(tmp_u, num, 9);
    ustrcat(out, tmp_u);
    ustrcat(out, (const void*)(uintptr_t)a);
    ustrcat(out, (const void*)(uintptr_t)b);
}

// 1.13c D2Client+0x8D880 (183 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8d880(uint32_t unit, void* out, void* tmp_u /*>=40 B*/, uint32_t (*ord10003)(void),
                     int (*ord10973)(uint32_t, int, int), int (*ord10463)(uint32_t),
                     void (*ustrcpy)(void*, const void*), void (*ustrcat)(void*, const void*),
                     void (*sprintf578)(char*, int, const char*, int), void (*win2Unicode)(void*, const char*, int)) {
    char num[16];
    uint32_t a = ord10003();
    uint32_t b = ord10003();
    if (ord10973(unit, 0x46, 0) < 1 && ord10463(unit) < 1) return;
    ustrcpy(out, (const void*)(uintptr_t)ord10003());
    int v = ord10973(unit, 0x46, 0);
    sprintf578(num, 10, "%d", v);
    win2Unicode(tmp_u, num, 9);
    ustrcat(out, (const void*)(uintptr_t)a);
    ustrcat(out, tmp_u);
    ustrcat(out, (const void*)(uintptr_t)b);
}
