// The play path's sound entry list (1.13c D2Client list at 0x10BF30, entries from the 200-slot pool 0x87D80 fills). The
// retail entry is a packed 0x59-byte slot; the play path holds the fields its rules read in a typed slot, named by
// their retail offsets. Positions, gain and pan stay with the browser mixer (the distance the sort reads arrives
// with the submit).
#pragma once
#include <cstdint>

namespace d2client_playsnd {
struct Entry {
    bool     used;        // +0x00 (0x87D80 sets 1)
    int32_t  id;          // +0x04 Sounds row; 0x5D7A0 replaces it with the group variant it starts
    int32_t  handle;      // +0x08 ++DAT 0x10BF38 at each allocation
    float    dist2;       // +0x18 squared distance to the listener (the 0x88DD0 sort's second key)
    int32_t  volume;      // +0x1C 0xFF at submit
    uint64_t unit;        // +0x28 the unit list's single member (0 none)
    int32_t  units;       // +0x28 unit list length
    int32_t  playing;     // +0x2C voice slot held (nonzero once 0x5D7A0 started it)
    int32_t  state;       // +0x30 0 queued, 1 playing, 2 done
    uint32_t start;       // +0x34 now + submit offset
    int32_t  flags;       // +0x38 submit p3 (bit 0: no variant pick)
    uint8_t  prio;        // +0x3C Sounds Priority (+'P' for the hero's own unit)
    int32_t  killed;      // +0x3D
    int32_t  asked;       // the row 0x88A70 was given (the group base whose history 0x5D7A0 updates)
    int32_t  kind;        // the caller's tag for the submit (not retail)
};
constexpr int kPool = 200;   // 0x87D80: 200 slots
extern Entry g_pool[kPool];
extern int32_t g_list[kPool];   // list order (pool indices), DAT 0x10BF30
extern int32_t g_listLen;
extern uint32_t g_now;           // DAT 0x10C268, the sound clock
uint8_t* rec(int32_t id);        // Sounds record (0x8E bytes) or nullptr
uint32_t seedRand(uint32_t n);   // 0x89640 on the client hero seed

Entry* find87fe0(int32_t id, const Entry* self);
Entry* busy87bd0(const Entry* self);
Entry* inst87c30(int32_t group, const Entry* self);
Entry* slot5cc10(int32_t id);
bool start5d7a0(Entry* e);
}
extern "C" int32_t d2_client_5ee70(const uint8_t* recs, int32_t count, int32_t id, uint32_t (*rand)(uint32_t));
