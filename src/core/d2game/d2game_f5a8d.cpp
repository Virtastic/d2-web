#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Game+0xF5A8D: finally-funclet of CallCatchBlock (0xF5938). Retail reads the
// caller's EBP/ESI/EDI; here they are explicit. The two __getptd() slot addresses
// (_unexpected, _translator) are passed in, the helpers are host seams.
struct D2GameF5A8DEnv {
    void (*findAndUnlinkFrame)(void* frameInfo);          // 0xF4890
    int  (*isExceptionObjectToBeDestroyed)(void* obj);    // 0xF486F
    int  (*abnormalTermination)();                        // 0x0789A
    void (*destructExceptionObject)(void* exc);           // 0xF58D4
};

static uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f5a8d(
    const D2GameF5A8DEnv* env, uint8_t* exc, uint32_t* edi, uint32_t savedEdiSlot,
    void* frameInfo, uint32_t* ptdUnexpected, uint32_t oldUnexpected,
    uint32_t* ptdTranslator, uint32_t oldTranslator, int32_t flag48, int32_t flag20) {
    edi[-1] = savedEdiSlot;
    env->findAndUnlinkFrame(frameInfo);
    *ptdUnexpected = oldUnexpected;
    *ptdTranslator = oldTranslator;
    if (rd32(exc) == 0xE06D7363u && rd32(exc + 0x10) == 3 &&
        (rd32(exc + 0x14) == 0x19930520u || rd32(exc + 0x14) == 0x19930521u) &&
        flag48 == 0 && flag20 != 0 &&
        env->isExceptionObjectToBeDestroyed(reinterpret_cast<void*>(static_cast<uintptr_t>(rd32(exc + 0x18))))) {
        env->abnormalTermination();
        env->destructExceptionObject(exc);
    }
}
