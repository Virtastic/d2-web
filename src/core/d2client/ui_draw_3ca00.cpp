// D2Client 0x3CA00 / 0x3CC60, real extern "C" bodies over the real Storm senders and Fog fatal path.
// Register args are explicit: 0x3CA00 takes EAX/ECX/EDX/ESI, 0x3CC60 takes EAX (count) plus two stack args.
#include <cstdint>
#include <emscripten.h>

#define W __attribute__((weak))
extern "C" {
uint32_t Fog_Ordinal_10265(uint32_t);
void Fog_Ordinal_10024(const void*, const void*, uint32_t);
[[noreturn]] void D2Client_exit_3664(int32_t);
uint32_t d2_client_window_handle();  // D2Client+0xD0B4 (-> Fog 10048)
void Storm_Sub34530_Real(uint32_t, uint32_t, uint32_t, uint32_t);  // #512
void Storm_Sub34510_Real(uint32_t, uint32_t, uint32_t, uint32_t);  // #513
void Storm_Sub344F0_Real(uint32_t, uint32_t, uint32_t, uint32_t);  // #514
void Storm_Sub344D0_Real(uint32_t, uint32_t, uint32_t, uint32_t);  // #515

W uint8_t DAT_6fb8359f[1];
W uint32_t DAT_6fbcc3cc, DAT_6fbc9898, DAT_6fbc9890, DAT_6fbc988c, DAT_6fba46d4;
W uint32_t DAT_6fba4748[3], DAT_6fba4754[6], DAT_6fba476c[6], DAT_6fba4784[3];
// Win32 seam: user32!GetSystemMetrics.
W int32_t d2_win32_GetSystemMetrics(int32_t) { return 0; }

// Seam for the FN probes: the four Storm senders (ordinals 512..515) and the Fog fatal path as function pointers.
struct D2c3CaEnv {
    void (*storm)(uint32_t ordinal, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi);  // #512..#515
    void (*fatal)(uint32_t id);                                                               // Fog #10265(id), #10024, exit(-1)
};
static void real_storm(uint32_t o, uint32_t a, uint32_t c, uint32_t d, uint32_t s) {
    switch (o) {
    case 512: Storm_Sub34530_Real(a, c, d, s); break;
    case 513: Storm_Sub34510_Real(a, c, d, s); break;
    case 514: Storm_Sub344F0_Real(a, c, d, s); break;
    default: Storm_Sub344D0_Real(a, c, d, s);
    }
}
static void real_fatal(uint32_t id) { Fog_Ordinal_10024(DAT_6fb8359f, 0, Fog_Ordinal_10265(id)); D2Client_exit_3664(-1); }
static const D2c3CaEnv kRealEnv = { real_storm, real_fatal };

// 1.13c D2Client+0x3CA00 (eax = handle, ecx = param_1, edx, esi = selector)
EMSCRIPTEN_KEEPALIVE void d2_client_3ca00_env(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi, const D2c3CaEnv* e) {
    uint32_t id;
    if (eax != 0) {
        switch (esi) {
        case 0:
            if (ecx != 0x111) { e->storm(515, eax, ecx, edx, esi); return; }
            id = 0x15b; break;
        case 1: e->storm(512, eax, ecx, edx, esi); return;
        case 2: e->storm(514, eax, ecx, edx, esi); return;
        case 3: e->storm(513, eax, ecx, edx, esi);
        default: return;
        }
    } else id = 0x157;
    e->fatal(id);
}
void FUN_6faeca00(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi) { d2_client_3ca00_env(eax, ecx, edx, esi, &kRealEnv); }

// 1.13c D2Client+0x3CC60 (count = EAX; table of 12-byte entries {esi, ecx, edx})
EMSCRIPTEN_KEEPALIVE void d2_client_3cc60_env(uint32_t count, uint32_t handle, const uint32_t* table, const D2c3CaEnv* e) {
    if (handle == 0) e->fatal(0x18c);
    for (; count != 0; --count, table += 3) d2_client_3ca00_env(handle, table[1], table[2], table[0], e);
}
void FUN_6faecc60(uint32_t count, uint32_t handle, const uint32_t* table) { d2_client_3cc60_env(count, handle, table, &kRealEnv); }

}  // extern "C"
