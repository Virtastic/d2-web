// The automap overlay's text block (1.13c D2Client+0x62680 and its line writers) as one browser entry.
// 0x62680 itself (d2client_62680) sequences the writers; the writers are the proven ports in
// d2client_batch_5fd20.cpp (0x5FFE0, 0x5FD20, 0x5FE90, 0x5FE10, 0x5FF70). Supplied here: the D2Win text
// width (the font's advance table through the 1.13c width loop d2win_12080), the string table (D2Lang)
// and the level-name line 0x60AA0 draws, whose text is the engine's own level name.
// Output: one record per drawn line (each D2Win #10150 call); the browser blits them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

void D2Lang_Unicode_sprintf(int cap, char16_t* out, const char16_t* fmt, ...);
extern "C" const char* d2_lang_string_by_index(int index);
extern "C" int32_t d2win_12080(const uint16_t* text, int32_t n, const void* env);

struct D2ClientB61Env { uint32_t (*ord10331)(uint32_t); uint32_t (*ord10184)(uint32_t); void (*f0f9f0)(); void (*f10c40)(); void (*f12410)(); void (*f0ffe0)(); void (*f10aa0)(); void (*f0fe90)(); void (*f0fd20)(); void (*f0fe10)(); void (*f0ff70)(); };
struct D2ClientB61State { uint32_t d_c3b8, d_bbfc, d_c394, d_c21c, d_9854; };
uint32_t d2client_62680(D2ClientB61State* s, const D2ClientB61Env* e);

namespace d2client_batch_5fd20 {
using W = char16_t;
struct Line {
    virtual ~Line() = default;
    virtual const W* str(int32_t id) = 0;
    virtual const char* narrow(int32_t n) = 0;
    virtual void fmt(W* buf, int32_t n, const W* f, const W* a) = 0;
    virtual int32_t measure(const W* buf) = 0;
    virtual void draw(const W* buf, int32_t x, int32_t y) = 0;
};
void sub_5FD20(Line&, uint8_t, int32_t, int32_t&, bool&);
void sub_5FE10(Line&, int32_t, int32_t&);
void sub_5FE90(Line&, int32_t, bool, int32_t, bool, int8_t, int32_t&);
void sub_5FF70(Line&, int32_t, int32_t&);
void sub_5FFE0(Line&, const uint8_t*, int32_t, int32_t&);
}

