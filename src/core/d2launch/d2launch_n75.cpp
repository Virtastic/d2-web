// src/core/d2launch/d2launch_n75.cpp -- UNMAPPED 1.13c D2Launch.dll rows (batch n75).
// Host seams only: ordinals (D2Win/D2Gfx/Fog/Storm) go through H::ord, internal callees through H::fn,
// DAT_6fa6xxxx globals through H::d32 / H::ptr, retail data tables through H::dat.
// Ordinal args the decompile did not recover are passed as 0 (host decides).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace n75 {
struct H {
    intptr_t (*ord)(int n, intptr_t a0, intptr_t a1, intptr_t a2, intptr_t a3, intptr_t a4, intptr_t a5);
    intptr_t (*fn)(uint32_t rva, intptr_t a0, intptr_t a1, intptr_t a2, intptr_t a3);
    uint32_t& (*d32)(uint32_t va);          // 32-bit DAT_ slot
    void*& (*ptr)(uint32_t va);             // pointer-width DAT_ slot
    void* (*dat)(uint32_t va);              // address of a retail data block
    uint32_t (*bitmask)(int inv, int idx);  // gdwBitMasks / gdwInvBitMasks
    uint32_t (*time32)();
    void (*win2uni)(char16_t* dst, const char* src, int n);
    void (*uni2win)(char* dst, const char16_t* src, int n);
    void (*exit_)(int);                     // _exit
};
template <class... A> static intptr_t O(H* h, int n, A... a) {
    intptr_t v[6] = {0, 0, 0, 0, 0, 0}; int i = 0; ((v[i++] = (intptr_t)a), ...);
    return h->ord(n, v[0], v[1], v[2], v[3], v[4], v[5]);
}
template <class... A> static intptr_t F(H* h, uint32_t rva, A... a) {
    intptr_t v[4] = {0, 0, 0, 0}; int i = 0; ((v[i++] = (intptr_t)a), ...);
    return h->fn(rva, v[0], v[1], v[2], v[3]);
}
#define D(x) (h->d32(0x6fa00000u | (x)))
#define P(x) (h->ptr(0x6fa00000u | (x)))
[[noreturn]] static void fatal(H* h, int line) { O(h, 10024, "", O(h, 10265, line)); h->exit_(-1); for (;;) {} }

// character-list node: retail byte offsets; the retail 32-bit next link at +0x34c is a native pointer at +0x350 here
static const int NODE_SZ = 0x358;
static inline uint8_t* nxt(uint8_t* n) { uint8_t* p; memcpy(&p, n + 0x350, sizeof p); return p; }
static inline void setnxt(uint8_t* n, uint8_t* p) { memcpy(n + 0x350, &p, sizeof p); }
static inline uint16_t u16(const uint8_t* n, int o) { uint16_t v; memcpy(&v, n + o, 2); return v; }
static inline uint32_t u32(const uint8_t* n, int o) { uint32_t v; memcpy(&v, n + o, 4); return v; }
static inline void su32(uint8_t* n, int o, uint32_t v) { memcpy(n + o, &v, 4); }
static inline void su16(uint8_t* n, int o, uint16_t v) { memcpy(n + o, &v, 2); }
static void ucat(char16_t* d, const char16_t* s) { while (*d) d++; while ((*d++ = *s++)) {} }
static void ucpy(char16_t* d, const char16_t* s) { while ((*d++ = *s++)) {} }
static int ulen(const char16_t* s) { int n = 0; while (s[n]) n++; return n; }
// pseudo-RVAs for seams that are not D2Launch code: (*DAT_656c4 + off)() is 0xFFFF0000|off
static const uint32_t VT = 0xFFFF0000u, SEAM_GETFILESIZE = 0xFFFF1000u, SEAM_ISLINEBREAK = 0xFFFF2000u;

// 1.13c D2LAUNCH+0x058BF
void unlock_fhandle_58bf(H* h, int fh) { F(h, 0x6B05, fh); }
// 1.13c D2LAUNCH+0x07866
void unlock_fhandle_7866(H* h, int fh) { F(h, 0x6B05, fh); }

// 1.13c D2LAUNCH+0x058E3  __getbuf (retail FILE layout)
struct RFile { char* _ptr; int _cnt; char* _base; int _flag; int _file; int _charbuf; int _bufsiz; char* _tmpfname; };
void getbuf_58e3(H* h, RFile* f) {
    D(0x64f90)++;
    char* b = (char*)malloc(0x1000);
    f->_base = b;
    if (!b) { f->_flag |= 4; f->_base = (char*)&f->_charbuf; f->_bufsiz = 2; }
    else { f->_flag |= 8; f->_bufsiz = 0x1000; }
    f->_cnt = 0;
    f->_ptr = f->_base;
}

// 1.13c D2LAUNCH+0x12020  qsort-style compare of two char** slots by C string
int strcmp_slots_12020(void* const* a, void* const* b) {
    const uint8_t* p = *(const uint8_t* const*)*a;
    const uint8_t* q = *(const uint8_t* const*)*b;
    for (;; p++, q++) {
        if (*p != *q) return *p < *q ? -1 : 1;
        if (!*p) return 0;
    }
}

