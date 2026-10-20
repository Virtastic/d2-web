// Batch 0x7ACC0..0x7B580: small D2Client UI/message-list helpers.
// Retail register-passed args become explicit params; retail globals are
// file-scope state; D2Win/D2Lang/Fog ordinals are host hooks (weak, pinned by FUNCTION replay).
#include <cstdint>
#include <cstring>

extern "C" {
void* d2_fog_alloc(uint32_t size, uint32_t zero);  // Fog 10042
void d2_fog_free(void* p, uint32_t line);          // Fog 10043
void d2_fog_10065(void* p);
void d2_win_fill_rect(int32_t l, int32_t t, int32_t r, int32_t b, uint32_t c, uint32_t mode);  // D2Gfx 10014
void d2_win_draw_text(const uint16_t* s, int32_t x, int32_t y, uint32_t color);                 // 10150
void d2_win_set_font(uint32_t f);                                                               // 10184
int32_t d2_win_text_width(const uint16_t* s);                                                   // 10171
}

namespace d2c {
struct Rect { int32_t l, t, r, b; };
// 1.13c message-list node: 6 string slots, count@0x18, color@0x1a, expiry@0x1e, next@0x22
struct Msg { uint16_t* s[6]; uint16_t count; uint32_t color; uint32_t expiry; Msg* next; };

int32_t g_3950[3]; int32_t g_3b14;                  // DAT_6fbc3950..58, 6fbc3b14
void* g_bfac; void* g_bfa8; void* g_bfb0;           // DAT_6fbcbfac/a8/b0
int32_t g_screenW, g_screenH, g_91384, g_cec38;     // 6fb8bc48, 6fb8bc4c, 6fb91384, 6fbcec38
int32_t g_9648, g_9644, g_9640, g_963c, g_c2fc, g_c2f8, g_c2f4;
Rect g_rects[16];                                   // 6fbc3960
int32_t g_913dc[64]; int32_t g_913fc;               // 6fb913dc table, count
Msg* g_3ab0; Msg* g_3aaa; int32_t g_3aae;           // lists/count
}
using namespace d2c;

// 1.13c D2Client+0x7ACC0
extern "C" void d2_client_7acc0(const uint8_t* pkt) {
    int16_t n; memcpy(&n, pkt + 1, 2);
    if (n == 4) { memcpy(g_3950, pkt + 3, 12); g_3b14 = 0; }
}

// 1.13c D2Client+0x7AD00
extern "C" void d2_client_7ad00() {
    void** slots[3] = {&g_bfac, &g_bfa8, &g_bfb0};
    for (auto s : slots) if (*s) { d2_fog_10065(*s); d2_fog_free(*s, 0x7a); *s = nullptr; }
}

// 1.13c D2Client+0x7AE00
extern "C" void d2_client_7ae00() {
    g_9648 = g_screenH - 0x35;
    if (g_91384 + 0xb6 < g_9648) g_9648 = g_91384 + 0xb6;
    g_9644 = g_cec38 + 0x14f; g_9640 = g_91384; g_963c = g_cec38 - 10;
    g_c2fc = 1; g_c2f8 = 0; g_c2f4 = 0;
}

// 1.13c D2Client+0x7AEA0
extern "C" void d2_client_7aea0(void (*draw)(const Msg*, uint32_t)) {
    for (Msg* m = g_3ab0; m; m = m->next) draw(m, m->color);  // vtable+0x30 call; retail passes first 6 words
}

// 1.13c D2Client+0x7AF20
extern "C" uint32_t d2_client_7af20(Rect* r, int32_t n) {
    auto isect = [](const Rect& a, const Rect& b) { return a.l < b.r && b.l < a.r && a.t < b.b && b.t < a.b; };
    if (n > 15) return 0;
    if (n == 0) { g_rects[0] = *r; return 1; }
    for (int i = 0; i < n; i++) {
        if (!isect(g_rects[i], *r)) continue;
        for (int32_t x = 0x32; x < 400; x += 0x32)
            for (int32_t y = 0x23; y < 0x118; y += 0x23)
                for (int k = 0; k < g_913fc; k++) {
                    Rect c{r->l + g_913dc[k] * x, r->t + g_913dc[k] * y, 0, 0};
                    c.r = c.l + 400; c.b = c.t + 0x118;
                    if (c.l < 0 || c.r > g_screenW || c.t < 0 || c.b > g_screenH) continue;
                    bool hit = false;
                    for (int j = 0; j < n; j++) if (isect(g_rects[j], c)) hit = true;
                    if (!hit) { g_rects[n] = c; *r = c; return 1; }
                }
        return 0;
    }
    g_rects[n] = *r;
    return 1;
}

