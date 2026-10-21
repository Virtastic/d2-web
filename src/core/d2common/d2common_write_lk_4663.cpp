#include <cstdint>
#include <cstddef>

// MSVCRT _write_lk (statically linked CRT): writes `count` bytes to a CRT fd.
// Text-mode fds (flag 0x80) get '\n' expanded to "\r\n" in 1024-byte chunks;
// the result is bytes written minus the injected '\r's, 0 for a ^Z (0x1a) text
// write that wrote nothing, -1 with errno 9 (access denied) / 28 (no space).
// Append fds (flag 0x20) seek to end first. The host sink replaces WriteFile.
// ponytail: sink is a callback, no CRT fd table; add the table when a caller needs real fds.
// 1.13c D2Common+0x04663
typedef int32_t (*WriteSink)(const char* buf, uint32_t n, uint32_t* err);

int32_t d2common_write_lk_4663(WriteSink sink, uint8_t flags, const char* buf, uint32_t count)
{
    if (count == 0) return 0;
    uint32_t total = 0, err = 0;
    int32_t crs = 0;
    if (!(flags & 0x80)) {
        uint32_t w = 0;
        if (sink(buf, count, &err)) { total = w = count; (void)w; }
    } else {
        char tmp[0x400 + 1];
        uint32_t pos = 0;
        do {
            int32_t n = 0;
            while (n < 0x400 && pos < count) {
                char c = buf[pos++];
                if (c == '\n') { crs++; tmp[n++] = '\r'; }
                tmp[n++] = c;
            }
            if (!sink(tmp, (uint32_t)n, &err)) break;
            total += (uint32_t)n;
        } while (pos < count);
    }
    if (total) return (int32_t)total - crs;
    if (err) return -1;
    if ((flags & 0x40) && buf[0] == 0x1a) return 0;
    return -1; // errno 28
}
