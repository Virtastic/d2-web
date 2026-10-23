// Prior art used: D2Client has no D2MOO module or solved implementation. The D2BS and PlugY
// address tables were checked for names; OpenDiablo2's d2core/d2ui manager was only a UI-state
// cross-check. These functions' 1.13c control flow comes from the cached local decompiles.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {

using Word = uintptr_t;

struct D2ClientBatch151Host {
    void* user;
    const uint8_t* tableRecords;
    int32_t tableCount;
    uint32_t tableStride;
    uint32_t selectedCount;
    // Invoke the named local D2Client helper by RVA, or the imported Common/Win function by ordinal.
    Word (*client)(void* user, uint32_t rva, Word a, Word b, Word c, Word d, Word e, Word f);
    Word (*imported)(void* user, uint32_t ordinal, Word a, Word b, Word c, Word d, Word e, Word f);
    void (*fatal)(void* user);
    // Caller stack values left unnamed by Ghidra; the adapter must pin these explicitly.
    uint32_t callerStackWord = 0;
    uint32_t placementContext = 0;
    // All callbacks consume/return native pointers. Only serialized retail fields are guest addresses.
    void* (*resolve)(void* user, uint32_t address) = nullptr;
    uint32_t (*encode)(void* user, const void* pointer) = nullptr;
};

// Semantic callback descriptor, not the packed 23-dword retail stack layout.
// Scalar names retain their retail dword index; pointer fields retain native width.
struct PlacementDescriptor151 {
    uint32_t flags = 0;
    void* owner = nullptr;                 // retail dword 1
    void* source = nullptr;                // retail dword 2
    void* state = nullptr;                 // retail dword 3
    uint32_t selectedIndex = 0;            // retail dword 4
    void* window = nullptr;                // retail dword 6
    uint32_t x = 0, y = 0;                 // retail dwords 7, 8
    uint32_t index = 0, context = 0;       // retail dwords 11, 12
    uint32_t workerAddress = 0, iteration = 0; // retail dwords 21, 22
};
static Word resolve(const D2ClientBatch151Host* h, uint32_t address) {
    return address && h->resolve ? reinterpret_cast<Word>(h->resolve(h->user, address)) : 0;
}

static uint32_t load32(const void* p, size_t offset) {
    uint32_t v;
    std::memcpy(&v, static_cast<const uint8_t*>(p) + offset, sizeof(v));
    return v;
}
static void store32(void* p, size_t offset, uint32_t v) {
    std::memcpy(static_cast<uint8_t*>(p) + offset, &v, sizeof(v));
}
static uint8_t* record(const D2ClientBatch151Host* h, int32_t index) {
    if (!h || index < 0 || index >= h->tableCount || !h->tableRecords) return nullptr;
    const uint32_t stride = h->tableStride ? h->tableStride : 0x23c;
    return const_cast<uint8_t*>(h->tableRecords) + static_cast<size_t>(index) * stride;
}
static Word cc(const D2ClientBatch151Host* h, uint32_t ordinal, Word a=0, Word b=0, Word c=0, Word d=0,
               Word e=0, Word f=0) {
    return h->imported ? h->imported(h->user, ordinal, a, b, c, d, e, f) : 0;
}
static Word fn(const D2ClientBatch151Host* h, uint32_t rva, Word a=0, Word b=0, Word c=0, Word d=0,
               Word e=0, Word f=0) {
    return h->client ? h->client(h->user, rva, a, b, c, d, e, f) : 0;
}
static void halt(const D2ClientBatch151Host* h) { if (h->fatal) h->fatal(h->user); }
static bool validInput(const D2ClientBatch151Host* h, const void* owner, int32_t index, uint8_t** row) {
    *row = record(h, index);
    return owner && *row && cc(h, 10511, reinterpret_cast<Word>(owner)) != 0;
}
static bool selectedTypeHasInlineCoords(const uint8_t* unit) {
    const int32_t type = static_cast<int32_t>(load32(unit, 0));
    return type == 2 || (type > 3 && type < 6);
}
static uint32_t unitY(const D2ClientBatch151Host* h, const uint8_t* unit) {
    if (!unit) return 0;
    const Word data = resolve(h, load32(unit, 0x2c));
    if (!data) return 0;
    return selectedTypeHasInlineCoords(unit) ? load32(reinterpret_cast<const void*>(data), 0x10)
                                              : static_cast<uint32_t>(cc(h, 10750, data));
}
static uint32_t unitX(const D2ClientBatch151Host* h, const uint8_t* unit) {
    if (!unit) return 0;
    const Word data = resolve(h, load32(unit, 0x2c));
    if (!data) return 0;
    return selectedTypeHasInlineCoords(unit) ? load32(reinterpret_cast<const void*>(data), 0x0c)
                                              : static_cast<uint32_t>(cc(h, 10867, data));
}
static void setSelectedCoords(const D2ClientBatch151Host* h, Word handle, const uint8_t* unit) {
    const uint32_t y = unitY(h, unit);
    const uint32_t x = unitX(h, unit);
    cc(h, 10739, handle, x, y, 0);
}

} // namespace

