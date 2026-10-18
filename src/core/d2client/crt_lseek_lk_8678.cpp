// D2Client 1.13c CRT seek leaf; 
#include <cstdint>
#include <emscripten.h>
extern "C" int32_t* d2_client_579c();
extern "C" uint8_t* d2_client_ioinfo_record(int32_t);

// Kernel and unrecovered CRT callees remain explicit host seams, in retail call order.
struct D2ClientLseekLkHost {
    uint32_t (*get_osfhandle)(int32_t);
    uint32_t (*set_file_pointer)(uint32_t, int32_t, int32_t*, uint32_t);
    uint32_t (*get_last_error)();
    void (*dosmaperr)(uint32_t);
};

// 1.13c D2Client+0x08678
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_8678(
    int32_t fd, int32_t offset, uint32_t origin, const D2ClientLseekLkHost* host) {
    const uint32_t handle = host->get_osfhandle(fd);
    if (handle == UINT32_MAX) {
        *d2_client_579c() = 9;
        return UINT32_MAX;
    }
    const uint32_t result = host->set_file_pointer(handle, offset, nullptr, origin);
    const uint32_t error = result == UINT32_MAX ? host->get_last_error() : 0;
    if (error != 0) {
        host->dosmaperr(error);
        return UINT32_MAX;
    }
    d2_client_ioinfo_record(fd)[4] &= uint8_t(0xFD);
    return result;
}