// 1.13c D2LAUNCH+0x12140  put v in the first free slot of the table at DAT_65e74 (count DAT_65e78)
int slot_add_12140(H* h, void* v) {
    void** tbl = (void**)P(0x65e74);
    if (!tbl) fatal(h, 0x1106);
    for (int i = 0; i < (int)D(0x65e78); i++)
        if (!tbl[i]) { tbl[i] = v; return 1; }
    return 0;
}

// 1.13c D2LAUNCH+0x12C80  release every frontend image handle (sub_9874 is a __fastcall(ecx=handle) thunk of Ordinal_10126)
void free_images_12c80(H* h) {
    static const uint32_t a[] = {0x65be4, 0x65be8, 0x65bec, 0x65c08, 0x65bf0, 0x65bf4, 0x65bf8, 0x65bfc, 0x65c00, 0x65c04,
                                 0x65c0c, 0x65c24, 0x65c40, 0x65c44, 0x65c48};
    static const uint32_t x[] = {0x65c18, 0x65c1c, 0x65c20, 0x65c60};
    static const uint32_t b[] = {0x65c28, 0x65c2c, 0x65c34, 0x65c38, 0x65c50, 0x65c54, 0x65c58, 0x65c5c, 0x65c64, 0x65c68,
                                 0x65c84, 0x65c88, 0x65c8c, 0x65c90, 0x65c94, 0x65c98, 0x65c6c, 0x65c70, 0x65c9c, 0x65ca0,
                                 0x65c74, 0x65c78, 0x65c7c, 0x65ca4, 0x65c80};
    static const uint32_t t[] = {0x65cb4, 0x65cb8, 0x65cbc, 0x65cc0, 0x65cc4, 0x65cc8, 0x65c3c};
    for (uint32_t v : a) F(h, 0x9874, D(v));
    if (O(h, 10227)) for (uint32_t v : x) F(h, 0x9874, D(v));
    for (uint32_t v : b) F(h, 0x9874, D(v));
    if (D(0x65c10)) {
        F(h, 0x9874, D(0x65c10)); D(0x65c10) = 0;
        F(h, 0x9874, D(0x65c14)); F(h, 0x9874, D(0x65c30)); F(h, 0x9874, D(0x65ca8)); F(h, 0x9874, D(0x65cac));
        static const uint32_t r[][2] = {{0x65cf4, 0x65d08}, {0x65d08, 0x65d1c}, {0x65d1c, 0x65d30}, {0x65d30, 0x65d44},
                                        {0x65d44, 0x65d58}, {0x65d88, 0x65d94}, {0x65d74, 0x65d80}};
        for (auto& q : r) for (uint32_t v = q[0]; v < q[1]; v += 4) F(h, 0x9874, D(v));
        F(h, 0x9874, D(0x65d9c)); F(h, 0x9874, D(0x65db0));
        if (O(h, 10227)) {
            for (uint32_t v = 0x65ccc; v < 0x65ce0; v += 4) F(h, 0x9874, D(v));
            for (uint32_t v = 0x65ce0; v < 0x65cf4; v += 4) F(h, 0x9874, D(v));
            F(h, 0x9874, D(0x65cb0));
        }
    }
    for (uint32_t v : t) F(h, 0x9874, D(v));
}

static void ld(H* h, uint32_t va, const char* sfx) {  // wsprintfA("%s\\<sfx>","DATA\\GLOBAL") + Ordinal_10111
    char buf[260]; snprintf(buf, sizeof buf, "%s\\%s", "DATA\\GLOBAL", sfx);
    D(va) = (uint32_t)O(h, 10111, buf);
}
#define LD(va, s) ld(h, 0x##va, s)

