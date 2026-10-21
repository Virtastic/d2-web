// D2Client+0x373C0: builds a status string from two table entries; env-seam style.
#include <cstdint>
#include <emscripten.h>
// Table at D2Client+0xFA2E0..0xFA75A (rva = va-0x6fab0000... caller supplies pointers), 10-byte packed records:
// i32 kind @0, i16 id @4, i32 flag @6.
struct D2c373C0Env {
    const uint8_t* tbl;                              // 0x6fbca2e0
    const uint8_t* tblEnd;                           // 0x6fbca75a
    int32_t bc4c;                                    // DAT_6fb8bc4c
    const uint16_t* (*ord10003)(int32_t idx);        // Ordinal_10003 (idx ignored on 1st call)
    int32_t (*f9F30)(int32_t a, int32_t b);          // D2Client+0x9F30
    void (*sprintf)(int32_t n, uint16_t* dst, const uint16_t* fmt, int32_t a);  // Unicode::sprintf
    void (*ord10028)();
    void (*ord10150)(const uint16_t* s, int32_t a, int32_t b);
};
static int32_t rd32(const uint8_t* p) { int32_t v; __builtin_memcpy(&v, p, 4); return v; }
static int32_t find(const D2c373C0Env* e, int32_t flag) {
    for (const uint8_t* p = e->tbl; p + 10 <= e->tblEnd; p += 10)
        if (rd32(p) == 6 && rd32(p + 6) == flag) return (int32_t)((p - e->tbl) / 10);
    return -1;
}
static void app(const D2c373C0Env* e, uint16_t* out, int32_t flag) {
    int32_t i = find(e, flag);
    if (i < 0) return;
    int16_t id; __builtin_memcpy(&id, e->tbl + i * 10 + 4, 2);
    if (id == -1) return;
    uint16_t t[30] = {0};
    e->sprintf(30, t, e->ord10003(e->f9F30(0, flag)), 0);
    uint32_t n = 0; while (out[n]) n++;
    for (uint32_t k = 0; k < 30 && t[k] && n < 399; k++) out[n++] = t[k];
    out[n] = 0;
}
extern "C" {
// 1.13c D2Client+0x373C0
EMSCRIPTEN_KEEPALIVE void d2c_373c0(const D2c373C0Env* e) {
    uint16_t buf[200] = {0};
    const uint16_t* s = e->ord10003(0);
    for (uint32_t k = 0; s[k] && k < 199; k++) buf[k] = s[k];
    app(e, buf, 1);
    app(e, buf, 0);
    e->ord10028();
    e->ord10150((const uint16_t*)(intptr_t)(e->bc4c - 0xc5), 0, 0);
}
}
