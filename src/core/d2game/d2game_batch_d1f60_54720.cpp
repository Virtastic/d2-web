// Stage A implementations from the cached 1.13c D2Game function bodies.
// Prior art used: item generation and save-file entries;
// D2MOO's ItemMode.cpp and PlrSaveItems.cpp supply the surrounding call sites.
// The imported 1.13c ordinal labels are not assumed to match 1.10f exports.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <unistd.h>

namespace {
struct ModeInput {
    int32_t type;
    int32_t mode;
};

#pragma pack(push, 1)
struct SavedItemInput {
    uint8_t unused0[2];
    uint32_t itemCode;
    uint8_t unused6[0x1a];
    uint32_t flags;
    uint8_t unused24[0x0c];
    uint8_t quality;
    uint8_t unused31;
    char name[52];
};
#pragma pack(pop)
static_assert(offsetof(SavedItemInput, itemCode) == 2);
static_assert(offsetof(SavedItemInput, flags) == 0x20);
static_assert(offsetof(SavedItemInput, quality) == 0x30);
static_assert(offsetof(SavedItemInput, name) == 0x32);

using ItemTest = int (*)(const void*, int);
using ItemTest3 = int (*)(const void*, int, int);
using Creator = void* (*)(void*, const void*);
using SetFlags = void (*)(void*, uint32_t);
using SetQuality = void (*)(void*, uint8_t);
}

// 1.13c D2Game+0xD1F60
// EAX = unit {type, class}, EDI = item, ESI = the ten output flags. Every test asks about the item: 0xA430 is
// (item, item type) for types 0x43 0x1b 0x23 5 6 0x33 0x2d 0xa in that order, 0xA8FE and 0xA9AC take the item alone,
// and 0xA802 (item, 0x7d, 0) is asked only when 0xA9AC answered 0. A null item or unit is retail's fatal exit.
extern "C" void D2Game_D1F60_ModeFlags(const ModeInput* unit, const void* item, int32_t* flags,
    ItemTest typeTest, ItemTest auxiliary, ItemTest firstState, ItemTest3 secondState)
{
    if (!item || !unit) _exit(-1);                 // asserts 0x676 / 0x677
    std::memset(flags, 0, 10 * sizeof(*flags));
    if (unit->type == 0 && unit->mode == 4) flags[7] = 1;
    if (unit->type == 0 && unit->mode == 6 && typeTest(item, 0x43)) flags[7] = 1;
    if (typeTest(item, 0x1b)) flags[0] = 1;
    if (typeTest(item, 0x23)) flags[1] = 1;
    if (typeTest(item, 5)) flags[2] = 1;
    if (typeTest(item, 6)) flags[3] = 1;
    if (typeTest(item, 0x33)) flags[4] = 1;
    if (auxiliary(item, 0)) flags[6] = 1;
    if (typeTest(item, 0x2d)) flags[5] = 1;
    if (firstState(item, 0) || secondState(item, 0x7d, 0)) flags[8] = 1;
    if (typeTest(item, 0xa)) flags[9] = 1;
}

// 1.13c D2Game+0x54720
extern "C" void* D2Game_54720_CreateSavedItem(void* game,
    const SavedItemInput* input, Creator create, SetFlags setFlags,
    SetQuality setQuality)
{
    if (!input) return nullptr;
    // Retail clears 0x84 bytes, then copies the code and the NUL-terminated
    // name into its item-creation request before calling the item factory.
    uint8_t request[0x8c] = {};
    std::memcpy(request + 0x0c, &input->itemCode, sizeof(input->itemCode));
    for (unsigned i = 0; i < 52; ++i) {
        request[0x58 + i] = static_cast<uint8_t>(input->name[i]);
        if (!input->name[i]) break;
    }
    void* item = create(game, request);
    if (!item) return nullptr;
    setFlags(item, input->flags);
    setQuality(item, input->quality);
    return item;
}
