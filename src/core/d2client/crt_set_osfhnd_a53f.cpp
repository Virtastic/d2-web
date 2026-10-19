// Prior art used: the equivalent D2Game CRT leaf in d2game_crt_set_osfhnd.cpp;
// the D2Client 1.13c decompile establishes this DLL's globals and exact branches.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern uint32_t g_fdcount; // +0x123990, _nfile
extern "C" uint8_t* d2_client_ioinfo_record(int32_t fd); // +0x1239A0, shared CRT table
extern "C" int32_t* d2_client_579c(void); // __errno
extern "C" int32_t* d2_client_57a5(void); // __doserrno

struct D2ClientSetOsfhndEnv {
    uint32_t app_type; // +0x0FA8FC, _app_type
    void (*set_std_handle)(uint32_t which, uint32_t handle); // kernel32 SetStdHandle
};

// 1.13c D2Client+0xA53F: __set_osfhnd. Install only into an unassigned descriptor;
// the standard-handle call is attempted first and its return value is ignored.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_a53f(int32_t file_handle,
                                                        uint32_t os_handle,
                                                        const D2ClientSetOsfhndEnv* env) {
    const uint32_t fd = static_cast<uint32_t>(file_handle);
    if (fd < g_fdcount) {
        uint8_t* record = d2_client_ioinfo_record(file_handle);
        int32_t previous;
        std::memcpy(&previous, record, sizeof(previous));
        if (previous == -1) {
            if (env->app_type == 1 && fd <= 2)
                env->set_std_handle(0xFFFFFFF6u - fd, os_handle);
            std::memcpy(record, &os_handle, sizeof(os_handle));
            return 0;
        }
    }
    *d2_client_579c() = 9;
    *d2_client_57a5() = 0;
    return -1;
}
