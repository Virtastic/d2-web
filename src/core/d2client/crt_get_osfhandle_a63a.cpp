// D2Client 1.13c CRT descriptor -> OS handle leaf. The descriptor table (_nfile at +0x123990, blocks
// at +0x1239A0) and the thread errno/doserrno cells are shared with the other D2Client CRT rows.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern uint32_t g_fdcount;                                   // +0x123990  _nfile
extern "C" uint8_t* d2_client_ioinfo_record(int32_t fd);     // +0x1239A0 record, stride 0x24
extern "C" int32_t* d2_client_579c(void);                    // 0x0579C _errno
extern "C" int32_t* d2_client_57a5(void);                    // 0x057A5 _doserrno

// 1.13c D2Client+0x0A63A
// __get_osfhandle(fh): a descriptor below _nfile (unsigned compare, so a negative fh is invalid) whose
// record has the FOPEN bit (osfile +4, bit 0) returns the record's osfhnd dword (+0). Anything else
// sets _terrno = EBADF (9), _tdoserrno = 0 and returns -1; the block table is not indexed on that path.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a63a(int32_t file_handle) {
    const uint32_t fd = static_cast<uint32_t>(file_handle);
    if (fd < g_fdcount) {
        uint8_t* const record = d2_client_ioinfo_record(file_handle);
        if ((record[4] & 1u) != 0) {
            uint32_t os_handle;
            std::memcpy(&os_handle, record, sizeof os_handle);
            return os_handle;
        }
    }
    *d2_client_579c() = 9;
    *d2_client_57a5() = 0;
    return UINT32_MAX;
}
