// src/core/d2ddraw/d2ddraw_9460.cpp -- 1.13c D2DDraw.dll debug-font glyph build: rasterise the 96 printable characters of a
// 10-point Courier into an 8x16 host bitmap one at a time, read each back pixel by pixel into a 12288-byte glyph table (0xFF where
// the pixel is the text colour, else 0), fill the per-glyph offsets and ask GDI for the advances. Read from the disassembly
// (0x6f8c9460..0x6f8c95d9). Every GDI/USER call and the Fog allocator is a seam; the module globals arrive as pointers.
#include <cstdint>

struct D2DDrawGlyphBuildEnv {
    const uint32_t* textColor;   // DAT 0xE910: the colour GetPixel is compared against (also handed to SetTextColor)
    const uint32_t* bkColor;     // DAT 0x101F0
    uint8_t** glyphs;            // DAT 0x101F4 := the Fog allocation
    uint32_t* offsets;           // DAT 0xF380: 96 byte offsets (i * 0x80)
    int32_t* widths;             // DAT 0xF500: 96 advances, filled by GetCharWidthA
    uint32_t strDisplay, strCourier, strFile;   // the module strings "DISPLAY" 0xC244, "Courier" 0xC23C, the source path 0xC214
    uint32_t (*createDC)(uint32_t driver, uint32_t device, uint32_t output, uint32_t mode);
    uint32_t (*createFont)(int32_t h, int32_t w, int32_t esc, int32_t orient, int32_t weight, uint32_t italic, uint32_t underline,
                           uint32_t strike, uint32_t charset, uint32_t outPrec, uint32_t clipPrec, uint32_t quality, uint32_t pitch,
                           uint32_t face);
    uint32_t (*createBitmap)(int32_t w, int32_t h, uint32_t planes, uint32_t bpp, uint32_t bits);
    uint32_t (*selectObject)(uint32_t dc, uint32_t obj);
    uint32_t (*setTextColor)(uint32_t dc, uint32_t c);
    uint32_t (*setBkColor)(uint32_t dc, uint32_t c);
    uint32_t (*setBkMode)(uint32_t dc, uint32_t mode);
    uint32_t (*setRect)(int32_t* rc, int32_t l, int32_t t, int32_t r, int32_t b);
    uint8_t* (*alloc)(uint32_t size, uint32_t file, uint32_t line, uint32_t flags);   // Fog 10042 (thunk 0x5F34): ECX size, EDX file
    uint32_t (*drawText)(uint32_t dc, const char* s, int32_t n, int32_t* rc, uint32_t fmt);
    uint32_t (*getPixel)(uint32_t dc, int32_t x, int32_t y);
    uint32_t (*getCharWidth)(uint32_t dc, uint32_t first, uint32_t last, int32_t* out);
    uint32_t (*deleteObject)(uint32_t obj);
    uint32_t (*deleteDC)(uint32_t dc);
};

// 1.13c D2DDraw+0x9460 (no arguments, no result). CreateDCA("DISPLAY"), CreateFontA(10, 0,0,0, 400, 0 x7, 1, "Courier"),
// CreateBitmap(8, 16, 1, 8, NULL), select the font then the bitmap, SetTextColor(*0xE910), SetBkColor(*0x101F0), SetBkMode(2,
// OPAQUE), SetRect(rc, 0, 0, 8, 16), *0x101F4 := Fog 10042(0x3000, file, 0x3A, 0). For c = 0x20..0x7F: DrawTextA(dc, &c, 1, rc, 0),
// then rows 0..15, columns 0..7: one byte per GetPixel, 0xFF when it equals *0xE910 (re-read each time), else 0; offsets[i] := i *
// 0x80. Then GetCharWidthA(dc, 0x20, 0x7F, 0xF500), DeleteObject(bitmap), DeleteObject(font), DeleteDC(dc). No result is checked.
extern "C" void d2ddraw_glyph_build_9460(const D2DDrawGlyphBuildEnv* e) {
    uint32_t dc = e->createDC(e->strDisplay, 0, 0, 0);
    uint32_t font = e->createFont(10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 1, e->strCourier);
    uint32_t bmp = e->createBitmap(8, 0x10, 1, 8, 0);
    e->selectObject(dc, font);
    e->selectObject(dc, bmp);
    e->setTextColor(dc, *e->textColor);
    e->setBkColor(dc, *e->bkColor);
    e->setBkMode(dc, 2);
    int32_t rc[4];
    e->setRect(rc, 0, 0, 8, 0x10);
    uint8_t* p = e->alloc(0x3000, e->strFile, 0x3A, 0);
    *e->glyphs = p;
    uint32_t off = 0;
    char c = 0x20;
    for (int i = 0; i < 96; ++i, ++c, off += 0x80) {
        e->drawText(dc, &c, 1, rc, 0);
        for (int32_t y = 0; y < 0x10; ++y)
            for (int32_t x = 0; x < 8; ++x)
                *p++ = e->getPixel(dc, x, y) == *e->textColor ? 0xFF : 0;
        e->offsets[i] = off;
    }
    e->getCharWidth(dc, 0x20, 0x7F, e->widths);
    e->deleteObject(bmp);
    e->deleteObject(font);
    e->deleteDC(dc);
}
