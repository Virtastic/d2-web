// Prior art used: D2Client has no D2MOO implementation; community pointer tables
// identify imports only. Rules below come from the cached 1.13c functions.
// Register arguments are explicit. Retail pointers in byte images are uint32_t;
// resolve maps them to native wasm64 pointers. Dependencies are mandatory callbacks.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

namespace {
template<class T> T read(const void* p, std::ptrdiff_t offset) {
    T v; std::memcpy(&v, static_cast<const uint8_t*>(p) + offset, sizeof v); return v;
}
template<class T> void write(void* p, uint32_t offset, T v) {
    std::memcpy(static_cast<uint8_t*>(p) + offset, &v, sizeof v);
}
}
struct D2Client21d60Env {
    void* context;
    void* (*resolve)(void*, uint32_t);
    // Ordinal_10265 -> Ordinal_10024, then retail exits with -1.
    void (*report)(void*, uint32_t);
    uint32_t* listHead; // DAT_6fbcc4d4, serialized linked-list addresses
    uint32_t* unitBuckets; // DAT_6fbba808: 128 serialized unit addresses
    void (*remove215b0)(void*, uint32_t);
    void (*upsert218c0)(void*, uint32_t, int32_t, uint32_t, uint32_t);
    void (*update4ef50)(void*, void*, const void*);
    // Retail passes the matched unit through ESI, then EAX respectively.
    void (*refreshA6210)(void*, void*);
    void (*refresh4c150)(void*, void*);
    void (*ordinal11090)(void*, void*, int32_t);
    void (*roomBounds10561)(void*, void*, int32_t*); // four rectangle words
    void* (*roomTiles10544)(void*, void*, uint32_t*);
    const uint32_t* modePairs; // DAT_6fba5538: fourteen pairs
    uint32_t (*select22490)(void*, const void*); // ESI object
    const uint8_t* const* selectionTable; // DAT_6fba6c58, 573 native pointers
    const int32_t* classValues; // DAT_6fba5998, eight entries
    const uint8_t* restrictedSelection; // identity of DAT_6fba6988
};
namespace {
[[noreturn]] void fatal(D2Client21d60Env* e, uint32_t line) {
    e->report(e->context, line); std::exit(-1);
}
void require(D2Client21d60Env* e, const void* p, uint32_t line) {
    if (!p) fatal(e, line);
}
}
extern "C" {
// 1.13c D2Client+0x21D60
void d2_client_21d60(D2Client21d60Env* e, uint32_t id, int32_t mode,
                     uint32_t arg1, uint32_t arg2, const uint32_t* data) {
    if (mode == 7) e->remove215b0(e->context, id);
    // Retail walks the list here without changing it, then calls the upsert.
    uint32_t address = *e->listHead;
    while (address) {
        void* node = e->resolve(e->context, address);
        if (read<uint32_t>(node, 8) == id) break;
        address = read<uint32_t>(node, 0x30);
    }
    e->upsert218c0(e->context, id, mode, arg1, arg2);
    address = *e->listHead;
    while (address) {
        void* node = e->resolve(e->context, address);
        if (read<uint32_t>(node, 8) == id) {
            write(node, 8, id); write(node, 0xc, arg1); write(node, 4, mode);
            write(node, 0x24, data[0]); write(node, 0x28, data[1]); write(node, 0x2c, data[2]);
            if (mode != 7) return;
            // Retail local struct has the low word of data[1], followed by data[0].
            struct Update { uint16_t low, padding; uint32_t value; };
            const Update update{static_cast<uint16_t>(data[1]), 0, data[0]};
            address = e->unitBuckets[id & 0x7f];
            while (address) {
                void* unit = e->resolve(e->context, address);
                if (read<uint32_t>(unit, 0xc) == id) {
                    if (read<uint32_t>(unit, 0) != 1) fatal(e, 0x5a);
                    e->update4ef50(e->context, unit, &update);
                    e->refreshA6210(e->context, unit);
                    e->refresh4c150(e->context, unit);
                    e->ordinal11090(e->context, unit, 1);
                    return;
                }
                address = read<uint32_t>(unit, 0xe4);
            }
            return;
        }
        address = read<uint32_t>(node, 0x30);
    }
    fatal(e, 0x95);
}
// 1.13c D2Client+0x21ED0
void* d2_client_21ed0(D2Client21d60Env* e, int32_t x, int32_t y, void* room) {
    // The cached decompiler loses the stack out-parameters. x86 instructions
    // confirm rectangle at esp+0x10 and count at esp+0xc, then 0x30-byte tiles.
    int32_t bounds[4];
    e->roomBounds10561(e->context, room, bounds);
    const int32_t tileX = static_cast<int32_t>(uint32_t(x) - uint32_t(bounds[0])) / 5;
    const int32_t tileY = static_cast<int32_t>(uint32_t(y) - uint32_t(bounds[1])) / 5;
    uint32_t count;
    auto* tiles = static_cast<uint8_t*>(e->roomTiles10544(e->context, room, &count));
    for (uint32_t i = 0; i < count; ++i) {
        auto* tile = tiles + size_t(i) * 0x30;
        if (read<int32_t>(tile, 8) == tileX && read<int32_t>(tile, 0xc) == tileY) return tile;
    }
    return nullptr;
}
// 1.13c D2Client+0x21FB0
void d2_client_21fb0(D2Client21d60Env* e, void* object, uint32_t value) {
    require(e, object, 0xcf2); write(object, 0x84, value);
}
// 1.13c D2Client+0x22010
void d2_client_22010(D2Client21d60Env* e, void* object, uint32_t value) {
    require(e, object, 0xcdc); write(object, 0x80, value);
}
// 1.13c D2Client+0x22040
uint32_t d2_client_22040(D2Client21d60Env* e, const void* object) {
    require(e, object, 0xcd1); return read<uint32_t>(object, 0x80);
}
// 1.13c D2Client+0x22070
void d2_client_22070(D2Client21d60Env* e, void* object, uint32_t value) {
    require(e, object, 0xcc6); write(object, 0x7c, value);
}
// 1.13c D2Client+0x220A0
uint32_t d2_client_220a0(D2Client21d60Env* e, const void* object) {
    require(e, object, 0xcbb); return read<uint32_t>(object, 0x7c);
}
// 1.13c D2Client+0x221C0
uint32_t d2_client_221c0(D2Client21d60Env* e, const void* object, uint32_t mode) {
    require(e, object, 0xc08);
    mode &= 15;
    return mode >= 1 && mode <= 9 ? 0x18e : (mode == 10 || mode == 11 ? 0x191 : 0);
}
// 1.13c D2Client+0x22280
uint32_t d2_client_22280(D2Client21d60Env* e, const void* object, uint32_t mode) {
    require(e, object, 0xb5c);
    mode &= 15;
    if (mode >= 14) fatal(e, 0xb60);
    return e->modePairs[mode * 2];
}
// 1.13c D2Client+0x22320
uint32_t d2_client_22320(D2Client21d60Env* e, const void* object) {
    require(e, object, 0xc63);
    const uint32_t address = read<uint32_t>(object, 0x78);
    return address ? read<uint32_t>(e->resolve(e->context, address), 0) : 0;
}
// 1.13c D2Client+0x224F0
int32_t d2_client_224f0(D2Client21d60Env* e, const void* object, int32_t classId) {
    const uint32_t index = e->select22490(e->context, object);
    if (index > 0x23c || classId > 7) fatal(e, 0xe86);
    const uint8_t* entry = e->selectionTable[index];
    if (!entry) return 0;
    if (index == 0x3d) return e->classValues[classId];
    const int32_t first = read<int32_t>(entry, 0x20);
    const int32_t second = read<int32_t>(entry, 0x28);
    if (first || second) {
        const int32_t a = read<int32_t>(entry, 0x24);
        if (a == classId || a == 8) return first;
        const int32_t b = read<int32_t>(entry, 0x2c);
        if (b == classId || b == 8) return second;
    }
    return 0;
}
// 1.13c D2Client+0x22570
uint32_t d2_client_22570(D2Client21d60Env* e, const void* object, int32_t minimumClass, int32_t classId) {
    const uint32_t index = e->select22490(e->context, object);
    if (index > 0x23c || classId > 7) fatal(e, 0xe72);
    const uint8_t* entry = e->selectionTable[index];
    if (entry && (entry != e->restrictedSelection || minimumClass <= classId))
        return read<uint32_t>(entry, std::ptrdiff_t(classId) * 4);
    return 0;
}
}
