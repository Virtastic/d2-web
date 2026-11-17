// 1.13c D2Client+0xBFEC0, the gold-pile branch: the name line the Alt ground-label pass (0x58FB0, dst = a label slot of
// 0x11FEF8, cap 0x100) and the hover text (0xC0D30) build for an item whose type (0xC220, D2Common #11088) is 4.
//   colour: quality (#10106) 4..9 -> 3, 2, 9, 4, 8, 10; else flag 0x800 or 0x400000 -> 5; else 0. The items.txt record's
//           quest byte (+0x12A) with a code other than "leg " -> 4; codes ceh/bet/tes/fed/toa/dhn/bey/mbr/pk1/pk2/pk3 or item
//           type 0x4A (#10731 thunk 0xC1CC) -> 8.
//   line:   wsprintf "%i" of stat 0xE, widened through a 9-unit buffer (at most 8 characters survive), then D2Lang 0xF9B, then
//           the record's name string (0xA5D90 case 4: #10695 record -> #10003(wNameStr)).
//   prefix: with a dst, 0xBF5F0 puts D2Lang 0xF9A and the character '0' + colour before a non-empty line.
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>
#include <emscripten.h>
#include <D2Items.h>
#include <DataTbls/ItemsTbls.h>

extern "C" const char16_t* d2lang_Ordinal_10003(uint16_t index);

static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80, "items.txt code");
static_assert(offsetof(D2ItemsTxt, wNameStr) == 0xF4, "items.txt name string");
static_assert(offsetof(D2ItemsTxt, nQuest) == 0x12A, "items.txt quest byte");

namespace {
uint32_t goldColour(const D2ItemsTxt* rec, uint32_t classId, uint32_t quality, uint32_t flags) {
    static const uint32_t byQuality[6] = {3, 2, 9, 4, 8, 10};
    uint32_t c = 0;
    if (quality - 4 <= 5) c = byQuality[quality - 4];
    else if (flags & 0x800 || flags & 0x400000) c = 5;
    if (!rec) return c;
    if (rec->nQuest && rec->dwCode != 0x2067656Cu) c = 4;
    static const uint32_t k8[] = {0x20686563u, 0x20746562u, 0x20736574u, 0x20646566u, 0x20616f74u, 0x206e6864u,
                                  0x20796562u, 0x2072626du, 0x20316b70u, 0x20326b70u, 0x20336b70u};
    for (uint32_t v : k8) if (v == rec->dwCode) c = 8;
    if (ITEMS_CheckItemTypeIdByItemId((int)classId, 0x4A)) c = 8;
    return c;
}
std::string utf8(const std::u16string& s) {
    std::string o;
    for (char16_t c : s) {
        if (c < 0x80) o.push_back((char)c);
        else if (c < 0x800) { o.push_back((char)(0xC0 | (c >> 6))); o.push_back((char)(0x80 | (c & 0x3F))); }
        else { o.push_back((char)(0xE0 | (c >> 12))); o.push_back((char)(0x80 | ((c >> 6) & 0x3F))); o.push_back((char)(0x80 | (c & 0x3F))); }
    }
    return o;
}
}  // namespace

// 1.13c D2Client+0xBFEC0 (gold branch). prefixed = 1: the label slot's text, colour prefix included (dst non-null);
// prefixed = 0: the line before 0xBF5F0, what the no-dst hover path hands 0xD384 with the colour as a separate word.
std::u16string d2c_gold_line(uint32_t classId, int32_t amount, uint32_t quality, uint32_t flags, bool prefixed) {
    const D2ItemsTxt* rec = DATATBLS_GetItemsTxtRecord((int)classId);
    char nb[16];
    std::snprintf(nb, sizeof nb, "%i", (int)amount);
    std::u16string line;
    for (int i = 0; nb[i] && i < 8; ++i) line.push_back((char16_t)(uint8_t)nb[i]);   // toUnicode(num, nb, 9)
    line += d2lang_Ordinal_10003(0xF9B);
    if (rec) line += d2lang_Ordinal_10003(rec->wNameStr);
    if (!prefixed || line.empty()) return line;
    std::u16string out = d2lang_Ordinal_10003(0xF9A);
    out.push_back((char16_t)(uint8_t)(char)('0' + goldColour(rec, classId, quality, flags)));
    return out + line;
}

extern "C" {
EMSCRIPTEN_KEEPALIVE const char* d2_client_gold_label(uint32_t classId, int32_t amount, uint32_t quality, uint32_t flags, int prefixed) {
    static std::string out;
    out = utf8(d2c_gold_line(classId, amount, quality, flags, prefixed != 0));
    return out.c_str();
}
// The pile's class from its items.txt code (the shell holds the code text, not the class).
EMSCRIPTEN_KEEPALIVE int d2_client_item_class_by_code(const char* code) {
    if (!code) return -1;
    uint32_t c = 0x20202020u;
    for (int i = 0; i < 4 && code[i]; ++i) c = (c & ~(0xFFu << (8 * i))) | ((uint32_t)(uint8_t)code[i] << (8 * i));
    return DATATBLS_GetItemIdFromItemCode(c);
}
}
