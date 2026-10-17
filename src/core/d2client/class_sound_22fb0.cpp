// 1.13c D2Client+0x22FB0: EAX = pUnit, [esp+4] = flag, ret 4.
// The class sound-profile base: the player's `0xF7694[class] + 0x28`, or the monster's `0x22A70` record
// `+0x54`, shifted by the room class from `0x226A0` and, last, by 0x18 when the stack flag is non-zero.
// Callees are behind the caller: 0x22430 (type), 0x22490 (class), 0x22A70 (record), 0x226A0 (room class).
// Call order is part of the rule: 0x226A0 runs only after the base dword is non-zero.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClassSoundProfile { uint8_t pad[0x28]; int32_t nBase; };   // 0xF7694[class] points here
static_assert(offsetof(D2ClassSoundProfile, nBase) == 0x28, "class profile +0x28 is the base");

struct D2UnitSoundRecord { uint8_t pad[0x54]; int32_t nBase; };     // the 0x22A70 result
static_assert(offsetof(D2UnitSoundRecord, nBase) == 0x54, "0x22A70 record +0x54 is the base");

enum {
    OP_TYPE        = 0x22430,   // ESI = pUnit -> the unit type (0 = player, 1 = monster)
    OP_CLASS       = 0x22490,   // ESI = pUnit -> the class 0..6
    OP_SND_RECORD  = 0x22A70,   // EAX = pUnit -> the monster sound record, or 0
    OP_ROOM_CLASS  = 0x226A0,   // EAX = pUnit -> the room-record class 0..6
};
typedef uint32_t (*D2Client22FB0Cb)(void* ctx, uint32_t rva, uint32_t unit);

static uint32_t rd32(const uint8_t* p, size_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_22fb0(uint32_t unit, int32_t flag, const uint32_t* classTbl,
                                                        D2Client22FB0Cb cb, void* ctx) {
    int32_t v;
    int32_t nType = (int32_t)cb(ctx, OP_TYPE, unit);                 // 0x22FB4
    if (nType == 0) {                                                // player
        uint32_t nClass = cb(ctx, OP_CLASS, unit);                   // 0x22FBD
        if (nClass >= 7) return 0;   // retail asserts 0xB22 and _exit(-1); the fatal path is not reproduced
        v = (int32_t)rd32((const uint8_t*)(uintptr_t)classTbl[nClass], 0x28);   // 0x22FE6
    } else if (nType == 1) {                                         // monster
        uint32_t rec = cb(ctx, OP_SND_RECORD, unit);                 // 0x22FF9
        if (!rec) return 0;                                          // 0x22FFE
        v = (int32_t)rd32((const uint8_t*)(uintptr_t)rec, 0x54);     // 0x23002
    } else {
        return 0;                                                    // 0x22FF5
    }
    if (!v) return 0;                                                // 0x23005
    uint32_t nRoomClass = cb(ctx, OP_ROOM_CLASS, unit);              // 0x23012
    switch (nRoomClass - 2) {                                        // 0x23017: unsigned, 0/1/>6 skip
    case 0: v += 4;    break;                                        // room class 2
    case 1: v += 8;    break;                                        // 3
    case 2: v += 0xC;  break;                                        // 4
    case 3: v += 0x10; break;                                        // 5
    case 4: v += 0x14; break;                                        // 6
    }
    if (flag) v += 0x18;                                             // 0x2303D, the stack argument
    return v;
}
