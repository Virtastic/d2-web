// src/core/realm/bnclient_batch_7415.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x7415.c (image base 0x6FF20000).
//
// Row: ___crtMessageBoxA 0x7415, MSVC 2003 (VC7.1) Release CRT. user32.dll loading and its five entry
// points, plus the two OS globals (DAT_6FF3E324, DAT_6FF3E330), are host seams; the cached entry points
// live in the caller's state block at +0x1E6DC..+0x1E6EC.
#include <cstdint>
#include <emscripten.h>

struct BnCrtMessageBoxState {
    void* message_box_a;                 // +0x1E6DC
    void* get_active_window;             // +0x1E6E0
    void* get_last_active_popup;         // +0x1E6E4
    void* get_process_window_station;    // +0x1E6E8
    void* get_user_object_information_a; // +0x1E6EC
};

struct BnCrtMessageBoxEnv {
    void*    (*load_library_a)(const char* name);
    void*    (*get_proc_address)(void* module, const char* name);
    int32_t  (*message_box_a)(void* hwnd, const char* text, const char* caption, uint32_t type);
    void*    (*get_active_window)(void);
    void*    (*get_last_active_popup)(void* hwnd);
    void*    (*get_process_window_station)(void);
    int32_t  (*get_user_object_information_a)(void* obj, int32_t index, void* info,
                                              uint32_t len, void* needed);
    uint32_t winsta_global;   // DAT_6FF3E324: only the value 2 enables the window-station probe
    uint32_t os_major;        // DAT_6FF3E330: < 4 chooses MB_TOPMOST(0x40000)
};

// 1.13c BNClient+0x07415 (249 B, callers 0x47FD __NMSG_WRITE, 0x72CA report_failure): ___crtMessageBoxA.
// First call loads user32.dll and resolves MessageBoxA (failure returns 0 without a box), then the other
// four entry points, and when DAT_6FF3E324 == 2 also the window-station pair. Unless the window station
// probe says the desktop is not visible, ask GetActiveWindow for a parent and its last active popup;
// otherwise the type gains MB_SERVICE_NOTIFICATION (0x40000) or MB_TOPMOST (0x200000) with os_major >= 4.
// Then MessageBoxA(parent, text, caption, type) and its result.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_crt_message_box_a(const char* text, const char* caption, uint32_t type,
                                                  BnCrtMessageBoxState* state,
                                                  const BnCrtMessageBoxEnv* env) {
    int32_t parent = 0;
    if (state->message_box_a == nullptr) {
        void* const module = env->load_library_a("user32.dll");
        if (module == nullptr) return 0;
        state->message_box_a = env->get_proc_address(module, "MessageBoxA");
        if (state->message_box_a == nullptr) return 0;
        state->get_active_window = env->get_proc_address(module, "GetActiveWindow");
        state->get_last_active_popup = env->get_proc_address(module, "GetLastActivePopup");
        if (env->winsta_global == 2) {
            state->get_user_object_information_a =
                env->get_proc_address(module, "GetUserObjectInformationA");
            if (state->get_user_object_information_a != nullptr)
                state->get_process_window_station =
                    env->get_proc_address(module, "GetProcessWindowStation");
        }
    }

    bool visible = true;
    if (state->get_process_window_station != nullptr) {
        void* const station = env->get_process_window_station();
        uint8_t info[12] = {};   // USEROBJECTFLAGS (fInherit, fReserved, dwFlags)
        uint8_t needed[4] = {};
        if (station == nullptr ||
            env->get_user_object_information_a(station, 1, info, sizeof info, needed) == 0 ||
            (info[8] & 1) == 0)   // WSF_VISIBLE
            visible = false;
    }
    if (visible) {
        if (state->get_active_window != nullptr) {
            void* const active = env->get_active_window();
            if (active != nullptr && state->get_last_active_popup != nullptr)
                parent = (int32_t)(intptr_t)env->get_last_active_popup(active);
        }
    } else if (env->os_major < 4) {
        type |= 0x40000;          // MB_SERVICE_NOTIFICATION
    } else {
        type |= 0x200000;         // MB_TOPMOST
    }
    return env->message_box_a((void*)(intptr_t)parent, text, caption, type);
}