// 1.13c D2Client+0x151B0. Validates a character-data record, refreshes the selected unit's
// coordinates and list selection, or creates the fallback entry when no selection exists.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_151b0(
    void* owner, int32_t index, const D2ClientBatch151Host* h) {
    uint8_t* row = nullptr;
    if (!validInput(h, owner, index, &row)) return 0;
    const Word listIndex = cc(h, 11133, reinterpret_cast<Word>(owner));
    fn(h, 0x2A0B0);
    const Word window = resolve(h, load32(owner, 0x2c));
    if (!window) { halt(h); return 0; }
    uint8_t* selected = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(cc(h, 10392, window)));
    if (selected) {
        const Word selectedRecord = cc(h, 10786, reinterpret_cast<Word>(owner), load32(row, 0x138));
        cc(h, 10243, window, selectedRecord);
        setSelectedCoords(h, window, selected);
        cc(h, 10554, window, load32(selected, 0));
        cc(h, 10748, window, load32(selected, 0x0c));
        return 1;
    }
    fn(h, 0xA1860, reinterpret_cast<Word>(owner), 0, 0, listIndex + 4, 0x2a283, 0x6fb509f0);
    cc(h, 10243, reinterpret_cast<Word>(owner), 0);
    return 1;
}

// 1.13c D2Client+0x15350. Opens or creates the row's list entry, queries its valid selection,
// and applies the same coordinate and selected-item updates as 0x151B0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_15350(
    void* owner, int32_t index, const D2ClientBatch151Host* h) {
    uint8_t* row = nullptr;
    row = record(h, index);
    if (!owner || !row) return 0;
    const Word valid = cc(h, 10511, reinterpret_cast<Word>(owner));
    if (!valid) return 0;
    const Word listIndex = cc(h, 11133, reinterpret_cast<Word>(owner));
    fn(h, 0x2A0B0);
    const Word window = resolve(h, load32(owner, 0x2c));
    if (!window) { halt(h); return 0; }
    uint8_t* selected = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(cc(h, 10392, window)));
    if (!selected) {
        selected = reinterpret_cast<uint8_t*>(fn(h, 0xA1A50, static_cast<Word>(-1), listIndex + 4,
                                                reinterpret_cast<Word>(owner), 0, 0));
        if (!selected) { cc(h, 10243, reinterpret_cast<Word>(owner), 0); return 1; }
    } else {
        fn(h, 0xA1860, reinterpret_cast<Word>(owner), 0, 0, listIndex + 4, 0x2a783, 0x6fb50740);
    }
    int32_t selection = 0;
    const Word found = cc(h, 10786, reinterpret_cast<Word>(owner), load32(row, 0x138),
                          valid, reinterpret_cast<Word>(owner));
    if (static_cast<int32_t>(found) < 1) selection = static_cast<int32_t>(found);
    cc(h, 10243, reinterpret_cast<Word>(owner), static_cast<uint32_t>(selection));
    setSelectedCoords(h, window, selected);
    cc(h, 10554, reinterpret_cast<Word>(owner), load32(selected, 0));
    cc(h, 10748, reinterpret_cast<Word>(owner), load32(selected, 0x0c));
    return 1;
}

