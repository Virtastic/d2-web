#include <cstdarg>
#include <cstdint>
#include <cstdio>

extern "C" void d2gfx_sub_79c0(uint32_t id, char* dest, uint32_t size);

// 1.13c D2Gfx+0x7AA0: fatal-error reporter. Builds the text with 79C0 (fault string),
// wvsprintfA's it, shows a modal MessageBoxA "D2Gfx.DLL" (0x2010), returns 1. Host seam replaces the box.
extern "C" int d2gfx_sub_7aa0(uint32_t id, void (*msgbox)(const char*, const char*, uint32_t), const char* fmt, ...) {
    char f[512], out[512];
    d2gfx_sub_79c0(id, f, sizeof f);
    (void)fmt;
    va_list ap; va_start(ap, fmt);
    vsnprintf(out, sizeof out, f, ap);
    va_end(ap);
    if (msgbox) msgbox(out, "D2Gfx.DLL", 0x2010);
    return 1;
}
