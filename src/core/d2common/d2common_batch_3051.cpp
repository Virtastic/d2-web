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

// 1.13c D2Common+0x7BF20 (__thiscall owner in ECX, room in EAX, key in EDI, two stack words p1 p2). Retail-layout tokens.
// Finds the owner's list entry (list head +0x47c, next +0x1ac, key +0x1d0) or asks 0x2D9B0 for one; hands the room
// to 0x2E5C0 only when its level's key (+0x58 then +0x1d0) equals the key. A result with a non-zero u16 at +0xE
// loses one count, runs the callback at [0x6FDE9BFC] (= 0x7BE50) and then 0x7BB20(result, 2).
struct D2Common7bf20Env {
    uint32_t (*sub2D9B0)(uint32_t pOwner, uint32_t nKey);
    uint32_t (*sub2E5C0)(uint32_t pRoom, uint32_t p1, uint32_t p2, uint32_t pOwner, uint32_t pNode);
    void (*callback)(uint32_t pResult);
    void (*sub7BB20)(uint32_t pResult, uint32_t nMode);
};
namespace {
uint32_t T32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
uint16_t T16(uint32_t a) { uint16_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
}
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_7BF20_Update(uint32_t pOwner, uint32_t pRoom, uint32_t nKey,
    uint32_t p1, uint32_t p2, const D2Common7bf20Env* env) {
    uint32_t node = T32(pOwner + 0x47c);
    while (node && T32(node + 0x1d0) != nKey) node = T32(node + 0x1ac);
    if (!node) node = env->sub2D9B0(pOwner, nKey);
    uint32_t arg = pRoom;
    if (pRoom && T32(T32(pRoom + 0x58) + 0x1d0) != nKey) arg = 0;
    const uint32_t res = env->sub2E5C0(arg, p1, p2, pOwner, node);
    if (res && T16(res + 0xe)) {
        const uint16_t c = static_cast<uint16_t>(T16(res + 0xe) - 1);
        std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(res + 0xe)), &c, 2);
        env->callback(res);
        env->sub7BB20(res, 2);
    }
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2common_4eb40_env(void* object,
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

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2common_4f4c0_env(void* object,
    int32_t key, const D2Common3051Env* env) { // 1.13c D2Common+0x4F4C0
    if (!object) return 0;
    void* root = link(object, 0xa8, env);
    for (void* node = root ? link(root, 4, env) : nullptr; node; node = link(node, 4, env))
        if (read16(link(node, 0, env), 0) == key && signed32(node, 0x34) == -1)
            return read32(node, 0x28);
    return 0;
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

// Callees are explicit: lookupId=0x2D5B0 (id of the owner's room rect holding x,y),
// getOrCreate=0x2D9B0, loadRoom=0x2E360, touch=0x2D600. `cur` is the register-passed room.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2e5c0(void* cur, int32_t x, int32_t y,
    void* owner, void* room, const D2Common3051Env* env,
    int32_t (*lookupId)(void*, int32_t, int32_t),
    void* (*getOrCreate)(void*, int32_t), void (*loadRoom)(void*),
    void (*touch)(void*, int32_t, int32_t)) { // 1.13c D2Common+0x2E5C0
    auto inside = [&](void* r) {
        return signed32(r, 0x34) <= x && signed32(r, 0x38) <= y &&
            x < add32(signed32(r, 0x3c), signed32(r, 0x34)) &&
            y < add32(signed32(r, 0x40), signed32(r, 0x38));
    };
    if (cur) {
        if (inside(cur)) return;
        void** list = static_cast<void**>(env->resolve(read32(cur, 8)));
        for (int32_t i = 0; i < signed32(cur, 0x2c); i++) {
            void* o = env->resolve(read32(list + i, 0));
            if (o != cur && inside(o)) return;
        }
    }
    if (!room) {
        const int32_t id = lookupId(owner, x, y);
        room = link(owner, 0x47c, env);
        while (room && signed32(room, 0x1d0) != id) room = link(room, 0x1ac, env);
        if (!room) room = getOrCreate(owner, id);
    }
    if (!read32(room, 0x10)) loadRoom(room);
    touch(owner, x, y);
}
