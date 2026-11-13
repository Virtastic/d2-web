// Session call-trace buffer (src/core/session/calltrace.h). Empty unless built with -DD2_CALLTRACE.
#ifdef D2_CALLTRACE
#include <emscripten.h>
#include <cstdarg>
#include <cstdio>
#include <string>
#include "calltrace.h"
namespace { std::string g_out; uint32_t g_tick; }
size_t d2ct_enter(const char* dll, uint32_t rva, const char* fmt, ...) {
    const size_t at = g_out.size();
    char b[160]; int n = snprintf(b, sizeof b, "%u %s+0x%X", g_tick, dll, rva);
    g_out.append(b, n);
    if (fmt && *fmt) {
        va_list ap; va_start(ap, fmt); g_out += ' '; vsnprintf(b, sizeof b, fmt, ap); g_out += b; va_end(ap);
    }
    g_out += '\n';
    return at;
}
void d2ct_ret(size_t at, const char* fmt, ...) {
    const size_t nl = g_out.find('\n', at);
    char b[128]; va_list ap; va_start(ap, fmt); vsnprintf(b, sizeof b, fmt, ap); va_end(ap);
    g_out.insert(nl, std::string(" -> ") + b);
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2_ct_reset() { g_out.clear(); }
EMSCRIPTEN_KEEPALIVE void d2_ct_tick(uint32_t t) { g_tick = t; }
EMSCRIPTEN_KEEPALIVE const char* d2_ct_ptr() { return g_out.c_str(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_ct_len() { return (uint32_t)g_out.size(); }
}
#endif
