// 1.13c BNClient+0x11780: the patch-status reply handler (__fastcall ECX = payload, EDX = size). dword0 = status.
// status 1..3 first runs B9B0; Storm 424 ("Patch","Patches",0x82,"",1) always; status 1: payload text copied (size-4) under the DAT_1F8E8 lock into
// DAT_1F670, DAT_1F508 = 0, 424 again with the text, E120(0x80000002,2,0,0,text,0), "downloading upgrade", DAT_1EEF4 = 0; status 2: DAT_1EEF4 = 1,
// "user has latest version"; else FCF0(1), "User must reinstall and patch" (3) or "user has unknown version", DAT_1EEF4 = 0. Returns 1.
#include <cstdint>
#include <cstring>

struct Bn11780Env {
    void (*on_bad)();                              // 1.13c BNClient+0xB9B0
    void (*storm424)(const void* a3, uint32_t a4); // Storm 424 via thunk 0x8066 (always "Patch","Patches",0x82,a3,a4)
    void (*e120)();                                // 1.13c BNClient+0xE120(0x80000002,2,0,0,text,0)
    void (*fcf0)(uint32_t arg);                    // 1.13c BNClient+0xFCF0
    void (*log)(const char* msg);                  // 1.13c BNClient+0x17420
};

extern "C" uint32_t bnclient_patch_status_11780(const uint32_t* p, uint32_t size, uint32_t* f508, uint32_t* eef4, uint8_t* f670, const Bn11780Env* env) {
    const uint32_t st = p[0];
    if (st != 0 && st < 4) env->on_bad();
    env->storm424(nullptr, 1);
    const char* msg;
    if (st == 1) {
        memcpy(f670, p + 1, size - 4);
        *f508 = 0;
        env->storm424(p + 1, size - 4);
        env->e120();
        msg = "downloading upgrade";
        *eef4 = 0;
    } else if (st == 2) {
        *eef4 = 1;
        msg = "user has latest version";
    } else {
        env->fcf0(1);
        msg = st == 3 ? "User must reinstall and patch" : "user has unknown version";
        *eef4 = 0;
    }
    env->log(msg);
    return 1;
}
