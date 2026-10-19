// 1.13c D2Gfx+0x053F1 (411 B): rebuild the CRT byte classification and case-map tables.
// Stage A implementation from build-wasm/ghidra-d2gfx/decomp/0x53F1.c. Windows locale APIs are
// explicit host seams; the retail table update order and the SBCS fallback are retained here.
#include <cstdint>

struct D2GfxCtypeInitHost {
    uint32_t code_page;
    uint32_t locale; // DAT_6FA9D6AC, independent of the multibyte code page
    uint8_t* ctype;          // retail _ctype + 1, 256 bytes
    uint8_t* case_map;       // retail _tolower, 256 bytes
    // Return exactly 1 on success; tables describe the selected Windows code page.
    int32_t (*get_cp_info)(uint32_t code_page, uint8_t lead_byte_ranges[12]);
    int32_t (*get_string_types)(uint32_t info_type, const uint8_t* input, int32_t length,
                                 uint16_t* output, uint32_t code_page, uint32_t locale, int32_t strict);
    int32_t (*map_case)(uint32_t locale, uint32_t flags, const uint8_t* input, int32_t length,
                         uint8_t* output, int32_t capacity, uint32_t code_page, int32_t strict);
};

// 1.13c D2Gfx+0x053F1. Retail first asks GetCPInfo. On success it classifies all byte values and
// fills case mappings through the CRT's locale APIs; if that initial API fails it uses the exact
// ASCII-only fallback. The locale API seams allow a caller with the matching Windows locale data
// to supply those results without making this wasm implementation depend on host locale state.
extern "C" void d2gfx_ctype_init_53f1(const D2GfxCtypeInitHost* h) {
    uint8_t chars[256];
    for (uint32_t i = 0; i < 256; ++i) chars[i] = static_cast<uint8_t>(i);
    chars[0] = ' ';
    uint8_t ranges[12]{};
    uint16_t types[256]{};
    uint8_t lower[256], upper[256];
    if (h->get_cp_info(h->code_page, ranges) == 1) {
        for (uint32_t p = 0; p < sizeof(ranges) && ranges[p] != 0; p += 2) {
            const uint8_t first = ranges[p];
            const uint8_t last = ranges[p + 1];
            if (first <= last) {
                for (uint32_t c = first; c <= last; ++c) chars[c] = ' ';
            }
        }
        h->get_string_types(1, chars, 256, types, h->code_page, h->locale, 0);
        h->map_case(h->locale, 0x100, chars, 256, lower, 256, h->code_page, 0);
        h->map_case(h->locale, 0x200, chars, 256, upper, 256, h->code_page, 0);
        for (uint32_t i = 0; i < 256; ++i) {
            if ((types[i] & 1u) != 0) {
                h->ctype[i] |= 0x10;
                h->case_map[i] = lower[i];
            } else if ((types[i] & 2u) != 0) {
                h->ctype[i] |= 0x20;
                h->case_map[i] = upper[i];
            } else {
                h->case_map[i] = 0;
            }
        }
        return;
    }
    for (uint32_t i = 0; i < 256; ++i) {
        if (i >= 0x41 && i <= 0x5a) {
            h->ctype[i] |= 0x10;
            h->case_map[i] = static_cast<uint8_t>(i + 0x20);
        } else if (i >= 0x61 && i <= 0x7a) {
            h->ctype[i] |= 0x20;
            h->case_map[i] = static_cast<uint8_t>(i - 0x20);
        } else {
            h->case_map[i] = 0;
        }
    }
}
