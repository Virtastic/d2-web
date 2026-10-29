// Engine-held draw-list grid ctx (the EDI struct of D2Client 7A9F0) and its first fillers.
// Guest addresses are byte offsets into g_ctx_arena (retail 32-bit layout, offset 0 = null).
#include <string>
#include <cstdio>
#include <initializer_list>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static std::string g_gout;
namespace {
constexpr uint32_t kCtx = 0x40;          // ctx base inside the arena (keeps 0 as null)
constexpr uint32_t kSq = kCtx + 0x38;    // node pool header (flags word, then 0x14-byte nodes)
constexpr uint32_t kCells = 0xEAC0 + kCtx + 0x40;  // cell array start
constexpr uint32_t kMaxN = 64;
constexpr uint32_t kUnitRec = kCells + kMaxN * kMaxN * 0x24 + 4096 * 0x30;  // fake unit records: [type, classId]
alignas(8) uint8_t g_arena[kUnitRec + 1024 * 8];

inline uint32_t rd(uint32_t o) { uint32_t v; memcpy(&v, g_arena + o, 4); return v; }
inline void wr(uint32_t o, uint32_t v) { memcpy(g_arena + o, &v, 4); }
}  // namespace

// 1.13c D2Client+0x7A040: tile (tx,ty) inside the N*N window anchored at sq+0xEA68/0xEA6C.
static bool d2c_7a040(int32_t tx, int32_t ty, uint32_t sq) {
    int32_t dx = tx - (int32_t)rd(sq + 0xEA68), dy = ty - (int32_t)rd(sq + 0xEA6C);
    int32_t n = (int32_t)rd(sq + 0xEA78);
    return dx >= 0 && dy >= 0 && dx < n && dy < n;
}

// 1.13c D2Client+0x79C80: append a kind-1 node for tile-entry `entry` to the tail of cell+0x10.
// subX/subY = D2Common #10561 room origin (the two words the retail code leaves in ESI/EBP).
static void d2c_79c80(uint32_t cell, uint32_t entry, int32_t subX, int32_t subY, uint32_t mask3) {
    uint32_t sq = kSq, n = rd(sq + 0xEA64);
    wr(sq + 0xEA64, n + 1);
    if (n >= 3000) return;
    uint32_t node = sq + 4 + n * 0x14;
    wr(node, 1);
    wr(node + 0xC, entry);
    wr(node + 0x10, 0);
    wr(node + 4, (uint32_t)((subX + (int32_t)rd(entry + 8) * 5) * 8));
    wr(node + 8, (uint32_t)((subY + (int32_t)rd(entry + 0xC) * 5) * 8));
    // entry +0/+4 = the tile's iso screen position; relation read off every flag-1 node of the 8B110 trace (Z rows):
    // o0 = (X-Y)*80 - 80, o1 = (X+Y)*40 + 80, X/Y = node/40. Retail sets them at tile load (not recovered).
    int32_t X = (subX + (int32_t)rd(entry + 8) * 5) / 5, Y = (subY + (int32_t)rd(entry + 0xC) * 5) / 5;
    wr(entry, (uint32_t)((X - Y) * 80 - 80)); wr(entry + 4, (uint32_t)((X + Y) * 40 + 80));
    wr(sq, rd(sq) | mask3);
    uint32_t p = rd(cell + 0x10);
    if (!p) { wr(cell + 0x10, node); return; }
    while (rd(p + 0x10)) p = rd(p + 0x10);
    wr(p + 0x10, node);
}

