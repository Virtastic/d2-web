// D2DDraw CRT locale/codepage helpers (batch n63). Win32 / thread-locale services are host seams.
#include <cstdint>
#include <cstring>

struct d2ddraw_n63_Env {
    const uint8_t* (*ctype)();                       // current locinfo ctype table (uint16 per char)
    int (*mbcurmax)();                               // locinfo mb_cur_max (lc_category[1].refcount)
    uint32_t (*isctype_mt)(int c, int mask);         // 0x5C5E
    int (*tolower_mt)(int c);                        // 0x5B74
    int (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* buf, int n);
    int (*getCPInfoMaxChar)(uint32_t cp);            // MaxCharSize, 0 on failure
    int (*multiByteToWideChar)(uint32_t cp, const char* s, int cb, uint16_t* w, int cw);
    int (*wideCharToMultiByte)(uint32_t cp, const uint16_t* w, int cw, char* s, int cb);
    void* (*calloc)(uint32_t n, uint32_t sz);        // 0x1F05
    void (*free)(void*);                             // 0x1E94
    const uint32_t* tab;                             // DAT_6f8cf784
    const uint32_t* tabNames;                        // DAT_6f8ceb98
};

static uint32_t n63_isctype(const d2ddraw_n63_Env* e, int c, int mask)
{
    if (e->mbcurmax() < 2) return ((const uint16_t*)e->ctype())[(uint8_t)c] & mask;
    return e->isctype_mt(c, mask);
}

// 1.13c D2DDRAW+0x05891
long d2ddraw_atol(const d2ddraw_n63_Env* e, const char* s)
{
    while (n63_isctype(e, (uint8_t)*s, 8)) s++;
    uint32_t c = (uint8_t)*s, d = c;
    const uint8_t* p = (const uint8_t*)s + 1;
    if (c == '-' || c == '+') d = *p++;
    int v = 0;
    while (d - '0' <= 9u) { v = (int)(d - '0') + v * 10; d = *p++; }
    return c == '-' ? -v : v;
}

// 1.13c D2DDRAW+0x05919
long d2ddraw_locale_info_long(const d2ddraw_n63_Env* e, uint32_t lcid)
{
    char b[7];
    b[6] = 0;
    if (!e->getLocaleInfoA(lcid, 0x1004, b, 6)) return -1;
    return d2ddraw_atol(e, b);
}

// 1.13c D2DDRAW+0x05960
int* d2ddraw_convert_string_cp(const d2ddraw_n63_Env* e, uint32_t from, uint32_t to,
                               const char* in, uint32_t* pcb, int* out, int outcb)
{
    uint32_t cb = *pcb;
    int* res = 0;
    if (from == to) return 0;
    bool single = e->getCPInfoMaxChar(from) == 1 && e->getCPInfoMaxChar(to) == 1;
    uint32_t nw = cb;
    if (single && cb == 0xffffffffu) nw = (uint32_t)strlen(in) + 1;
    if (!single) { nw = (uint32_t)e->multiByteToWideChar(from, in, (int)cb, 0, 0); if (!nw) return 0; }
    uint16_t* w = (uint16_t*)e->calloc(2, nw);
    if (!w) return 0;
    if (e->multiByteToWideChar(from, in, (int)cb, w, (int)nw)) {
        if (!out) {
            int n = (int)nw;
            if (single || (n = e->wideCharToMultiByte(to, w, (int)nw, 0, 0)) != 0) {
                nw = (uint32_t)n;
                res = (int*)e->calloc(1, nw);
                if (res) {
                    int r = e->wideCharToMultiByte(to, w, (int)nw, (char*)res, (int)nw);
                    if (!r) { e->free(res); res = 0; }
                    else if (cb != 0xffffffffu) *pcb = (uint32_t)r;
                }
            }
        } else if (e->wideCharToMultiByte(to, w, (int)nw, (char*)out, outcb)) res = out;
    }
    e->free(w);
    return res;
}

// 1.13c D2DDRAW+0x05B3A
int d2ddraw_isdigit(const d2ddraw_n63_Env* e, int c) { return (int)n63_isctype(e, c, 4); }

// 1.13c D2DDRAW+0x05C3C
int d2ddraw_tolower(const d2ddraw_n63_Env* e, int c) { return e->tolower_mt(c); }

// 1.13c D2DDRAW+0x06A10  (idx arrives in EAX; returns table entry, *name = name-table entry)
uint32_t d2ddraw_tab_lookup(const d2ddraw_n63_Env* e, int idx, uint32_t* name)
{
    *name = e->tabNames[idx];
    switch (idx) {
    case 0: return e->tab[2];
    case 1: return e->tab[1];
    case 2: return e->tab[0];
    case 3: return e->tab[0x41];
    case 4: return e->tab[0x42];
    case 6: return e->tab[0x43];
    default: return 0;
    }
}
