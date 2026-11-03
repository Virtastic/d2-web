#include <cstdint>
// 1.13c D2Launch+0x14710 -- (no arguments) validate the character name in the edit box. D2Win#10166 (ECX = DAT 253B8) answers the
// UTF-16 text (null = none); the name is converted to UTF-8 (Unicode::toUtf, 32 bytes) and accepted when 1 < len < 16, neither the
// first nor the last character is '_' or '-', and 0x12340 (ESI = the UTF-8 name) answers < 2. D2Win#10147(ECX = DAT 25298, EDX = ok)
// then enables or disables the button.
struct D2l14710Seams {
    const void* (*text10166)(uint32_t ecx);
    int (*ustrlen)(const void* wide);
    void (*toUtf)(char* dst, const void* wide, uint32_t n);
    int (*f12340)(const char* utf);
    void (*apply10147)(uint32_t ecx, uint32_t ok);
};
extern "C" void d2l_14710_core(uint32_t g253b8, uint32_t g25298, const D2l14710Seams* s) {
    char utf[32];
    uint32_t ok = 0;
    const void* w = s->text10166(g253b8);
    if (w) {
        int len = s->ustrlen(w);
        s->toUtf(utf, w, 0x20);
        if (len >= 2 && len < 16 && utf[len - 1] != '_' && utf[0] != '_' && utf[len - 1] != '-' && utf[0] != '-')
            if (s->f12340(utf) < 2) ok = 1;
    }
    s->apply10147(g25298, ok);
}