// 1.13c D2Client+0x15500. Selects a record between the row's two bounds and updates the
// current list entry to either the selected unit or the empty-selection sentinel.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_15500(
    void* owner, int32_t index, const D2ClientBatch151Host* h) {
    if (!h || !owner || !fn(h, 0xA0A30)) return 0;
    uint8_t* row = nullptr;
    if (!validInput(h, owner, index, &row)) return 0;
    const Word selectedRecord = cc(h, 10786, reinterpret_cast<Word>(owner), load32(row, 0x64), index);
    uint8_t* selected = reinterpret_cast<uint8_t*>(fn(h, 0xA5F70));
    if (!selected) selected = reinterpret_cast<uint8_t*>(fn(h, 0xA1A50, static_cast<Word>(-1), selectedRecord,
                                                            reinterpret_cast<Word>(owner), 0));
    else fn(h, 0xA1AD0, 0, selectedRecord, reinterpret_cast<Word>(owner), 0, 0);
    const Word lower = load32(row, 0x140);
    cc(h, 10786, reinterpret_cast<Word>(owner), lower, index);
    Word upper = cc(h, 10786, reinterpret_cast<Word>(owner), load32(row, 0x138), index, selectedRecord);
    // This register participates in retail signed 32-bit address arithmetic.
    if (!h->encode) { halt(h); return 0; }
    int32_t clamped = static_cast<int32_t>(h->encode(h->user, owner));
    const int32_t signedUpper = static_cast<int32_t>(upper);
    const int32_t signedLower = static_cast<int32_t>(lower);
    if (signedUpper < clamped) clamped = signedUpper;
    if (clamped < signedLower) clamped = signedLower;
    // Retail starts with the upper-bound result and substitutes the clamped value only
    // when that value is strictly below it. This matters when the bounds are inverted.
    const uint32_t stored = clamped < signedUpper ? static_cast<uint32_t>(clamped) : static_cast<uint32_t>(upper);
    cc(h, 10243, index, stored);
    if (!selected) {
        cc(h, 10554, index, 1);
        cc(h, 10748, index, static_cast<Word>(-1));
        return 1;
    }
    cc(h, 10554, index, load32(selected, 0));
    cc(h, 10748, index, load32(selected, 0x0c));
    const Word y = fn(h, 0x1660, reinterpret_cast<Word>(selected));
    const Word x = fn(h, 0x1630, reinterpret_cast<Word>(selected));
    cc(h, 10739, resolve(h, load32(owner, 0x2c)), x, y, 0);
    return 1;
}

