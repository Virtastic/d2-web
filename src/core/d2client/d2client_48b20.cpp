// 1.13c D2Client+0x48B20: build the NPC dialog menu. void(void).
// Tells the server which NPC is being talked to (a 13-byte 0x38 packet) on the four interaction modes that need it, creates the
// menu control at the NPC's screen position, adds the NPC's name as the title row, then adds one row per entry of the NPC's
// menu-table row (string id + handler), with four special entries: 0x1507 (a formatted line), 0xD09 (shown only below a stat
// limit, with a count), 0xFB4 (shown only with a value, which also sets the row's entry count to 2 or 3), 0x2BA0 (gated on a
// flag check and the game mode). The closing row (string 0x102E) always follows. Every string and control call is a callee.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kT = 0xB2000000u;
constexpr uint32_t kCtlG = 0x119763, kPlayer = 0x11BBFC, kTable = 0xF3BA0, kCount = 0xF42F0, kFlags = 0x11973B;
constexpr uint32_t kStr = 0xCEACC, kStrcpy = 0xCEAB8, kStrlen = 0xCEAB0, kStrcat = 0xCEABC, kSprintf = 0xCEAB4;
constexpr uint32_t kW2U = 0xCEAC0, kWsprintf = 0xCF114, kAddRow = 0xA36F0;
constexpr uint32_t kOnSelect = kT | 0x48380, kOnClose = kT | 0x48350, kFmtD = kT | 0xD057C;
uint32_t str(const cl::Env& e, uint32_t id) { return e.H(kStr, 0, 0, 0, 0, 0, 0, id); }
void fatal(const cl::Env& e, uint32_t line) {                     // error box then exit(-1): not reachable under a vector
    const uint32_t m = e.H(0xBF92, line);
    e.H(0xBF80, kT | 0xD359F, m);
    e.H(0x3664, 0xFFFFFFFFu);
}
void addRow(const cl::Env& e, uint32_t eax, uint32_t text, uint32_t font, uint32_t fn, uint32_t show) {
    volatile uint32_t a[6] = { text, font, 0, 1, fn, show };
    e.Xs(show);
    e.H(kAddRow, text, font, 0, 1, fn, eax, e.R(G(kCtlG)));
    if (e.blk) e.blk(e.ctx, kAddRow, P(a), 6);
}
// strcpy(buf, text-of-n): "%d" into a char buffer, widened, appended (0xD09 count and 0xFB4 value)
void appendNumber(const cl::Env& e, uint32_t buf, uint32_t n) {
    volatile uint16_t small[10] = {};
    volatile char cb[0x1C] = {};
    e.H(kWsprintf, P(cb), kFmtD, n);
    e.H(kW2U, 9, 0, 0, 0, 0, 0, P(small), P(cb));
    e.H(kStrcat, 0, 0, 0, 0, 0, 0, buf, P(small));
}
}
extern "C" void d2c_48b20(const cl::Env& e) {
    if (e.R(G(kCtlG))) { fatal(e, 0xC74); return; }
    if (!e.H(0x46150)) { e.H(0x48150); return; }
    const uint32_t mode = e.R(G(0x119725));
    if (mode == 0xFC || mode == 0xC6 || mode == 0x203 || mode == 0x96) {
        const uint32_t pl = e.R(G(kPlayer));
        const uint32_t pid = pl ? e.R(pl + 0xC) : 0xFFFFFFFFu, gid = e.R(G(0x11971D));
        volatile uint8_t pkt[16] = { 0x38, 3, 0, 0, 0 };
        for (uint32_t i = 0; i < 4; ++i) { pkt[5 + i] = (uint8_t)(gid >> 8 * i); pkt[9 + i] = (uint8_t)(pid >> 8 * i); }
        e.H(0x143E0, P(pkt), 0, 0, 0, 0, 0, 0, 0, 0xD);
        if (e.blk) e.blk(e.ctx, 0x143E0, P(pkt), 4);
    }
    volatile uint32_t xy[2] = { 0, 0 };
    e.H(0x46880, 0, 0, 0, 0, 0, 0, 0, 0, 0, P(xy));
    const uint32_t x = xy[0], y = xy[1];
    volatile uint32_t ca[11] = { x, y, kOnSelect, kOnClose, 0, 0, 1, 0, 0, 0, 1 };
    e.Xs(0, 1);
    const uint32_t ctl = e.H(0xA3E40, x, y, kOnSelect, kOnClose, 0);
    if (e.blk) e.blk(e.ctx, 0xA3E40, P(ca), 11);
    e.W(G(kCtlG), ctl);
    if (!ctl) { fatal(e, 0x529); return; }
    e.W(ctl + 0x5C, 1);
    {
        volatile uint16_t title[0x50] = {};                          // Unicode ctor: each char := 0
        const uint32_t npc = e.H(0x46150);
        const uint32_t name = e.H(0xA5D90, 0, 0, 0, 0, 0, npc);
        e.H(kStrcpy, 0, 0, 0, 0, 0, 0, P(title), name);
        e.H(kStrlen, 0, 0, 0, 0, 0, 0, P(title));
        addRow(e, 4, P(title), 0x15, 0, 0);
    }
    const uint32_t npc = e.H(0x46150);
    if (!npc) return;
    const uint32_t id = e.R(npc + 4);
    const int32_t count = (int32_t)e.R(G(kCount));
    int32_t idx = 0;
    while (idx < count && e.R(G(kTable) + (uint32_t)idx * 0x27) != id) ++idx;
    if (idx >= count) idx = 0;                                       // not found: row 0
    if (idx >= count) return;
    const uint32_t row = G(kTable) + (uint32_t)idx * 0x27;
    e.W(G(0x119663), 1);
    for (int32_t j = 0; j < (int32_t)e.R(row + 4) - 1; ++j) {
        volatile uint16_t buf[0x200] = {};
        const uint32_t b = P(buf);
        const uint32_t s = e.R(row + 8 + 2 * (uint32_t)j) & 0xFFFF;
        uint32_t show = 1;
        if (s == 0x1507) {
            uint32_t k = str(e, 0x2B0D);
            const uint32_t lang = e.R(G(0xF23E8));
            if ((lang & 0xFFFF) != 0x421) k = str(e, lang);
            const uint32_t fmt = str(e, 0x58A8);
            volatile uint32_t a[5] = { 0x200, b, fmt, k, e.R(G(0x11C334)) };
            e.H(kSprintf, a[0], a[1], a[2], a[3], a[4]);
            if (e.blk) e.blk(e.ctx, kSprintf, P(a), 5);
        } else if (s == 0xD09) {
            const uint32_t pl = e.R(G(kPlayer));
            const int32_t have = (int32_t)e.H(0xC1C6, pl, 6, 0);
            if (have >= (int32_t)e.H(0xC196, pl)) continue;
            uint32_t n = e.H(0xCDFC, e.R(G(kPlayer)));
            if (!n) n = 1;
            e.H(kStrcpy, 0, 0, 0, 0, 0, 0, b, str(e, s));
            appendNumber(e, b, n);
            show = n ? 1 : 0;
        } else if (s == 0xFB4) {
            const bool none = !e.H(0xC58C, e.R(G(kFlags)), 4, 0) && !e.H(0xC58C, e.R(G(kFlags)), 4, 1);
            const uint32_t v = e.H(0xCDF0, e.R(G(kPlayer)));
            if (!v) { e.W(row + 4, 2); continue; }
            e.W(row + 4, 3);
            if (none) {
                e.H(kStrcpy, 0, 0, 0, 0, 0, 0, b, str(e, 0xFB5));
                appendNumber(e, b, v * 100);
            } else {
                e.H(kStrcpy, 0, 0, 0, 0, 0, 0, b, str(e, 0xFB4));
            }
        } else if (s == 0x2BA0) {
            if (e.H(0xC58C, e.R(G(kFlags)), 0x29, 0)) continue;
            if (e.H(0xC58C, e.R(G(kFlags)), 0x29, 1) != 1) {
                if (e.R8(G(0x11C390)) != 2) continue;
                if (e.H(0xC58C, e.R(G(kFlags)), 0x29, 1)) continue;
            }
            e.H(kStrcpy, 0, 0, 0, 0, 0, 0, b, str(e, s));
        } else {
            e.H(kStrcpy, 0, 0, 0, 0, 0, 0, b, str(e, s));
        }
        addRow(e, 0, b, 0xF, e.R(row + 0x12 + 4 * (uint32_t)j), show);
    }
    addRow(e, 0, str(e, 0x102E), 0xF, kOnSelect, 1);
}