// 1.13c D2LAUNCH+0x11170  character-create frontend images
void load_charcreate_11170(H* h) {
    if (D(0x65c10)) return;
    LD(65c30, "ui\\FrontEnd\\MediumSelButtonBlank");
    LD(65c10, O(h, 10227) ? "ui\\FrontEnd\\charactercreationscreenEXP" : "ui\\FrontEnd\\CharacterCreate");
    LD(65c14, "ui\\FrontEnd\\fire");
    LD(65ca8, "ui\\FrontEnd\\textbox");
    LD(65cac, "ui\\FrontEnd\\clickbox");
    if (O(h, 10227)) {
        LD(65cb0, "ui\\FrontEnd\\joingameclickboxgrey");
        static const struct { uint32_t va; const char* n; } e[] = {
            {0x65ccc, "assassin\\asnu1"}, {0x65cd0, "assassin\\asnu2"}, {0x65cd4, "assassin\\asfw"}, {0x65cd8, "assassin\\asnu3"},
            {0x65cdc, "assassin\\asbw"}, {0x65ce0, "druid\\dznu1"}, {0x65ce4, "druid\\dznu2"}, {0x65ce8, "druid\\dzfw"},
            {0x65cec, "druid\\dznu3"}, {0x65cf0, "druid\\dzbw"}};
        for (auto& q : e) { char b[64]; snprintf(b, sizeof b, "ui\\FrontEnd\\%s", q.n); ld(h, q.va, b); }
    }
    static const struct { uint32_t va; const char* n; } c[] = {
        {0x65cf4, "amazon\\amnu1"}, {0x65cf8, "amazon\\amnu2"}, {0x65cfc, "amazon\\amfw"}, {0x65d00, "amazon\\amnu3"}, {0x65d04, "amazon\\ambw"},
        {0x65d08, "necromancer\\nenu1"}, {0x65d0c, "necromancer\\nenu2"}, {0x65d10, "necromancer\\nefw"}, {0x65d14, "necromancer\\nenu3"}, {0x65d18, "necromancer\\nebw"},
        {0x65d1c, "barbarian\\banu1"}, {0x65d20, "barbarian\\banu2"}, {0x65d24, "barbarian\\bafw"}, {0x65d28, "barbarian\\banu3"}, {0x65d2c, "barbarian\\babw"},
        {0x65d30, "sorceress\\sonu1"}, {0x65d34, "sorceress\\sonu2"}, {0x65d38, "sorceress\\sofw"}, {0x65d3c, "sorceress\\sonu3"}, {0x65d40, "sorceress\\sobw"},
        {0x65d44, "paladin\\panu1"}, {0x65d48, "paladin\\panu2"}, {0x65d4c, "paladin\\pafw"}, {0x65d50, "paladin\\panu3"}, {0x65d54, "paladin\\pabw"},
        {0x65d88, "sorceress\\sofws"}, {0x65d8c, "sorceress\\sonu3s"}, {0x65d90, "sorceress\\sobws"},
        {0x65d74, "necromancer\\nefws"}, {0x65d78, "necromancer\\nenu3s"}, {0x65d7c, "necromancer\\nebws"},
        {0x65d9c, "barbarian\\bafws"}, {0x65db0, "paladin\\pafws"}};
    for (auto& q : c) { char b[64]; snprintf(b, sizeof b, "ui\\FrontEnd\\%s", q.n); ld(h, q.va, b); }
}

// 1.13c D2LAUNCH+0x13010  common frontend images
void load_frontend_13010(H* h) {
    bool x = O(h, 10227) != 0;
    LD(65be4, "UI\\FrontEnd\\blizno");
    LD(65be8, x ? "UI\\FrontEnd\\trademarkscreenEXP" : "UI\\FrontEnd\\trademark");
    LD(65c08, "UI\\CURSOR\\ohand");
    LD(65bf0, "ui\\FrontEnd\\Diablo2");
    LD(65bf4, "ui\\FrontEnd\\D2logoBlackLeft");
    LD(65bf8, "ui\\FrontEnd\\D2logoFireLeft");
    LD(65bfc, "ui\\FrontEnd\\D2logoBlackRight");
    LD(65c00, "ui\\FrontEnd\\D2logoFireRight");
    LD(65c04, x ? "ui\\FrontEnd\\gameselectscreenEXP" : "ui\\FrontEnd\\TitleScreen");
    LD(65c0c, x ? "ui\\CharSelect\\creditsbckgexpand" : "ui\\CharSelect\\creditsbckg");
    LD(65c24, "ui\\FrontEnd\\WideButtonBlank");
    LD(65c40, "ui\\FrontEnd\\WideButtonBlank02");
    LD(65c44, "ui\\FrontEnd\\NarrowButtonBlank");
    LD(65c48, "ui\\FrontEnd\\GWListBox");
    if (x) {
        LD(65c18, "ui\\CharSelect\\Realmselectbutton");
        LD(65c1c, "ui\\CharSelect\\TallButtonBlank");
        LD(65c20, "ui\\CharSelect\\characterselectscreenEXP");
        LD(65c60, "ui\\FrontEnd\\CinematicsSelectionEXP");
    }
    LD(65c28, "ui\\FrontEnd\\MediumButtonBlank");
    LD(65c2c, "ui\\CharSelect\\ShortButtonBlank");
    LD(65c34, "ui\\FrontEnd\\textbox2");
    LD(65c38, "ui\\FrontEnd\\3WideButtonBlank");
    LD(65c50, "ui\\FrontEnd\\TermsofUseBckg");
    LD(65c54, "ui\\CharSelect\\charselectbckg");
    LD(65c58, "ui\\CharSelect\\DifficultyLevels");
    LD(65c5c, "ui\\FrontEnd\\CinematicsSelection");
    LD(65c64, "ui\\CharSelect\\charselectbox");
    LD(65c68, "ui\\CharSelect\\charselectboxgrey");
    LD(65c80, "ui\\CharSelect\\CreateNewCharImage");
    LD(65c84, "ui\\FrontEnd\\PopUpOK");
    LD(65c88, "ui\\FrontEnd\\PopUp_340x224");
    LD(65c8c, "ui\\FrontEnd\\PopUpWide");
    LD(65c90, "ui\\FrontEnd\\PopUpLargest");
    LD(65c94, "ui\\FrontEnd\\TileableDialog");
    LD(65c98, "ui\\FrontEnd\\PopUpOK2");
    LD(65c6c, "ui\\FrontEnd\\PopUpOKCancel");
    LD(65c70, "ui\\FrontEnd\\PopUpOKCancel2");
    LD(65c9c, "ui\\FrontEnd\\OkCancelButtonBlank");
    LD(65ca0, "ui\\FrontEnd\\CancelButtonBlank");
    LD(65c74, "ui\\CharSelect\\selectchararrows");
    LD(65c78, x ? "ui\\CharSelect\\Realmselect" : "ui\\FrontEnd\\CurRealm");
    LD(65c7c, x ? "ui\\CharSelect\\RealmSelectbuttonthin" : "ui\\FrontEnd\\NarrowButtonBlank02");
    LD(65ca4, "ui\\CharSelect\\password");
    LD(65cb4, "UI\\FrontEnd\\realmbckg");
    LD(65cb8, "UI\\FRONTEND\\joingamescrollbars");
    LD(65cbc, x ? "UI\\FrontEnd\\tcpipscreen" : "UI\\FrontEnd\\tcpipbckg");
    LD(65cc0, "UI\\FrontEnd\\IPAddressBox");
    LD(65cc4, "UI\\FrontEnd\\downloadpatchbckg");
    LD(65cc8, "UI\\FrontEnd\\downloadpatchball2");
    LD(65c3c, "UI\\FrontEnd\\EditTextBoxWide");
}

