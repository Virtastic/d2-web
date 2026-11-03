#include <cstdint>
// 1.13c D2Launch+0x19680 -- (no arguments) refresh the screen state (0x11CB0), pick the video resolution: Fog#10191 == 0 gives
// "640x292" and mode 0, otherwise "640x146" and mode 2. 0x14920 builds the video path into a 260-byte buffer (ESI = buf, EBX = format
// string, EDI = resolution), D2gfx#10027(buf, mode, 0) plays it, then 0x18160 rebuilds the menu.
struct D2l19680Seams {
    void (*f11cb0)();
    int (*fog10191)();
    void (*f14920)(char* buf, const char* fmt, const char* res);
    void (*play)(char* buf, uint32_t mode, uint32_t zero);   // D2gfx#10027
    void (*f18160)();
};
extern "C" void d2l_19680_core(const char* fmt, const D2l19680Seams* s) {
    char res[8]; char buf[260];
    s->f11cb0();
    uint32_t mode = 0;
    const char* src = "640x292";
    if (s->fog10191()) { mode = 2; src = "640x146"; }
    for (int i = 0; i < 8; ++i) res[i] = src[i];
    s->f14920(buf, fmt, res);
    s->play(buf, mode, 0);
    s->f18160();
}
