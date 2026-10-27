// 1.13c D2Win+0x118F0 (SetPopUpUnicodeText): the pop-up text and its four argument words.
#include <cstdint>
#include <cstring>
// Retail layout from D2Win+0xC9E58: text[0x400] (0x800 B), a, b, c, d, 0.
static struct { uint16_t text[0x400]; uint32_t a, b, c, d, zero; } g_popup;
extern "C" void d2win_SetPopUpUnicodeText(const uint16_t* text, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    uint32_t n = 0;
    if (text) while (text[n]) n++;
    if (!text || n > 0x3ff) memset(g_popup.text, 0, sizeof g_popup.text);   // null or too long: cleared
    else memcpy(g_popup.text, text, (n + 1) * 2);                           // strcpy: the old tail stays
    g_popup.a = a; g_popup.b = b; g_popup.c = c; g_popup.d = d; g_popup.zero = 0;
}
extern "C" void d2win_popup_reset(int fill) { memset(&g_popup, fill, sizeof g_popup); }
extern "C" const void* d2win_popup_state() { return &g_popup; }
extern "C" void d2win_popup_set_tail(uint32_t v) { g_popup.zero = v; }   // D2Win+0xCA668 = 0x6f9aa668, the word SetPopUp zeroes