// Shared node alloc (kind, entry) from the pool; 0 when the 3000-node pool is full (counter still bumps).
static uint32_t d2c_node(uint32_t sq, uint32_t kind, uint32_t entry) {
    uint32_t n = rd(sq + 0xEA64);
    wr(sq + 0xEA64, n + 1);
    if (n >= 3000) return 0;
    uint32_t node = sq + 4 + n * 0x14;
    wr(node, kind); wr(node + 0xC, entry); wr(node + 0x10, 0);
    return node;
}
inline uint32_t zl(uint32_t node) { return (rd(rd(node + 0xC) + 0x14) >> 14) & 7; }
// Insert `node` into the list at headp, ordered by entry layer (flags>>14&7). Replicates retail quirk:
// the last node of a list is never compared, so a new node ties/lands after it.
static void d2c_sorted_insert(uint32_t headp, uint32_t node) {
    uint32_t cur = rd(headp);
    if (!cur) { wr(headp, node); return; }
    uint32_t prev = 0;
    for (uint32_t nx = rd(cur + 0x10); nx; nx = rd(nx + 0x10)) {
        if (zl(node) < zl(cur)) {
            wr(node + 0x10, cur);
            if (!prev) wr(headp, node); else wr(prev + 0x10, node);
            break;
        }
        prev = cur; cur = nx;
    }
    if (!rd(cur + 0x10)) wr(cur + 0x10, node);
}
static void d2c_set_xy(uint32_t node, uint32_t entry, int32_t subX, int32_t subY) {
    wr(node + 4, (uint32_t)((subX + (int32_t)rd(entry + 8) * 5) * 8));
    wr(node + 8, (uint32_t)((subY + (int32_t)rd(entry + 0xC) * 5) * 8));
    int32_t X = (subX + (int32_t)rd(entry + 8) * 5) / 5, Y = (subY + (int32_t)rd(entry + 0xC) * 5) / 5;   // entry iso position, as 79C80
    wr(entry, (uint32_t)((X - Y) * 80 - 80)); wr(entry + 4, (uint32_t)((X + Y) * 40 + 80));
}

// 1.13c D2Client+0x79D30: kind-1 node into cell+0x1C, layer-sorted; sq mask bit1 (gdwBitMasks[1]).
static void d2c_79d30(uint32_t cell, uint32_t entry, int32_t subX, int32_t subY) {
    uint32_t sq = kSq, node = d2c_node(sq, 1, entry);
    if (!node) return;
    d2c_set_xy(node, entry, subX, subY);
    wr(sq, rd(sq) | 2);
    d2c_sorted_insert(cell + 0x1C, node);
}

// 1.13c D2Client+0x79E20: as 79D30 into cell+0x20, mask bit0 (gdwBitMasks[0]); first sets cell flag 4
// when entry+0x1C is not 0/0xD and entry+0x14 bit2 is clear (the flag is set even if the pool is full).
static void d2c_79e20(uint32_t cell, uint32_t entry, int32_t subX, int32_t subY) {
    uint32_t t = rd(entry + 0x1C);
    if (t && t != 0xD && !(rd(entry + 0x14) & 4)) wr(cell, rd(cell) | 4);
    uint32_t sq = kSq, node = d2c_node(sq, 1, entry);
    if (!node) return;
    d2c_set_xy(node, entry, subX, subY);
    wr(sq, rd(sq) | 1);
    d2c_sorted_insert(cell + 0x20, node);
}

// Player/tile globals read by 79F20 (D2Client+0x924F4 mode, +0xBC3B18 x, +0xBC3B1C z, +0xBC3B20 y).
struct HideG { int32_t mode, px, pz, py; };
static HideG g_hide;

// 1.13c D2Client+0x79F20: hide predicate. subX/subY = Ordinal_10561 room origin.
static bool d2c_79f20(uint32_t entry, int32_t subX, int32_t subY) {
    if (g_hide.mode) {
        uint32_t p = rd(entry + 0x10);
        if (!p || (int32_t)rd(p + 0x28) == g_hide.pz) return false;
        return g_hide.px < (int32_t)rd(p) || g_hide.py < (int32_t)rd(p + 4);
    }
    int32_t x = subX + (int32_t)rd(entry + 8) * 5, y = subY + (int32_t)rd(entry + 0xC) * 5;
    uint32_t k = rd(entry + 0x1C);
    if (k < 32) {
        if (g_hide.px * 5 < x && x - 0x14 < g_hide.px * 5 && ((0x15B2u >> k) & 1)) return true;  // 1,4,5,7,8,10,12
    }
    if (g_hide.py * 5 < y && y - 0x14 < g_hide.py * 5 && k < 32 && ((0x1ACCu >> k) & 1)) return true;  // 2,3,6,7,9,11,12
    return false;
}

