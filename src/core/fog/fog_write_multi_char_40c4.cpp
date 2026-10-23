// 1.13c Fog+0x040C4 -- MSVC CRT _write_multi_char (printf engine helper).
#include <cstdio>

// Writes `ch` `count` times to `f` via write_char (0x4091), bumping *written per char;
// stops early once a write fails (write_char sets *written = -1).
// 1.13c Fog+0x040C4
int fog_write_multi_char(std::FILE* f, int ch, int count, int* written) {
    while (count > 0) {
        --count;
        if (std::fputc(ch, f) == EOF) { *written = -1; } else { ++*written; }
        if (*written == -1) break;
    }
    return *written;
}
