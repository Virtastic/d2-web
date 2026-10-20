// Prior art: none vendored (1.13c-only SCmd tail). Decompile-derived from the cached 1.13c functions.
// Register inputs are explicit; callees and the allocator are injected seams.
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2GameBatch8c9Env {
    // 0x8A330(a, b): the shared tail of all three entry points.
    uint32_t (*sub8A330)(const void* a, const void* b);
    // 0x9BEB0 (unit in EAX, game in EDI): the 16-bit client/unit key; -1 means none.
    int16_t  (*sub9BEB0)(const void* unit, const void* game);
    // D2Common ordinal 10920: the block whose +0x98 holds the 20-byte record list.
    void*    (*ordinal10920)();
    // Ordinal_10045(file, line, 0): allocator -- NOT added to the tree, injected seam.
    void*    (*alloc10045)(const char* file, uint32_t line, uint32_t zero);
    // 0xE03A0(game, id) (EAX = type): the unit lookup used by 0x8D500.
    int32_t  (*subE03A0)(int32_t type, int32_t game, uint32_t id);
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// 1.13c D2Game+0x8C910
// unit (EAX, also param_1), param_2, game (EDI). Record list at ordinal10920()+0x98, 20-byte nodes:
// [0]=key (unit+0xC or -1) [1]=unit [2]=param_2 [4]=next. FAITHFUL (allocator injected;
// the null-EAX fatal path _exit(-1) is reproduced as abort()).
extern "C" uint32_t d2_fn_d2game_8C910(void* unit, uint32_t param_2, void* game, const D2GameBatch8c9Env* env)
{
    if (unit == nullptr) std::abort();
    if (rd32(unit, 0x80) == 0) return 0;
    int16_t k1 = env->sub9BEB0(unit, game);
    if (k1 == -1) return 0;
    int16_t k2 = env->sub9BEB0(unit, game);
    if (k1 != k2) return 0;
    uint8_t* blk = (uint8_t*)env->ordinal10920();
    if (!blk) return 0;
    uint32_t* node; uint32_t key = rd32(unit, 0xC);   // param_1 == unit here, never null
    for (node = (uint32_t*)(uintptr_t)rd32(blk, 0x98); node; node = (uint32_t*)(uintptr_t)node[4])
        if (key == node[0]) goto found;
    node = (uint32_t*)env->alloc10045("..\\Source\\D2Game\\GAME\\SCmd.cpp", 0x6fa, 0);
    if (!node) return 0;
    node[0] = node[1] = node[2] = node[3] = node[4] = 0;
    node[4] = rd32(blk, 0x98);
    wr32(blk, 0x98, (uint32_t)(uintptr_t)node);
    node[0] = key;
found:
    if (node[2] != param_2) {
        node[1] = (uint32_t)(uintptr_t)unit;
        node[2] = param_2;
        return 1;
    }
    return env->sub8A330((const void*)(uintptr_t)param_2, unit);
}

// 1.13c D2Game+0x8D500
// unit (EAX), this (ECX), param_1, EDI value `edi`. FAITHFUL.
extern "C" uint32_t d2_fn_d2game_8D500(void* unit, void* thisp, int32_t param_1, int32_t edi, const D2GameBatch8c9Env* env)
{
    int32_t r = 0;
    if (unit && rd32(unit, 0) == 1) {
        uint32_t a = rd32(unit, 0x14);
        if (a && rd32((void*)(uintptr_t)a, 0x28)) {
            uint8_t* b = (uint8_t*)(uintptr_t)rd32((void*)(uintptr_t)a, 0x28);
            if (rd32(b, 0x28)) r = env->subE03A0(/*EAX*/ 0, rd32(b, 0x28), rd32(b, 0x2c));
        }
    }
    if (r == param_1 && unit && rd32(unit, 0) == 1) {
        uint32_t a = rd32(unit, 0x14);
        if (a && a != (uint32_t)-0x3c && rd32((void*)(uintptr_t)a, 0x40) != (uint32_t)edi)
            return env->sub8A330(thisp, (const void*)(uintptr_t)edi);
    }
    return 0;
}

// 1.13c D2Game+0x8E110
// game (ECX), unit (EAX), param_2, EDI value. Type 0 -> 0x8C910, type 1 -> 0x8D500, else 0. FAITHFUL.
extern "C" uint32_t d2_fn_d2game_8E110(void* game, void* unit, uint32_t param_2, int32_t edi, const D2GameBatch8c9Env* env)
{
    if (!game || !unit) std::abort();   // 0x712 / 0x713 fatal paths
    uint32_t t = rd32(unit, 0);
    if (t == 0) return d2_fn_d2game_8C910(unit, param_2, game, env);
    if (t != 1) return 0;
    return d2_fn_d2game_8D500(unit, game, (int32_t)(intptr_t)game, edi, env);
}
