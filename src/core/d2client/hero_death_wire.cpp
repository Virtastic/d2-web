// The hero's death as the browser asks it: two retail 1.13c D2Client routines exported for the page.
//   d2_hero_is_down(type, mode, flags)  -> 1.13c D2Client+0xA5440 (falling or lying dead; flag 0x10000 forces it)
//   d2_death_closes_panel(panel)        -> 1.13c D2Client+0xBE350 run with its first argument 1 (the hero's own
//                                          "die" action) over a table holding only `panel` open; 1 when it closes it.
// The closable flags are the table 0xBE350 indexes (D2Client+0x209D8), 0x26 dwords read from the 1.13c image.
#include <cstdint>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" uint32_t d2_client_a5440(uint32_t type, uint32_t mode, uint32_t flags);

namespace d2client_close_panels {
using U = uint32_t;
struct State { U active[0x26]; U closable[0x26]; U sel[6]; U c28, c2c, c38, c3c; };
U sub_BE350(State& s, int param1, int param2, void (*close)(U, U, int));
}

namespace {
// 1.13c D2Client+0x209D8
constexpr uint32_t kClosable[0x26] = {0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0,
                                      1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1};
uint32_t g_closed;
void note_close(uint32_t, uint32_t, int) { ++g_closed; }
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_hero_is_down(uint32_t type, uint32_t mode, uint32_t flags) {
    return d2_client_a5440(type, mode, flags);
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_death_closes_panel(uint32_t panel) {
    if (panel >= 0x26) return 0;
    d2client_close_panels::State s{};
    for (uint32_t i = 0; i < 0x26; ++i) s.closable[i] = kClosable[i];
    s.active[panel] = 1;
    g_closed = 0;
    d2client_close_panels::sub_BE350(s, 1, 0, note_close);
    return g_closed != 0;
}
