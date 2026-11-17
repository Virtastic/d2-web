// The colour of an item's name line, as the hovered-item tooltip picks it.
// 1.13c D2Client+0x93450 (the colour choice before its text is composed; the table is 0x6FB445AC in the
// retail image, d2client_93450.cpp kQualCol): quality 4..9 (magic, set, rare, unique, crafted, tempered)
// maps to D2Win colour 3, 2, 9, 4, 8, 10; any other quality is 5 (grey) when the item is socketed
// (flag 0x800) or ethereal (flag 0x400000), else 0 (white).
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_item_name_colour(int32_t quality, int32_t flags) {
    static const uint8_t kQualCol[6] = { 3, 2, 9, 4, 8, 10 };
    if ((uint32_t)(quality - 4) <= 5u) return kQualCol[quality - 4];
    if ((flags & 0x800) || (flags & 0x400000)) return 5;
    return 0;
}