namespace {
using W = char16_t;
struct Rec { int32_t kind, x, y; uint16_t text[101]; uint16_t pad; };   // 212 bytes
enum { K_GAME = 1, K_NAME = 2, K_VERSION = 3, K_DIFFICULTY = 4, K_ADDRESS = 5, K_EXPANSION = 6 };

struct Ctx {
    const uint8_t* adv; int32_t width; uint8_t difficulty;
    const char* name; const char* address; const char* version;   // version: null = retail's own 0x5FE90 text
    Rec* out; int32_t cap, n, kind;
    D2ClientB61State st;
} *c;

const uint8_t* glyph(uint32_t ch) { static uint8_t rec[4]; rec[3] = c->adv[ch & 0xffff]; return rec; }
void widen(W* d, const char* s, int cap) {   // UTF-8 to UTF-16, cut to cap-1 units
    int o = 0;
    for (const uint8_t* p = (const uint8_t*)s; s && *p && o < cap - 1;) {
        uint32_t u = *p++;
        if (u >= 0xe0 && p[0] && p[1]) { u = ((u & 0xf) << 12) | ((p[0] & 0x3f) << 6) | (p[1] & 0x3f); p += 2; }
        else if (u >= 0xc0 && p[0]) { u = ((u & 0x1f) << 6) | (p[0] & 0x3f); p++; }
        d[o++] = (W)u;
    }
    d[o] = 0;
}
struct Env : d2client_batch_5fd20::Line {
    W tmp[300];
    const W* str(int32_t id) override { widen(tmp, d2_lang_string_by_index(id & 0xffff), 300); return tmp; }
    const char* narrow(int32_t) override { return c->address; }
    void fmt(W* buf, int32_t n, const W* f, const W* a) override { D2Lang_Unicode_sprintf(n, buf, f, (uintptr_t)a); }
    int32_t measure(const W* buf) override {
        uint16_t u[300]; int32_t n = 0;
        while (buf[n] && n < 299) { u[n] = buf[n]; n++; }
        struct E { const uint8_t* (*g)(uint32_t); } e = { glyph };
        return d2win_12080(u, n, &e);
    }
    void draw(const W* buf, int32_t x, int32_t y) override {
        if (c->n >= c->cap) return;
        Rec& r = c->out[c->n++];
        std::memset(&r, 0, sizeof r);
        r.kind = c->kind; r.x = x; r.y = y;
        for (int i = 0; i < 100 && buf[i]; i++) r.text[i] = (uint16_t)buf[i];
    }
} env;
void tail(const W* buf) {   // the writers' shared tail: x = screen width - width - 0x10, then the cursor steps
    int32_t w = env.measure(buf);
    env.draw(buf, c->width - w - 0x10, (int32_t)c->st.d_c21c);
    c->st.d_c21c += 0x10;
}
uint32_t unitRoomStub(uint32_t h) { return h; }
uint32_t roomFlagStub(uint32_t) { return 0; }
void nop() {}
void h_game() { c->kind = K_GAME; }   // no game-info block here: 0x5FFE0 returns without one
void h_level() {                      // 0x60AA0: the level's name line, then the cursor steps
    c->kind = K_NAME;
    W b[100]; widen(b, c->name, 100); tail(b);
}
void h_version() {
    c->kind = K_VERSION;
    int32_t y = (int32_t)c->st.d_c21c;
    if (!c->version) d2client_batch_5fd20::sub_5FE90(env, c->width, true, 13, false, 0, y);
    else if (!*c->version) y += 0x10;   // 0x5FE90 steps the cursor even when it draws nothing
    else { W b[100]; widen(b, c->version, 100); tail(b); return; }
    c->st.d_c21c = (uint32_t)y;
}
void h_difficulty() {
    c->kind = K_DIFFICULTY;
    int32_t y = (int32_t)c->st.d_c21c; bool fatal = false;
    d2client_batch_5fd20::sub_5FD20(env, c->difficulty, c->width, y, fatal);
    c->st.d_c21c = (uint32_t)y;
}
void h_address() {
    c->kind = K_ADDRESS;
    int32_t y = (int32_t)c->st.d_c21c;
    d2client_batch_5fd20::sub_5FE10(env, c->width, y);
    c->st.d_c21c = (uint32_t)y;
}
void h_expansion() {
    c->kind = K_EXPANSION;
    int32_t y = (int32_t)c->st.d_c21c;
    d2client_batch_5fd20::sub_5FF70(env, c->width, y);
    c->st.d_c21c = (uint32_t)y;
}
}  // namespace

// adv: the font's 65536-byte advance table. name/address/version: UTF-8; address null = not a TCP game;
// version null = retail's own `v 1.13`, "" = no version text. out: cap records of 212 bytes
// (kind, x, y as i32, then 101 UTF-16 units and a pad). Returns the line count.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_area_name_block(const uint8_t* adv, int32_t width, int32_t difficulty,
    int32_t expansion_on, const char* name, const char* address, const char* version, uint8_t* out, int32_t cap) {
    Ctx x = {};
    c = &x;
    x.adv = adv; x.width = width; x.difficulty = (uint8_t)difficulty;
    x.name = name; x.address = address; x.version = version; x.out = (Rec*)out; x.cap = cap;
    x.st = { 1, 1, address ? 6u : 0u, 0, expansion_on != 0 ? 1u : 0u };
    D2ClientB61Env e{unitRoomStub, roomFlagStub, nop, nop, nop, h_game, h_level, h_version, h_difficulty, h_address, h_expansion};
    d2client_62680(&x.st, &e);
    int32_t n = x.n;
    c = nullptr;
    return n;
}
