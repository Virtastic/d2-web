// Ear drop for a killed player (1.13c D2Game+0x12AF0, split out of d2game_batch_10160.cpp). EDI is the victim
// unit, one stack word (the drop parameter); ret 4. Only a player (unit+0 == 0) whose 'ear ' item code resolves
// through D2Common #10450 drops: its coords (#10141) and room (#10331) pick a drop spot (0xE930: room, out
// coords, 1; ESI = in coords), the item level comes from #10973(unit, 12, 0), the ear is created by 0x11980
// (EAX = level, ECX = EDX = 1; unit, item index, parameter, 4, 2, 0, 0, 0) and placed by 0xE990 (EAX = out y,
// ECX = item, EDX = out x, EBX = drop room; parameter, 0, out x). #10450 writes the index into the argument
// slot, so 0x11980 receives the index and the original parameter separately.
#include <cstdint>
#include <cstring>

struct D2G12AF0Env {
    int32_t  (*itemIndexFromCode)(uint32_t code, uint32_t* outIndex);              // D2Common #10450
    void     (*unitCoords)(void* unit, uint32_t* outXY);                            // D2Common #10141
    void*    (*unitRoom)(void* unit);                                               // D2Common #10331
    void*    (*dropSpot)(void* room, uint32_t* outXY, int32_t one, const uint32_t* inXY);   // 0xE930
    int32_t  (*itemLevel)(void* unit, int32_t a, int32_t b);                         // D2Common #10973
    void*    (*createItem)(int32_t level, int32_t ecx, int32_t edx, void* unit, uint32_t index,
                           uint32_t param, int32_t s3, int32_t s4, int32_t s5, int32_t s6, int32_t s7);  // 0x11980
    void     (*placeItem)(uint32_t eaxY, void* item, uint32_t edxX, void* ebxRoom,
                          uint32_t param, int32_t zero, uint32_t x);                 // 0xE990
};

// 1.13c D2Game+0x12AF0
extern "C" void d2g_12af0(void* unit, uint32_t param, const D2G12AF0Env* env)
{
    if (unit == nullptr) return;
    uint32_t type; std::memcpy(&type, unit, 4);
    if (type != 0) return;
    uint32_t index = param;
    if (env->itemIndexFromCode(0x20726165, &index) == 0) return;
    uint32_t xy[2] = { 0, 0 }, spot[2] = { 0, 0 };
    env->unitCoords(unit, xy);
    void* room = env->dropSpot(env->unitRoom(unit), spot, 1, xy);
    if (room == nullptr) return;
    const int32_t level = env->itemLevel(unit, 12, 0);
    void* item = env->createItem(level, 1, 1, unit, index, param, 4, 2, 0, 0, 0);
    if (item == nullptr) return;
    env->placeItem(spot[1], item, spot[0], room, param, 0, spot[0]);
}
