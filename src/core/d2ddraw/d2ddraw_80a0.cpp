// 1.13c D2DDraw+0x080a0 -- install the 0x48-dword colour table. Decompile-derived.
#include <cstdint>

struct D2DDrawColorTab { uint32_t* cur; uint32_t tab[0x48]; };  // +0xF784 pointer, +0xF788 storage

// 1.13c D2DDraw+0x080a0
// Copies 0x48 dwords from src into the table storage and points the current-table global at it.
extern "C" void d2ddraw_set_color_table(D2DDrawColorTab* g, const uint32_t* src) {
    for (int32_t i = 0; i < 0x48; ++i) g->tab[i] = src[i];
    g->cur = g->tab;
}
