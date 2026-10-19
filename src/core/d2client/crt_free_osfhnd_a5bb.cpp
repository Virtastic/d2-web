// src/core/d2client/crt_free_osfhnd_a5bb.cpp -- 1.13c D2Client.dll __free_osfhnd, transcribed from the
// cached Ghidra decompile build-wasm/ghidra-d2client/decomp/0xA5BB.c (127 B). Alpha lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector. Prior art: the same CRT leaf in
// src/core/d2game/d2game_crt_free_osfhnd.cpp; the D2Client decompile establishes this DLL's globals.
//
// Globals: _nfile +0x123990 (0x6FBD3990, owned by d2client_batch_b56.cpp), _pioinfo +0x1239A0
// (0x6FBD39A0, owned by d2client_batch_03f82.cpp) and __app_type +0x0FA8FC (0x6FBAA8FC). The two
// callees 0x579C/0x57A5 are _errno/__doserrno (d2client_batch_4857.cpp). SetStdHandle is a PE import, so
// it is a host seam on the env after the retail argument, as in crt_set_osfhnd_a53f.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern uint32_t g_fdcount;                               // +0x123990, _nfile
extern "C" uint8_t* d2_client_ioinfo_record(int32_t fd); // +0x1239A0, shared CRT table
extern "C" int32_t* d2_client_579c(void);                // 0x0579C _errno
extern "C" int32_t* d2_client_57a5(void);                // 0x057A5 _doserrno

struct D2ClientFreeOsfhndEnv {
    uint32_t app_type;                                       // +0x0FA8FC, __app_type
    void (*set_std_handle)(uint32_t which, uint32_t handle); // kernel32 SetStdHandle
};

// 1.13c D2Client+0xA5BB
// __free_osfhnd(fd): a descriptor below _nfile whose ioinfo record has FOPEN (byte +4, bit 0x01) set and a
// live OS handle clears that handle to -1 and returns 0. In a console app (__app_type == 1) fds 0, 1 and 2
// additionally clear the matching standard handle first (STD_INPUT/OUTPUT/ERROR_HANDLE, -10/-11/-12).
// Anything else -- out of range, not FOPEN, or an already -1 handle -- latches errno EBADF (9) and
// _doserrno 0 and returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a5bb(int32_t file_handle, const D2ClientFreeOsfhndEnv* env) {
    const uint32_t fd = (uint32_t)file_handle;
    if (fd < g_fdcount) {
        uint8_t* record = d2_client_ioinfo_record(file_handle);
        int32_t osfhnd;
        std::memcpy(&osfhnd, record, sizeof(osfhnd));
        if ((record[4] & 0x01) != 0 && osfhnd != -1) {       // FOPEN and a live OS handle
            if (env->app_type == 1 && fd <= 2)
                env->set_std_handle(0xFFFFFFF6u - fd, 0);    // STD_INPUT/OUTPUT/ERROR_HANDLE
            const uint32_t invalid = 0xFFFFFFFFu;
            std::memcpy(record, &invalid, sizeof(invalid));
            return 0;
        }
    }
    *d2_client_579c() = 9;                                   // EBADF
    *d2_client_57a5() = 0;
    return -1;
}
