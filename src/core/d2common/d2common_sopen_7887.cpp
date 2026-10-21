#include <cstdint>

struct D2CommonSopenEnv {
    int32_t (*tsopen_lk)(uint32_t share_flag, int32_t* fhandle, int32_t* held,
                         const char* filename, uint32_t open_flag, uint8_t mode); // 0x75A0
    void (*unlock)(const int32_t* held, int32_t fhandle);                         // 0x78CC funclet
};

// __sopen: zero the fhandle out-slot and held flag, delegate to __tsopen_lk, then run the
// finally funclet (+0x78CC, releases the lock when held) and return the open result.
// 1.13c D2Common+0x07887
int32_t d2common_sopen_7887(const char* filename, uint32_t open_flag, uint32_t share_flag,
                            uint32_t mode, const D2CommonSopenEnv* env) {
    int32_t fhandle = 0, held = 0;
    const int32_t r = env->tsopen_lk(share_flag, &fhandle, &held, filename, open_flag,
                                     static_cast<uint8_t>(mode));
    env->unlock(&held, fhandle);
    return r;
}
