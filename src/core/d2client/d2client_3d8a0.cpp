// 1.13c D2Client+0x3D8A0 -- the mouse-over tooltip for a 0x30-square cell. stdcall, 5 stack params (ret 0x14):
//   p1 = pointer to a signed word (an id), p2 = hover flag, p3 = a number, p4 = x of the cell's left edge, p5 = y of its bottom edge.
// Mouse = (DAT 0x11B828 x, DAT 0x11B824 y). Outside [p4, p4+0x30] x [p5-0x30, p5] the call does nothing. Inside, the flag is kept only
// when it already is 1, else 0.
// Inside it builds the text in a zeroed 256-wchar buffer: A = str(0xF9E); rec = lookup(id); b = rec ? rec.w[+8] : 0x1506, c = rec ? rec.w[+0xA] : 0x1506;
// strcpy(str(c)); strcat(A); strcat(str(0x105C)); strcat(str(0xF9D)); then the decimal text of p3 (wsprintfA "%i", toUnicode(.., 9)) is appended,
// then A again and str(b); finally Ordinal_10085 (0xD384, ECX = text, EDX = p4) is called with (p5-0x32, flag, 1).
// Retail loads the low half of EBP from the record and leaves the upper half as the caller's; the port zero-extends it.
#include <cstdint>
#include <cstring>
struct D2c3D8A0Env {
    const void* (*str)(uint32_t id);                                    // 0xD1EC (Ordinal_10003, ECX = id)
    const uint8_t* (*lookup)(int32_t id);                               // 0x17F0 (EAX = id), record or null
    void (*ctor)(void* p);                                              // IAT 0xCEB00 (ECX = p), run 256 + 10 times
    void (*strcpy)(void* dst, const void* src);                         // IAT 0xCEAB8
    void (*strcat)(void* dst, const void* src);                         // IAT 0xCEABC
    void (*toUnicode)(void* dst, const char* src, uint32_t n);          // IAT 0xCEAC4 (stack 9)
    void (*draw)(void* text, int32_t x, int32_t y, int32_t flag, int32_t one);   // 0xD384 (ECX = text, EDX = x)
};
extern "C" void d2c_3d8a0(const int16_t* p1, int32_t p2, int32_t p3, int32_t p4, int32_t p5, int32_t mouseX, int32_t mouseY, const D2c3D8A0Env* env) {
    bool inside = !(mouseX < p4 || mouseX > p4 + 0x30 || mouseY < p5 - 0x30 || mouseY > p5);
    if (!inside) return;
    if (p2 != 1) p2 = 0;
    uint8_t text[0x200]; uint8_t num[0x14]; char ascii[16];
    const void* a = env->str(0xf9e);
    for (int i = 0; i < 0x100; ++i) env->ctor(text + i * 2);
    int32_t id = *p1;
    const uint8_t* rec = env->lookup(id);
    uint32_t b = 0x1506;
    if (rec) { uint16_t w; std::memcpy(&w, rec + 8, 2); b = w; }
    rec = env->lookup(id);
    uint32_t c = 0x1506;
    if (rec) { uint16_t w; std::memcpy(&w, rec + 10, 2); c = w; }
    env->strcpy(text, env->str(c));
    env->strcat(text, a);
    env->strcat(text, env->str(0x105c));
    env->strcat(text, env->str(0xf9d));
    for (int i = 0; i < 10; ++i) env->ctor(num + i * 2);
    {   // wsprintfA "%i"
        int n = 0; uint32_t u = p3 < 0 ? 0u - (uint32_t)p3 : (uint32_t)p3; char r[12]; int k = 0;
        do { r[k++] = (char)('0' + u % 10); u /= 10; } while (u);
        if (p3 < 0) ascii[n++] = '-';
        while (k) ascii[n++] = r[--k];
        ascii[n] = 0;
    }
    env->toUnicode(num, ascii, 9);
    env->strcat(text, num);
    env->strcat(text, a);
    env->strcat(text, env->str(b));
    env->draw(text, p4, p5 - 0x32, p2, 1);
}
