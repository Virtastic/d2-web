// 1.13c BNClient+0x05C66 (444 B, caller 0x5E22 __write). Own file.
// __write_lk(handle, buf, count). count 0 writes nothing and returns 0. ioinfo[handle].osfile (+4): bit 0x20
// (FAPPEND) seeks to end first (0x79CF __lseeki64_lk(h,0,0,2)); bit 0x80 (FTEXT) inserts CR before every LF and
// writes at most 0x400 bytes per WriteFile, stopping if a call does not consume its whole chunk. A failed write
// keeps the posted last error. With bytes written, return them minus the inserted CRs. Error 5 (ACCESS_DENIED)
// sets _terrno = 9 and _tdoserrno = 5; any other error goes through __dosmaperr (0x37DF) and touches no errno
// slot. With no error: a zero-byte write on a device (osfile bit 0x40, FDEV) whose first byte is 0x1A returns 0
// without touching the slots; otherwise _terrno = 0x1C, _tdoserrno = 0, -1.
// The errno slots are reached through the retail getters 0x37CD (_terrno) and 0x37D6 (_tdoserrno); the env
// supplies them (default: the ptd block from bn_getptd, +0x08 and +0x0C).
#include <cstdint>
#include <emscripten.h>

extern "C" void* bn_getptd(void);   // 0x03255 __getptd

struct BnWriteLkEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);                                      // DAT_6FF40C00[h>>5] + (h&31)*0x24
    void     (*seek_end)(int32_t handle);                                          // 0x79CF __lseeki64_lk(h,0,0,2)
    int32_t  (*write_file)(void* os_handle, const void* buf, uint32_t count,
                           uint32_t* written);                                     // KERNEL32 WriteFile
    uint32_t (*get_last_error)(void);                                              // KERNEL32 GetLastError
    void     (*dosmaperr)(uint32_t dos_error);                                     // 0x37DF __dosmaperr
    int32_t* (*terrno_slot)(void);                                                 // 0x37CD: &_terrno
    int32_t* (*doserrno_slot)(void);                                               // 0x37D6: &_tdoserrno
};

static uint8_t* bn_no_ioinfo_5c66(int32_t) { return nullptr; }
static uint32_t bn_no_last_error_5c66(void) { return 0; }
static void bn_no_dosmaperr_5c66(uint32_t) {}
static void bn_no_seek_end_5c66(int32_t) {}
static int32_t bn_no_write_file_5c66(void*, const void*, uint32_t, uint32_t* written) {
    if (written != nullptr) *written = 0;
    return 0;
}
static int32_t* bn_default_terrno_5c66(void) { return (int32_t*)((uint8_t*)bn_getptd() + 0x08); }
static int32_t* bn_default_doserrno_5c66(void) { return (int32_t*)((uint8_t*)bn_getptd() + 0x0C); }

static const BnWriteLkEnv g_bn_write_lk_default = {
    bn_no_ioinfo_5c66, bn_no_seek_end_5c66, bn_no_write_file_5c66, bn_no_last_error_5c66,
    bn_no_dosmaperr_5c66, bn_default_terrno_5c66, bn_default_doserrno_5c66,
};

// 1.13c BNClient+0x05C66
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_write_lk(int32_t handle, const void* buf, uint32_t count,
                                         const BnWriteLkEnv* env) {
    if (env == nullptr) env = &g_bn_write_lk_default;
    if (count == 0) return 0;

    uint8_t* const pio = env->ioinfo_entry(handle);
    const uint8_t osfile = pio[4];
    if ((osfile & 0x20) != 0) env->seek_end(handle);   // FAPPEND

    uint32_t total = 0;      // local_41c
    uint32_t crlf_count = 0; // local_420
    uint32_t dos_error = 0;  // local_410

    if ((osfile & 0x80) == 0) {   // not FTEXT
        uint32_t written = 0;
        if (env->write_file(*(void**)pio, buf, count, &written) == 0)
            dos_error = env->get_last_error();
        else
            total = written;
    } else {
        const char* const begin = (const char*)buf;
        uint32_t consumed = 0;
        uint32_t emitted, written;
        do {
            char chunk[0x400];
            emitted = 0;
            while (consumed < count && emitted < 0x400) {
                const char c = begin[consumed++];
                if (c == '\n') {
                    ++crlf_count;
                    chunk[emitted++] = '\r';
                }
                chunk[emitted++] = c;
            }
            written = 0;
            if (env->write_file(*(void**)pio, chunk, emitted, &written) == 0) {
                dos_error = env->get_last_error();
                break;
            }
            total += written;
        } while (emitted <= written && consumed < count);
    }

    if (total != 0) return (int32_t)(total - crlf_count);

    if (dos_error != 0) {
        if (dos_error == 5) {
            int32_t* const terrno = env->terrno_slot();
            int32_t* const doserrno = env->doserrno_slot();
            *terrno = 9;      // EBADF
            *doserrno = 5;    // ERROR_ACCESS_DENIED
            return -1;
        }
        env->dosmaperr(dos_error);
        return -1;
    }

    if ((osfile & 0x40) != 0 && *(const char*)buf == '\x1a') return 0;   // FDEV + ^Z
    int32_t* const terrno = env->terrno_slot();
    int32_t* const doserrno = env->doserrno_slot();
    *terrno = 0x1C;
    *doserrno = 0;
    return -1;
}
