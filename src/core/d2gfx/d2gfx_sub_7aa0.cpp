#include <cstdarg>
#include <cstdint>
#include <cstdio>

extern "C" void d2gfx_sub_79c0(uint32_t enabled, uint32_t base, uint32_t head, uint32_t faultRec,
                               uint8_t* dest, uint32_t size,
                               uint32_t (*handler)(uint32_t faultRec, uint8_t* dest, uint32_t size));

// The fault-hook chain globals 0x79C0 walks (its data segment is not ported) and the MessageBoxA seam.
struct D2GfxFaultHost {
    uint32_t enabled, base, head;
    uint32_t (*handler)(uint32_t faultRec, uint8_t* dest, uint32_t size);
    void (*msgbox)(const char* text, const char* caption, uint32_t flags);
};

// 1.13c D2Gfx+0x7AA0: cdecl(faultRec, ...) fatal-error reporter. 0x79C0(faultRec) fills the 512-byte format
// buffer (EBX = buffer, EDI = 0x200), wvsprintfA formats it with the caller's varargs into a second 512-byte
// buffer, MessageBoxA(0, text, "D2Gfx.DLL", 0x2010), returns 1.
extern "C" int d2gfx_sub_7aa0(const D2GfxFaultHost* h, uint32_t faultRec, ...) {
    char fmt[512], out[512];
    d2gfx_sub_79c0(h->enabled, h->base, h->head, faultRec, (uint8_t*)fmt, sizeof fmt, h->handler);
    va_list ap; va_start(ap, faultRec);
    vsnprintf(out, sizeof out, fmt, ap);
    va_end(ap);
    if (h->msgbox) h->msgbox(out, "D2Gfx.DLL", 0x2010);
    return 1;
}
