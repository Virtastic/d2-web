// D2Game+0xE310 (unit in EAX -> per-difficulty table entry), from the 1.13c decompile and disassembly.
// The core takes the D2Common ordinals and the table as an env so a probe can hook the callees.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

#define IMP(ret, name, ...) namespace ordns_e310 { __attribute__((weak, import_module("env"), import_name(#name))) ret name(__VA_ARGS__); } using namespace ordns_e310
IMP(void*, Ordinal_10331, void*);
IMP(int, Ordinal_10826, void*);
IMP(int, Ordinal_10459, int);
IMP(int, Ordinal_10265, int);
IMP(void, Ordinal_10024, const char*, int);

// DAT_6fd2f86c: count; DAT_6fd2f870..: entries; DAT_6fd2f87c: fallback entry.
struct E310Tbl { uint32_t count; uint32_t entry[4]; };
extern E310Tbl g_d2game_6fd2f86c;
E310Tbl g_d2game_6fd2f86c;

struct D2GameE310Env {
    void*    (*getCtx)(const void* unit);   // D2Common #10331 (stdcall, the unit)
    uint32_t (*ctxToKey)(void* ctx);        // #10826
    uint8_t  (*keyToIndex)(uint32_t key);   // #10459 (only AL is used)
    void     (*fatal)(uint32_t code);       // #10265 + #10024 log, then _exit(-1)
    uint32_t tableCount;                    // DAT_6fd2f86c
    const uint32_t* table;                  // &DAT_6fd2f870
    uint32_t fallback;                      // DAT_6fd2f87c
};

// 1.13c D2Game+0x0E310
static uint32_t e310_core(const uint8_t* unit, const D2GameE310Env* env)
{
    void* g = env->getCtx(unit);
    if (!g) { env->fatal(0xd60); exit(-1); }
    uint8_t b = env->keyToIndex(env->ctxToKey(g));
    if (!unit) { env->fatal(0xa2a); exit(-1); }
    const uint8_t* p = *(const uint8_t* const*)(unit + 0x80);
    if (p && p[0x6d]) b = (uint8_t)(b + 2);
    if (env->tableCount <= b) return env->fallback;
    return env->table[b];
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_e310_port(const uint8_t* unit, const D2GameE310Env* env) { return e310_core(unit, env); }

static void e310_fatal(uint32_t code) { Ordinal_10024("D2Game", Ordinal_10265((int)code)); }

uint32_t D2GAME_E310(void* unit)
{
    D2GameE310Env env;
    env.getCtx = [](const void* u) { return Ordinal_10331(const_cast<void*>(u)); };
    env.ctxToKey = [](void* c) { return (uint32_t)Ordinal_10826(c); };
    env.keyToIndex = [](uint32_t k) { return (uint8_t)Ordinal_10459((int)k); };
    env.fatal = e310_fatal;
    env.tableCount = g_d2game_6fd2f86c.count;
    env.table = g_d2game_6fd2f86c.entry;
    env.fallback = g_d2game_6fd2f86c.entry[3];
    return e310_core((const uint8_t*)unit, &env);
}