// control-record pool DAT_652b8[0x40] / count DAT_65e30: Ordinal_10098(rec), fatal(0x624) when full
static uint32_t pool_add(H* h, intptr_t rec) {
    if (D(0x65e30) >= 0x40) fatal(h, 0x624);
    uint32_t v = (uint32_t)O(h, 10098, rec);
    h->d32(0x6fa652b8u + D(0x65e30) * 4) = v;
    D(0x65e30)++;
    return v;
}
#define REC(rva) ((intptr_t)h->dat(0x6fa00000u | (rva)))
// 1.13c D2LAUNCH+0x12B10 shape: pool_add(&DAT_60f20[idx*0x30]); returns the handle
static uint32_t pool_idx(H* h, int idx) { return pool_add(h, (intptr_t)((uint8_t*)h->dat(0x6fa60f20u) + idx * 0x30)); }

// 1.13c D2LAUNCH+0x15090  two-button popup (p3 selects the pair); pool DAT_653c0 / count DAT_65e34
void popup_15090(H* h, intptr_t text, intptr_t cb, int p3) {
    (void)text; (void)cb;
    for (int i = 0; i < (int)D(0x65e34); i++) O(h, 10050, h->dat(0x6fa653c0u + i * 4));
    D(0x65e34) = 0;
    auto add = [&](intptr_t rec) { uint32_t v = (uint32_t)O(h, 10098, rec); h->d32(0x6fa653c0u + D(0x65e34) * 4) = v; D(0x65e34)++; return v; };
    uint8_t* tbl = (uint8_t*)h->dat(0x6fa60f20u);
    bool alt = D(0x65ad8) != 0;
    uint32_t a = add((intptr_t)h->dat(alt ? 0x6fa63d70u : 0x6fa638c0u));
    O(h, 10196, a, 1);
    if (D(0x65e34) < 0x40) {
        uint32_t b = add((intptr_t)(tbl + ((alt ? (p3 == 0) * 2 + 0xf8 : (p3 != 0) * 2 + 0xdf) * 0x30)));
        if (D(0x65e34) < 0x40) {
            uint32_t va = alt ? 0x6fa63dd0u : 0x6fa63920u;
            uint32_t c = add((intptr_t)h->dat(va));
            O(h, 10001, a); O(h, 10001, b, 1); O(h, 10001, c, 1);
            F(h, 0x12A70, c, 1, 0);
            P(0x6552c) = h->dat(va);  // _DAT_6fa6552c
            return;
        }
    }
    fatal(h, 0x60d);
}

