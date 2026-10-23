// 1.13c Fog+0xB1EF -- CRT _chsize_lk(fd, size): grow with zeros (binary mode) or truncate.
#include <cstdint>

struct FogChsizeEnv {
    uint32_t (*lseek)(int fd, int off, int whence);   // __lseek_lk, 0xFFFFFFFF on error
    void* (*osfhandle)(int fd);                       // __get_osfhandle
    int (*setEndOfFile)(void* h);                     // SetEndOfFile
    int (*setmode)(int fd, int mode);                 // __setmode_lk
    int (*write)(int fd, const void* buf, uint32_t n);// FUN_6ff5701c, -1 on error
    uint32_t (*getLastError)();
    int* errnoSlot;                                   // FUN_6ff56a5a
    uint32_t* doserrnoSlot;                           // FUN_6ff56a63
};

int fog_sub_B1EF(const FogChsizeEnv& e, int fd, int size) {
    int rc = 0;
    uint32_t cur = e.lseek(fd, 0, 1);
    uint32_t end;
    if (cur == 0xFFFFFFFFu || (end = e.lseek(fd, 0, 2)) == 0xFFFFFFFFu) return -1;
    int rem = size - (int)end;
    if (rem < 1) {
        if (rem < 0) {
            e.lseek(fd, size, 0);
            rc = (e.setEndOfFile(e.osfhandle(fd)) != 0) - 1;
            if (rc == -1) { *e.errnoSlot = 13; *e.doserrnoSlot = e.getLastError(); }
        }
    } else {
        char buf[4096] = {};
        int mode = e.setmode(fd, 0x8000);
        do {
            uint32_t n = rem < 0x1000 ? (uint32_t)rem : 0x1000;
            int w = e.write(fd, buf, n);
            if (w == -1) {
                if (*e.doserrnoSlot == 5) *e.errnoSlot = 13;
                rc = -1;
                break;
            }
            rem -= w;
        } while (rem > 0);
        e.setmode(fd, mode);
    }
    e.lseek(fd, (int)cur, 0);
    return rc;
}
