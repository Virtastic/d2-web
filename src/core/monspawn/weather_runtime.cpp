#include "weather_runtime.h"

#include <array>
#include <vector>
#include <emscripten.h>

namespace d2::weather {
namespace {

Pool g_rainPool(kRainCapacity);
Pool g_bubblePool(kBubbleCapacity);
std::array<uint32_t, 4> g_rainFrameCounts{};
std::array<uint32_t, 4> g_bubbleFrameCounts{};
bool g_frameCountsReady = false;
std::vector<Particle> g_rainParticles;
std::vector<Particle> g_bubbleParticles;

void copyParticles(const Pool& pool, std::vector<Particle>& out) {
    out.clear();
    for (int32_t i = 0; i <= pool.maxIndex(); ++i) {
        const Entry& entry = pool.data()[i];
        if (entry.generation) out.push_back({entry.x, entry.y, entry.type, entry.frame});
    }
}

} // namespace

bool loadResources(const MpqVfs& vfs) {
    static constexpr const char* rainFiles[] = {
        "data\\global\\uncompoverlays\\Rain1.dc6", "data\\global\\uncompoverlays\\Rain2.dc6",
        "data\\global\\uncompoverlays\\Rain3.dc6", "data\\global\\uncompoverlays\\Rain4.dc6"};
    static constexpr const char* bubbleFiles[] = {
        "data\\global\\uncompoverlays\\bubble1.dc6", "data\\global\\uncompoverlays\\bubble2.dc6",
        "data\\global\\uncompoverlays\\bubble3.dc6", "data\\global\\uncompoverlays\\bubble4.dc6"};
    bool complete = true;
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < 4; ++i) {
        g_rainFrameCounts[i] = vfs.read(rainFiles[i], bytes)
            ? dc6FrameCount(bytes.data(), bytes.size()) : 0;
        complete &= g_rainFrameCounts[i] != 0;
        g_bubbleFrameCounts[i] = vfs.read(bubbleFiles[i], bytes)
            ? dc6FrameCount(bytes.data(), bytes.size()) : 0;
        complete &= g_bubbleFrameCounts[i] != 0;
    }
    g_frameCountsReady = complete;
    return complete;
}

void reset() {
    g_rainPool.reset();
    g_bubblePool.reset();
    g_rainParticles.clear();
    g_bubbleParticles.clear();
}

void tick() {
    if (!g_frameCountsReady) return;
    // This headless simulation has no camera delta and stores positions in world space. The
    // blended-shadows argument is therefore immaterial here; the browser applies camera projection
    // after reading particles. Native setting/camera parity is not represented by this tick.
    g_rainPool.update(true, 0, 0, g_rainFrameCounts);
    g_bubblePool.update(true, 0, 0, g_bubbleFrameCounts);
}

const Particle* rainParticles(int* count) {
    copyParticles(g_rainPool, g_rainParticles);
    if (count) *count = static_cast<int>(g_rainParticles.size());
    return g_rainParticles.data();
}

const Particle* bubbleParticles(int* count) {
    copyParticles(g_bubblePool, g_bubbleParticles);
    if (count) *count = static_cast<int>(g_bubbleParticles.size());
    return g_bubbleParticles.data();
}

} // namespace d2::weather

extern "C" {
EMSCRIPTEN_KEEPALIVE const d2::weather::Particle* d2_session_weather_rain(int* count) {
    return d2::weather::rainParticles(count);
}
EMSCRIPTEN_KEEPALIVE const d2::weather::Particle* d2_session_weather_bubbles(int* count) {
    return d2::weather::bubbleParticles(count);
}
}