// 1.13c D2LAUNCH+0x0C720  add a character record from a realm-server packet (name in EAX, ctx/sort keys on the stack)
int char_add_c720(H* h, uint32_t expire, const uint8_t* pkt, uint32_t sortA, uint32_t sortB, const char* name, intptr_t ctx) {
    uint8_t* n = (uint8_t*)O(h, 10042, NODE_SZ, 0x4d3, 0);
    memset(n, 0, NODE_SZ);
    su32(n, 0x340, (uint32_t)(uintptr_t)pkt);  // retail stores param_2 here
    su32(n, 0x344, sortB);
    su32(n, 0x348, expire);
    O(h, 501, n, ctx, 0x7fffffff);  // Storm strcpy of the account name
    char16_t* wn = (char16_t*)(n + 0x100);
    if (!name) wn[0] = 0; else h->win2uni(wn, name, 0xff);
    su16(n, 0x326, (uint16_t)O(h, 10085, pkt));
    if (F(h, 0x9F40)) {
        char tmp[0x100]; h->uni2win(tmp, wn, 0xff);
        F(h, 0xF3A0, n);
        return 0;
    }
    const uint8_t* p = pkt + 2;
    int i;
    for (i = 0; i < 0xb; i++) n[0x300 + i] = *p++;
    for (; i < 0x10; i++) n[0x300 + i] = 0xff;
    n[0x320] = (uint8_t)(*p++ - 1);
    for (i = 0; i < 0xb; i++) n[0x310 + i] = *p++;
    for (; i < 0x10; i++) n[0x310 + i] = 0xff;
    setnxt(n, nullptr);
    su16(n, 0x322, p[0]);
    su16(n, 0x324, (uint16_t)O(h, 10085, p + 1));
    su16(n, 0x328, (uint16_t)O(h, 10085, p + 3));
    for (int k = 0; k < 3; k++) { uint8_t v = p[5 + k]; n[0x32e + k] = v == 0xff ? 0 : v; }
    strncpy((char*)n + 0x32a, (const char*)p + 8, 3);
    uint8_t cls = n[0x320]; uint32_t unk = 5;
    uint16_t fl = u16(n, 0x324);
    if (fl & 4) {
        if (!(fl & 8)) { if (O(h, 10227) || n[0x320] < 5) unk = 1; }
        else { switch (cls) { case 0: case 1: case 6: case 9: cls = 8; break; default: cls = 9; } unk = 5; }
    }
    uint32_t hd = (uint32_t)O(h, 10154, cls, unk, n + 0x300, n + 0x310);
    if (!hd) {
        for (i = 0; i < 16; i++) { n[0x300 + i] = 0; n[0x310 + i] = 0xff; }
        n[0x301] = 1;
        hd = (uint32_t)O(h, 10154, 7, 5, n + 0x300, n + 0x310);
    }
    su32(n, 0x338, hd);
    if (!hd) fatal(h, 0x4d3);
    su32(n, 0x334, 0);
    O(h, 10080, hd, 0); O(h, 10172, hd, 0); O(h, 10093);
    uint8_t* head = (uint8_t*)P(0x65ec8);
    if (head) {
        if (!O(h, 10227)) {
            uint8_t* t = head; while (nxt(t)) t = nxt(t);
            setnxt(t, n); n = head;
        } else {  // expansion: insert sorted by (key2 @+0x344, key1 @+0x340) against the stack keys
            uint8_t* prev = nullptr; uint8_t* c = head;
            while (sortB <= u32(c, 0x344)) {
                if (sortB == u32(c, 0x344) && u32(c, 0x340) < sortA) break;
                prev = c; c = nxt(c); if (!c) break;
            }
            if (prev) { setnxt(n, nxt(prev)); setnxt(prev, n); n = head; }
            else setnxt(n, head);
        }
    }
    P(0x65ec8) = n;
    D(0x65ed0)++;
    F(h, 0xB600);
    D(0x65eb0) = 1;
    return 1;
}

static void level_line(H* h, const uint8_t* n, char16_t* line) {  // " %d " level text appended to the O(10003) caption
    char lv[10]; snprintf(lv, sizeof lv, " %d ", (unsigned)u16(n, 0x322));
    char16_t wl[10] = {0}; h->win2uni(wl, lv, 9);
    ucpy(line, (const char16_t*)O(h, 10003));
    ucat(line, wl);  // ponytail: retail also appends an uninitialised stack tail (aUStack_124/1f4); empty here
}

// 1.13c D2LAUNCH+0x0A8C0  refresh the selected-character panel
int sel_panel_a8c0(H* h) {
    if (O(h, 10227)) { F(h, 0x9BF0); return 1; }
    if (!D(0x65ed0)) D(0x64db0) = 0;
    int d = (int)D(0x64db0) - (int)D(0x65ed4);
    O(h, 10147); O(h, 10147);
    if (d >= 0 && d < (int)D(0x64dbc)) {
        if (D(0x64db0) == D(0x64db4) && D(0x65ee4)) { O(h, 10182); O(h, 10131); goto L; }
        O(h, 10182);
    }
    O(h, 10131);
L:
    O(h, 10131); O(h, 10131); O(h, 10131);
    uint8_t* n = (uint8_t*)P(0x65ec8);
    if (!n) return 1;
    for (uint32_t i = 0; D(0x64db0) != i; i++) { n = nxt(n); if (!n) return 1; }
    uint8_t* cur = (uint8_t*)P(0x65ecc);
    if (cur) {
        if (u32(cur, 0x334) == 0) O(h, 10054, u32(cur, 0x338), 1);
        O(h, 10043, cur, 0x5f3, 0);
        P(0x65ecc) = nullptr;
    }
    cur = (uint8_t*)O(h, 10042, NODE_SZ, 0x5f7, 0);
    P(0x65ecc) = cur;
    memcpy(cur, n, 0x34c);
    setnxt(cur, nullptr);
    if (u32(n, 0x334) == 1 || u32(n, 0x334) == 2) {
        O(h, 10106);
    } else {
        uint8_t cls = cur[0x320]; uint32_t unk = 5; uint16_t fl = u16(cur, 0x324);
        if (fl & 4) {
            if (!(fl & 8)) { if (O(h, 10227) || cur[0x320] < 5) unk = 1; }
            else cls = (F(h, 0x9E10) != 0) + 8;
        }
        uint32_t hd = (uint32_t)O(h, 10154, cls, unk, cur + 0x300, cur + 0x310);
        su32(cur, 0x338, hd);
        if (!hd) {
            for (int i = 0; i < 16; i++) { cur[0x300 + i] = 0; cur[0x310 + i] = 0xff; }
            cur[0x301] = 1;
            hd = (uint32_t)O(h, 10154, 7, 5, cur + 0x300, cur + 0x310);
            su32(cur, 0x338, hd);
        }
        if (!hd) fatal(h, 0x62e);
        O(h, 10080, hd, 0);
        O(h, 10172, hd, 0);
        O(h, 10093);
        O(h, 10140, hd, 0x118);
        O(h, 10179, hd, 0x172);
        fl = u16(cur, 0x324);
        uint32_t m = (fl & 8) ? (h->bitmask(0, 4) | 5) : (h->bitmask(1, 4) & 5);
        m = (fl & 4) ? (m | h->bitmask(0, 5)) : (m & h->bitmask(1, 5));
        m = (fl & 0x20) ? (m | h->bitmask(0, 12)) : (m & h->bitmask(1, 12));
        O(h, 10194, hd, m);
        O(h, 10106);
        char title[0x100];
        snprintf(title, sizeof title, "%s", (char*)cur);
        if (((char*)cur)[0x32a]) snprintf(title, sizeof title, "%s {%s}", (char*)cur, (char*)cur + 0x32a);
        O(h, 10022, 0, title);
        O(h, 10106);
        F(h, 0x9E40);
        char16_t line[200] = {0};
        level_line(h, n, line);
        O(h, 10000, line, 0, 0);
        O(h, 10106);
        if (u16(cur, 0x100) == 0) O(h, 10003, 0, 0, 0);
        O(h, 10000);
        if (!(n[0x324] & 0x20)) return 1;
        O(h, 10147);
        return 1;
    }
    O(h, 10003); O(h, 10000); O(h, 10106); O(h, 10106);
    return 1;
}

