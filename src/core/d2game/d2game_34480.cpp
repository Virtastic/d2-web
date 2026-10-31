// D2Game+0x34480 -- run quest callback slot 6 for every quest attached to a unit.
// eax = target, ecx = game, edx = player (unused). A 0x1c-byte argument block {game, event 6, target, 0, ...} lives on the stack; the rest of it is
// never written (zeroed here). The target's quest chain (+0x74) is walked as {questData, next}; for each entry whose
// questData+0xa0+event*4 slot is set and whose byte +0xa is 1, IsBadCodePtr(slot) non-zero is fatal (line 0x23e),
// otherwise slot(ecx = questData, edx = &block). The event is re-read from the block on every pass.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game34480Env {
    uint32_t (*isBadCodePtr)(uint32_t fn);
    void     (*call)(uint32_t fn, uint32_t questData, uint32_t block);
    void     (*fatal)(uint32_t line);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x34480
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_34480(uint32_t target, uint32_t game, const D2Game34480Env* env)
{
    uint32_t block[7] = { game, 6, target, 0, 0, 0, 0 };
    const uint32_t b = (uint32_t)(uintptr_t)block;
    for (uint32_t n = rd32(target + 0x74); n != 0; n = rd32(n + 4)) {
        const uint32_t qd = rd32(n);
        const uint32_t fn = rd32(qd + 0xa0 + block[1] * 4);
        if (fn == 0 || *(const uint8_t*)(uintptr_t)(qd + 0xa) != 1) continue;
        if (env->isBadCodePtr(fn)) { env->fatal(0x23e); return; }
        env->call(rd32(rd32(n) + 0xa0 + block[1] * 4), rd32(n), b);
    }
}
