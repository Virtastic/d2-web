// d2game+0xdb860 D2Game_10033: probe. stdcall (unit, int* a2, int* a3). The unit blob is a retail D2UnitStrc, marshalled
// through la::Graph (the 8-byte seed at +0x20 is mapped); a2/a3 are plain dword blobs read by address.
#include <emscripten.h>
#include <cstdint>
#include "live_adapt.h"
#include "blob_sizes.h"
bool D2Game_10033(D2UnitStrc* pUnit, int32_t* a2, int32_t* a3);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_db860(uint32_t* r) {
    if (!la::Graph::tables()) { r[4] = 0x7EADBEEF; return; }
    la::Graph g(r); laBlobSizes(g);
    r[4] = D2Game_10033(g.unit(g.arg(0)), (int32_t*)(uintptr_t)r[1], (int32_t*)(uintptr_t)r[2]) ? 1 : 0;   // retmask 0xFF
    g.writeBack();
}
