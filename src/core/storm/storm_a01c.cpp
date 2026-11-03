// CRT getcwd wrapper.
// getcwd(buf, size) = _getdcwd(0, buf, size) under CRT lock 7.
#include <cstddef>
#include <cstdint>

extern "C" char *storm_getdcwd(uint32_t drive, char *buf, size_t len, int (*validdrive)(uint32_t),
                               uint32_t (*get_path)(uint32_t drive, char *out260), int *err, int *doserr);

// 1.13c Storm+0x0A01C  cdecl __getcwd(buf, size)
extern "C" char *storm_a01c(char *buf, int32_t size, void (*lock)(int), void (*unlock)(int),
                            int (*validdrive)(uint32_t), uint32_t (*get_path)(uint32_t, char *), int *err) {
    lock(7);
    char *r = storm_getdcwd(0, buf, (size_t)size, validdrive, get_path, err, nullptr);  // drive 0 never validates
    unlock(7);
    return r;
}
