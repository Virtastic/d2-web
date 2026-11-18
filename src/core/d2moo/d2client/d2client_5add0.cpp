// 1.13c D2Client+0x5ADD0 (ret 8): stack arg0 = number, arg1 = x. "%d" of the number widened into a 32-char buffer (last char zero);
// while the text measures wider than 0x2e (0xD35A, current font) its last character is cut; then font 6 (0xD3A8 returns the previous
// font), draw 0xD37E (ECX text, EDX x + 8, stack 0x37, 0, 0), font restored.
#include <cstdio>
#include "d2client_batch_5a5e0.h"
namespace d2client_batch_5a5e0 {
void drawNumber(Services& s, int32_t n, int32_t x) {
 char a[32]; snprintf(a, sizeof a, "%d", n);
 uint16_t w[32] = {}; for (int i = 0; i < 31 && a[i]; ++i) w[i] = (uint8_t)a[i];
 size_t len = 0; while (w[len]) ++len;
 uint16_t* e = w + len;
 while (s.textWidth(w) > 0x2e) { *--e = 0; }
 const uint32_t prev = s.fontSet(6);
 s.showText(w, x + 8, 0x37, 0);
 s.fontSet(prev);
}
}
