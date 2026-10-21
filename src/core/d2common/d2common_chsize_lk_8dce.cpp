#include <cstdint>
#include <cstring>

// 1.13c D2Common+0x08DCE _chsize_lk(fh, size): MSVC CRT file resize. Remembers the position,
// seeks to the end; if the file is shorter, appends zeros in 4096-byte writes (binary mode forced
// via __setmode_lk, restored after); if longer, seeks to size and SetEndOfFile (errno 13 on fail);
// finally seeks back. Returns 0 or -1. Env seam: CRT/Win32 helpers owner-supplied.
struct D2ChsizeEnv {
    uint32_t (*lseekLk)(uint32_t fh, int32_t off, int whence);   // 0x7A5A... __lseek_lk
    int (*setmodeLk)(uint32_t fh, int mode);                     // __setmode_lk
    int (*writeLk)(uint32_t fh, const char* buf, uint32_t n);    // FUN_6fd54663
    intptr_t (*getOsfhandle)(uint32_t fh);                       // __get_osfhandle
    int (*setEndOfFile)(intptr_t h);                             // kernel32
    uint32_t (*getLastError)();
    void (*setErrno)(int);                                       // FUN_6fd544bc
    void (*setDosErrno)(uint32_t);                               // FUN_6fd544c5
    uint32_t (*getDosErrno)();
};

int d2common_chsize_lk_8dce(D2ChsizeEnv& e, uint32_t fh, int32_t size)
{
    int rc = 0;
    uint32_t pos = e.lseekLk(fh, 0, 1), end;
    if (pos == 0xffffffffu || (end = e.lseekLk(fh, 0, 2)) == 0xffffffffu) return -1;
    int32_t rem = (int32_t)((uint32_t)size - end);
    if (rem < 1) {
        if (rem < 0) {
            e.lseekLk(fh, size, 0);
            rc = (e.setEndOfFile(e.getOsfhandle(fh)) != 0) - 1;
            if (rc == -1) { e.setErrno(0xd); e.setDosErrno(e.getLastError()); }
        }
    } else {
        char buf[0x1000];
        memset(buf, 0, sizeof buf);
        int old = e.setmodeLk(fh, 0x8000);
        do {
            uint32_t n = rem < 0x1000 ? (uint32_t)rem : 0x1000;
            int w = e.writeLk(fh, buf, n);
            if (w == -1) {
                if (e.getDosErrno() == 5) e.setErrno(0xd);
                rc = -1;
                break;
            }
            rem -= w;
        } while (rem > 0);
        e.setmodeLk(fh, old);
    }
    e.lseekLk(fh, (int32_t)pos, 0);
    return rc;
}