// 1.13c D2LAUNCH+0x0B600  rebuild the character-list rows
void char_list_b600(H* h) {
    if (O(h, 10227)) { F(h, 0xAF00); return; }
    uint8_t* n = (uint8_t*)P(0x65ec8);
    int y = (int)h->d32(0x6fa65710u) - 0xd;
    int idx = 0;
    char utf[0x100];
    h->uni2win(utf, (const char16_t*)O(h, 10003), 0x100);
    O(h, 10131);
    for (int i = 0; i < (int)D(0x64dbc); i++) {
        O(h, 10106);
        if (O(h, 10009) == 7) O(h, 10094);
    }
    if (D(0x65edc)) O(h, 10106);
    if (D(0x65ad4) == 1 && D(0x65eb8)) return;
    for (; n; n = nxt(n), idx++) {
        uint32_t hd = u32(n, 0x338);
        if (idx < (int)D(0x65ed4) || (int)(D(0x65ed4) + D(0x64dbc)) <= idx) { O(h, 10194, hd, 0); continue; }
        int slot = idx - (int)D(0x65ed4);
        O(h, 10140, hd, 0x118, y);
        O(h, 10179, hd, 0x172);
        uint32_t ty = u32(n, 0x334);
        if (ty == 1) {
            O(h, 10194, hd, 0);
            O(h, 10182, D(0x65aac), 0x200, (int)h->d32(0x6fa65710u + slot * 4) - 3);
            O(h, 10131);
            O(h, 10003, 1, 0, 0); O(h, 10000);
            if (D(0x65ad4) == 1 && D(0x65ee4)) {
                O(h, 10003, 0, 0, 0); O(h, 10000); O(h, 10003, 0, 0, 0); O(h, 10000);
            } else {
                O(h, 10003, 0, 0, 0); O(h, 10000); O(h, 10000, 0, 0, 0);
            }
            y += 0x5d;
            continue;
        }
        uint16_t fl = u16(n, 0x324);
        uint32_t m = (fl & 8) ? (h->bitmask(0, 4) | 1) : (h->bitmask(1, 4) & 1);
        m = (fl & 4) ? (m | h->bitmask(0, 5)) : (m & h->bitmask(1, 5));
        O(h, 10194, hd, m);
        if (ty == 2) { O(h, 10003, 1, 0, 0); O(h, 10000); y += 0x5d; continue; }
        if (ty != 0) fatal(h, 0x3c5);
        char buf[0x200] = {0};
        const char* cn = (const char*)O(h, 10205, n[0x320], (fl >> 8) & 0x1f, fl & 4, fl & 0x20);
        if (cn) strncat(buf, cn, sizeof buf - 1);
        strncat(buf, (const char*)n, sizeof buf - strlen(buf) - 1);
        if (((char*)n)[0x32a]) { size_t l = strlen(buf); snprintf(buf + l, sizeof buf - l, " {%s}", (char*)n + 0x32a); }
        O(h, 10022, (fl & 4) ? 1 : 4, buf);
        F(h, 0x9E40);
        char16_t line[200] = {0};
        level_line(h, n, line);
        O(h, 10000, line, 0, 0);
        if (n[0x324] & 0x20) { O(h, 10003, 2); O(h, 10000); }
        if (n[0x32e]) { O(h, 10003, 4); O(h, 10000); }
        uint32_t exp = u32(n, 0x348), now = h->time32();
        uint32_t days = now < exp ? (exp - now) / 0x15180 : 0;
        if (D(0x65ad4) == 1 && days < 0x3d) {
            O(h, 10003);  // Unicode::sprintf(100, buf, fmt, days) into the caption
            O(h, 10000, days <= 0xe ? 1 : 0);
        }
        y += 0x5d;
    }
}

