// Browser export of the 1.13c D2Client+0x45660 music-adoption test (level_music_due.h). The browser's active level is -1
// when nothing plays; the native active id is 0 then.
#include <cstdint>
#include <emscripten.h>
#include "level_music_due.h"

extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_level_music_due(int32_t level, int32_t activeLevel, int32_t changedTick, int32_t tick) {
    return d2client_music_selection_due(activeLevel < 0 ? 0 : uint32_t(activeLevel) + 1, uint32_t(level) + 1, true, uint32_t(tick - changedTick));
}
