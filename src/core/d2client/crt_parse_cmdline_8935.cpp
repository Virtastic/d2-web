// 1.13c D2Client+0x08935: the VC7.1 __parse_cmdline body the linker pulled into the DLL
//. Prior art: the identical MSVC 2003 body already ported for D2MCPClient
// (src/core/realm/mcp_crt_batch_b56.cpp, 0x0200D) and D2DDraw (src/core/gfx/d2ddraw_batch_22ed.cpp,
// 0x22ED). Retail takes cmdstart in EAX, the output area in ECX, the character-count cell in ESI and
// argv/numargs on the stack; this port takes all of them as arguments. The only global read is the
// code-page class table at RVA 0x123740, indexed one byte past its base (the port receives it).
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

namespace {
constexpr uint8_t kLeadByteClass = 0x04; // _M1 in the VC7.1 multibyte class table

bool lead_byte(const uint8_t* mbctable, uint8_t c) {
    return (mbctable[(size_t)c + 1] & kLeadByteClass) != 0;
}
} // namespace

// 1.13c D2Client+0x08935
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_8935(
    char** argv, int32_t* numargs, const char* cmd, char* args, const uint8_t* mbctable) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(cmd);
    uint8_t* out = reinterpret_cast<uint8_t*>(args);
    int32_t chars = 0;
    bool in_quote = false;
    uint8_t c = 0;

    *numargs = 1;
    if (argv != nullptr) *argv++ = args;

    // First item: quotes only toggle, they are not stored; a space or tab outside quotes ends it.
    do {
        if (*p == '"') {
            in_quote = !in_quote;
            ++p;
            c = '"';
        } else {
            ++chars;
            if (out != nullptr) *out++ = *p;
            c = *p++;
            if (lead_byte(mbctable, c)) { // a lead byte carries its trail byte along
                ++chars;
                if (out != nullptr) *out++ = *p;
                ++p;
            }
            if (c == 0) { // the empty command line ends the first item
                --p;
                goto first_item_done;
            }
        }
    } while (in_quote || (c != ' ' && c != '\t'));
    if (out != nullptr) out[-1] = 0;

first_item_done:
    in_quote = false;
    while (*p != 0) {
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == 0) break;
        if (argv != nullptr) *argv++ = reinterpret_cast<char*>(out);
        ++*numargs;
        for (;;) {
            bool copychar = true;
            uint32_t backslashes = 0;
            while (*p == '\\') {
                ++backslashes;
                ++p;
            }
            if (*p == '"') {
                const uint8_t* next = p;
                if ((backslashes & 1) == 0) {
                    // A doubled quote inside a quoted run is one literal quote.
                    if (!in_quote || (next = p + 1, *next != '"')) {
                        copychar = false;
                        next = p;
                    }
                    in_quote = !in_quote;
                }
                backslashes >>= 1;
                p = next;
            }
            for (; backslashes != 0; --backslashes) {
                if (out != nullptr) *out++ = '\\';
                ++chars;
            }
            c = *p;
            if (c == 0 || (!in_quote && (c == ' ' || c == '\t'))) break;
            if (copychar) {
                if (out == nullptr) {
                    if (lead_byte(mbctable, c)) {
                        ++p;
                        ++chars;
                    }
                } else {
                    if (lead_byte(mbctable, c)) {
                        *out++ = c;
                        ++p;
                        ++chars;
                    }
                    *out++ = *p;
                }
                ++chars;
            }
            ++p;
        }
        if (out != nullptr) *out++ = 0;
        ++chars;
    }
    if (argv != nullptr) *argv = nullptr;
    ++*numargs;
    return chars;
}
