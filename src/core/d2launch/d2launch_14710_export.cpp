// The play path asks the 1.13c character-name rule (D2Launch+0x14710 with its dash/underscore counter at +0x12340)
// whether the typed name enables the create button. The rule's own core runs; only the three D2Win/Unicode seams
// it takes are bound to a plain UTF-16 buffer.
#ifdef __EMSCRIPTEN__
#include <cstdint>
#include <emscripten.h>
struct D2l14710Seams {
    const void* (*text10166)(uint32_t ecx);
    int (*ustrlen)(const void* wide);
    void (*toUtf)(char* dst, const void* wide, uint32_t n);
    int (*f12340)(const char* utf);
    void (*apply10147)(uint32_t ecx, uint32_t ok);
};
extern "C" void d2l_14710_core(uint32_t g253b8, uint32_t g25298, const D2l14710Seams* s);
int32_t d2launch_count_dash_underscore(const char* buf, int32_t len);
namespace {
const uint16_t* g_name;
uint32_t g_ok;
const void* seam_text(uint32_t) { return g_name; }
int seam_len(const void* w) { int n = 0; const uint16_t* p = (const uint16_t*)w; while (p[n]) n++; return n; }
void seam_utf(char* d, const void* w, uint32_t n) {
    const uint16_t* p = (const uint16_t*)w; uint32_t i = 0;
    for (; i + 1 < n && p[i]; i++) d[i] = (char)p[i];
    d[i] = 0;
}
int seam_count(const char* s) { int n = 0; while (s[n]) n++; return d2launch_count_dash_underscore(s, n); }
void seam_apply(uint32_t, uint32_t ok) { g_ok = ok; }
}
// `name` is a null-terminated UTF-16 string; returns 1 when retail enables the create button for it.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_launch_charname_ok(const uint16_t* name) {
    static const D2l14710Seams S = { seam_text, seam_len, seam_utf, seam_count, seam_apply };
    g_name = name; g_ok = 0;
    d2l_14710_core(0, 0, &S);
    return (int32_t)g_ok;
}
#endif
