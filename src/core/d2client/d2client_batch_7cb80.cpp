// src/core/d2client/d2client_batch_7cb80.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x7CB80..0x7FE80: chat/system
// message queues, the scroll-in cel animation, unit name labels, the automap-marker line draw) reimplemented from
// the cached decompiles. Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
// Base 0x6FAB0000. Unrecovered callees (ordinals, other RVAs) are typed seams in Env (the d2client_batch_5d110.cpp
// pattern); registers the decompiler lost (string-table ids, text/colour args) are noted at each use.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace d2c7cb {
typedef char16_t U16;
static uint32_t rd32(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static uint16_t rd16(const uint8_t* p, size_t o) { uint16_t v; memcpy(&v, p + o, 2); return v; }

// Message-line node (retail 0x26 bytes, pointers 4 bytes; ours uses native pointers, retail offsets noted).
struct Node {
    U16* line[6];        // +0x00..0x17
    uint16_t count;      // +0x18
    uint32_t color;      // +0x1A
    uint32_t expire;     // +0x1E  GetTickCount()+10000
    Node* next;          // +0x22
};
struct Q { Node* head; uint16_t count; };   // retail packed 6-byte records at +0x113AA4/+0x113AAA/+0x113AB0
struct L { U16* v; L* next; };              // ord10059 wrapped-line list: [0]=line, [1]=next

struct G {
    uint32_t h3b10, f3a68, f3a60, f3a70, f3aca, cc2fc, d5d4, ad50, bbfc_set;
    uint8_t  f3b0b;
    void*    obj3a74;                // object with vtable (dtor at slot 0, slot 0x3C call)
    Q        q[3];                   // 3aa4 / 3aaa / 3ab0
    uint32_t b03, b07;               // +0x113B03 tick stamp, +0x113B07 frame
    uint8_t* cel2;                   // +0x11BFAC scroin2
    uint8_t* cel3;                   // +0x11BFA8 scroin3
    uint32_t bbfc;                   // +0x11BBFC (also used as pointer to {int, +0xC})
    uint8_t* bbfcp;                  // same global as pointer
    int32_t  cbf34, c250, c268, ade0, ae8_tag_valid;
    uint8_t* c24c;                   // +0x10C24C record array
    uint32_t d5d0, c9721;
    int32_t  scrW, scrH;             // 8bc48 / 8bc4c
    int32_t  panelMode;              // +0x11C414
    int32_t  ad4, ad8;               // 3ad4 / 3ad8 camera offsets
    uint32_t cc3b8, bb9cc, bb9d0, bb9d4, bb9d8, cbefc;
    uint8_t* v3ae8;                  // object, tag at +0x80
    int32_t  f3b14;
    uint32_t tick91c90[5];           // 6fb91c90
    uint16_t sel3950[5];             // 6fbc3950
    int32_t  t91c40[10], t91c68[10]; // x/y pairs
    int32_t  cb9a0, cb9a4;
    uint32_t ad80[64]; uint8_t* tbl8d538[64];
    uint8_t* cbf68; int32_t cbf78; uint32_t c3918;
    const int32_t* t84988;           // 8 x {4 ints} then 8 x {4 ints} at +0x10
};

struct Env {
    // generic seams
    uint32_t (*tick)();                       // GetTickCount
    void (*fatal)(uint32_t line);             // ord10265 + ord10024 + _exit(-1)
    void (*ord10184)();
    void (*fn38a70)(uint32_t,uint32_t,uint32_t,uint32_t);
    void (*free43)(uint32_t line, uint32_t h);           // Ordinal_10043
    void (*ord10660)(uint32_t);
    void (*fn7b3d0)(); void (*fn53d20)(); void (*fn7ad00)(); void (*fn7beb0)(Q*);
    void (*fn7b9c0)(Q*); void (*fn7b330)(Q*, Node* old);
    void (*fn7aea0)();                        // 2aea0
    void (*obj3c)(void* obj);                 // vtable[0x3C]
    void (*objDtor)(void* obj, uint32_t del); // vtable[0]
    // 7CC60
    void (*fn7c380)(); uint8_t* (*loadCel)(const char*);
    void (*draw10041)(const uint8_t* rec72, int32_t x, int32_t y, int32_t a, int32_t mode, int32_t b);
    // 7CFC0 / 7DFB0 / 7D610 / 7D850
    const uint8_t* (*fn4d630)(void* thisp, uint32_t id);  // record, name at +0x66
    uint32_t (*fn4d850)(); const char* (*fn4d850s)();
    std::u16string (*str10003)(uint32_t a, uint32_t b);   // string-table fetch; id register not recovered
    uint32_t (*ffd90)(uint32_t a, uint32_t b, uint32_t c);
    int32_t (*textWidth)(const U16* p, int32_t n);        // ord10171
    int32_t (*isLineBreak)(const U16* p, int32_t n);
    L* (*wrap10059)(const U16* text, int32_t width, int32_t* count);
    void (*freeL)(L*);
    uint32_t (*fab1230)(); uint32_t (*ord10038)(uint32_t);
    std::u16string (*ord10688)(uint32_t id, bool* ok);    // record +0x40 string
    uint32_t (*fn55480)(); void (*ord10358)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
    // 7D850 tail
    int32_t* (*fn37fe0)(uint32_t); int32_t* (*fn37d80)(); void (*fn38350)(void*, int32_t*);
    // 7DAA0 / 7DC00
    void (*unitPos)(uint8_t* unit, int32_t* x, int32_t* y);   // ord10628
    void (*nameText)(uint32_t h, char* out);                  // ord10834
    void (*ord10271)(uint32_t); uint32_t (*fn2b4f0)(U16* buf);
    void (*fn2ca40)(int32_t x, uint8_t* unit, const U16* text);
    // 7E930 / 7EE50
    void (*fn7cef0)(); void (*fn486a0)();
    void (*fn7c730)(); void (*fn7c770)(); void (*fn72790)(uint32_t,uint32_t,uint32_t);
    void (*fn7b20)(); void (*ord10227)(); uint32_t (*fn55440)();
    // 7FE80
    int32_t (*ord10047)(); int32_t (*cellW)(uint32_t); int32_t (*cellH)(uint32_t);
    void (*line10010)(int32_t,int32_t,int32_t,int32_t,uint8_t,uint8_t);
};
static G g; static Env e;
extern "C" G* d2c7cb_state() { return &g; }
extern "C" Env* d2c7cb_env() { return &e; }

// ---- Unicode helpers (D2Lang Unicode::*, 2-byte chars) ----
static size_t ulen(const U16* s) { size_t n = 0; while (s[n]) ++n; return n; }
static void ucpy(U16* d, const U16* s) { while ((*d++ = *s++)) {} }
static void ucat(U16* d, const U16* s) { ucpy(d + ulen(d), s); }
static void ucpy(U16* d, const std::u16string& s) { ucpy(d, s.c_str()); }
static void ucat(U16* d, const std::u16string& s) { ucat(d, s.c_str()); }
static void win2u(U16* d, const char* s, int n) { int i = 0; for (; i < n - 1 && s[i]; ++i) d[i] = (uint8_t)s[i]; d[i] = 0; }
static void usprintf(size_t cap, U16* d, const std::u16string& fmt, const U16* a = nullptr, const U16* b = nullptr) {
    std::u16string o; const U16* args[2] = { a, b }; int k = 0;
    for (size_t i = 0; i < fmt.size() && o.size() + 1 < cap; ++i) {
        if (fmt[i] == u'%' && i + 1 < fmt.size() && fmt[i + 1] == u's') { if (k < 2 && args[k]) o += args[k]; ++k; ++i; }
        else o += fmt[i];
    }
    if (o.size() >= cap) o.resize(cap - 1);
    ucpy(d, o);
}

// 1.13c D2Client+0x7CB80
void sub_7CB80() {
    if (g.h3b10) { e.ord10660(g.h3b10); g.h3b10 = 0; }
    if (g.f3a68) { e.fn7b3d0(); e.free43(0x68a, 0); g.f3a68 = 0; }
    if (g.f3a60) { e.fn7b3d0(); e.free43(0x68a, 0); e.free43(0x6e0, 0); g.f3a60 = 0; }
    if (g.f3a70) { e.fn53d20(); g.f3a70 = 0; }
    if (g.obj3a74) { e.objDtor(g.obj3a74, 1); g.obj3a74 = nullptr; }
    e.fn7ad00();
    g.cc2fc = 0;
    e.fn7beb0(&g.q[0]); e.fn7beb0(&g.q[1]); e.fn7beb0(&g.q[2]);
    g.f3b0b = 0;
}

// 1.13c D2Client+0x7CC60
void sub_7CC60(int32_t param_1) {
    if (!g.bbfc) return;
    uint32_t rec[18] = {};
    e.fn7c380();
    char path[260];
    if (!g.cel2) { snprintf(path, sizeof path, "%s\\%s", "DATA\\GLOBAL", "UI\\menu\\scroin2"); g.cel2 = e.loadCel(path); }
    if (!g.cel3) { snprintf(path, sizeof path, "%s\\%s", "DATA\\GLOBAL", "UI\\menu\\scroin3"); g.cel3 = e.loadCel(path); }
    if (!param_1) return;
    e.ord10184();
    // retail: record = 0x48 bytes, word0 = index, cel pointer at +0x34 (host-side pointer here)
    struct Rec { uint32_t w0; uint8_t pad[0x30]; uint8_t* cel; uint8_t tail[0x0C]; };
    if (!g.b03) { g.b03 = e.tick(); g.b07 = 0; }
    else if (g.b03 + 0x32 < e.tick()) { g.b03 = e.tick(); g.b07++; }
    for (int32_t i = 0; i < 5; ++i) {
        if (g.b07 <= g.tick91c90[i]) continue;
        uint32_t d = g.b07 - g.tick91c90[i]; uint32_t mode = 5;
        if (d == 1) { e.fn38a70(0, 0, 0, 0); mode = 0; }
        else if ((int32_t)d < 5) mode = 0;
        else if ((int32_t)d < 10) mode = 1;
        else if ((int32_t)d < 0xf) mode = 2;
        uint16_t s = g.sel3950[i];
        if (s >= 6) continue;
        if (s > 4) e.fatal(0x164d);
        Rec a{}; a.w0 = i; a.cel = g.cel2;
        e.draw10041((const uint8_t*)&a, g.t91c40[s * 2] + g.cb9a0, g.t91c40[s * 2 + 1] - g.cb9a4, -1, (int32_t)mode, 0);
        Rec b{}; b.w0 = (d > 0x14) ? 0 : d; b.cel = g.cel3;
        e.draw10041((const uint8_t*)&b, g.t91c68[s * 2] + g.cb9a0, g.t91c68[s * 2 + 1] - g.cb9a4, -1, 3, 0);
    }
    e.ord10184();
    (void)rec;
}

// 1.13c D2Client+0x7CFC0
uint32_t sub_7CFC0(void* thisp, const uint8_t* msg, U16* out) {
    const uint8_t* r = e.fn4d630(thisp, rd32(msg, 3));
    char nm[16]; if (!r) nm[0] = 0; else { memcpy(nm, r + 0x66, 16); nm[15] = 0; }
    U16 name[32]; win2u(name, nm, 16);
    uint32_t id = rd32(msg, 3);
    switch (msg[7]) {
    case 1: case 2: case 10: case 11:
        usprintf(0x132, out, e.str10003(0, 0), name); break;
    case 3: ucpy(out, name); ucat(out, e.str10003(0, 0)); e.fn38a70(0, 0, 0, 0); return 1;
    case 4: ucpy(out, name); ucat(out, e.str10003(0, 0)); return 1;
    case 5: case 6: case 7: case 9: ucpy(out, name); ucat(out, e.str10003(0, 0)); break;
    case 8: ucpy(out, e.str10003(0, 0)); ucat(out, name); break;
    default: break;
    }
    if (g.bbfcp && e.ffd90(id, rd32(g.bbfcp, 0xc), 4)) return 0;   // ecx at the call is a leftover register: id assumed
    return 1;
}

// 1.13c D2Client+0x7D610   (text = stack arg, color = byte the decompiler shows as the return-address slot)
void sub_7D610(const U16* text, uint8_t color) {
    e.ord10184();
    Node* nd = (Node*)calloc(1, sizeof(Node));
    nd->expire = e.tick() + 10000;
    int32_t n = (int32_t)ulen(text), rem = n, off = 0;
    for (;;) {
        int32_t k;
        for (;;) { k = rem; if (k > 0 && e.textWidth(text + off, k) > g.scrW - 0x46) { rem = k - 1; continue; } break; }
        if (off + k != n && k > 0) {
            const U16* p = text + off;
            do { if (e.isLineBreak(p, k + 1)) break; --k; } while (k > 0);
        }
        // retail also runs Unicode::toUtf(buf, p, k) here; the buffer is never read.
        if (k == 0 || nd->count > 5) break;
        U16* ln = (U16*)calloc(k + 1, 2);
        memcpy(ln, text + off, (size_t)k * 2);
        nd->line[nd->count++] = ln;
        rem = n - k; off = k;                 // retail assigns offset = k (not off + k)
    }
    nd->color = color;
    Q& q = g.q[1];
    if (q.head) {
        uint32_t cnt = 0; Node* t = q.head;
        while (t->next) { t = t->next; ++cnt; }
        if ((cnt & 0xffff) != (uint32_t)q.count - 1) e.fatal(0xddf);
        t->next = nd; nd = q.head;
    }
    q.head = nd; q.count++;
    e.fn7b9c0(&q);
    if (q.count > 2) { Node* old = q.head; q.head = old->next; e.fn7b330(&q, old); q.count--; }
    if (g.ade0 && g.obj3a74) { e.obj3c(g.obj3a74); e.fn7aea0(); }
    e.ord10184();
    e.fn38a70(0, 0, 0, 0);
}

// 1.13c D2Client+0x7D850   (text/color: see 7D610; the wrapped-line list comes from ord10059(width), count from ESI)
void sub_7D850(const U16* text, uint8_t color) {
    int32_t i = 0, cnt = 0;
    Node* nd = (Node*)calloc(1, sizeof(Node));
    nd->expire = e.tick() + 10000;
    e.ord10184();
    L* l = e.wrap10059(text, g.scrW - 0x46, &cnt);
    nd->count = (uint16_t)(cnt > 6 ? 6 : cnt);
    for (; i < cnt; ++i) {
        if (i < nd->count) nd->line[i] = l->v; else e.free43(0xd56, 0);
        L* nx = l->next; e.freeL(l); l = nx;
    }
    nd->color = color;
    Q& q = g.q[0];
    if (q.head) {
        uint32_t c = 0; Node* t = q.head;
        while (t->next) { t = t->next; ++c; }
        if ((c & 0xffff) != (uint32_t)q.count - 1) e.fatal(0xd68);
        t->next = nd; nd = q.head;
    }
    q.head = nd;
    e.fn7b9c0(&q);
    uint16_t oldCount = q.count++;
    int32_t sum = 0;
    for (Node* p = q.head; p; p = p->next) sum += p->count;
    if (sum > 0x12) { Node* old = q.head; q.head = old->next; q.count = oldCount; e.fn7b330(&q, old); }
    if (g.cbf34) {
        uint8_t* rec = (!g.c24c || g.c250 < 7) ? nullptr : g.c24c + 0x354;
        if (rec && rec[0x3c]) {
            int32_t* p = nullptr;
            if (rd16(rec, 0x45) == 0 || !(p = e.fn37fe0(0))) {
                p = e.fn37d80();
                if (!p) goto done;
                p[1] = 6; p[0xe] = 0; p[0xd] = g.c268; p[9] = 0; p[7] = 0xff;
                ((uint8_t*)(p + 0xf))[0] = rec[0x4e];
                e.fn38350(nullptr, p);
            }
            if (!g.bbfc) ((uint8_t*)(p + 0xf))[0] += 'P';
        }
    }
done:
    if (g.ade0 && g.obj3a74) { e.obj3c(g.obj3a74); e.fn7aea0(); }
    e.ord10184();
}

// shared body of 7DAA0 / 7DC00: unit label screen position; returns false when culled
static bool labelPos(uint8_t* unit, int32_t dy, int32_t* x, int32_t* y) {
    e.unitPos(unit, x, y);
    *y += dy - g.ad8; *x -= g.ad4;
    int32_t q = (g.scrW + ((g.scrW >> 31) & 3)) >> 2;
    if (g.panelMode == 1) *x -= q; else if (g.panelMode == 2) *x += q; else if (g.panelMode == 3) return false;
    return *y > 0 && *y < g.scrH && *x > -100 && *x < g.scrW + 100;
}
static void labelText(const std::u16string& s, U16* buf /*200 chars*/) {
    size_t n = s.size() > 199 ? 199 : s.size();
    memset(buf, 0, 200 * 2); memcpy(buf, s.c_str(), n * 2);
}

// 1.13c D2Client+0x7DAA0   (fastcall: unit in ECX; unit+0xA4 = name handle)
void sub_7DAA0(uint8_t* unit) {
    if (!unit) return;
    uint32_t h = rd32(unit, 0xa4);
    if (!h) return;
    int32_t x, y;
    if (!labelPos(unit, -10, &x, &y)) return;
    char buf[0x118] = {};
    e.nameText(h, buf);
    long v = atol(buf);
    if (v > 0 && v < 0xffff) {
        std::u16string s = e.str10003((uint32_t)v, 0);
        if (s.empty()) return;
        U16 t[200]; labelText(s, t);
        e.fn2ca40(x, unit, t);
    }
}

// 1.13c D2Client+0x7DC00   (fastcall: unit in ECX)
void sub_7DC00(uint8_t* unit) {
    if (!unit) return;
    uint32_t h = rd32(unit, 0xa4);
    if (!h) return;
    int32_t x, y;
    if (!labelPos(unit, -0x1e, &x, &y)) return;
    char buf[0x118] = {};
    e.nameText(h, buf);
    uint32_t arg = h;                    // iVar7: stays the handle unless the 0xFFFF-prefixed branch is reached
    if ((uint8_t)buf[0] == 0xff && (uint8_t)buf[1] == 0xff) {
        long v = atol(buf + 2);
        if ((v > 0x42c && v < 0x510) || (v > 0x5586 && v < 0x5616) || v == 0x2aa4 || v == 0x5586) {
            arg = (uint32_t)x;
            std::u16string s = e.str10003((uint32_t)v, 0);
            if (!s.empty()) {
                U16 t[200]; labelText(s, t);
                e.fn2ca40(x, unit, t);
                return;
            }
        }
    }
    U16 t[0x100] = {};
    e.ord10271(arg);
    if ((uint8_t)e.fn2b4f0(t)) { t[0xff] = 0; e.fn2ca40((int32_t)arg, unit, t); }
}

// 1.13c D2Client+0x7DE00
void sub_7DE00() {
    if (!g.v3ae8) return;
    uint32_t tag = rd32(g.v3ae8, 0x80);
    if (tag == 0x20736b62) { sub_7CC60(0); return; }
    if (tag == 0x20646b62) { if (!g.f3b14) { sub_7CC60(1); return; } }
    else if (tag != 0x20317274) return;
    e.fn7c380();
}

// 1.13c D2Client+0x7DFB0   (msg = chat/system message record: [1]=kind, [3]=u32 id, [7]=sub, [8]=name[16], [0x18]=text2)
void sub_7DFB0(void* thisp, uint8_t* m) {
    U16 out[0x132] = {}, name[32] = {}, second[32] = {};
    char* nm = (char*)m + 8;
    if (strlen(nm) > 14) m[0x17] = 0;
    auto lenchk = [&](uint32_t line) { if (e.str10003(0, 0).size() > 0x103) e.fatal(line); };
    auto tail = [&] { ucat(out, e.str10003(0, 0)); };
    auto sec = [&]() -> bool {                          // second string at +0x18
        if (!m[0x18]) return false;
        win2u(second, (char*)m + 0x18, 16); return true;
    };
    switch (m[1]) {
    case 0: lenchk(0x113a); win2u(name, nm, 16); ucpy(out, name); tail(); break;
    case 1: lenchk(0x1140); win2u(name, nm, 16); ucpy(out, name); tail(); break;
    case 2:
        if (g.bbfc && strcmp(nm, e.fn4d850s()) == 0) break;      // same-name message: empty line is queued
        lenchk(0x114a); win2u(name, nm, 16);
        if (!sec()) usprintf(0x132, out, e.str10003(0, 0), name);
        else usprintf(0x132, out, e.str10003(0, 0), name, second);
        break;
    case 3:
        lenchk(0x1158); win2u(name, nm, 16);
        if (!sec()) usprintf(0x132, out, e.str10003(0, 0), name);
        else usprintf(0x132, out, e.str10003(0, 0), name, second);
        break;
    case 4: lenchk(0x1166); win2u(name, nm, 16); ucpy(out, name); tail(); break;
    case 5: lenchk(0x1172); win2u(name, nm, 16); ucpy(out, name); tail(); break;
    case 6: {
        win2u(name, nm, 16);
        std::u16string tl; bool plain = false;
        uint8_t sub = m[7];
        if (sub == 0) { if (!sec()) plain = true; else tl = second; }
        else if (sub == 1) {
            uint32_t v = m[0x18] == 0 ? e.fab1230() : e.ord10038(rd16(m, 0x18));
            if (!v) plain = true; else tl = e.str10003(0, 0);
        } else {
            bool ok = false; std::u16string r;
            if (sub == 2) r = e.ord10688(rd32(m, 3), &ok);
            if (sub != 2 || !ok) plain = true;
            else tl = rd32(m, 3) != 0xb ? r : e.str10003(0, 0);
        }
        if (plain) { ucpy(out, name); tail(); break; }
        std::u16string pre = e.str10003(0, 0);
        if (tl.size() + 0x10 + pre.size() >= 0x132) return;
        ucpy(out, name); ucat(out, pre); ucat(out, tl);
        break;
    }
    case 7:
        if (sub_7CFC0(thisp, m, out)) sub_7D610(out, m[2]);       // colour byte: m[2] assumed (register not recovered)
        return;
    case 8: win2u(name, nm, 16); ucpy(out, name); tail(); break;
    case 9: case 0x10: ucpy(out, e.str10003(0, 0)); break;
    case 10:
        if (!nm[0]) ucpy(out, e.str10003(0, 0));
        else { win2u(name, nm, 16); ucpy(out, name); tail(); }
        break;
    case 0xb: win2u(name, nm, 16); usprintf(0x132, out, e.str10003(0, 0), name); break;
    case 0xc: case 0xe: return;
    case 0xd: lenchk(0x116c); win2u(name, nm, 16); ucpy(out, name); tail(); break;
    case 0xf: case 0x11: usprintf(0x132, out, e.str10003(rd32(m, 3), 0)); break;
    case 0x12:
        usprintf(0x132, out, e.str10003(0, 0));
        if (g.cc3b8) { uint32_t v = e.fn55480(); e.ord10358(g.cc3b8, v, 5, 0, 1); }
        g.bb9cc = e.tick(); g.bb9d0 = 4000; g.bb9d4 = 10000; g.bb9d8 = 4000; g.cbefc = 6;
        e.fn38a70(0, 0, 0, 0);
        break;
    default: return;
    }
    sub_7D850(out, m[2]);
}

// 1.13c D2Client+0x7E930
void sub_7E930() {
    if (!g.f3aca) e.fn7cef0(); else e.fn486a0();
    if (g.d5d4) g.ad50 = e.tick() + 100;
    g.f3aca = 0;
}

// 1.13c D2Client+0x7EE50
void sub_7EE50() {
    if (!g.f3a70) return;
    e.ord10227();
    if (g.d5d0 != 0x26) e.fatal(0x139b);
    uint32_t prev = (uint32_t)g.ade0;
    g.ade0 = 0;
    if (prev) e.fn7c730();
    e.ord10227();
    if (g.d5d0 != 0x26) e.fatal(0x139b);
    if (g.f3a68) return;
    for (uint32_t i = 0; (int32_t)i < (int32_t)g.d5d0; ++i) {
        if (g.ad80[i] != 1) continue;
        switch (rd32(g.tbl8d538[i], 0x60)) {
        case 0: break;
        case 1: e.fn72790(i, 1, 0); break;
        case 4: if (g.c9721) e.fn7b20(); break;
        default: return;
        }
    }
    if (!g.bbfcp || (rd32(g.bbfcp, 0) == 0 && e.fn55440() == 0)) g.ade0 = 1;
    // retail re-reads DAT_ade0 (== 0 here) into the compare variable before this test
    if (g.ade0) e.fn7c770();
}

// 1.13c D2Client+0x7FE80
void sub_7FE80() {
    if (e.ord10047()) return;
    int32_t bot = g.scrH - 0x2f, left = 0, right = g.scrW;
    int32_t q = (g.scrW + ((g.scrW >> 31) & 3)) >> 2;
    if (g.panelMode == 1) right = g.scrW - q * 2;
    else if (g.panelMode == 2) left = q << 1;
    uint8_t* e0 = (uint8_t*)(uintptr_t)rd32(g.cbf68, 0x11c);   // 32-bit pointer in retail; host resolves
    int32_t n = (int32_t)rd32(g.cbf68, 0x110);
    if (n < 0) return;
    for (int32_t i = 0; i <= n; ++i, e0 += 0x28) {
        if (!rd16(e0, 0)) continue;
        int32_t x = (int32_t)rd32(e0, 4), y = (int32_t)rd32(e0, 8);
        uint8_t c1 = (uint8_t)rd16(e0, 0x24), c2 = e0[0x25];
        if (!g.cbf78) {
            if (rd32(e0, 0x1c) == 0) {
                int32_t w = e.cellW(g.c3918), h = e.cellH(g.c3918);
                int32_t span = (int32_t)rd32(e0, 0xc) - y;
                if (span < h) { w = (span * w) / h; h = span; }
                int32_t x2 = w + x, y2 = h + y;
                if ((left <= x && x < right && y >= 0 && y < bot) || (left <= x2 && x2 < right && y2 >= 0 && y2 < bot))
                    e.line10010(x, y, x2, y2, c1, c2);
            } else if (left <= x && x < right && y >= 0 && y < bot) e.line10010(x, y, x, y, c1, c2);
        } else {
            int32_t k = (int32_t)rd32(e0, 0x10);
            if (left <= x && x < right && (int32_t)rd32(e0, 8) >= 0 && y < bot && k < 8 && k >= 0) {
                const int32_t* a = g.t84988 + k * 4;
                e.line10010(a[0] + x, a[1] + y, a[2] + x, a[3] + y, c1, c2);
                const int32_t* b = g.t84988 + k * 4 + 4;
                e.line10010(b[0] + x, b[1] + y, b[2] + x, b[3] + y, c1, c2);
            }
        }
    }
}
} // namespace d2c7cb
