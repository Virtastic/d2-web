// Client-side walk path of a monster (1.13c D2Client 0xAF3F0 / 0x4DC70 case 1; the retail captures).
// The client unit walks its own D2Common path after a 0x67/0x68 order: it is placed on the order's tile (+0x8000),
// then once per client tick, BEFORE that tick's packets, moves dwVelocity*16 (16.16) toward the current point and
// snaps onto it when within one step (the remainder is dropped). Reaching the last point ends the walk
// (D2Client+0x4DBE8 idles the unit); an order that finds the walk ended restarts the walk clock (mode 2 -> 1 -> 2),
// an order arriving on the tick the walk takes its last step included (the step runs before the packet),
// one that finds it live keeps it (2 -> 2, Common #11090 early return).
// ponytail: the points are the SERVER's path for the same order (the client re-paths toward the same target).
#include <cstdint>
#include <cstdlib>
#include <unordered_map>
#include "../d2moo/include/Path/Path.h"
#include <emscripten.h>
namespace {
struct Path { int32_t x = 0, y = 0, step = 0, n = 0; int32_t px[D2DynamicPathStrc::MAXPATHLEN], py[D2DynamicPathStrc::MAXPATHLEN]; };
// cur: the walk the client is on; next: an order received this tick, taken only after the live walk has stepped
struct Walk { int32_t old = 1, ldx = 0, ldy = 0, i = 0; bool live = false, pending = false; Path cur, next; };
std::unordered_map<uint32_t, Walk> g_walks;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_walk_order(uint32_t id, int32_t tx, int32_t ty, int32_t velocity, int32_t n, const D2PathPointStrc* pts) {
    Walk& w = g_walks[id];
    // old: the server's unit stood within a tile of the previous order's last point (the placeholder rule this replaces)
    w.old = !(w.ldx || w.ldy) || !(std::abs(tx - w.ldx) > 1 || std::abs(ty - w.ldy) > 1);
    if (n > 0) { w.ldx = pts[n-1].X; w.ldy = pts[n-1].Y; }
    Path& o = w.next;
    o.x = (tx << 16) | 0x8000; o.y = (ty << 16) | 0x8000; o.step = velocity * 16; w.pending = true;
    o.n = n > (int32_t)D2DynamicPathStrc::MAXPATHLEN ? (int32_t)D2DynamicPathStrc::MAXPATHLEN : n;
    for (int32_t k = 0; k < o.n; ++k) { o.px[k] = (pts[k].X << 16) | 0x8000; o.py[k] = (pts[k].Y << 16) | 0x8000; }
}
// One client tick for a unit: step the live path, then take a pending order. Returns 1 when an order arrived and the
// previous walk had already ended (the walk clock restarts).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_walk_tick(uint32_t id) {
    auto it = g_walks.find(id);
    if (it == g_walks.end()) return 0;
    Walk& w = it->second;
    Path& p = w.cur;
    auto step = [&w, &p] {
        const int64_t dx = p.px[w.i] - p.x, dy = p.py[w.i] - p.y, d2 = dx * dx + dy * dy;
        int64_t d = 0; while ((d + 1) * (d + 1) <= d2) ++d;   // integer sqrt
        if (d <= p.step) { p.x = p.px[w.i]; p.y = p.py[w.i]; if (++w.i >= p.n) w.live = false; }
        else { p.x += (int32_t)(dx * p.step / d); p.y += (int32_t)(dy * p.step / d); }
    };
    if (w.live) step();
    if (!w.pending) return 0;
    w.pending = false;
    const int32_t restart = !w.live;   // the live walk stepped first: an order on the tick it reaches its end finds it ended
    p = w.next;
    w.i = 0; w.live = p.n > 0;
    if (w.live) step();   // the order's own tick already moves the unit one step (retail MP, units 3 and 7)
    return restart && w.old;   // and the server unit had reached the previous order's end (1 tile): both views must agree the walk ended
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_walk_reset() { g_walks.clear(); }
