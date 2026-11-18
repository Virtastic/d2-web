// 1.13c D2Launch+0x12770: intro-video setup. Every callee is a seam so a replay probe pins it.
#include <cstdint>
struct D2l12770Seams {
    void (*f11cb0)();                                       // 0x11CB0
    int32_t (*o10191)();                                    // Ordinal_10191 (Fog), result selects the resolution pair
    void (*o10004)(char* buf, int32_t z);                   // Ordinal_10004(buf96, 0)
    void (*spf)(char* buf, const char* fmt, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);  // sprintf (cdecl; unused args 0)
    int32_t (*o268)(int32_t z, const char* path, int32_t z2, uint32_t* out);   // Ordinal_268 (file exists, out handle)
    void (*o253)(uint32_t handle);                          // Ordinal_253
    void (*o10027)(const char* path, int32_t flag, int32_t z);   // Ordinal_10027
    int32_t (*o422)(const char* key, const char* val, int32_t t, char* buf, int32_t cap);   // Storm 422 registry read
    int32_t (*scan)(const char* buf, const char* fmt, int32_t* q);   // sscanf "%d.%d.%d.%d"
    void (*o425)(const char* key, const char* val, int32_t t, const char* buf);   // Storm 425 registry write
};
// img1e1e8 / img1e1f0: the 8-byte strings at image RVA 0x1E1E8 ("640x146") and 0x1E1F0 ("640x292"); lits are the image literals (0x1E19C 0x1E1B8 0x1E1BC 0x1E1DC).
extern "C" void d2l_12770(const D2l12770Seams* s, const char* img1e1e8, const char* img1e1f0, const char* fmtNoDash, const char* lit1b8,
                          const char* fmtDash, const char* dataLocal, const char* fmtVer) {
    char buf96[96], path[268], reg[100];
    int32_t q[4] = { 0, 0, 0, 0 };
    s->f11cb0();
    int32_t flag = 0;
    const char* name;
    if (s->o10191() == 0) name = img1e1f0;
    else { flag = 2; name = img1e1e8; }
    s->o10004(buf96, 0);
    s->spf(path, fmtDash, (uintptr_t)dataLocal, (uintptr_t)buf96, (uintptr_t)name, 0);
    uint32_t h = 0;
    if (s->o268(0, path, 0, &h) == 0) { s->spf(path, fmtNoDash, (uintptr_t)dataLocal, (uintptr_t)lit1b8, (uintptr_t)name, 0); }
    else s->o253(h);
    s->o10027(path, flag, 0);
    if (s->o422("Diablo II", "Aux Battle.net", 0, reg, 100)) s->scan(reg, fmtVer, q);
    q[3] |= 0x80;
    s->spf(reg, fmtVer, 0xd8, 0x94, 0xf6, (uintptr_t)(uint32_t)q[3]);
    s->o425("Diablo II", "Aux Battle.net", 0, reg);
}
