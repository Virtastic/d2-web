#include <cstdint>

struct Gm56250Io {
    void (*getSavePath)(char* dir, uint32_t cap);                        // 0xA316: ECX=dir, EDX=cap
    int (*format)(char* dst, const char* fmt, const char* dir, const char* name);   // 0x2A6D
    void* (*open)(const char* path, const char* mode);                   // 0x2FD0
    uint32_t (*read)(void* dst, uint32_t elem, uint32_t count, void* file);         // 0x30CC
    int (*close)(void* file);                                            // 0x32B7
};

// Call shapes exactly as 0x56250 enters them: legacy is __thiscall (buffer in ECX, &unit in EAX) with
// pGame pClient size pRoomArg nXArg nYArg on the stack; current has size in EAX and pGame pClient
// buffer &unit pRoomArg nXArg nYArg on the stack.
struct Gm56250Parsers {
    uint32_t (*legacy)(uint32_t* unit, const uint8_t* save, uint32_t size, uint32_t game, uint32_t client, uint32_t room, uint32_t x, uint32_t y);
    uint32_t (*current)(uint32_t size, uint32_t game, uint32_t client, const uint8_t* save, uint32_t* unit, uint32_t room, uint32_t x, uint32_t y);
};

namespace {
constexpr uint32_t kMagic = 0xAA55AA55u;         // D2SMAGIC_HEADER, first dword of a .d2s
constexpr uint32_t kFirstCurrent = 0x5Cu;        // unsigned version < 0x5C -> legacy parser
constexpr uint32_t kReadCap = 0x2000u;           // _fread(save, 1, 0x2000, file)
constexpr uint32_t kErrBadHeader = 9u;
constexpr uint32_t kErrNoUnit = 0x0Eu;
constexpr uint32_t kPathCap = 0x400u;

uint32_t le32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
}

// 1.13c D2Game+0x56250: D2GAME_SAVE_ReadFile -- build
// "<savePath><name>.d2s", read up to 0x2000 bytes, check the 8-byte header, dispatch to the legacy
// (version < 0x5C) or current save parser, and copy the parsed unit out. The file system, the path
// formatter and both parsers are injected: the row's rule is the read/header/dispatch sequence.
uint32_t GAME_56250(uint32_t game, uint32_t client, const char* name, uint32_t* ppPlayer,
                    uint32_t room, uint32_t x, uint32_t y,
                    const Gm56250Io& io, const Gm56250Parsers& parsers) {
    char dir[kPathCap];     // 1.13c leaves these uninitialized; every byte read was written first
    char path[500];
    uint8_t save[kReadCap];
    uint32_t unit = 0;

    *ppPlayer = 0;
    io.getSavePath(dir, kPathCap);
    io.format(path, "%s%s.d2s", dir, name);
    void* file = io.open(path, "rb");
    if (!file) return kErrNoUnit;
    const uint32_t n = io.read(save, 1, kReadCap, file);
    io.close(file);
    if (n == 0) return kErrNoUnit;
    if (n < 8 || le32(save) != kMagic) return kErrBadHeader;
    const uint32_t version = le32(save + 4);
    const uint32_t result = version < kFirstCurrent
        ? parsers.legacy(&unit, save, n, game, client, room, x, y)
        : parsers.current(n, game, client, save, &unit, room, x, y);
    if (result) return result;
    *ppPlayer = unit;
    return unit ? 0u : kErrNoUnit;
}
