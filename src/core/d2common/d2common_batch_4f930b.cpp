// D2Common 1.13c implementation batch. External DLL services are explicit callbacks so their
// call protocol can be attached when these rows receive their Stage B vectors.
#include <cstdint>
#include <cstddef>
#include <cstdio>
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
}

// 1.13c D2Common+0x17AA0
void D2Common_17AA0_RunActTable0(
    void* game, void* self, int index,
    void* tableA, int (*fnA)(), void* tableB, int (*fnB)(),
    void (*runTable)(void*, void*, int (*)(), void (*)(int*)),
    void (*apply)(void*, int, int, int)) {
    runTable(game, tableA, fnA, nullptr);
    runTable(game, tableB, fnB, nullptr);
    apply(self, index, 0x28, 0x2e);
}

// 1.13c D2Common+0x17AE0
void D2Common_17AE0_RunActTable1(
    void* game, void* self, int index,
    void* tableA, int (*fnA)(), void (*postA)(int*),
    void* tableB, int (*fnB)(), void (*postB)(int*),
    void (*runTable)(void*, void*, int (*)(), void (*)(int*)),
    void (*apply)(void*, int, int, int)) {
    runTable(game, tableA, fnA, postA);
    runTable(game, tableB, fnB, postB);
    apply(self, index, 1, 0x11);
}

// 1.13c D2Common+0x2D420
void D2Common_2D420_MarkRoomChainFlag(void* /*unused*/, void* drlg) {
    if (!at<uint32_t>(drlg, 8) || !at<uint32_t>(drlg, 0x22c)) return;
    const auto* counters = static_cast<const uint8_t*>(at<void*>(drlg, 0x22c));
    void* node = at<void*>(drlg, 0x10);
    for (uint32_t offset = 0; node; offset += 4, node = at<void*>(node, 0x24)) {
        uint32_t counter;
        std::memcpy(&counter, counters + offset, sizeof counter);
        if (counter) put(node, 0x60, at<uint32_t>(node, 0x60) | 1u);
    }
}

// 1.13c D2Common+0x2D4E0
void D2Common_2D4E0_LoadTownTiles(void* drlg, void (*loadTiles)(void*, const char*)) {
    char path[0x110] = {};
    const char* format;
    switch (at<uint8_t>(drlg, 0x480)) {
    case 0: format = "%s\\Tiles\\Act1\\Town\\Floor.dt1"; break;
    case 1: format = "%s\\Tiles\\Act2\\Town\\Ground.dt1"; break;
    case 2: format = "%s\\Tiles\\ACT3\\Kurast\\sets.dt1"; break;
    default: return;
    }
    std::snprintf(path, sizeof path, format, "DATA\\GLOBAL");
    loadTiles(static_cast<uint8_t*>(drlg) + 0xc, path);
}

// 1.13c D2Common+0x7B8D0
void D2Common_7B8D0_InitDrlgLists(void* drlg) {
    auto* base = static_cast<uint8_t*>(drlg);
    put<void*>(drlg, 0xbc, base + 0xa0);
    put<void*>(drlg, 0x188, base + 0xa0);
    put<void*>(drlg, 0x1a8, base + 0x18c);
    put<void*>(drlg, 0x274, base + 0x18c);
    put<void*>(drlg, 0x294, base + 0x278);
    put<void*>(drlg, 0x360, base + 0x278);
    put<uint8_t>(drlg, 0xe4, 0);
    put<uint8_t>(drlg, 0x1d0, 1);
    put<uint8_t>(drlg, 700, 2);
    put<void*>(drlg, 0x380, base + 0x364);
    put<void*>(drlg, 0x44c, base + 0x364);
    put<uint8_t>(drlg, 0x3a8, 3);
}

// 1.13c D2Common+0x43580
void D2Common_43580_InitDrlgRoom(
    void* drlg, const int32_t* levelThresholds,
    void (*grid)(int, int),
    void (*setup)(void*, int, void*, int),
    void (*mid)(),
    void (*act0)(), void (*act1)(),
    void (*act2SetLevel)(int), void (*act2A)(), void (*act2B)(), void (*act2C)(),
    void (*act3)(), void (*act4)(),
    void (*finish)(void*)) {
    const uint32_t gridArg = at<uint32_t>(at<void*>(drlg, 0x1b4), 0x478);
    void* room = at<void*>(drlg, 0x14);
    put<uint32_t>(room, 0x54, 0);
    put<uint32_t>(room, 0x58, 0);
    put<int32_t>(room, 0x5c, at<int32_t>(drlg, 0x24) / 8);
    put<int32_t>(room, 0x60, at<int32_t>(drlg, 0x28) / 8);
    const int gridX = at<int32_t>(room, 0x5c);
    const int gridY = at<int32_t>(room, 0x60);
    grid(gridX, gridY);
    grid(gridX, gridY);
    grid(gridX, gridY);
    grid(gridX, gridY);
    const int level = at<int32_t>(drlg, 0x1d0);
    int variant = 0;
    for (int i = 1; i < 6; ++i) {
        if (level < levelThresholds[i]) { variant = i - 1; break; }
    }
    setup(static_cast<uint8_t*>(drlg) + 0x1c, static_cast<int>(gridArg),
          static_cast<uint8_t*>(room) + 100, at<int32_t>(room, 0x264));
    mid();
    switch (level == 0x86 ? 1 : variant) {
    case 0: act0(); break;
    case 1: act1(); break;
    case 2:
        act2SetLevel(reinterpret_cast<intptr_t>(drlg));
        act2A();
        act2B();
        act2C();
        break;
    case 3: act3(); break;
    case 4: act4(); break;
    default: break;
    }
    finish(drlg);
}