// 1.13c D2LAUNCH+0x0D160  expansion char-select screen setup
void charsel_exp_d160(H* h) {
    F(h, 0x15D30);
    F(h, 0x12BF0, "palette\\sky\\pal.pl2");
    ((uint8_t*)P(0x65abc))[0x20c] = 0;
    D(0x65ed0) = 0; D(0x65ed4) = 0; D(0x64db0) = 0; D(0x65eb0) = 1;
    D(0x65984) = 0; D(0x65ee0) = 0; D(0x65edc) = 0; D(0x65ed8) = 0;
    pool_add(h, REC(0x62d50));
    D(0x65980) = pool_add(h, REC(0x62d80));
    D(0x6570c) = pool_add(h, REC(0x62db0));
    D(0x65aac) = pool_add(h, REC(0x62de0));
    D(0x65998) = pool_add(h, REC(0x62e10));
    D(0x65708) = pool_add(h, REC(0x62e40));
    D(0x65878) = pool_add(h, REC(0x62c60));
    O(h, 10193); O(h, 10193); O(h, 10193);
    D(0x65aa0) = pool_add(h, REC(0x62e70));
    O(h, 10035); O(h, 10195);
    for (uint32_t v = 0x65734; v <= 0x65750; v += 4) D(v) = 0;
    D(0x64dbc) = 8;
    for (int i = 0; i < 8; i++) {
        intptr_t rec = REC(0x627e0) + i * 0x30;
        if (O(h, 10227)) {
            uint32_t v = pool_add(h, rec + 0x180);
            h->d32(0x6fa65858u + i * 4) = v;
            O(h, 10167);
        }
        O(h, 10109);
        h->d32(0x6fa65734u + i * 4) = pool_add(h, rec);
        O(h, 10109);
        h->d32(0x6fa65710u + i * 4) = (i / 2) * 0x5d + 0xb2;
    }
    D(0x65ab0) = pool_add(h, REC(0x62b40));
    if (!F(h, 0xC430)) return;
    O(h, 10147);
    D(0x65ee4) = 0;
    if (D(0x65ad4) == 1) { O(h, 10106); O(h, 10022, 0); F(h, 0xCD70); }
    else if (D(0x65ad4) == 0 || D(0x65ad4) == 3 || D(0x65ad4) == 2) {
        F(h, 0xAE60);
        if (!O(h, 10227)) { intptr_t a = O(h, 10003); intptr_t c = O(h, 10003); D(0x64db4) = D(0x65ed0); F(h, 0xBEB0, a, c); }
        F(h, 0xCC50);
    }
    F(h, 0xA8C0);
    F(h, 0xB600);
    if ((int)D(0x65ed0) - (int)D(0x64dbc) > 0) O(h, 10160);
    if (O(h, 10035)) O(h, 10045);
    O(h, 10147);
}

// 1.13c D2LAUNCH+0x0D5D0  char-select screen setup (classic / realm layout)
void charsel_d5d0(H* h) {
    if (O(h, 10227)) { F(h, 0xD160); return; }
    F(h, 0x15D30);
    O(h, 10006, D(0x65c54));
    F(h, 0x12BF0, "palette\\sky\\pal.pl2");
    if (D(0x65ad4) == 1) {
        int r = (int)F(h, 0x15B60);
        if (r == -1 || r == 0) {
            F(h, 0xE250);
            popup_15090(h, O(h, 10003, r == -1 ? 0x14aa : 0x14e3), 0x57df0, 1);
            return;
        }
    }
    D(0x65eb0) = 1; D(0x65ed0) = 0; D(0x65ed4) = 0; D(0x64db0) = 0;
    D(0x65984) = 0; D(0x65ee0) = 0; D(0x65edc) = 0; D(0x65eb8) = 0;
    uint32_t s = pool_idx(h, 0x7a);
    O(h, 10069, s); O(h, 10201, s);
    ((uint8_t*)P(0x65abc))[0x20c] = 0;
    D(0x6570c) = pool_idx(h, 0x7c);
    uint32_t a = pool_idx(h, 0x7d); D(0x65708) = a; O(h, 10147, a, 0);
    D(0x65980) = pool_idx(h, 0x7e);
    D(0x65730) = pool_idx(h, 0x99);
    D(0x6587c) = pool_idx(h, 0x9a);
    O(h, 10131, D(0x65730), 0); O(h, 10131, D(0x6587c), 0);
    D(0x65878) = pool_idx(h, 0x9b);
    D(0x65704) = pool_idx(h, 0x9d);
    D(0x65988) = pool_idx(h, 0x9e);
    D(0x65aac) = pool_idx(h, 0x9f);
    for (uint32_t v = 0x65734; v <= 0x65750; v += 4) D(v) = 0;
    auto rows = [&](int n, uint32_t rec, int y) {
        D(0x64dbc) = n;
        for (int i = 0; i < n; i++, rec += 0x30, y += 0x5d) {
            O(h, 10109, 1);
            h->d32(0x6fa65734u + i * 4) = pool_add(h, REC(rec));
            O(h, 10109, 0);
            h->d32(0x6fa65710u + i * 4) = y;
        }
    };
    if (D(0x65ad4) == 1) {
        rows(4, 0x62720, 0xf6);
        D(0x65edc) = pool_add(h, REC(0x62ba0));
        D(0x65ee0) = pool_add(h, REC(0x625d0));
        O(h, 10071, 0x8f, 0x1d, 0x14);
        pool_add(h, REC(0x62210));
        D(0x65ab4) = pool_add(h, REC(0x62270));
        D(0x65aa8) = pool_add(h, REC(0x622a0));
        uint32_t last = pool_add(h, REC(0x62240));
        O(h, 10000, last, O(h, 10003, 0x2b32));
        char* buf = (char*)h->dat(0x6fa65758u);
        if (!O(h, 422, "Diablo II", "Preferred Realm", "", buf, 0x100) || !F(h, 0xBE50, buf)) {
            intptr_t realm = F(h, VT | 0x98);
            O(h, 501, buf, F(h, 0x9B44, realm), 0x7fffffff);  // BNGatewayAccess::Realm
            O(h, 425, "Diablo II", "Preferred Realm", "", buf);
            if (!F(h, 0xBE50, buf)) {
                void* src = nullptr;
                if ((int)D(0x65e78) >= 1) {
                    void** tbl = (void**)P(0x65e74);
                    if (!tbl[0]) fatal(h, 0x1143);
                    src = tbl[0];
                }
                O(h, 501, buf, src, 0x7fffffff);
                O(h, 425, "Diablo II", "Preferred Realm", "", buf);
            }
        }
    } else {
        rows(5, 0x626f0, 0x99);
        O(h, 10071, 0x32, 0x1d, 0x14);
    }
    D(0x65ab0) = pool_add(h, REC(0x62b40));
    if (D(0x65ad4) == 1) return;  // retail path above jumps to the shared tail via LAB_6fa4daa1 only for the classic layout
    D(0x65990) = pool_add(h, REC(0x62b70));
    O(h, 10131); O(h, 10147);
    F(h, 0xD050);
    if (O(h, 10035)) O(h, 10045);
    O(h, 10147);
}

