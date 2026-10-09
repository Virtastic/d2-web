// D2Client's Fog-backed rain and bubble pools (1.13c). The native slot is 0x18 bytes and its
// allocator handle packs the slot index in the high word and a generation in the low word.
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace d2::weather {

constexpr uint32_t kRainCapacity = 0x200;
constexpr uint32_t kBubbleCapacity = 0x80;
constexpr uint32_t kEntryStride = 0x18;

struct Entry {
    uint16_t generation = 0;  // Native active WORD at +0; zero means unused.
    uint16_t reserved = 0;
    int32_t x = 0;            // +4
    int32_t y = 0;            // +8
    int32_t type = 0;         // +0x0c
    int32_t frame = 0;        // +0x10
    int32_t timer = 0;        // +0x14; native also clears its upper WORD (+0x16) on frame advance.
};

// DC6's fixed header is 24 bytes. OpenDiablo2's d2dc6/DC6Header confirms directions at +16 and
// frames-per-direction at +20. The engine reads only these little-endian fields for pool lifetime;
// the production loader still owns decoding and drawing the cel data.
inline uint32_t dc6FrameCount(const uint8_t* bytes, size_t size) {
    if (!bytes || size < 24) return 0;
    auto u32 = [bytes](size_t at) {
        return uint32_t(bytes[at]) | (uint32_t(bytes[at + 1]) << 8) |
               (uint32_t(bytes[at + 2]) << 16) | (uint32_t(bytes[at + 3]) << 24);
    };
    const uint32_t directions = u32(16), perDirection = u32(20);
    if (!directions || !perDirection || directions > 4096 || perDirection > 4096 ||
        directions > UINT32_MAX / perDirection) return 0;
    return directions * perDirection;
}
static_assert(sizeof(Entry) == 0x18);
static_assert(offsetof(Entry, x) == 0x04 && offsetof(Entry, y) == 0x08);
static_assert(offsetof(Entry, type) == 0x0c && offsetof(Entry, frame) == 0x10);
static_assert(offsetof(Entry, timer) == 0x14);

class Pool {
public:
    explicit Pool(uint32_t capacity) : entries_(capacity) { reset(); }

    void reset() {
        cursor_ = 0;
        maxIndex_ = -1;
        activeCount_ = 0;
        generation_ = 0x8000;
        for (Entry& entry : entries_) entry.generation = 0;
    }

    // Fog 10064 scans forward from cursor to capacity and does not wrap. A free lowers cursor,
    // making earlier holes eligible again; full is represented by -1.
    int32_t allocate() {
        uint32_t index = cursor_;
        while (index < entries_.size() && entries_[index].generation != 0) ++index;
        if (index >= entries_.size()) return -1;

        Entry& entry = entries_[index];
        entry = {};
        entry.generation = generation_;
        ++generation_;
        if (generation_ == 0) generation_ = 0x8000;
        ++activeCount_;
        if (maxIndex_ < static_cast<int32_t>(index)) maxIndex_ = static_cast<int32_t>(index);
        cursor_ = index + 1;
        return static_cast<int32_t>((index << 16) | entry.generation);
    }

    // Fog 10067 requires an active slot. A zero low WORD selects by index alone; a nonzero
    // generation must match. Native rain/bubble expiry passes this index-only form to Fog 10065.
    Entry* resolve(uint32_t handle) {
        const uint32_t index = handle >> 16;
        if (index >= entries_.size()) return nullptr;
        Entry& entry = entries_[index];
        const uint16_t requestedGeneration = static_cast<uint16_t>(handle);
        return entry.generation != 0 &&
               (requestedGeneration == 0 || entry.generation == requestedGeneration) ? &entry : nullptr;
    }
    const Entry* resolve(uint32_t handle) const {
        return const_cast<Pool*>(this)->resolve(handle);
    }

    bool free(uint32_t handle) {
        Entry* entry = resolve(handle);
        if (!entry) return false;
        const int32_t index = static_cast<int32_t>(handle >> 16);
        entry->generation = 0;
        if (index < static_cast<int32_t>(cursor_)) cursor_ = static_cast<uint32_t>(index);
        if (index == maxIndex_) {
            while (maxIndex_ >= 0 && entries_[static_cast<size_t>(maxIndex_)].generation == 0) --maxIndex_;
        }
        --activeCount_;
        return true;
    }

    // Native ordinal 10013 reports whether blended shadows are enabled. The Rain/bubble updater
    // subtracts camera deltas only when that value is zero. Timer advances every tick, and the
    // animation frame advances every second tick. The frame-count table ends an entry as soon as
    // its incremented frame reaches the number of decoded frames.
    template <size_t N>
    void update(bool blendedShadowsEnabled, int32_t cameraDx, int32_t cameraDy,
                const std::array<uint32_t, N>& frameCounts) {
        for (int32_t i = 0; i <= maxIndex_; ++i) {
            Entry& entry = entries_[static_cast<size_t>(i)];
            if (entry.generation == 0) continue;
            if (!blendedShadowsEnabled) {
                entry.x -= cameraDx;
                entry.y -= cameraDy;
            }
            --entry.timer;
            if (entry.timer < 1) {
                ++entry.frame;
                entry.timer = 2;
                if (entry.type >= 0 && static_cast<size_t>(entry.type) < N &&
                    frameCounts[static_cast<size_t>(entry.type)] <= static_cast<uint32_t>(entry.frame)) {
                    free(static_cast<uint32_t>(i) << 16);
                }
            }
        }
    }

    Entry* data() { return entries_.data(); }
    const Entry* data() const { return entries_.data(); }
    size_t capacity() const { return entries_.size(); }
    int32_t maxIndex() const { return maxIndex_; }
    int32_t activeCount() const { return activeCount_; }
    uint32_t cursor() const { return cursor_; }
    uint16_t generation() const { return generation_; }

private:
    std::vector<Entry> entries_;
    uint32_t cursor_ = 0;
    int32_t maxIndex_ = -1;
    int32_t activeCount_ = 0;
    uint16_t generation_ = 0x8000;
};

struct Particle {
    int32_t x, y, type, frame;
};

} // namespace d2::weather
