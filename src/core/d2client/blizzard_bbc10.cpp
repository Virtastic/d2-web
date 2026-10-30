// D2Client client missiles of Blizzard (Skills.txt id 59): the blizzardcenter do-function (Missiles.txt pCltDoFunc 13) 1.13c
// D2Client+0xBBC10 and the blizzard1-4 shards it spawns through 0xB8DF0.
// The client runs its own center whatever the server did. The center (Range 100) spawns a shard on every 4th frame of its
// life (elapsed % Skills.txt frequency), a shard lives 24 frames (CltParam 1/2); the shard's TravelSound (2393) plays at its
// creation, its HitSound (2394) when its life ends. The center's TravelSound (2393) plays at creation.
// Client units sit in a 128-slot hash by id, and each frame walks the slots in order, newest-first within a slot: that order
// is why a shard ends on its 23rd or 24th frame after birth. Ported as a sound stream (no gfx).
// Not simulated: the shard position rolls and their collision test (mask 4, 0xB8DF0 skips the spawn on a blocked cell: one
// retail birth at frame 28 of a retail recording is blocked, as is one at the last slot of the first center).
#include <cstdint>
#include <vector>
#include <algorithm>

namespace d2client_ambient { void submit(int32_t row); }

namespace d2client_blizzard {
namespace {
constexpr int32_t kBlizzard = 59;        // Skills.txt id
constexpr int32_t kSoundLoop = 2393, kSoundImpact = 2394;   // Missiles.txt blizzardcenter / blizzard1-4 TravelSound, HitSound
constexpr int32_t kCastDelay = 8;        // frames from the cast to the center's creation (fitted to a retail recording of skill 59)
constexpr int32_t kCenterRange = 100, kFrequency = 4, kShardLife = 24;
constexpr uint32_t kFirstId = 106;       // the client's unit id counter at the first cast (fitted to a retail recording)
struct Obj { uint32_t id; bool center; int32_t life; };
std::vector<Obj> objs;
std::vector<int64_t> casts;
int64_t frame = 0;
uint32_t nextId = kFirstId;

void spawn(bool center) { objs.push_back({nextId++, center, center ? kCenterRange : kShardLife}); }
}

void reset() { objs.clear(); casts.clear(); frame = 0; nextId = kFirstId; }

void cast(int32_t skillId) { if (skillId == kBlizzard) casts.push_back(frame + kCastDelay); }

void tick() {
    ++frame;
    for (uint32_t b = 0; b < 128; ++b) {
        std::vector<uint32_t> ids;
        for (const Obj& o : objs) if ((o.id & 127u) == b) ids.push_back(o.id);
        std::sort(ids.rbegin(), ids.rend());
        for (uint32_t id : ids) {
            auto it = std::find_if(objs.begin(), objs.end(), [id](const Obj& o) { return o.id == id; });
            if (it == objs.end()) continue;
            if (it->center) {
                const int32_t elapsed = kCenterRange - it->life;
                --it->life;
                const bool ended = it->life < 1;
                if (elapsed % kFrequency == 0) { spawn(false); d2client_ambient::submit(kSoundLoop); }
                if (ended) objs.erase(std::find_if(objs.begin(), objs.end(), [id](const Obj& o) { return o.id == id; }));
            } else if (--it->life < 1) {
                objs.erase(it);
                d2client_ambient::submit(kSoundImpact);
            }
        }
    }
    for (size_t i = 0; i < casts.size();) {
        if (casts[i] == frame) { spawn(true); d2client_ambient::submit(kSoundLoop); casts.erase(casts.begin() + i); } else ++i;
    }
}
}
