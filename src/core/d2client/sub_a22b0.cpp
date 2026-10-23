#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xA22B0
// Retail layout (32-bit): a = EAX struct {+0 kind, +4 id, +0x2C handle}; p = arg {+0, +4 next, +8, +0xC}.
struct D2CA22B0Env {
    int      (*ord_10747)(void);
    void     (*sub_a0660)(uint32_t p);
    void     (*ord_10286)(uint32_t h, int a, int b);
    void     (*ord_10434)(void);
    uint32_t (*tbl_count)(void);              // *(sgptDataTables+0xA80)
    uint32_t (*tbl_rec_tag)(uint32_t idx);    // rec(+0xA78 + idx*0x1A8): 0 if null, else dword at +0x10
    int8_t   (*ord_10788)(uint32_t h);
    void     (*ord_10425)(uint32_t h, uint32_t v);
    void     (*ord_11129)(void);
    void     (*sub_a1cf0)(uint32_t first, uint32_t next);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a22b0(const uint32_t* a, const uint32_t* p, const D2CA22B0Env* e) {
    uint32_t x = p[2], y = p[3];
    if (e->ord_10747()) e->sub_a0660((uint32_t)(uintptr_t)p);
    e->ord_10286(a[0xb], (int)x, (int)y);
    e->ord_10434();
    if (a[0] == 1 && (int32_t)a[1] >= 0 && a[1] < e->tbl_count()) {
        uint32_t tag = e->tbl_rec_tag(a[1]);
        if (tag == 0x20204250u) {
            int8_t c = e->ord_10788(a[0xb]);
            e->ord_10425(a[0xb], (((x >> 8) << 8) | (uint8_t)(c + 0x38)) & 0xffffff3fu);
        }
    }
    e->ord_11129();
    e->sub_a1cf0(p[0], p[1]);
}
