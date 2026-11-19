// The D2Client host runtime, party-panel part. The shell hands over the live roster (one entry per player, own first) and
// the panel origin; this lays them out as retail's memory (roster list at D2Client+0x11BC14, +0x80 next; panel row list
// at +0x11BFD8, +0xCD next) and runs the 1.13c code on it: 0x772E0 (rebuild rows) -> 0x77170 (append a row) -> 0x769E0
// (the row's five button records), then 0x76B20 (the mouse tooltips) against a seam that records what retail would draw.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

extern "C" {
void d2_client_769e0(uint32_t entry, int32_t x_base, uint32_t unit, uint32_t g_bfe0, uint32_t g_bfe4,
                     int (*sub_4dd20)(void), uint32_t (*sub_4de20)(void), uint32_t (*sub_4ddd0)(void), uint32_t (*sub_4dd80)(void));
void d2_client_77170(uint32_t player, uint32_t tail_ebp, uint32_t* list_fd8, uint32_t (*ord10042)(uint32_t, int, int),
                     void (*sub_769e0)(uint32_t, uint32_t), uint32_t (*ord10265)(int), void (*ord10024)(uint32_t, uint32_t),
                     void (*exit_fn)(int));
}
struct D2ClientRows772e0Env { void (*clear)(); void (*row)(uint32_t y, uint32_t node); };
extern "C" void d2client_772e0_port(const uint32_t* headGlobal, const D2ClientRows772e0Env* env);
struct D2cGlobals76B20 { uint32_t curY, curX, cfg, viewX, viewY, player, roster, rows, color; };
struct D2cSeam76B20 {
    uint32_t (*room)(uint32_t); uint32_t (*color)(uint32_t, uint32_t, uint32_t); uint32_t (*roomTest)(uint32_t);
    uint32_t (*testA)(uint32_t); uint32_t (*testB)(uint32_t); uint32_t (*testC)(uint32_t); uint32_t (*testD)(uint32_t);
    void (*nameTip)(uint32_t); void (*frame)(uint32_t, uint32_t, uint32_t, uint32_t); int32_t (*center)(uint32_t, uint32_t);
    uint32_t (*str)(uint32_t); void (*text)(uint32_t, int32_t, int32_t, uint32_t, uint32_t);
};
extern "C" void d2c_76b20(const D2cGlobals76B20* g, const D2cSeam76B20* s);

namespace {
constexpr uint32_t kMaxRoster = 9, kEntry = 0x100, kRow = 0xD1, kMaxOps = 16;
struct Op { int32_t kind, a, b, c, d; };   // kind 0 frame(a,b,w=c,two=d); 1 line(id=a,x=b,y=c,colour=d); 2 nameTip(roster index=a)
struct Host {
    uint8_t roster[kMaxRoster][kEntry]; uint8_t rows[kMaxRoster][kRow]; uint8_t cfg[0x200];
    uint32_t flags[kMaxRoster]; uint32_t n = 0, nRows = 0, rowHead = 0, curEntry = 0, rosterHead = 0;
    uint32_t vx = 0, vy = 0, mx = 0, my = 0, colour = 0, inTown = 0, unit = 1;
    Op ops[kMaxOps]; uint32_t nOps = 0;
};
Host H;
inline uint32_t A(const void* p) { return uint32_t(uintptr_t(p)); }
inline void put32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
inline uint32_t flagsOf(uint32_t entry) { return entry >= A(H.roster) ? H.flags[(entry - A(H.roster)) / kEntry] : 0; }
// the four relation tests of D2Client+0x4DD20 / 0x4DE20 / 0x4DDD0 / 0x4DD80: bit 8 hostile, 1 loot, 2 hear, 4 squelch, own -> entry
int t4dd20() { return (flagsOf(H.curEntry) & 8) != 0; }
uint32_t t4de20() { return (flagsOf(H.curEntry) & 1) != 0; }
uint32_t t4ddd0() { return (flagsOf(H.curEntry) & 2) != 0; }
uint32_t t4dd80() { return (flagsOf(H.curEntry) & 4) != 0; }
uint32_t s4dd20(uint32_t e) { return (flagsOf(e) & 8) != 0; }
uint32_t s4dd80(uint32_t e) { return (flagsOf(e) & 4) != 0; }
uint32_t s4ddd0(uint32_t e) { return (flagsOf(e) & 2) != 0; }
uint32_t s4de20(uint32_t e) { return (flagsOf(e) & 1) != 0; }
uint32_t allocRow(uint32_t, int, int) { return A(H.rows[H.nRows]); }
void clearRows() { H.rowHead = 0; H.nRows = 0; }
void node769e0(uint32_t player, uint32_t row) {
    H.curEntry = player;
    d2_client_769e0(row, int32_t(0x5A + 0x26 * H.nRows), player, 1, 1, t4dd20, t4de20, t4ddd0, t4dd80);
}
uint32_t nid(int) { return 0; }
void nfat(uint32_t, uint32_t) { __builtin_trap(); }
void nexit(int) { __builtin_trap(); }
void append(uint32_t player) {
    uint32_t tail = H.rowHead;
    d2_client_77170(player, tail, &H.rowHead, allocRow, node769e0, nid, nfat, nexit);
    H.nRows++;
}
uint32_t room(uint32_t) { return 1; }
uint32_t roomTest(uint32_t) { return H.inTown; }
uint32_t colourFn(uint32_t, uint32_t, uint32_t) { return 0; }
void nameTip(uint32_t entry) { if (H.nOps < kMaxOps) H.ops[H.nOps++] = {2, int32_t((entry - A(H.roster)) / kEntry), 0, 0, 0}; }
void frame(uint32_t a, uint32_t b, uint32_t w, uint32_t two) { if (H.nOps < kMaxOps) H.ops[H.nOps++] = {0, int32_t(a), int32_t(b), int32_t(w), int32_t(two)}; }
// 0x75370 is w - width(string)/2 with the text width of the browser's font (Ordinal_10028); the host holds no font, so it records
// the centre column w (the frame op carries the same w) and the shell draws the line centred on it, which is that left edge.
int32_t center(uint32_t, uint32_t w) { return int32_t(w); }
uint32_t strId(uint32_t id) { return id; }
void text(uint32_t id, int32_t x, int32_t y, uint32_t c, uint32_t) { if (H.nOps < kMaxOps) H.ops[H.nOps++] = {1, int32_t(id), x, y, int32_t(c)}; }
}

