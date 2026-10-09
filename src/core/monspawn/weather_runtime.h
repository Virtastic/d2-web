#pragma once

#include "weather_pool.h"
#include "../../assets/mpqvfs.h"

namespace d2::weather {

bool loadResources(const MpqVfs& vfs);
void reset();
void tick();
const Particle* rainParticles(int* count);
const Particle* bubbleParticles(int* count);

// General non-Act-V weather state from D2Client 1.13c FUN_6FB30BE0 and
// FUN_6FB30E30. Fields keep their native global identities until their render semantics are fully
// identified. The seed is the client CUnit's mutable {low, high} pair.
struct SeedState {
    uint32_t low = 0;
    uint32_t high = 0;
};

inline uint32_t rollClientSeed(SeedState& seed);

struct GeneralWeatherState {
    int32_t updateCountdown = 0; // DAT_6FBC3938
    int32_t secondaryCountdown = 0; // DAT_6FBC38BC
    int32_t currentValue = 0; // DAT_6FBC3918
    int32_t targetValue = 0; // DAT_6FBC394C
    bool secondaryEvent = false; // DAT_6FBC393C is set when the gated secondary countdown expires

    // The ramp portion of FUN_6FB30E30 uses two steps when DAT_6FBCBF78 is zero and one
    // when it is nonzero. This helper covers only the target approach; advanceGeneralMode
    // covers the non-Act-V countdown branch, not the alternate-mode target-roll branch.
    void approachTarget(bool alternateWeatherMode) {
        const int32_t step = alternateWeatherMode ? 1 : 2;
        if (currentValue < targetValue) currentValue = std::min(currentValue + step, targetValue);
        else if (currentValue > targetValue) currentValue = std::max(currentValue - step, targetValue);
    }

    // FUN_6FB30BE0. With a client unit, consume exactly three successive D2Seed rolls; without
    // one, native installs fixed fallback values. On first 6FB31750 initialization this runs twice:
    // once through 6FB314F0, then through its one-time initialization block.
    void initialize(SeedState* clientSeed) {
        if (!clientSeed) {
            updateCountdown = 0x138;
            secondaryCountdown = 0x4e2;
            currentValue = targetValue = 0x7f;
            return;
        }
        updateCountdown = static_cast<int32_t>(rollClientSeed(*clientSeed) % 0x177) + 0x7d;
        secondaryCountdown = static_cast<int32_t>(rollClientSeed(*clientSeed) % 0x5dc) + 500;
        currentValue = targetValue = static_cast<int32_t>(rollClientSeed(*clientSeed) % 0x47) + 0x5c;
    }

    // Non-Act-V branch of FUN_6FB30E30. Advance one weather update and mutate the same CUnit seed
    // only when the native countdown expires. Act V's alternate branch is separate.
    void advanceGeneralMode(SeedState& clientSeed, bool secondaryTimerEnabled = false) {
        approachTarget(false);
        --updateCountdown;
        if (updateCountdown == 0) {
            updateCountdown = static_cast<int32_t>(rollClientSeed(clientSeed) % 0x177) + 0x7d;
            targetValue = static_cast<int32_t>(rollClientSeed(clientSeed) % 0x47) + 0x5c;
        }
        if (secondaryTimerEnabled && --secondaryCountdown == 0) secondaryEvent = true;
    }
};

inline uint32_t rollClientSeed(SeedState& seed) {
    const uint64_t next = uint64_t(seed.low) * 0x6ac690c5u + seed.high;
    seed.low = static_cast<uint32_t>(next);
    seed.high = static_cast<uint32_t>(next >> 32);
    return seed.low;
}

} // namespace d2::weather
