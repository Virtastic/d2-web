#include <cassert>
#include "../../src/core/storm/storm_batch_3b1e0.cpp"
namespace {
StormEventNode3b1e0 node{};
unsigned phase;
uint32_t messageData[8]{};
void enter(uint32_t lock) { assert(lock == 0x458c8 && phase == 0); phase = 1; }
void leave(uint32_t lock) { assert(lock == 0x458c8 && phase == 2); phase = 0; }
StormEventNode3b1e0* allocate(uint32_t list, uint32_t arg, uint32_t flags, uint32_t mode) {
    assert(phase == 1 && list == 0x458bc && arg == 0 && flags == 0 && mode == 2);
    phase = 2; return &node;
}
const void* resolve(uint32_t token) { assert(token == 0xf1234567); return messageData; }
uint32_t region[24]{};
uint32_t regionId(uint32_t list) { assert(list == 0x458f0); return 19; }
uint32_t* regionCreate(uint32_t list, uint32_t id, uint32_t a, uint32_t b) {
    assert(list == 0x458f0 && id == 19 && a == 0 && b == 0); return region;
}
void checkRegionLock(uint32_t lock) { assert(lock == 0x45940); }
void* find(uint32_t list, uint32_t handle) { assert(list == 0x458f0 && handle == 19); return region; }
void clear(void* p) { assert(p == region); }
void release(uint32_t list, void* p) { assert(list == 0x458f0 && p == region); }
}
int main() {
    StormCalls3b1e0 c{};
    c.enter = enter; c.leave = leave; c.eventNode = allocate; c.resolveMessageData = resolve;
    assert(Storm_Sub343D0(messageData,c) == 1);
    assert(node.messageData == messageData);
    assert(Storm_BreakRetailMessageChain3b1e0(0xf1234567,c) == 1);
    assert(node.messageData == messageData);
    assert(phase == 0);
    c.enter = c.leave = checkRegionLock; c.regionId = regionId; c.regionCreate = regionCreate;
    c.regionFind = find; c.regionClear = clear; c.regionRelease = release;
    StormState3b1e0 s{}; uint32_t handle = 0;
    Storm_Sub30B70(&handle,0,s,c);
    assert(handle == 19 && region[0x48/4] == 0x7f7fffff && region[0x5c/4] == 0);
    Storm_Sub30580(handle,s,c);
}