// 1.13c D2Client+0x7A340: kind-1 node into cell+0x14 with the fade state machine; `now` = GetTickCount().
static void d2c_7a340(uint32_t cell, uint32_t entry, int32_t subX, int32_t subY, uint32_t now) {
    uint32_t t = rd(entry + 0x1C);
    if (t && t != 0xD && !(rd(entry + 0x14) & 4)) wr(cell, rd(cell) | 4);
    uint32_t node = d2c_node(kSq, 1, entry);
    if (!node) return;
    int32_t at = (int32_t)(now + 500);
    if (!(rd(entry + 0x14) & 4)) {
        bool hide = d2c_79f20(entry, subX, subY);
        uint32_t f = rd(entry + 0x24);
        int32_t b = g_arena[entry + 0x28];
        if (!hide && (f & 1)) {
            g_arena[entry + 0x29] = 0x80; g_arena[entry + 0x2A] = 0xFF;
            wr(entry + 0x2C, (uint32_t)(b == 0x80 ? at : ((0x80 - b) * 500) / 0x7F + at));
            wr(entry + 0x24, (f & ~1u) | 2);
        } else if (hide && !(f & 1)) {
            g_arena[entry + 0x29] = 0xFF; g_arena[entry + 0x2A] = 0x80;
            wr(entry + 0x2C, (uint32_t)(b == 0xFF ? at : ((b - 0xFF) * 500) / 0x7F + at));
            wr(entry + 0x24, f | 3);
        }
    }
    d2c_set_xy(node, entry, subX, subY);
    d2c_sorted_insert(cell + 0x14, node);
}

// 1.13c D2Client+0x7A500: kind-0 (unit) node into cell+0x18, appended. posX/posY = the unit's
// subtile position (path+0xC/+0x10 for types 2,4,5, else Ordinal_10867/10750), 0 when no path.
static void d2c_7a500(uint32_t cell, uint32_t unit, int32_t posX, int32_t posY) {
    uint32_t node = d2c_node(kSq, 0, unit);
    if (!node) return;
    wr(node + 4, (uint32_t)(posX * 8));
    wr(node + 8, (uint32_t)(posY * 8));
    uint32_t p = rd(cell + 0x18);
    if (!p) { wr(cell + 0x18, node); return; }
    while (rd(p + 0x10)) p = rd(p + 0x10);
    wr(p + 0x10, node);
}

// 1.13c D2Client+0x7A180 / +0x7A260: unit node (kind 2 / kind 0) appended at the tail of cell+0x10,
// sq mask bit3 (gdwBitMasks[3]). posX/posY as 7A500. Same body, only the node kind differs.
static void d2c_unit_tail10(uint32_t cell, uint32_t unit, int32_t posX, int32_t posY, uint32_t kind) {
    uint32_t node = d2c_node(kSq, kind, unit);
    if (!node) return;
    wr(node + 4, (uint32_t)(posX * 8));
    wr(node + 8, (uint32_t)(posY * 8));
    wr(kSq, rd(kSq) | 8);
    uint32_t p = rd(cell + 0x10);
    if (!p) { wr(cell + 0x10, node); return; }
    while (rd(p + 0x10)) p = rd(p + 0x10);
    wr(p + 0x10, node);
}
static void d2c_7a180(uint32_t c, uint32_t u, int32_t x, int32_t y) { d2c_unit_tail10(c, u, x, y, 2); }
static void d2c_7a260(uint32_t c, uint32_t u, int32_t x, int32_t y) { d2c_unit_tail10(c, u, x, y, 0); }

// 1.13c D2Client+0xA6180: unit skip predicate. monBit = bit 0x14 of the monster-type row for unit[1]
// (D2Client 0x1510), objFlags = byte +0x1B7 of Ordinal_10688(unit[1]); caller resolves both.
static bool d2c_a6180(uint32_t unit, uint32_t monBit, uint32_t objFlags) {
    if (!unit) return false;
    uint32_t type = rd(unit), mode = rd(unit + 0x10);
    switch (type) {
    case 0: return mode == 0x11;
    case 1: return !monBit && mode == 0xC;
    case 2: return (objFlags & 2) || (mode == 2 && (objFlags & 1));
    case 3: return (rd(unit + 0xC4) & 0x10000) != 0;
    case 4: return mode == 3;
    }
    return false;
}

