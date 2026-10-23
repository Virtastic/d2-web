// 1.13c Fog+0x18B50: usercall(eax = log sub-name, [stack] format, ...). A NULL format is fatal (reporter
// type 6 at line 0x13c, log close, _exit(-1)); otherwise the text goes to the named log (default name
// when eax is null). Log-file I/O is host state, so the port writes the formatted text to stderr.
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
void fog_error_report(int32_t type, const char* file, uint32_t line, const char* format, ...);

extern "C" void fog_sub_18b50(const char* logName, const char* format, ...) {  // 1.13c Fog+0x18B50
    if (format == nullptr) {
        fog_error_report(6, "", 0x13c, "Unrecoverable internal error %08x", 0u);
        std::_Exit(-1);
    }
    (void)logName;
    va_list ap;
    va_start(ap, format);
    std::vfprintf(stderr, format, ap);
    va_end(ap);
}
