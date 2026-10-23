// 1.13c Fog+0x0701C: statically linked MSVCRT _write(fd, buf, count). Retail translates \n to \r\n in
// text-mode fds (1KB chunks) and returns input bytes consumed; ours writes raw bytes (POSIX, no CRLF
// since the port's fds are binary), 0 for count 0, -1 on error. EOF-^Z/ERROR_ACCESS_DENIED mapping dropped.
#include <unistd.h>

extern "C" int fog_sub_701c(unsigned fd, const char* buf, unsigned count) {  // 1.13c Fog+0x0701C
    if (count == 0) return 0;
    long n = ::write((int)fd, buf, count);
    return n < 0 ? -1 : (int)n;
}
