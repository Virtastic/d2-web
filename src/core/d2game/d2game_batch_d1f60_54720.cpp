// Stage A implementations from the cached 1.13c D2Game function bodies.
// Prior art used: item generation and save-file entries;
// D2MOO's ItemMode.cpp and PlrSaveItems.cpp supply the surrounding call sites.
// The imported 1.13c ordinal labels are not assumed to match 1.10f exports.
#include <cstddef>
#include <cstdint>
#include <cstring>

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

using Predicate = int (*)(const ModeInput*, int);
using Creator = void* (*)(void*, const void*);
using SetFlags = void (*)(void*, uint32_t);
using SetQuality = void (*)(void*, uint8_t);
}

// 1.13c D2Game+0xD1F60
extern "C" void D2Game_D1F60_ModeFlags(const ModeInput* item, int32_t* flags,
    Predicate equipment, Predicate auxiliary, Predicate firstState,
    Predicate secondState)
{
    if (!item || !flags) return;
    std::memset(flags, 0, 10 * sizeof(*flags));
    if (item->type == 0 && item->mode == 4) flags[7] = 1;
    if (item->type == 0 && item->mode == 6 && equipment(item, 7)) flags[7] = 1;
    for (int i = 0; i < 5; ++i) flags[i] = equipment(item, i) != 0;
    flags[6] = auxiliary(item, 6) != 0;
    flags[5] = equipment(item, 5) != 0;
    flags[8] = firstState(item, 8) || secondState(item, 8);
    flags[9] = equipment(item, 9) != 0;
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
