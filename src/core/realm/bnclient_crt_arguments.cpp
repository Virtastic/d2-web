// Prior art used: CE_Database's CRT export names; D2MOO has no BNClient CRT implementation.
// Rules checked against cached retail 1.13c body 439E and its 450A caller.
#include "bnclient_crt_arguments.h"
#include <cstdlib>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
static BnCrtArguments arguments;
extern "C" BnCrtArguments* bn_crt_arguments() { return &arguments; }
// GetCommandLineA enters the same state consumed by CRT init.
extern "C" void bn_crt_argument_inputs(const char* command) {
    arguments.command_line = command ? command : "";
}
// 1.13c BNClient+0x0439E -- count pass permits null argv/storage; argc includes the final null slot.
extern "C" EMSCRIPTEN_KEEPALIVE void bn_parse_cmdline(const char* input, char** argv,
    char* storage, int* argc, int* bytes, const uint8_t* mbctype) {
    const auto* p = reinterpret_cast<const uint8_t*>(input);
    bool quoted = false;
    *argc = 1; *bytes = 0;
    auto emit = [&](uint8_t c) { if (storage) *storage++ = char(c); ++*bytes; };
    auto lead = [&](uint8_t c) { return (mbctype[unsigned(c) + 1] & 4) != 0; };
    if (argv) *argv++ = storage;
    uint8_t c;
    do {
        c = *p++;
        if (c == '"') quoted = !quoted;
        else {
            emit(c);
            if (lead(c)) emit(*p++);
            if (!c) { --p; break; }
        }
    } while (quoted || (c != ' ' && c != '\t'));
    if (c && storage) storage[-1] = 0;
    quoted = false;
    while (*p) {
        while (*p == ' ' || *p == '\t') ++p;
        if (!*p) break;
        if (argv) *argv++ = storage;
        ++*argc;
        for (;;) {
            bool copy = true;
            unsigned slashes = 0;
            while (*p == '\\') { ++p; ++slashes; }
            if (*p == '"') {
                if (!(slashes & 1)) {
                    if (quoted && p[1] == '"') ++p;
                    else copy = false;
                    quoted = !quoted;
                }
                slashes >>= 1;
            }
            while (slashes--) emit('\\');
            c = *p;
            if (!c || (!quoted && (c == ' ' || c == '\t'))) break;
            if (copy) {
                if (lead(c)) emit(*p++);
                emit(*p);
            }
            ++p;
        }
        emit(0);
    }
    if (argv) *argv = nullptr;
    ++*argc;
}
// Native caller for the parser: two passes, one contiguous pointer/string allocation.
// TODO: retail 450A calls 67C7 before this; do not map 450A until its locale-init path exists.
extern "C" EMSCRIPTEN_KEEPALIVE int bn_setargv() {
#ifdef _WIN32
    GetModuleFileNameA(nullptr, arguments.program_name, 260);
#else
    const auto n = readlink("/proc/self/exe", arguments.program_name, 259);
    if (n >= 0) arguments.program_name[n] = 0;
#endif
    int count, bytes;
    bn_parse_cmdline(arguments.command_line, nullptr, nullptr, &count, &bytes, bn_mbctype());
    auto** block = static_cast<char**>(std::malloc(size_t(count) * sizeof(char*) + size_t(bytes)));
    if (!block) return -1;
    bn_parse_cmdline(arguments.command_line, block, reinterpret_cast<char*>(block + count),
                     &count, &bytes, bn_mbctype());
    arguments.argc = count - 1;
    arguments.argv = block;
    return 0;
}
