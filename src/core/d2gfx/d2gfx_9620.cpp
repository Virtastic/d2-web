#include <cstdint>

// 1.13c D2Gfx+0x9620
// Retail: EAX = object, ECX = destination, stack = light byte. Bit 2 of the byte at object+8 picks the run blit
// (0x8E30, 15 rows) or the fixed-shape blit (0x93E0); the palette is row (light >> 3) of the 32-row table at
// palBase (retail module global +0x11678). The pixel pointer is the dword at object+0x10. The callees come in
// as seams.
extern "C" void d2gfx_sub_9620(const uint8_t* obj, uint32_t dest, uint8_t light, uint32_t palBase,
                               void (*s8e30)(uint32_t pix, uint32_t rows, uint32_t pal, uint32_t dest),
                               void (*s93e0)(uint32_t pix, uint32_t pal, uint32_t dest))
{
    const uint32_t pix = *(const uint32_t*)(obj + 0x10);
    const uint32_t pal = palBase + ((uint32_t)(light >> 3) << 8);
    if (obj[8] & 4) s8e30(pix, 0xF, pal, dest);
    else s93e0(pix, pal, dest);
}
