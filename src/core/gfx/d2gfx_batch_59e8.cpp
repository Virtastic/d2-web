// src/core/gfx/d2gfx_batch_59e8.cpp -- one UNMAPPED 1.13c D2Gfx.dll row reimplemented from the
// cached decompile build-wasm/ghidra-d2gfx/decomp/0x59E8.c (alpha lane: no spec, no vector).
// Retail base 0x6FA70000; every module global below is named at its retail RVA.
//
// MSVC 2003 (VC7.1) Release CRT ___crtMessageBoxA the linker pulled into the DLL:
// a lazy user32 binding plus the owner-window / topmost-bit selection around MessageBoxA. Callers
// 0x608C and 0x360C are the CRT's fatal-error paths. The user32 entry points are host seams at the END
// of the argument list, the retail arguments keeping their order, in the same shape as the D2DDraw
// sibling (src/core/gfx/d2ddraw_batch_3e87.cpp, 0x0456B).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// --- seam environment ----------------------------------------------------------------------------
struct D2GfxMsgBoxEnv {
    void*    (*load_library_a)(const char* name);                                 // LoadLibraryA
    void*    (*get_proc_address)(void* module, const char* name);                 // GetProcAddress
    uint32_t (*get_active_window)(void);                                          // GetActiveWindow
    uint32_t (*get_last_active_popup)(uint32_t hwnd);                             // GetLastActivePopup
    int      (*get_user_object_information_a)(uint32_t handle, int index, void* info,
                                              uint32_t len, uint32_t* needed);    // GetUserObjectInformationA
    uint32_t (*get_process_window_station)(void);                                 // GetProcessWindowStation
    int      (*message_box_a)(uint32_t hwnd, const char* text, const char* caption,
                              uint32_t type);                                     // MessageBoxA
};

static void* env_load_library_a(const char*) { return nullptr; }
static void* env_get_proc_address(void*, const char*) { return nullptr; }
static uint32_t env_get_active_window(void) { return 0; }
static uint32_t env_get_last_active_popup(uint32_t) { return 0; }
static int env_get_user_object_information_a(uint32_t, int, void*, uint32_t, uint32_t*) { return 0; }
static uint32_t env_get_process_window_station(void) { return 0; }
static int env_message_box_a(uint32_t, const char*, const char*, uint32_t) { return 0; }
static const D2GfxMsgBoxEnv g_msgbox_env = {
    env_load_library_a, env_get_proc_address, env_get_active_window, env_get_last_active_popup,
    env_get_user_object_information_a, env_get_process_window_station, env_message_box_a,
};

// +0x210E0..+0x210F0: cached user32 entry points; +0x20DA4 platform id and +0x20DB0 major version
// (both zero in the image until the CRT version probe runs, which this row does not).
static void*    g_msgbox_a;                  // +0x210E0
static void*    g_getactivewindow;           // +0x210E4
static void*    g_getlastactivepopup;        // +0x210E8
static void*    g_getprocesswindowstation;   // +0x210EC
static void*    g_getuserobjectinfo;         // +0x210F0
static uint32_t g_os_platform;               // +0x20DA4
static uint32_t g_os_version;                // +0x20DB0

// 1.13c D2Gfx+0x059E8 (___crtMessageBoxA): bind user32 once. LoadLibraryA failure or a missing
// MessageBoxA returns 0 without a dialog; GetActiveWindow/GetLastActivePopup are cached regardless.
// On the NT platform (id 2) GetUserObjectInformationA and, when that resolves, GetProcessWindowStation
// are cached too. A process window station that exists and reports itself visible (USEROBJECTFLAGS
// bit 0 at +8) takes the active window -- or its last active popup -- as the owner; otherwise the
// owner is NULL and the type gains the topmost bit, 0x40000 on pre-4.0 and 0x200000 from 4.0 on.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_crtMessageBoxA(const char* text, const char* caption,
                                                  uint32_t type, const D2GfxMsgBoxEnv* env) {
    uint32_t hwnd = 0, station;  // retail keeps the station handle apart from the owner

    if (g_msgbox_a == nullptr) {
        void* hmod = env->load_library_a("user32.dll");
        if (hmod == nullptr) return 0;
        g_msgbox_a = env->get_proc_address(hmod, "MessageBoxA");
        if (g_msgbox_a == nullptr) return 0;
        g_getactivewindow = env->get_proc_address(hmod, "GetActiveWindow");
        g_getlastactivepopup = env->get_proc_address(hmod, "GetLastActivePopup");
        if (g_os_platform == 2) {
            g_getuserobjectinfo = env->get_proc_address(hmod, "GetUserObjectInformationA");
            if (g_getuserobjectinfo != nullptr)
                g_getprocesswindowstation = env->get_proc_address(hmod, "GetProcessWindowStation");
        }
    }

    uint8_t  objinfo[16] = {};
    uint32_t needed = 0;
    if (g_getprocesswindowstation == nullptr ||
        ((station = env->get_process_window_station()) != 0 &&
         env->get_user_object_information_a(station, 1, objinfo, 0xC, &needed) != 0 &&
         (objinfo[8] & 1) != 0)) {
        if (g_getactivewindow != nullptr &&
            (hwnd = env->get_active_window()) != 0 &&
            g_getlastactivepopup != nullptr) {
            hwnd = env->get_last_active_popup(hwnd);
        }
    } else if (g_os_version < 4) {
        type |= 0x40000;
    } else {
        type |= 0x200000;
    }
    return env->message_box_a(hwnd, text, caption, type);
}
