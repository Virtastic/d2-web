// D2Client 1.13c weather driver (0x81340 body): the 0x28-byte particle pool, the intensity target and its
// four-phase cycle, and every client hero-seed draw they make. Draw-for-draw port; see
#pragma once
#include <cstdint>

namespace d2client::weather {

struct Seed { uint32_t lo, hi; };

// Per-tick inputs the native code reads through D2Common ordinals and render globals.
struct Env {
    int32_t levelId;        // Ordinal_10826(Ordinal_10331(hero)): the level the hero stands in
    int32_t act;            // Ordinal_10459(levelId): 0-based act; 4 = Act V (weather setup differs)
    bool    weatherLevel;   // Ordinal_10215(levelId): 0x81340 only runs the pass when this is set
    int32_t cellW = 640;    // D2Client+0x8BC48 (screen width)
    int32_t cellH = 480;    // D2Client+0x8BC4C (screen height)
    int32_t tickDx = 0;     // DAT_6FBCC408: camera delta this frame
    int32_t tickDy = 0;     // DAT_6FBCC40C
};

void reset();                           // session create: all native globals back to their load-time values
void tick(Seed& seed, const Env& env);  // one 0x81340 pass (first call also runs the 0x81750 setup)
int32_t liveCount();                    // *(DAT_6FBCBF68 + 0x114)
float intensity();                      // DAT_6FBC38F4
int32_t target();                       // DAT_6FBC3934

} // namespace d2client::weather
