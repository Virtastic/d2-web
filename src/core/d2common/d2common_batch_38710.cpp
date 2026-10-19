// D2Common 1.13c implementation batch. External DLL services are explicit callbacks so
// their call protocol can be attached when these rows receive their Stage B vectors.
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace {
template<class T> T at(const void* p, size_t offset) {
    T value;
    std::memcpy(&value, static_cast<const uint8_t*>(p) + offset, sizeof value);
    return value;
}
template<class T> void put(void* p, size_t offset, T value) {
    std::memcpy(static_cast<uint8_t*>(p) + offset, &value, sizeof value);
}
// These objects are retail-layout images: pointer slots in the x86-32 images are DWORDs even
// though the engine that hosts these functions is wasm64. Resolve the guest value explicitly;
// never load a native pointer-sized value from a retail offset.
using RetailPointerResolver = void* (*)(uint32_t guestAddress);
void* retailPointer(const void* p, size_t offset, RetailPointerResolver resolve) {
    const uint32_t guestAddress = at<uint32_t>(p, offset);
    return guestAddress != 0 && resolve != nullptr ? resolve(guestAddress) : nullptr;
}
}

// 1.13c D2Common+0x38710
void D2Common_38710_ForwardRoomSelection(void* room, uint32_t flags, int mode,
                                          void (*select)(void*, uint32_t, int)) {
    select(room, flags, mode);
}

// 1.13c D2Common+0x1DAD0
void D2Common_1DAD0_VisitPathCells(void* unit, uint32_t mask, int context,
                                    void (*onCell)(), void (*step)(void*, void*, int),
                                    void* (*roomOf)(void*), uint16_t (*collides)(void*, unsigned, unsigned, int, uint32_t),
                                    void* (*find)(void*, unsigned, unsigned, int),
                                    RetailPointerResolver resolve) {
    auto* path = static_cast<uint8_t*>(retailPointer(unit, 0x2c, resolve));
    if (!path) return;
    put(path, 0x34, at<uint32_t>(path, 0x34) & 0xfff800ffu);
    put(path, 0x3c, uint32_t{0x10});
    put(path, 0x98, uint32_t{0});
    const int mode = at<int32_t>(path, 0x40);
    if (mode == 8 || mode == 11) return; // retail reports a fatal mode assertion
    step(path, unit, 0);
    void* room = roomOf(unit);
    const int count = at<int32_t>(path, 0x28);
    for (int i = 0; i < count; ++i) {
        const unsigned x = at<uint16_t>(path, 0x9c + 4 * i);
        const unsigned y = at<uint16_t>(path, 0x9e + 4 * i);
        if (collides(room, x, y, context, mask) && find(room, x, y, context)) onCell();
    }
}

// 1.13c D2Common+0x3B760
void D2Common_3B760_RoomBounds(const void* room, int32_t out[4]) {
    if (!room) return; // retail asserts
    out[1] = at<int32_t>(room, 0x50);
    out[3] = at<int32_t>(room, 0x58) + out[1];
    out[0] = at<int32_t>(room, 0x4c);
    out[2] = at<int32_t>(room, 0x54) + out[0];
}

// 1.13c D2Common+0x2FA20
uint32_t D2Common_2FA20_PathDirection(const void* unit,
                                      uint32_t (*directionTo)(const void*, unsigned, unsigned),
                                      RetailPointerResolver resolve) {
    if (!unit) return 0; // retail asserts
    const auto* path = static_cast<const uint8_t*>(retailPointer(unit, 0x2c, resolve));
    if (!path) return 0;
    const int type = at<int32_t>(unit, 0);
    const bool dynamic = type == 2 || type == 4 || type == 5;
    const uint32_t x = dynamic ? at<uint32_t>(path, 0x0c) : at<uint16_t>(path, 2);
    if (at<uint16_t>(path, 0x10) == x) {
        const uint32_t y = dynamic ? at<uint32_t>(path, 0x10) : at<uint16_t>(path, 6);
        if (at<uint16_t>(path, 0x12) == y) return at<uint8_t>(path, 0x65);
    }
    return directionTo(unit, at<uint16_t>(path, 0x10), at<uint16_t>(path, 0x12));
}

// 1.13c D2Common+0x37350
void D2Common_37350_GetRoomFields(const void* object, uint32_t* a, uint32_t* b,
                                   uint32_t* c, uint32_t* d,
                                   RetailPointerResolver resolve) {
    if (!object) return; // retail asserts
    const auto* data = static_cast<const uint8_t*>(retailPointer(object, 4, resolve));
    if (!data) return;
    *a = at<uint32_t>(data, 0x1c);
    *b = at<uint32_t>(data, 0x20);
    *c = at<uint32_t>(data, 0x24);
    *d = 0;
}