// 1.13c D2LAUNCH+0x18E10  terms-of-service screen: load tos-unicode_<lang>.txt and wrap it into lines
int tos_18e10(H* h) {
    D(0x65294) = 0;
    F(h, 0x14A20);
    char lang[8] = {0}, fname[0x100] = {0};
    O(h, 10004, lang, 2);
    O(h, 501, fname, "tos-unicode_", 0x80);
    O(h, 503, fname, lang, 0x80);
    O(h, 503, fname, h->dat(0x6fa5e090u), 0x80);
    F(h, VT | 0x50, fname);
    O(h, 10014, 0x548d0);
    if (D(0x65294)) {
        for (int i = 0; i < (int)D(0x65e34); i++) O(h, 10050, h->dat(0x6fa653c0u + i * 4));
        D(0x65e34) = 0;
        return 0;
    }
    if (F(h, VT | 0x68)) {
        F(h, 0x12B60); F(h, 0x176C0); pool_idx(h, 0); pool_idx(h, 0);  // idx args not recovered
        intptr_t hf = O(h, 10107, 0, 0, 3, 0x8000000, 0);
        if (hf != -1) {
            uint32_t size = (uint32_t)F(h, SEAM_GETFILESIZE, hf);
            char16_t* buf = (char16_t*)O(h, 10042, size, 0xc34, 0);
            uint32_t got = 0;
            O(h, 10110, hf, buf, size, &got, 0);
            O(h, 10109, hf); O(h, 10108, hf);
            const char16_t* text = nullptr; int chars = 0;
            if (size == got && size > 2) {
                if (buf[0] == 0xfeff) { chars = (int)((size - 2) >> 1); text = buf + 1; }
                else if (buf[0] == 0xfffe) { chars = (int)((size - 2) >> 1); text = buf + 1; F(h, 0x1000, text, chars); }
                else { char16_t* w = (char16_t*)O(h, 10042, size * 2, 0xc52, 0); h->win2uni(w, (const char*)buf, (int)size); O(h, 10043, buf, 0xc54, 0); text = w; chars = (int)size; }
            }
            int pos = 0;
            while (pos < chars) {
                char16_t line[200] = {0};
                int start = pos, j = 0;
                do {
                    if (pos >= chars) break;
                    char16_t c = text[pos];
                    if (c == 0xd || c == 10) { line[j] = 0x20; j = 200; }
                    else if (c == 9) { line[j++] = 0x20; }
                    else line[j++] = c;
                    pos++;
                    if (O(h, 10028, line) > 0x21c) {
                        int k = ulen(line);
                        if (k >= 2) for (;;) { if (F(h, SEAM_ISLINEBREAK, text + start, k)) break; if (!(--k > 1)) break; }
                        pos = start + k;
                        line[k] = 0;
                        if (k && line[k - 1] == 0x20) line[k - 1] = 0;
                        break;
                    }
                } while (j < 200);
                int len = ulen(line);
                for (int i = 0; i < len; i++) if (line[i] != 0x20) { O(h, 10000, line, 0, 0); break; }
            }
            O(h, 10043, text, 0xc9b, 0);
            O(h, 10037);
            pool_add(h, REC(0x61df0)); pool_add(h, REC(0x61e20)); pool_add(h, REC(0x61e50));
            O(h, 10003, 0, 0, 0); O(h, 10000);
            pool_add(h, REC(0x61e80)); pool_add(h, REC(0x61eb0));
            return 1;
        }
    }
    popup_15090(h, O(h, 10003), 0x57df0, 1);
    return 0;
}
}  // namespace n75