extern "C" {
// Start a roster: panel origin, whether the loot box shows (the expansion bit of the game config), whether the room is a town.
EMSCRIPTEN_KEEPALIVE void d2_client_party_host_begin(int32_t originX, int32_t originY, int32_t lootShown, int32_t inTown) {
    memset(&H, 0, sizeof H); H.vx = uint32_t(originX); H.vy = uint32_t(originY); H.inTown = inTown ? 1 : 0;
    if (lootShown) H.cfg[0x1EB] = 4;
}
// One roster entry, the own player first: level, party id (0xFFFF none), the own relation flags toward it, player id.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_party_host_add(int32_t level, int32_t party, int32_t flags, int32_t id) {
    if (H.n >= kMaxRoster) return -1;
    uint8_t* e = H.roster[H.n]; uint32_t ea = A(e);
    put32(ea + 0x10, uint32_t(id)); put32(ea + 0x1c, uint32_t(flags)); e[0x20] = uint8_t(level); e[0x21] = uint8_t(level >> 8);
    e[0x22] = uint8_t(party); e[0x23] = uint8_t(party >> 8); memcpy(e + 0x66, "?", 2);
    if (H.n) put32(A(H.roster[H.n - 1]) + 0x80, ea);
    H.flags[H.n] = uint32_t(flags) & 0xf;
    return int32_t(H.n++);
}
// 1.13c D2Client+0x772E0: rebuild the panel rows; returns the row count.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_party_host_build() {
    H.rosterHead = H.n ? A(H.roster[0]) : 0; clearRows();
    const D2ClientRows772e0Env env = { clearRows, [](uint32_t, uint32_t node) { append(node); } };
    d2client_772e0_port(&H.rosterHead, &env);
    return int32_t(H.nRows);
}
// 1.13c D2Client+0x76B20 at the mouse point (800x600 screen coordinates); returns the number of recorded draws.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_party_host_tip(int32_t mx, int32_t my) {
    H.mx = uint32_t(mx); H.my = uint32_t(my); H.nOps = 0;
    uint32_t cfg = A(H.cfg);
    D2cGlobals76B20 g{A(&H.my), A(&H.mx), A(&cfg), A(&H.vx), A(&H.vy), A(&H.unit), A(&H.rosterHead), A(&H.rowHead), A(&H.colour)};
    // the globals are addresses of 32-bit words; cfg holds a pointer to the config block
    static uint32_t cfgWord; cfgWord = cfg; g.cfg = A(&cfgWord);
    D2cSeam76B20 s{room, colourFn, roomTest, s4dd20, s4dd80, s4ddd0, s4de20, nameTip, frame, center, strId, text};
    d2c_76b20(&g, &s);
    return int32_t(H.nOps);
}
EMSCRIPTEN_KEEPALIVE int32_t d2_client_party_host_op(int32_t i, int32_t field) {
    if (unsigned(i) >= H.nOps) return -1;
    const Op& o = H.ops[i];
    return field == 0 ? o.kind : field == 1 ? o.a : field == 2 ? o.b : field == 3 ? o.c : o.d;
}
// A built row's field, for the shell's hit tests: 0 y base (+0xC9), 1 level (+0xBD), 2 button kind k at 4+k.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_party_host_row(int32_t i, int32_t field) {
    if (unsigned(i) >= H.nRows) return -1;
    uint32_t r = A(H.rows[i]); uint32_t v;
    uint32_t off = field == 0 ? 0xC9 : field == 1 ? 0xBD : 0x24 + 0x20 * uint32_t(field - 2) + 0x10;
    memcpy(&v, (void*)(uintptr_t)(r + off), 4); return int32_t(v);
}
}