extern "C" {
// Reset the engine-held ctx with an N*N grid anchored at (ox,oy); returns the arena base.
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_reset(int32_t n, int32_t ox, int32_t oy) {
    if (n < 1 || n > (int32_t)kMaxN) return 0;
    memset(g_arena, 0, sizeof g_arena);
    wr(kCtx + 0xEAA8, kCells);
    wr(kCtx + 0xEAAC, (uint32_t)(n * n));
    wr(kCtx + 0xEAB0, (uint32_t)n);
    wr(kSq + 0xEA78, (uint32_t)n);
    wr(kSq + 0xEA68, (uint32_t)ox);
    wr(kSq + 0xEA6C, (uint32_t)oy);
    for (int32_t i = 0; i < n * n; i++) {
        wr(kCells + i * 0x24 + 8, (uint32_t)(ox + i % n));
        wr(kCells + i * 0x24 + 0xC, (uint32_t)(oy + i / n));
    }
    return kCtx;
}
// Test seam: 7A040 / 79C80 over the engine-held ctx. entry_xy = entry[8], entry[0xC] (nPosX, nPosY).
EMSCRIPTEN_KEEPALIVE int32_t d2_dg_7a040(int32_t tx, int32_t ty) { return d2c_7a040(tx, ty, kSq); }
EMSCRIPTEN_KEEPALIVE int32_t d2_dg_79c80(int32_t cell, int32_t px, int32_t py, int32_t subX, int32_t subY) {
    if (cell < 0 || cell >= (int32_t)rd(kCtx + 0xEAAC)) return -1;
    uint32_t entry = kCells + kMaxN * kMaxN * 0x24 - 0x30;  // scratch entry at arena tail
    wr(entry + 8, (uint32_t)px);
    wr(entry + 0xC, (uint32_t)py);
    d2c_79c80(kCells + cell * 0x24, entry, subX, subY, 8);
    return (int32_t)rd(kSq + 0xEA64);
}

// ---- staged engine state for the room walk (flattened by the session feeder; arena offsets, 0x30-byte tile entries) ----
namespace {
struct StUnit { uint32_t unit; int32_t cls, type, sx, sy; uint32_t a6180, bit20, bit7; };
struct StRoom { int32_t r5c, r60, r64, r68, subX, subY; uint32_t nA, nB, nU, a0, b0, u0; };
constexpr uint32_t kStageTiles = kCells + kMaxN * kMaxN * 0x24;
constexpr uint32_t kMaxTiles = 4096, kMaxRooms = 256, kMaxUnits = 1024;
uint8_t g_stage[kMaxTiles * 0x30];
struct { int32_t slack, roofH; } g_tileInfo[kMaxTiles];  // D2CMP #10083 return, u16 pTile+4
StRoom g_rooms[kMaxRooms]; StUnit g_units[kMaxUnits];
uint32_t g_nTiles, g_nRooms, g_nUnits, g_roomNodes[kMaxRooms];
int32_t g_view[4], g_ox, g_oy; uint32_t g_now, g_skipUnits;
}
static_assert(kStageTiles + kMaxTiles * 0x30 > 0, "stage lives after the cells in the arena");

// 1.13c D2Client+0x7A5D0 (room callback of D2Common #11152). Fed by staged rooms; Ordinal_10013 return taken as 0.
static bool d2c_7a5d0(uint32_t ri) {
    StRoom& r = g_rooms[ri];
    int32_t x0 = ((r.r5c - r.r60) - r.r68) * 0x50 - 0x50, y0 = (r.r60 + 2 + r.r5c) * 0x28;
    int32_t x1 = ((r.r5c - r.r60) + r.r64) * 0x50 - 0x50, y1 = (r.r60 + r.r5c + r.r68 + 2 + r.r64) * 0x28;
    const int32_t *f = g_view;  // f[0..3] = left, top, right, bottom
    if (x0 > f[2] || y0 > f[3] + 400 || x1 < f[0] - 200 || y1 < f[1]) return true;
    uint32_t base = rd(kSq + 0xEA64);
    int32_t n = (int32_t)rd(kCtx + 0xEAB0);
    for (int pass = 0; pass < 2; pass++) {
        uint32_t cnt = pass ? r.nB : r.nA, first = pass ? r.b0 : r.a0;
        for (uint32_t k = 0; k < cnt; k++) {
            uint32_t ti = first + k, e = kStageTiles + ti * 0x30;
            memcpy(g_arena + e, g_stage + ti * 0x30, 0x30);
            // entry +8/+0xC are room-relative tile coords (D2DrlgTileDataStrc nPosX/nPosY, as 79C80 reads them); iso pixel = world tile (x-y)*80, (x+y)*40.
            int32_t wx = r.r5c + (int32_t)rd(e + 8), wy = r.r60 + (int32_t)rd(e + 0xC);
            int32_t ex = (wx - wy) * 0x50 - 0x50, ey = (wx + wy) * 0x28 + 0x50;
            int32_t dy = g_tileInfo[ti].slack, top = ey - (pass ? 0 : g_tileInfo[ti].roofH);
            int32_t a, b, o[2];
            auto fl = [](int32_t v) { return v >= 0 ? v / 160 : -((-v + 159) / 160); };
            a = fl(ex + 2 * ey); b = fl(2 * ey - ex); o[0] = a; o[1] = b - 1;
            if (ex > f[2] || f[0] > ex + 0xA0) continue;
            if (top + dy > f[3] || f[1] > top - dy) continue;
            if (!d2c_7a040(o[0], o[1], kSq)) continue;
            uint32_t cell = kCells + (uint32_t)(n * (o[1] - g_oy) + (o[0] - g_ox)) * 0x24;
            if (pass) { d2c_79c80(cell, e, r.subX, r.subY, 8); continue; }
            uint32_t fl14 = rd(e + 0x14);
            if (!(fl14 & 0x1C000)) d2c_79c80(cell, e, r.subX, r.subY, 8);
            else {
                uint32_t t = rd(e + 0x1C);
                if (t == 0xF) d2c_79d30(cell, e, r.subX, r.subY);
                else if (t >= 0x10 && t <= 0x13) d2c_79e20(cell, e, r.subX, r.subY);
                else d2c_7a340(cell, e, r.subX, r.subY, g_now);
            }
        }
    }
    if (!g_skipUnits) for (uint32_t k = 0; k < r.nU; k++) {
        StUnit& u = g_units[r.u0 + k];
        auto fl = [](int32_t v) { return v >= 0 ? v / 160 : -((-v + 159) / 160); };
        int32_t px = (u.sx - u.sy) * 16, py = (u.sx + u.sy) * 8;
        int32_t tx = fl(px + 2 * py), ty = fl(2 * py - px) - 1;   // ty-1 as the tile passes (8B110 Z rows: every unit sits one row above the unit-spec reading)
        if (!d2c_7a040(tx, ty, kSq)) continue;
        uint32_t cell = kCells + (uint32_t)(n * (ty - g_oy) + (tx - g_ox)) * 0x24;
        if (u.a6180 || u.bit20) d2c_7a260(cell, u.unit, u.sx, u.sy);
        else { d2c_7a500(cell, u.unit, u.sx, u.sy); if (u.bit7) d2c_7a180(cell, u.unit, u.sx, u.sy); }
    }
    g_roomNodes[ri] = rd(kSq + 0xEA64) - base;
    return true;
}

// 1.13c D2Client+0x7A9F0: size the grid (7A0D0), then walk the staged rooms in #11152 order.
// Ordinal_10013 ("big" mode) is taken as 0.
EMSCRIPTEN_KEEPALIVE void d2_dg_stage_reset() { g_nTiles = g_nRooms = g_nUnits = 0; }
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_stage_tile(uint32_t type, uint32_t flags, int32_t px, int32_t py, int32_t slack, int32_t roofH) {
    if (g_nTiles >= kMaxTiles) return 0;
    uint8_t* e = g_stage + g_nTiles * 0x30; memset(e, 0, 0x30); e[0x28] = 0xFF;
    memcpy(e + 8, &px, 4); memcpy(e + 0xC, &py, 4); memcpy(e + 0x14, &flags, 4); memcpy(e + 0x1C, &type, 4);
    g_tileInfo[g_nTiles] = {slack, roofH};
    return g_nTiles++;
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_stage_unit(uint32_t unit, int32_t type, int32_t sx, int32_t sy, uint32_t a6180, uint32_t bit20, uint32_t bit7) {
    if (g_nUnits >= kMaxUnits) return 0;
    uint32_t rec = kUnitRec + g_nUnits * 8;   // unit arg = classId; the node's unit pointer is the record (written in d2_dg_frame, after the arena reset)
    g_units[g_nUnits] = {rec, (int32_t)unit, type, sx, sy, a6180, bit20, bit7};
    return g_nUnits++;
}
EMSCRIPTEN_KEEPALIVE void d2_dg_stage_room(int32_t r5c, int32_t r60, int32_t r64, int32_t r68, int32_t subX, int32_t subY,
                                           uint32_t nA, uint32_t a0, uint32_t nB, uint32_t b0, uint32_t nU, uint32_t u0) {
    if (g_nRooms < kMaxRooms) g_rooms[g_nRooms++] = {r5c, r60, r64, r68, subX, subY, nA, nB, nU, a0, b0, u0};
}
// view = world rect (l,t,r,b), ox/oy = grid origin from #10512 on ctx+0x24/0x28 (a-3, b-ctx[0xEAB4]); n = grid side.
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_frame(int32_t l, int32_t t, int32_t r, int32_t b, int32_t ox, int32_t oy, int32_t n,
                                          uint32_t skipUnits, uint32_t now) {
    if (n < 1 || n > (int32_t)kMaxN) return 0;
    d2_dg_reset(n, ox, oy);
    g_view[0] = l; g_view[1] = t; g_view[2] = r; g_view[3] = b; g_ox = ox; g_oy = oy; g_skipUnits = skipUnits; g_now = now;
    for (uint32_t i = 0; i < g_nUnits; i++) { wr(g_units[i].unit, (uint32_t)g_units[i].type); wr(g_units[i].unit + 4, (uint32_t)g_units[i].cls); }
    for (uint32_t i = 0; i < g_nRooms; i++) { g_roomNodes[i] = 0; d2c_7a5d0(i); }
    return rd(kCtx + 0xEAAC);
}
// Z rows in the 8B110 capture format (the retail capture tool/main.cpp sessGridLine), for cell-by-cell diffs against the retail captures
EMSCRIPTEN_KEEPALIVE const char* d2_dg_dump_z() {
    g_gout.clear(); char b[48]; uint32_t n = rd(kCtx + 0xEAB0);
    for (uint32_t i = 0; i < n * n; i++) { uint32_t c = kCells + i * 0x24, l = rd(c + 0x10), p14 = rd(c + 0x14); if (!l && !p14) continue;
        std::snprintf(b, sizeof b, "c%x p14=%d", i, p14 != 0); g_gout += b;
        for (uint32_t nd = l, k = 0; nd && k < 256; nd = rd(nd + 0x10), k++) { uint32_t o = rd(nd + 0xC);
            std::snprintf(b, sizeof b, " %x:%x:%x:%x:%x", rd(nd), rd(nd + 4), rd(nd + 8), rd(o), rd(o + 4)); g_gout += b; }
        g_gout += '\n'; }
    return g_gout.c_str();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_room_nodes(uint32_t i) { return i < g_nRooms ? g_roomNodes[i] : 0; }
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_cell_count() { return rd(kCtx + 0xEAAC); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_node_count() { return rd(kSq + 0xEA64); }
EMSCRIPTEN_KEEPALIVE void d2_dg_hide_globals(int32_t mode, int32_t px, int32_t pz, int32_t py) { g_hide = {mode, px, pz, py}; }
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_arena() { return (uint32_t)(uintptr_t)g_arena; }
EMSCRIPTEN_KEEPALIVE uint32_t d2_dg_cells() { return kCells; }
EMSCRIPTEN_KEEPALIVE void d2_dg_79d30(uint32_t cell, uint32_t entry, int32_t sx, int32_t sy) { d2c_79d30(cell, entry, sx, sy); }
EMSCRIPTEN_KEEPALIVE void d2_dg_79e20(uint32_t cell, uint32_t entry, int32_t sx, int32_t sy) { d2c_79e20(cell, entry, sx, sy); }
EMSCRIPTEN_KEEPALIVE int32_t d2_dg_79f20(uint32_t entry, int32_t sx, int32_t sy) { return d2c_79f20(entry, sx, sy); }
EMSCRIPTEN_KEEPALIVE void d2_dg_7a340(uint32_t cell, uint32_t entry, int32_t sx, int32_t sy, uint32_t now) { d2c_7a340(cell, entry, sx, sy, now); }
EMSCRIPTEN_KEEPALIVE void d2_dg_7a500(uint32_t cell, uint32_t unit, int32_t px, int32_t py) { d2c_7a500(cell, unit, px, py); }
EMSCRIPTEN_KEEPALIVE void d2_dg_7a180(uint32_t c, uint32_t u, int32_t x, int32_t y) { d2c_7a180(c, u, x, y); }
EMSCRIPTEN_KEEPALIVE void d2_dg_7a260(uint32_t c, uint32_t u, int32_t x, int32_t y) { d2c_7a260(c, u, x, y); }
EMSCRIPTEN_KEEPALIVE int32_t d2_dg_a6180(uint32_t u, uint32_t monBit, uint32_t objFlags) { return d2c_a6180(u, monBit, objFlags); }

// G-stream walk of the built grid: 8B110's tail, D2Client+0x9F270 (floor nodes -> Ordinal_10069) then +0x9EE90 (tile timers -> Ordinal_10001/10058),
// each env seam recording the D2Gfx call in retail's G format. ecx/edx leftovers are not recoverable from C: written 0.
// Units are skipped by the feeder, so 666D0 (-> 6BA90) never runs; 1BA90/9E970 are silent stubs. 9EE90's colour tables are retail .data: counts read 0,
// which the record does not carry (colours print as a pointer).
static uint32_t g_r13 = 0, g_ledx;   // edx a Ordinal_10069 call left behind: the next 10013 reads it (8B110 trace, the retail captures)
static void gRec(int ord, std::initializer_list<uint32_t> a, uint32_t ecx = 0, uint32_t edx = 0) {
    char b[40]; std::snprintf(b, sizeof b, "%d %x,%x", ord, ecx, edx); g_gout += b;
    for (uint32_t v : a) { if (v >= 0x100000u) g_gout += ",*"; else { std::snprintf(b, sizeof b, ",%x", v); g_gout += b; } }
    g_gout += '\n';
}
struct D2ClientCellWalk9f270Env { uint32_t arg; void (*sub666d0)(uint32_t); uint32_t (*ord10013)(); void (*sub1ba90)();
    void (*ord10069)(uint32_t, int32_t, int32_t, uint32_t, uint32_t); void (*sub9e970)(uint32_t, uint32_t, uint32_t); };
// D2Client+0x9E970 over the arena's cell+0x14 list (overlay_list_9e970.cpp). Draw tables are retail .data (counts read 0, as 9EE90), colours not carried.
struct D2Client9E970Node; struct D2Client9E970Entry;
struct D2Client9E970Env { uint32_t colorMask, tickGate; const uint32_t *perCelCount, *tabNormal, *tabAlt; uint32_t ctxX, ctxY, ctxOffX, ctxOffY;
    uint32_t (*ord10013)(); uint32_t (*getTick)(); void (*fatal)(uint32_t); uint32_t (*loadColor)(uint32_t, uint8_t*);
    uint32_t (*draw)(uint32_t, int32_t, int32_t, const uint32_t*, uint32_t); uint32_t (*drawAlpha)(uint32_t, int32_t, int32_t, const uint32_t*, uint32_t, uint32_t);
    D2Client9E970Entry* (*entry)(uint32_t); uint32_t (*celIndex)(uint32_t); };
extern "C" void d2_client_9e970(D2Client9E970Env&, D2Client9E970Node*, uint32_t, D2Client9E970Node* (*)(uint32_t));
struct D2c9EE90Env { uint32_t (*rd32)(uint32_t); uint8_t (*rd8)(uint32_t); void (*wr32)(uint32_t, uint32_t); void (*wr8)(uint32_t, uint8_t);
    int32_t (*ord10013)(); uint32_t (*getTick)(); uint32_t (*ord10265)(uint32_t); void (*ord10024)(uint32_t, uint32_t); void (*exitProc)(int32_t);
    void (*f58230)(uint32_t, uint8_t*, uint8_t*, uint8_t*, uint8_t*); int32_t (*ord10001)(uint32_t, int32_t, int32_t, const uint32_t*, uint32_t);
    int32_t (*ord10058)(uint32_t, int32_t, int32_t, const uint32_t*, uint32_t, uint32_t); void (*f166d0)(uint32_t); };
extern "C" void d2_client_9f270(const uint8_t*, uint32_t, D2ClientCellWalk9f270Env*);
extern "C" void d2c_9ee90(const D2c9EE90Env*, uint32_t);
static uint32_t gRd32(uint32_t a) { uint32_t v = 0; if (a < sizeof g_arena - 4) memcpy(&v, g_arena + a, 4); return v; }
static int32_t g_vx, g_vy;   // ctx+0x24/+0x28, the view origin (iso px)
EMSCRIPTEN_KEEPALIVE void d2_dg_set_origin(int32_t x, int32_t y) { g_vx = x; g_vy = y; }
EMSCRIPTEN_KEEPALIVE const char* d2_dg_walk_g() {
    wr(kCtx + 0x24, (uint32_t)g_vx); wr(kCtx + 0x28, (uint32_t)g_vy);
    g_gout.clear(); g_ledx = 0;
    D2ClientCellWalk9f270Env w{0, [](uint32_t) {}, []() -> uint32_t { gRec(10013, {}, 0, g_ledx); return g_r13; }, []() {},
        [](uint32_t, int32_t x, int32_t y, uint32_t m, uint32_t arg) { g_ledx = x > 0 ? x : 0; gRec(10069, {0x100000u, (uint32_t)x, (uint32_t)y, m, arg}, 0, g_ledx); }, [](uint32_t room, uint32_t mode, uint32_t) {
            static const uint32_t z[1024] = {};
            D2Client9E970Env e{0, 0, z, z, z, 0, 0, 0, 0, []() -> uint32_t { gRec(10013, {}, 0, g_ledx); g_ledx = 0; return g_r13; }, []() -> uint32_t { return g_now; }, [](uint32_t) {},
                [](uint32_t, uint8_t* o) -> uint32_t { o[0] = o[1] = o[2] = o[3] = 0; return 0; },
                [](uint32_t, int32_t x, int32_t y, const uint32_t*, uint32_t g) -> uint32_t { gRec(10001, {0x100000u, (uint32_t)x, (uint32_t)y, 0x100000u, g}, y > 0 ? y : 0); return 1; },
                [](uint32_t, int32_t x, int32_t y, const uint32_t*, uint32_t g, uint32_t al) -> uint32_t { gRec(10058, {0x100000u, (uint32_t)x, (uint32_t)y, 0x100000u, g, al}, y > 0 ? y : 0); return 1; },
                [](uint32_t p) { return (D2Client9E970Entry*)(g_arena + p); }, [](uint32_t p) { return gRd32(p); }};
            e.ctxX = gRd32(kCtx + 4); e.ctxY = gRd32(kCtx + 8); e.ctxOffX = gRd32(kCtx + 0x24); e.ctxOffY = gRd32(kCtx + 0x28);
            d2_client_9e970(e, (D2Client9E970Node*)(g_arena + gRd32(room + 0x14)), mode, [](uint32_t p) { return (D2Client9E970Node*)(g_arena + p); });
        }};
    // (no tail 10013: the first 10013 is c124's 9E970 call)
    d2_client_9f270(g_arena, kCtx, &w);
    D2c9EE90Env e{gRd32, [](uint32_t a) -> uint8_t { return a < sizeof g_arena ? g_arena[a] : 0; }, [](uint32_t a, uint32_t v) { if (a < sizeof g_arena - 4) memcpy(g_arena + a, &v, 4); },
        [](uint32_t a, uint8_t v) { if (a < sizeof g_arena) g_arena[a] = v; }, []() -> int32_t { gRec(10013, {}); return g_r13; }, []() -> uint32_t { return g_now; },
        [](uint32_t) -> uint32_t { return 0; }, [](uint32_t, uint32_t) {}, [](int32_t) {}, [](uint32_t, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) { *r = *g = *b = *a = 0; },
        [](uint32_t, int32_t x, int32_t y, const uint32_t*, uint32_t g) -> int32_t { gRec(10001, {0x100000u, (uint32_t)x, (uint32_t)y, 0x100000u, g}, y > 0 ? y : 0); return 1; },
        [](uint32_t, int32_t x, int32_t y, const uint32_t*, uint32_t g, uint32_t al) -> int32_t { gRec(10058, {0x100000u, (uint32_t)x, (uint32_t)y, 0x100000u, g, al}, y > 0 ? y : 0); return 1; }, [](uint32_t) {}};
    d2c_9ee90(&e, kCtx);
    return g_gout.c_str();
}
}
