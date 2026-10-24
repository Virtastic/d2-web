// D2Client 0x3CA00 / 0x3CC60 / 0x429C0, real extern "C" bodies over the real Storm senders and Fog fatal path.
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

// 1.13c D2Client+0x3CA00 (eax = handle, ecx = param_1, edx, esi = selector)
void FUN_6faeca00(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi) {
    uint32_t id;
    if (eax != 0) {
        switch (esi) {
        case 0:
            if (ecx != 0x111) { Storm_Sub344D0_Real(eax, ecx, edx, esi); return; }
            id = 0x15b; break;
        case 1: Storm_Sub34530_Real(eax, ecx, edx, esi); return;
        case 2: Storm_Sub344F0_Real(eax, ecx, edx, esi); return;
        case 3: Storm_Sub34510_Real(eax, ecx, edx, esi);
        default: return;
        }
    } else id = 0x157;
    Fog_Ordinal_10024(DAT_6fb8359f, 0, Fog_Ordinal_10265(id));
    D2Client_exit_3664(-1);
}

// 1.13c D2Client+0x3CC60 (count = EAX; table of 12-byte entries {esi, ecx, edx})
void FUN_6faecc60(uint32_t count, uint32_t handle, const uint32_t* table) {
    if (handle == 0) {
        Fog_Ordinal_10024(DAT_6fb8359f, 0, Fog_Ordinal_10265(0x18c));
        D2Client_exit_3664(-1);
    }
    for (; count != 0; --count, table += 3) FUN_6faeca00(handle, table[1], table[2], table[0]);
}

static bool modeAB() { return DAT_6fbc9898 == 1 || (DAT_6fbc9898 == 2 && (DAT_6fbc988c == 4 || DAT_6fbc988c == 5)); }

// 1.13c D2Client+0x429C0
EMSCRIPTEN_KEEPALIVE void FUN_6faf29c0() {
    if (d2_win32_GetSystemMetrics(0x4b) != 0) {
        if (DAT_6fbcc3cc == 0) goto tail;
        if (DAT_6fbc9898 == 1 ? DAT_6fbc9890 != 0 : (DAT_6fbc9898 == 2 && (DAT_6fbc988c == 4 || DAT_6fbc988c == 5)))
            FUN_6faecc60(1, d2_client_window_handle(), DAT_6fba4748);
    }
    if (DAT_6fbcc3cc != 0) {
        if (modeAB()) FUN_6faecc60(2, d2_client_window_handle(), DAT_6fba4754);
        if (DAT_6fbc9898 == 2 && DAT_6fbc988c != 4 && DAT_6fbc988c == 5)
            FUN_6faecc60(2, d2_client_window_handle(), DAT_6fba476c);
    }
tail:
    if (d2_win32_GetSystemMetrics(0x4b) != 0 && DAT_6fbcc3cc != 0 && DAT_6fbc9898 == 1 && DAT_6fbc9890 == 0 &&
        DAT_6fba46d4 != 0)
        FUN_6faecc60(1, d2_client_window_handle(), DAT_6fba4784);
}
}  // extern "C"
