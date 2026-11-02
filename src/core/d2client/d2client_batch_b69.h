#pragma once
// D2Client batch b69 env: shared by the batch .cpp and the fn probe (src/core/fn/d2client_811f0.cpp).
#include <cstdint>
#include <cstddef>

struct D2cB69Env {
    uint8_t* const* unit;                 // DAT_6fbcbbfc (player unit; +0x20 seed lo, +0x24 seed hi, +0x60 inv)
    void (*fatal)(int32_t msg);           // Ordinal_10265(msg)+Ordinal_10024+_exit(-1); does not return
    // fade / weather state
    int32_t *g_bf84, *g_bf80, *g_bf88, *g_bf6c, *g_bf70, *g_97f8, *g_d343c, *g_d3438, *g_d3430, *g_d3434;
    uint32_t *g_c3934, *g_c3914;
    float *g_c38f4; const float* g_875b0;
    uint8_t* g_bf68; uint8_t* g_bf60; uint8_t* g_bf64;   // units, +0x114 tested
    int32_t *g_bf78, *g_bf7c;
    void (*f80ca0)(); void (*f814f0)(); void (*f80e30)(); void (*f80160)(); void (*f800d0)();
    void (*f80f60)(uint64_t*); void (*f80be0)(); void (*f812f0)(); void (*f81470)(const uint8_t*);
    int32_t (*f18d0)(uint64_t*, int32_t esi, int32_t edi);   // 0x18D0: ESI/EDI are hidden register inputs
    const int32_t* tblEsi; const int32_t* tblEdi;            // DAT_6fbc38ac[4] / DAT_6fbc38c4[4], written by 0x81750
    int32_t (*ord10331)(const uint8_t*); int32_t (*ord10826)(int32_t); char (*ord10215)(int32_t);
    char (*ord10499)(int32_t); char (*ord10459)(int32_t); void (*ord10063)();
    // rain/bubble setup (0x81750)
    int32_t *g_d3444, *g_c38c4, *g_c38c8, *g_c38cc, *g_c38d0, *g_c38b4, *g_c38ac, *g_c38b0, *g_c38b8;
    uint8_t *g_c38d4, *g_c38a0, *g_c38f8, *g_c38e0, *g_c3940;     // 12-byte tables
    int32_t *g_rain, *g_rainH, *g_bub, *g_bubH;                    // c3904 / c391c / c3890 / c3880 (4 ints each)
    uint8_t (*ord10190)(int8_t, int8_t, int8_t);
    void (*fmt_overlay)(char* buf, const char* name);              // wsprintfA(buf,"%s\\UncompOverlays\\<name>","DATA\\GLOBAL")
    int32_t (*fadb380)(const char* path, int32_t z); int32_t (*ord10086)(int32_t);
    // 0x8F430 (held-item tab setup); b = &DAT_6fbbb23c
    uint32_t* tbl_ba808; int32_t *g_9721, *g_971d, *g_97a8, *g_bc50, *g_911c4, *g_911c8; uint8_t* b_bbb23c;
    void (*f3ba30)(); void (*f3b9f0)(); void (*f3ba70)(); void (*f3bab0)(); void (*f3b9c0)();
    // 0x86A40
    uint32_t* tbl_bae08; int32_t *g_9e94; const uint32_t* g_9e98; int32_t *g_aad84, *g_aad80; const uint32_t* g_8d538;
    int32_t *g_8d5d0, *g_c3a68, *g_aad88, *g_aadd0, *g_aadbc, *g_aadc0, *g_aae10, *g_aae14, *g_aadd8;
    int32_t *g_9e14, *g_8bc48, *g_cc418, *g_cc414, *g_cc3dc, *g_cc3e0, *g_cc3f8, *g_cc3fc;
    int32_t *g_cb834, *g_cb858, *g_cb864, *g_cb854;
    uint8_t* g_c9721;
    void (*ord10227)(); void (*ord10006)(); int32_t (*ord10048)(); void (*ord10977)(int32_t*, int32_t, int32_t);
    void (*f72790)(uint32_t, int32_t, int32_t); void (*f7b20)(); int32_t (*f55440)(); void (*f3bb90)(); void (*f3bbf0)();
    void (*feccc0)(int32_t, const void*); const void* g_805b0; void (*f3a4d0)(int32_t, int32_t, int32_t); void (*f31aa0)();
    int32_t* (*f54e20)(uint32_t id, uint32_t* tbl, int32_t n);
};

extern "C" {
void d2c_81120(const D2cB69Env* e, int32_t mode);
void d2c_811f0(const D2cB69Env* e, uint64_t* seed);
}
