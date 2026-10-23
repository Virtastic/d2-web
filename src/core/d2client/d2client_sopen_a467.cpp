#include <cstdint>
#include <emscripten.h>

struct D2ClientCrtUnwindEnv;
struct D2ClientSopenEnv {
    int32_t (*tsopen_lk)(uint32_t share_flag, int32_t* fhandle, int32_t* held,
                         const char* filename, uint32_t open_flag, uint8_t mode); // 0x0A180
    const D2ClientCrtUnwindEnv* unwind;
};

extern "C" void d2_client_a4ac(const uint32_t* ebp, const D2ClientCrtUnwindEnv* env);

// The 1.13c body initializes its local held flag and SEH state, delegates the open to __tsopen_lk,
// marks the finally state, then invokes the recovered 0x0A4AC cleanup funclet.
// 1.13c D2Client+0x0a467
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_a467(const char* filename, uint32_t open_flag,
                                                       uint32_t share_flag, uint32_t mode,
                                                       const D2ClientSopenEnv* env) {
    // EBP-relative locals: [EBP-0x20] is the fhandle out-slot and [EBP-0x1C] the held flag.
    uint32_t frame[18]{};
    uint32_t* ebp = frame + 9;
    // The decompile explicitly zeros local_24[1], the fhandle out-slot at EBP-0x20.
    // The unwind contract uses EBP-0x1C as the held flag; it also starts zeroed.
    ebp[-8] = 0;
    ebp[-7] = 0;
    // local_8 is the compiler's SEH state; the finally is entered after the call.
    ebp[-2] = 0;
    const int32_t result = env->tsopen_lk(share_flag, reinterpret_cast<int32_t*>(&ebp[-8]),
                                          reinterpret_cast<int32_t*>(&ebp[-7]), filename,
                                          open_flag, static_cast<uint8_t>(mode));
    ebp[-2] = 0xffffffffu;
    d2_client_a4ac(ebp, env->unwind);
    return result;
}