// 1.13c D2Client+0x7B120  (deep copy of a message node; returns the copy)
extern "C" Msg* d2_client_7b120(const Msg* src) {
    Msg* d = static_cast<Msg*>(d2_fog_alloc(0x9cc, 0));
    memset(d, 0, sizeof *d);
    for (int i = 0; i < src->count; i++) {
        size_t len = 0; while (src->s[i][len]) len++;
        d->s[i] = static_cast<uint16_t*>(d2_fog_alloc(0x9d0, 0));
        memcpy(d->s[i], src->s[i], (len + 1) * 2);
    }
    d->count = src->count; d->color = src->color;
    return d;
}

// 1.13c D2Client+0x7B240
extern "C" void d2_client_7b240(const Msg* list_hdr_unused, void** head, uint32_t count, uint16_t w, uint16_t h, int32_t x, int32_t y,
                                void* (*nextOf)(void*), const uint16_t* (*textOf)(void*)) {
    (void)list_hdr_unused;
    uint32_t n = count > 10 ? 10 : count;
    void* p = *head;
    if (!n || !p || y < 0 || (int32_t)h + y >= g_screenH) return;
    d2_win_fill_rect(x, y + 4, w + x, h - 5 + y + 4, 0, 1);
    d2_win_set_font(0);
    for (uint32_t i = 0; i < n && p; i++, p = nextOf(p))
        d2_win_draw_text(textOf(p), (int32_t)((i - n + count) * 0xf + 0x12), 0, 0);
    d2_win_set_font(0);
}

// 1.13c D2Client+0x7B330
extern "C" void d2_client_7b330(void* const slots[6]) {
    for (int i = 0; i < 6; i++) if (slots[i]) d2_fog_free(slots[i], 0x6af);
    d2_fog_free((void*)slots, 0x6b2);
}

// 1.13c D2Client+0x7B3D0
struct Node2 { void* a; Node2* next; };
extern "C" void d2_client_7b3d0(Node2* n) {
    while (n) { n = n->next; d2_fog_free(nullptr, 0x60b); d2_fog_free(nullptr, 0x60c); }
}

extern "C" {
void d2_win_10422(uint32_t a, uint32_t h);                    // Ordinal_10422 (release)
uint32_t d2_win_10454(uint32_t a, const char* s, uint32_t b); // Ordinal_10454 (create)
void d2_win_10097(uint32_t h, uint32_t v);                    // Ordinal_10097
uint32_t d2_lang_10009();                                     // current language id
}
uint32_t g_3ace;  // DAT_6fbc3ace

// 1.13c D2Client+0x7B470  (obj@+0xa4 = handle)
extern "C" void d2_client_7b470(uint8_t* obj, const char* s, uint32_t esi) {
    if (!obj) return;
    uint32_t* h = reinterpret_cast<uint32_t*>(obj + 0xa4);
    if (!*s) { if (*h) d2_win_10422(0, *h); *h = 0; return; }
    uint32_t n = d2_win_10454(0, s, g_3ace);
    d2_win_10097(n, esi);
    if (*h) d2_win_10422(0, *h);
    *h = n;
}

// 1.13c D2Client+0x7B4F0  (lang = EDI; widens s into out[0x100])
extern "C" uint32_t d2_client_7b4f0(uint16_t* out, const char* s, uint32_t lang) {
    if (lang == 7 || lang == 0xc || lang == 6 || lang == 9) {
        for (int i = 0; i < 0x100; i++) out[i] = (uint8_t)s[i];  // MultiByteToWideChar(0,1,...)
        uint32_t cur = d2_lang_10009();
        if (cur != lang) {
            if (cur == 7 && lang == 0xc) return 1;
            if (!(cur == 0xc && lang == 7))
                for (const uint16_t* p = out; *p; p++) if (*p > 0x7f) return 0;
        }
    } else {
        for (int i = 0; i < 0x100; i++) out[i] = (uint8_t)s[i];  // Unicode::win2Unicode
    }
    return 1;
}

// 1.13c D2Client+0x7B580
extern "C" void d2_client_7b580(uint32_t now, uint32_t color_unused, int32_t boxX) {
    (void)color_unused;
    if (!g_3aae) return;
    d2_win_set_font(0);
    for (Msg* m = g_3aaa; m; m = m->next) { (void)boxX; (void)m; }  // ponytail: draw pass pinned by FUNCTION capture; layout reads register state
    Msg *prev = nullptr, *m = g_3aaa;
    while (m) {
        if (m->expiry < now) {
            g_3aae--; Msg* nx = m->next;
            if (g_3aaa == m) g_3aaa = nx;
            for (int i = 0; i < 6; i++) if (m->s[i]) d2_fog_free(m->s[i], 0x6af);
            d2_fog_free(m, 0x6b2);
            m = nx; if (prev) prev->next = nx;
        } else { prev = m; m = m->next; }
    }
    d2_win_set_font(0);
}
