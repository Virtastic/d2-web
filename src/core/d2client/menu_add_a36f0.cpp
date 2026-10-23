// D2Client sub_A36F0: append an entry to a popup menu. Decompile-derived; callees are seams.
// 32-bit retail layout, menu = uintptr_t base, byte offsets.
#include <cstdint>
namespace menu_add_a36f0 {
struct Seams {
 virtual ~Seams()=default;
 virtual void fatal(int32_t msg)=0;                          // Ordinal_10265 + 10024 + _exit(-1)
 virtual int32_t ustrlen(const uint16_t* s)=0;               // Unicode::strlen
 virtual void ustrncpy(uint16_t* d, const uint16_t* s, int32_t n)=0; // Unicode::strncpy
 virtual void layoutFixed()=0;                               // FUN_6fb532d0 (menu+0x58==0)
 virtual void layoutCentered()=0;                            // FUN_6fb53580 (menu+0x58!=0)
 virtual void selectFirst(uintptr_t m)=0;                    // FUN_6fb53370
};
struct Globals { uint32_t g9640, g963c, g9648, g9644, gc2fc; }; // DAT_6fbc9640/963c/9648/9644/6fbcc2fc
template<class T> static T& at(uintptr_t b, uint32_t o) { return *(T*)(b + o); }
// 1.13c D2Client+0xA36F0   (this = menu, eax = kind; stack: text,p2,p3,p4,p5,p6)
void sub_A36F0(Seams& s, uintptr_t m, int32_t kind, const uint16_t* text, uint32_t p2, uint32_t p3, uint32_t p4, int32_t p5, int32_t p6, Globals& g) {
 int32_t i = at<int32_t>(m, 0x50);
 if (i > 9) s.fatal(0x55d);
 at<int32_t>(m, 0x50) = i + 1;
 uintptr_t e = m + (uintptr_t)i * 0x110;
 at<uint32_t>(e, 0x158) = p2; at<uint32_t>(e, 0x15c) = 0;
 at<uint32_t>(e, 0x164) = p3; at<uint32_t>(e, 0x168) = p4;
 if (kind == 3 && at<int32_t>(m, 0x5c) == 1) s.fatal(0x569);
 at<int32_t>(e, 0x16c) = kind; at<int32_t>(e, 0x170) = p5; at<int32_t>(e, 0x174) = p6;
 int32_t n = s.ustrlen(text) + 1; if (n > 0x77) n = 0x77;
 s.ustrncpy((uint16_t*)(e + 0x68), text, n);
 at<uint16_t>(m, (i * 0x88 + n) * 2 + 0x68) = 0;
 if (p6) ++at<int32_t>(m, 0x4c);
 if (!at<int32_t>(m, 0x58)) s.layoutFixed(); else s.layoutCentered();
 g.g9640 = at<uint32_t>(m, 0x30); g.g963c = at<uint32_t>(m, 0x2c);
 g.g9648 = at<uint32_t>(m, 0x38) + g.g9640; g.g9644 = at<uint32_t>(m, 0x34) + g.g963c;
 g.gc2fc = 1;
 if (at<int32_t>(m, 0x44) == -1 && p6) s.selectFirst(m);
}
}
