// D2Client 1.13c CRT 64-bit seek leaf. The OS handle and Win32 calls are host seams;
// the descriptor table and errno cell are shared with the other D2Client CRT rows.
#include <cstdint>
#include <emscripten.h>

extern "C" int32_t* d2_client_579c();
extern "C" uint8_t* d2_client_ioinfo_record(int32_t);

struct D2ClientLseeki64LkEnv {
    uint32_t (*get_osfhandle)(int32_t fd); // 0x0A63A
    uint32_t (*set_file_pointer)(uint32_t handle, int32_t low, int32_t* high, uint32_t origin);
    uint32_t (*get_last_error)(void);
    void (*dosmaperr)(uint32_t error);      // 0x057AE
};

// 1.13c D2Client+0x0A4BC
extern "C" EMSCRIPTEN_KEEPALIVE int64_t d2_client_a4bc(
    uint32_t fd, int32_t offset_low, int32_t offset_high, uint32_t origin,
    const D2ClientLseeki64LkEnv* env) {
    const uint32_t handle = env->get_osfhandle(static_cast<int32_t>(fd));
    if (handle == UINT32_MAX) {
        *d2_client_579c() = 9; // EBADF; retail leaves _doserrno untouched on this path.
        return -1;
    }

    int32_t high = offset_high;
    const uint32_t low = env->set_file_pointer(handle, offset_low, &high, origin);
    if (low == UINT32_MAX) {
        const uint32_t error = env->get_last_error();
        if (error != 0) {
            env->dosmaperr(error);
            return -1;
        }
    }

    d2_client_ioinfo_record(static_cast<int32_t>(fd))[4] &= uint8_t(0xFD); // clear FEOFLAG
    const uint64_t result = (static_cast<uint64_t>(static_cast<uint32_t>(high)) << 32) | low;
    return static_cast<int64_t>(result);
}
