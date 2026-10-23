// 1.13c Fog+0xA0CB -- SEH cleanup funclet: if the caller's locked flag is set, release the fhandle lock.
#include <cstdint>

struct FogA0CBEnv {
    int locked;                        // caller frame EBP-0x1C
    int fh;                            // caller frame EBP-0x20
    void (*unlockFhandle)(int fh);     // Fog+0xA33A (__unlock_fhandle)
};

void fog_sub_a0cb(const FogA0CBEnv* env) {
    if (env->locked != 0) env->unlockFhandle(env->fh);
}