// 1.13c D2Common+0x3C070
void D2Common_3C070_IsoBounds(const void* room, int32_t out[4]) {
    if (!room) return; // retail asserts
    const int32_t x = at<int32_t>(room, 0x5c);
    const int32_t y = at<int32_t>(room, 0x60);
    const int32_t w = at<int32_t>(room, 0x64);
    const int32_t h = at<int32_t>(room, 0x68);
    out[1] = (y + 2 + x) * 0x28;
    out[0] = (x - y - h) * 0x50 - 0x50;
    out[2] = (x - y + w) * 0x50 - 0x50;
    out[3] = (y + x + h + 2 + w) * 0x28;
}

// 1.13c D2Common+0x3BBD0
void D2Common_3BBD0_AnimateRoomTiles(const void* room, void (*animate)(),
                                    RetailPointerResolver resolve) {
    if (!room) return;
    const auto* data = static_cast<const uint8_t*>(retailPointer(room, 0x10, resolve));
    if (!data) return;
    for (int32_t i = 0, count = at<int32_t>(data, 0x2c); i < count; ++i) animate();
}

// 1.13c D2Common+0x12890
void D2Common_12890_WriteGameLog(FILE* stream, int game, int frame, int client,
                                  int type, int size, const char* format, ...) {
    if (!stream) return;
    char message[512];
    const int head = std::snprintf(message, sizeof message, "Game:%02d\tFrame:%d\tClient:%02d\tType:%sSize:%d\n",
                                   game, frame, client, type == 1 ? "System\t" : "Game\t", size);
    if (head < 0 || size_t(head) >= sizeof message) return;
    va_list args;
    va_start(args, format);
    std::vsnprintf(message + head, sizeof message - size_t(head), format, args);
    va_end(args);
    std::fputs(message, stream);
    std::fflush(stream);
}

// 1.13c D2Common+0x37430
uint32_t D2Common_37430_GetRoomField(const void* object, RetailPointerResolver resolve) {
    if (!object) return 0; // retail asserts
    const auto* data = static_cast<const uint8_t*>(retailPointer(object, 4, resolve));
    return data ? at<uint32_t>(data, 0x14) : 0;
}

// 1.13c D2Common+0x325C0
uint8_t D2Common_325C0_MeleeRange(void* unit,
                                   int (*equippedHand)(void*, void**, int*, int*),
                                   uint8_t (*itemRange)(void*),
                                   const void* (*monStats)(int),
                                   void (*weaponClass)(void*, void*, void**, int, int),
                                   RetailPointerResolver resolve) {
    if (!unit) return 0; // retail asserts
    const int type = at<int32_t>(unit, 0);
    if (type == 0) {
        void* item = unit;
        int body = 0, held = 0;
        void* inventory = retailPointer(unit, 0x60, resolve);
        if (inventory && equippedHand(inventory, &item, &body, &held)) return itemRange(unit);
    } else if (type == 1) {
        const auto* stats = static_cast<const uint8_t*>(monStats(at<int32_t>(unit, 4)));
        if (stats) {
            const uint8_t range = at<uint8_t>(stats, 0x0e);
            if (range != 0xff) return range;
            void* value = unit;
            weaponClass(unit, retailPointer(unit, 0x60, resolve), &value, -1, 1);
            return value == reinterpret_cast<void*>(6) ? 0 : 2;
        }
    }
    return 0;
}

// 1.13c D2Common+0x4CB90
void D2Common_4CB90_FindFreePosition(void* room, int32_t position[2], int radius,
                                      uint32_t mask, void** result,
                                      void (*bounds)(void*, int32_t[4]),
                                      void* (*roomAt)(void*, int32_t, int32_t),
                                      uint16_t (*collision)(void*, const int32_t[4], uint32_t)) {
    int32_t box[4];
    bounds(room, box);
    const int32_t startX = position[0], startY = position[1];
    for (int extent = 1, low = -1; low >= -0x31; ++extent, --low) {
        for (int dy = low; dy < extent; dy += 2) {
            position[1] = startY + dy;
            void* yRoom = position[1] < box[1] || position[1] >= box[1] + box[3]
                              ? roomAt(room, position[0], position[1]) : room;
            if (!yRoom) continue;
            for (int dx = low; dx < extent; dx += 2) {
                position[0] = startX + dx;
                int32_t current[4];
                bounds(yRoom, current);
                void* xRoom = position[0] < current[0] || position[0] >= current[0] + current[2]
                                  ? roomAt(yRoom, position[0], position[1]) : yRoom;
                if (!xRoom) continue;
                const int diameter = radius + 2;
                int32_t target[4] = {position[0] - diameter / 2, position[1] - diameter / 2,
                                     position[0] - diameter / 2 + diameter - 1,
                                     position[1] - diameter / 2 + diameter - 1};
                if (!collision(xRoom, target, mask)) { *result = xRoom; return; }
            }
        }
    }
    *result = nullptr;
}

// 1.13c D2Common+0x22E70
uint8_t D2Common_22E70_ItemTableByte(uint32_t index, const void* records, uint32_t count) {
    if (index >= count || !records) return 0; // retail asserts
    return at<uint8_t>(records, size_t(index) * 0x1a8 + 0x11c);
}
