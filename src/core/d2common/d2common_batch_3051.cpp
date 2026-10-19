// Stage A: 1.13c control flow, with register inputs and imported calls explicit.
// Prior art: D2MOO Items.cpp supplies the two item decoders. Other bodies follow the
// cached 1.13c routines. Retail-layout links remain 32-bit tokens; resolve them to
// native wasm64 pointers instead of truncating native pointers to retail addresses.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "D2Items.h"
#include "D2BitManip.h"

struct D2Common3051Env {
    void* (*resolve)(uint32_t);
    // Adapter performs Fog 10265(messageId), then Fog 10024. This routine exits even
    // if that diagnostic adapter returns, matching the following retail _exit(-1).
    void (*diagnostic)(uint32_t);
};
namespace {
uint8_t* bytes(void* p) { return static_cast<uint8_t*>(p); }
uint32_t read32(void* p, uint32_t off) {
    uint32_t v; std::memcpy(&v, bytes(p) + off, sizeof(v)); return v;
}
int32_t signed32(void* p, uint32_t off) { return static_cast<int32_t>(read32(p, off)); }
int16_t read16(void* p, uint32_t off) {
    int16_t v; std::memcpy(&v, bytes(p) + off, sizeof(v)); return v;
}
void* link(void* p, uint32_t off, const D2Common3051Env* env) {
    const uint32_t token = read32(p, off);
    return token ? env->resolve(token) : nullptr;
}
[[noreturn]] void fatal(const D2Common3051Env* env, uint32_t message) {
    env->diagnostic(message);
    std::_Exit(-1);
}
int32_t add32(int32_t a, int32_t b) {
    return static_cast<int32_t>(static_cast<uint32_t>(a) + static_cast<uint32_t>(b));
}
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3b5b0(void* object,
    const D2Common3051Env* env) { // 1.13c D2Common+0x3B5B0
    if (!object) fatal(env, 0x2c5);
    return read32(object, 0x20);
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7bf20(void* owner,
    uint32_t arg1, uint32_t arg2, int32_t ownerId, const D2Common3051Env* env,
    void* (*getOrCreateRoom)(void*, int32_t),
    void* (*findPath)(uint32_t, uint32_t, void*, void*),
    void (*pathReleased)(), void (*notify)(void*, uint32_t)) { // 1.13c D2Common+0x7BF20
    void* node = link(owner, 0x47c, env);
    while (node && signed32(node, 0x1d0) != ownerId) node = link(node, 0x1ac, env);
    if (!node) node = getOrCreateRoom(owner, ownerId);
    void* path = findPath(arg1, arg2, owner, node);
    if (path && read16(path, 0x0e)) {
        const uint16_t count = static_cast<uint16_t>(read16(path, 0x0e)) - 1u;
        std::memcpy(bytes(path) + 0x0e, &count, sizeof(count));
        pathReleased();
        notify(path, 2);
    }
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4eb40(void* object,
    uint32_t value, const D2Common3051Env* env) { // 1.13c D2Common+0x4EB40
    if (!object) fatal(env, 0x638);
    void* root = link(object, 0xa8, env);
    void* node = root ? link(root, 4, env) : nullptr;
    if (!node) fatal(env, 0xab1);
    do {
        if (read32(node, 0) == value) return 1;
        node = link(node, 4, env);
    } while (node);
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4f4c0(void* object,
    int32_t key, const D2Common3051Env* env) { // 1.13c D2Common+0x4F4C0
    if (!object) return 0;
    void* root = link(object, 0xa8, env);
    for (void* node = root ? link(root, 4, env) : nullptr; node; node = link(node, 4, env))
        if (read16(link(node, 0, env), 0) == key && signed32(node, 0x34) == -1)
            return read32(node, 0x28);
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void* d2_fn_d2common_41f70(void* p1, uint32_t a,
    uint32_t b, uint32_t c, void* out, void* fallbackBase,
    void (*format)(void*, uint32_t, uint32_t, uint32_t, void*, char*, uint8_t*,
                   uint32_t, uint32_t), void* (*currentString)()) { // 1.13c D2Common+0x41F70
    uint8_t local[12]{};
    format(p1, a, b, c, out, reinterpret_cast<char*>(local + 4), local, 0, 1);
    void* result = currentString();
    return result ? result : bytes(fallbackBase) + 0x404;
}

// Adopt the existing native-layout decoder rather than applying x86 offsets to a
// wasm64 D2UnitStrc. Version, gamble, header and socket-count inputs stay explicit.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2a600(D2UnitStrc* item,
    D2BitBufferStrc* buffer, int32_t checkHeader, int32_t gamble, int* socketedItems,
    uint32_t version) { // 1.13c D2Common+0x2A600
    return ITEMS_DecodeItemBitstreamComplete(item, buffer, checkHeader, gamble, socketedItems, version);
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2c0c0(D2UnitStrc* item,
    D2BitBufferStrc* buffer, int32_t checkHeader, uint32_t version) { // 1.13c D2Common+0x2C0C0
    return ITEMS_DecodeItemBitstreamCompact(item, buffer, checkHeader, version);
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2e790(void* path,
    const D2Common3051Env* env, const int32_t (*mapping)[2],
    void (*normalize)(void*, int32_t*, int32_t*, int32_t*),
    void (*resolve)(void*, int32_t, int32_t, int32_t, void*, int32_t*)) { // 1.13c D2Common+0x2E790
    int32_t x = signed32(path, 0), y = signed32(path, 4);
    // Retail saves this pointer before normalize, which may mutate the path.
    void* auxiliary = link(path, 0x60, env);
    int32_t level = signed32(path, ((x == 0 || x == 1) && read32(path, 0x30)) ? 0x40 : 0x10);
    if ((read32(path, 0xc8) >> 3) & 1) normalize(path, &x, &y, &level);
    resolve(path, x, y, level, auxiliary, &x);
    for (int32_t i = 0; i < 14; ++i)
        if (x == mapping[i][1]) return mapping[i][0];
    return -1;
}

extern "C" EMSCRIPTEN_KEEPALIVE void* d2_fn_d2common_44ce0(void* owner, int32_t index,
    uint8_t* tableRows, int32_t rowCount, const D2Common3051Env* env) { // 1.13c D2Common+0x44CE0
    for (void* node = link(owner, 0x90, env); node; node = link(node, 0x44, env)) {
        if (!read32(node, 0)) fatal(env, 0x410);
        if (signed32(node, 0) == index) return bytes(node) + 4;
    }
    if (index < 0 || index >= rowCount) return reinterpret_cast<void*>(uintptr_t{0x48});
    return tableRows + static_cast<uint32_t>(index) * 0x9c + 0x48;
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_2d5b0(void* room,
    int32_t x, int32_t y, const D2Common3051Env* env) { // 1.13c D2Common+0x2D5B0
    for (void* node = link(room, 0x47c, env); node; node = link(node, 0x1ac, env)) {
        const int32_t nx = signed32(node, 0x1c), ny = signed32(node, 0x20);
        if (x >= nx && y >= ny && x < add32(nx, signed32(node, 0x24)) &&
            y < add32(ny, signed32(node, 0x28))) return read32(node, 0x1d0);
    }
    return 0;
}

// The cached body has no stores or calls on its nonfatal paths: it traverses the
// rectangles and returns normally on either match or exhaustion.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2d600(void* owner, int32_t x,
    int32_t y, const D2Common3051Env* env) { // 1.13c D2Common+0x2D600
    if (!owner) fatal(env, 0x2fd);
    void* node = link(owner, 0x10, env);
    if (!node) fatal(env, 0x2fe);
    do {
        if (x >= signed32(node, 0x34) && y >= signed32(node, 0x38) &&
            x < add32(signed32(node, 0x34), signed32(node, 0x3c)) &&
            y < add32(signed32(node, 0x38), signed32(node, 0x40))) return;
        node = link(node, 0x24, env);
    } while (node);
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_41d00(const char* left,
    const char* right, const D2Common3051Env* env) { // 1.13c D2Common+0x41D00
    if (std::strlen(left) > 8) fatal(env, 0xd6);
    if (std::strlen(right) > 8) fatal(env, 0xd7);
    return std::memcmp(left, right, 8) == 0;
}