// 1.13c D2Client+0x15ab0. Builds one placement descriptor for each eligible entry in the
// selected range and dispatches it to the client's placement worker.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_15ab0(
    void* owner, int32_t index, const D2ClientBatch151Host* h) {
    uint8_t* row = nullptr;
    if (!h || !owner || !(row = record(h, index))) return 0;
    fn(h, 0x2A0B0);
    const Word window = resolve(h, load32(owner, 0x2c));
    if (!window) { halt(h); return 0; }
    uint8_t* selected = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(cc(h, 10392, window)));
    if (!selected) return 0;
    const int32_t first = static_cast<int32_t>(fn(h, 0x40550, reinterpret_cast<Word>(owner)));
    if (first < 0 || static_cast<uint32_t>(first) >= h->selectedCount) return 0;
    const int32_t selectedX = static_cast<int32_t>(fn(h, 0x1630, reinterpret_cast<Word>(selected)));
    const int32_t selectedY = static_cast<int32_t>(fn(h, 0x1660, reinterpret_cast<Word>(selected)));
    const int32_t ownerX = static_cast<int32_t>(fn(h, 0x1630, reinterpret_cast<Word>(owner)));
    const int32_t ownerY = static_cast<int32_t>(fn(h, 0x1660, reinterpret_cast<Word>(owner)));
    const int32_t count = static_cast<int32_t>(cc(h, 10786, reinterpret_cast<Word>(owner), load32(row, 0x138)));
    if (count <= 0) return 1;
    PlacementDescriptor151 desc;
    desc.flags = 0x21;
    desc.owner = owner;
    desc.source = reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<uint32_t>(first))); // retail dword 2 = 0x40550 result
    desc.window = reinterpret_cast<void*>(window);
    desc.x = static_cast<uint32_t>(selectedX) * 2u - static_cast<uint32_t>(ownerX);
    desc.y = static_cast<uint32_t>(selectedY) * 2u - static_cast<uint32_t>(ownerY);
    desc.context = h->placementContext;
    desc.workerAddress = 0x6fb23b60;
    for (int32_t i = 0; i < count; ++i) {
        desc.iteration = static_cast<uint32_t>(i);
        fn(h, 0xB30F0, reinterpret_cast<Word>(&desc));
    }
    return 1;
}

// 1.13c D2Client+0x15c40 (sub_15C40). Creates a UI placement descriptor, chooses its mode from the
// current client selection state, and returns the selected unit's screen-relative coordinates.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_15c40(
    void* owner, int32_t index, uint32_t extra, const D2ClientBatch151Host* h) {
    if (!h || !record(h, index)) return 0;
    if (owner) store32(owner, 0xc4, load32(owner, 0xc4) | 0x40);
    int32_t screenY = 0;
    int32_t screenX = 0;
    // Normalize A1340: stack output Y, EDI output X, EBX owner.
    if (!fn(h, 0xA1340, reinterpret_cast<Word>(&screenY), reinterpret_cast<Word>(&screenX), reinterpret_cast<Word>(owner))) return 0;
    const int32_t selectedIndex = static_cast<int32_t>(fn(h, 0x40550, reinterpret_cast<Word>(owner)));
    if (selectedIndex < 0 || static_cast<uint32_t>(selectedIndex) >= h->selectedCount) return 0;
    const Word state = fn(h, 0xA5F70);
    PlacementDescriptor151 desc;
    desc.flags = state ? 0x20 : 0x420;
    desc.owner = owner;
    desc.source = desc.owner;
    desc.state = reinterpret_cast<void*>(state);
    desc.selectedIndex = static_cast<uint32_t>(selectedIndex);
    desc.y = static_cast<uint32_t>(screenY);
    desc.index = static_cast<uint32_t>(index);
    desc.context = extra;
    void* placed = reinterpret_cast<void*>(fn(h, 0xB30F0, reinterpret_cast<Word>(&desc)));
    if (!placed) return 1;
    cc(h, 10637, reinterpret_cast<Word>(placed), state ? 1 : 2);
    const Word x = fn(h, 0x1630, reinterpret_cast<Word>(owner));
    const Word y = fn(h, 0x1660, reinterpret_cast<Word>(owner));
    const int16_t deltaY = static_cast<int16_t>(static_cast<uint32_t>(screenY) - static_cast<uint32_t>(y));
    const int16_t deltaX = static_cast<int16_t>(static_cast<uint32_t>(screenX) - static_cast<uint32_t>(x));
    cc(h, 10019, h->callerStackWord, (static_cast<uint32_t>(deltaY) << 16) + static_cast<uint32_t>(static_cast<int32_t>(deltaX)));
    return 1;
}
