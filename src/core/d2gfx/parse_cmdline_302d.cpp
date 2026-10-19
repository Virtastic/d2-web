// Visual Studio 2003 CRT command-line tokenizer.
#include <cstdint>
#include <emscripten.h>

namespace {

// The retail body consults only bit 2 of _mbctype[byte + 1]. CRT setup may
// replace this default non-DBCS table with the active code-page table.
uint8_t mbctype[257]{};
} // namespace

// 1.13c D2Gfx+0x302D
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_parse_cmdline(
    const char* source, char* args, int32_t* byte_count, char** argv, int32_t* argc) {
    auto is_lead_byte = [](uint8_t byte) { return (mbctype[static_cast<uint16_t>(byte) + 1] & 4u) != 0; };
    const auto* cursor = reinterpret_cast<const uint8_t*>(source);
    auto* output = reinterpret_cast<uint8_t*>(args);
    *byte_count = 0;
    *argc = 1;
    if (argv != nullptr) *argv++ = args;

    bool in_quotes = false;
    uint8_t current;
    const uint8_t* next;
    do {
        if (*cursor == '"') {
            in_quotes = !in_quotes;
            next = cursor + 1;
            current = '"';
        } else {
            ++*byte_count;
            if (output != nullptr) *output++ = *cursor;
            current = *cursor;
            next = cursor + 1;
            if (is_lead_byte(current)) {
                ++*byte_count;
                if (output != nullptr) *output++ = *next;
                next = cursor + 2;
            }
            if (current == 0) {
                --next;
                cursor = next;
                goto parse_arguments;
            }
        }
        cursor = next;
    } while (in_quotes || (current != ' ' && current != '\t'));

    if (output != nullptr) output[-1] = 0;

parse_arguments:
    in_quotes = false;
    while (*cursor != 0) {
        while (*cursor == ' ' || *cursor == '\t') ++cursor;
        if (*cursor == 0) break;

        if (argv != nullptr) *argv++ = reinterpret_cast<char*>(output);
        ++*argc;
        for (;;) {
            bool copy_character = true;
            uint32_t slashes = 0;
            while (*cursor == '\\') {
                ++slashes;
                ++cursor;
            }
            if (*cursor == '"') {
                const uint8_t* after_quote = cursor;
                if ((slashes & 1u) == 0) {
                    if (!in_quotes || cursor[1] != '"') {
                        copy_character = false;
                    } else {
                        ++after_quote;
                    }
                    in_quotes = !in_quotes;
                }
                slashes >>= 1;
                cursor = after_quote;
            }
            while (slashes != 0) {
                if (output != nullptr) *output++ = '\\';
                ++*byte_count;
                --slashes;
            }

            current = *cursor;
            if (current == 0 || (!in_quotes && (current == ' ' || current == '\t'))) break;
            if (copy_character) {
                if (is_lead_byte(current)) {
                    if (output != nullptr) *output++ = current;
                    ++cursor;
                    ++*byte_count;
                }
                if (output != nullptr) *output++ = *cursor;
                ++*byte_count;
            }
            ++cursor;
        }
        if (output != nullptr) *output++ = 0;
        ++*byte_count;
    }
    if (argv != nullptr) *argv = nullptr;
    ++*argc;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_set_parse_cmdline_mbctype(const uint8_t* table) {
    if (table == nullptr) {
        for (auto& value : mbctype) value = 0;
        return;
    }
    for (uint32_t i = 0; i < sizeof(mbctype); ++i) mbctype[i] = table[i];
}

