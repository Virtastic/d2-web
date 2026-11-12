// 1.13c D2Client+0x51E80 live seam: the hover resolver (d2c_51e80) run over the
// browser's candidate rows instead of a retail unit graph. The rows are laid out as the unit records the walk reads
// (type +0, mode +0x10, flags +0xC4, next +0xE8; room list head +0x74), and every callee the walk makes is answered here
// from the row, each from the 1.13c disassembly:
//   0x51D10 cursor-blocked 0 | 0x51A80 stored target | 0xA68E0 row.hit | 0x51860 stores the winner | 0xC1F0 room | 0x1630/0x1660 head x/y
//   0xC2F8 identity | 0xC3DC one room | 0x51470 0 (type 5 only) | 0xA5440 d2_client_a5440 | 0xC538 #10648 (type 1, mode 0 or 12)
//   0xC1FC #10494 state 0x8F | 0xCD00 #11020 UNITS_IsDoor | 0x510B0 squared distance | 0x51230 / 0x512E0 door-between tests.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "cl_env.h"
extern "C" void d2c_51e80(const cl::Env& e, uint32_t g);
extern "C" uint32_t d2_client_a5440(uint32_t type, uint32_t mode, uint32_t flags);

struct D2cHoverRow {
    int32_t type;      // unit type: 0 player, 1 monster/NPC, 2 object, 4 item (3 missiles are never hover targets)
    int32_t mode;      // unit +0x10
    uint32_t flags;    // unit +0xC4
    int32_t x, y;      // #10141 position
    uint32_t hit;      // 0xA68E0 verdict for the pointer
    uint32_t attached; // #10494(unit, 0x8F)
    uint32_t isDoor;   // #11020
    uint32_t orient;   // ObjectTxt +0x13C (orientation byte), objects only
};
static_assert(sizeof(D2cHoverRow) == 36, "D2cHoverRow");

namespace {
constexpr int kMax = 256;
constexpr uint32_t kStride = 0x100;
struct World {
    alignas(16) uint8_t units[(kMax + 2) * kStride];   // [0..n) rows, [n] the player (head), [n+1] the room
    D2cHoverRow rows[kMax + 2];
    alignas(16) uint32_t g[16];
    uint32_t arr[2];
    uint32_t n, stored, result;
};
World W;
uint32_t A(uint32_t i) { return (uint32_t)(uintptr_t)&W.units[i * kStride]; }
const D2cHoverRow* row(uint32_t a) {
    uint32_t b = (uint32_t)(uintptr_t)W.units;
    if (a < b || a >= b + (kMax + 2) * kStride) return nullptr;
    return &W.rows[(a - b) / kStride];
}
uint32_t rd(void*, uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr(void*, uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
// 0x51230 / 0x512E0: D is a door in mode 0; the axis is y when ObjectTxt +0x13C == 1, else x. Player position P is (0,0) without a player.
// 0x51230 (D = candidate, B = best): 1 when D lies strictly between B and the player. 0x512E0 (D = best, U = candidate): 0 when D lies strictly between the player and U.
bool between(uint32_t lo, uint32_t m, uint32_t hi) { return (lo < m && m < hi) || (hi < m && m < lo); }
uint32_t axis(const D2cHoverRow* r, bool y) { return (uint32_t)(y ? r->y : r->x); }
uint32_t doorTest(bool want, const D2cHoverRow* D, const D2cHoverRow* O) {
    if (D && D->mode != 0) return 0;
    const D2cHoverRow* P = &W.rows[W.n];
    bool y = D && D->orient == 1;
    bool b = between(axis(O, y), axis(D, y), axis(P, y));
    return want ? (b ? 1u : 0u) : (b ? 0u : 1u);
}
uint32_t host(void*, uint32_t rva, uint32_t a, uint32_t c, uint32_t, uint32_t b, uint32_t s,
              uint32_t s0, uint32_t s1, uint32_t s2, uint32_t, uint32_t) {
    const D2cHoverRow* h = &W.rows[W.n];
    switch (rva) {
    case 0x51d10: wr(0, a, 0); wr(0, c, 0); return 0;
    case 0x51a80: return W.stored;
    case 0xa68e0: { const D2cHoverRow* r = row(s0); return r && r->hit; }
    case 0x51860: W.result = a; return 0;
    case 0xc1f0: return A(W.n + 1);
    case 0x1630: return (uint32_t)h->x;
    case 0x1660: return (uint32_t)h->y;
    case 0xc2f8: return 0;
    case 0xc3dc: wr(0, s1, (uint32_t)(uintptr_t)W.arr); wr(0, s2, 1); return 1;
    case 0x51470: return 0;
    case 0xa5440: { const D2cHoverRow* r = row(a); return r ? d2_client_a5440((uint32_t)r->type, (uint32_t)r->mode, r->flags) : 0; }
    case 0xc538: { const D2cHoverRow* r = row(s0); return r && r->type == 1 && (r->mode == 0 || r->mode == 12); }
    case 0xc1fc: { const D2cHoverRow* r = row(s0); return r && r->attached; }
    case 0xcd00: { const D2cHoverRow* r = row(s0); return r && r->isDoor; }
    case 0x510b0: { const D2cHoverRow *p = row(a), *q = row(b); if (!p || !q) return 0; int32_t dx = q->x - p->x, dy = q->y - p->y; return (uint32_t)(dx * dx + dy * dy); }
    case 0x51230: return doorTest(true, row(s), row(s0));
    case 0x512e0: return doorTest(false, row(s), row(s0));
    default: return 0;
    }
}
}  // namespace

// rows[0..n): hover candidates in the browser's encounter order (one room). head = the player's position. stored = index of the
// stored hover target (-1 none). type4Gate = [0xCC2F0]. Returns the index of the unit the resolver hands to 0x51860, or -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_hover_pick(const D2cHoverRow* rows, int32_t n, int32_t hx, int32_t hy,
                                                              int32_t stored, uint32_t type4Gate) {
    if (n < 0) n = 0;
    if (n > kMax) n = kMax;
    memset(&W, 0, sizeof W);
    W.n = (uint32_t)n;
    for (int32_t i = 0; i < n; i++) {
        W.rows[i] = rows[i];
        uint8_t* u = &W.units[i * kStride];
        uint32_t t = (uint32_t)rows[i].type, m = (uint32_t)rows[i].mode, nx = i + 1 < n ? A(i + 1) : 0;
        memcpy(u, &t, 4); memcpy(u + 0x10, &m, 4); memcpy(u + 0xc4, &rows[i].flags, 4); memcpy(u + 0xe8, &nx, 4);
    }
    W.rows[n].x = hx; W.rows[n].y = hy;                       // the head: a player (type 0), never in its own list
    uint8_t* room = &W.units[(n + 1) * kStride];
    uint32_t first = n ? A(0) : 0;
    memcpy(room + 0x74, &first, 4);
    W.arr[0] = A(n + 1);
    static const int32_t prio[8] = { 0, 0, -6, 0, -4, 0, 0, 2 };   // D2Client+0xF2190, read from the retail image
    W.g[0] = 0; W.g[1] = type4Gate; W.g[2] = A(n);
    memcpy(&W.g[8], prio, sizeof prio);
    W.stored = (stored >= 0 && stored < n) ? A((uint32_t)stored) : 0;
    cl::Env e = { nullptr, host, rd, wr, 0, 0 };
    d2c_51e80(e, (uint32_t)(uintptr_t)W.g);
    if (!W.result) return -1;
    return (int32_t)((W.result - A(0)) / kStride);
}
