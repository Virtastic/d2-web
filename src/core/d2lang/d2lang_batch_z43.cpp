// D2Lang batch z43: key-intern list, char-set tests, ctype wrappers, strtok, UTF-8 codec, xtoa, doexit.
// Globals/CRT/allocator become explicit arguments or host seams.
#include <cstdint>
#include <cstring>

namespace {
struct z43_Node { uint32_t val; uint32_t key; uint32_t next; };   // 32-bit links as in retail
struct z43_Utf8 { uint16_t cmask, cval, shift, pad; int32_t lmask, lmin; };   // DAT_6fc102e8 rows, 16 bytes
}

// 1.13c D2Lang+0x9FB0 -- find (or intern) the converted string for key; list head is *head (handle).
// seams: res(h) handle->node, alloc(size,tag) handle, toUnicode(key) -> handle of the u16 string.
extern "C++" uint32_t z43_9fb0(uint32_t* head, const char* key, void* (*res)(uint32_t), uint32_t (*alloc)(uint32_t),
                               uint32_t (*toUnicode)(const char*)) {
    for (uint32_t h = *head; h; h = ((z43_Node*)res(h))->next) {
        z43_Node* n = (z43_Node*)res(h);
        if (!strcmp((const char*)res(n->key), key)) return n->val;
    }
    uint32_t h = alloc(sizeof(z43_Node));
    z43_Node* n = (z43_Node*)res(h);
    n->next = *head; *head = h;
    n->key = alloc((uint32_t)strlen(key) + 1);
    strcpy((char*)res(n->key), key);
    n->val = toUnicode(key);   // retail: "(not xlated) call ken" suffix appended, widened via Unicode::toUnicode
    return n->val;
}

static int z43_set(const char* s, uint16_t* b) {   // widen up to 20 bytes; 20 long truncates to 19 (retail quirk)
    int n = 0;
    while (n < 20 && s[n]) { b[n] = (uint8_t)s[n]; n++; }
    return n == 20 ? 19 : n;
}
// 1.13c D2Lang+0x8420 -- is *ch one of the chars in byte set s
extern "C++" uint32_t z43_8420(const char* s, const uint16_t* ch) {
    uint16_t b[20] = {0}; int n = z43_set(s, b);
    for (int i = 0; i < n; i++) if (*ch == b[i]) return 1;
    return 0;
}
// ctype wrappers: seams give mb_cur_max>1 flag, the __isctype_mt slow path, and the 16-bit class table.
struct z43_Ctype { bool multi; int (*isctype)(int c, int mask); const uint16_t* tbl; };
static int z43_is(const z43_Ctype& t, int c, int m) { return t.multi ? t.isctype(c, m) : (t.tbl[c] & m); }
// 1.13c D2Lang+0x1301 _isalpha
extern "C++" int z43_1301(const z43_Ctype& t, int c) { return z43_is(t, c, 0x103); }
// 1.13c D2Lang+0x1340 _isspace
extern "C++" int z43_1340(const z43_Ctype& t, int c) { return z43_is(t, c, 8); }
// 1.13c D2Lang+0x137A _isalnum
extern "C++" int z43_137a(const z43_Ctype& t, int c) { return z43_is(t, c, 0x107); }

// 1.13c D2Lang+0x16E4 -- strtok; *tok is the per-thread _token (ptd seam)
extern "C++" uint32_t z43_16e4(uint8_t** tok, uint8_t* s, const uint8_t* delim) {
    uint8_t m[32] = {0}; uint8_t c;
    do { c = *delim++; m[c >> 3] |= 1 << (c & 7); } while (c);
    if (!s) s = *tok;
    while ((m[*s >> 3] & (1 << (*s & 7))) && *s) s++;
    uint8_t* p = s;
    for (;; p++) {
        if (!*p) break;
        if (m[*p >> 3] & (1 << (*p & 7))) { *p++ = 0; break; }
    }
    *tok = p;
    return p == s ? 0 : (uint32_t)(uintptr_t)s;   // retail returns the 32-bit pointer or 0
}

// 1.13c D2Lang+0x8DC0 -- encode ch to dst; returns bytes written or -1
extern "C++" int z43_8dc0(const z43_Utf8* tab, uint8_t* dst, uint16_t ch) {
    int i = 0;
    const z43_Utf8* e = tab;
    for (;; e++) {
        if (!e->cmask) return -1;
        i++;
        if ((int)ch <= e->lmask) break;
    }
    int sh = e->shift;
    *dst = (uint8_t)((int)ch >> (sh & 31)) | (uint8_t)e->cval;
    while (sh > 0) { sh -= 6; *++dst = ((uint8_t)((int)ch >> (sh & 31)) & 0x3f) | 0x80; }
    return i;
}

// 1.13c D2Lang+0x8BB0 -- validate one UTF-8 char in src (n bytes avail); returns length or -1
extern "C++" int z43_8bb0(const z43_Utf8* tab, const uint8_t* src, int n) {
    if (n < 1 || !tab->cmask) return -1;
    uint8_t b0 = src[0]; uint32_t v = b0; int i = 1;
    const z43_Utf8* e = tab;
    while (((int)(int16_t)e->cmask & b0) != (int)(int16_t)e->cval) {
        if (n <= i) return -1;
        uint8_t c = src[i] ^ 0x80;
        if (c & 0xc0) return -1;
        e++; v = v << 6 | c; i++;
        if (!e->cmask) return -1;
    }
    return e->lmin <= (int32_t)((uint32_t)e->lmask & v) ? i : -1;
}

// 1.13c D2Lang+0x147D _doexit -- seams: lock(8), terminate, onexit table walk, initterm pair, final fn, ExitProcess.
struct z43_ExitSeam {
    void (*lock)(); void (*terminate)(uint32_t); void (*runOnexit)(); void (*initterm0)(); void (*initterm1)();
    void (*final)(); void (*exitProcess)(uint32_t);
};
extern "C++" void z43_147d(z43_ExitSeam& s, uint32_t* exiting, uint8_t* quick, uint32_t* done, uint32_t code,
                           int param2, int param3) {
    s.lock();
    if (*done == 1) s.terminate(code);
    *exiting = 1; *quick = (uint8_t)param3;
    if (param2 == 0) { s.runOnexit(); s.initterm0(); }
    s.initterm1(); s.final();
    if (param3) return;
    *done = 1; s.exitProcess(code);
}
