// 1.13c D2Win+0x18F25 (81 bytes): MSVC CRT __cfltcvt. Dispatch on the format character in the
// third slot ('e'/'E' to 0x18D13, 'f' to 0x18E1F, else to 0x18E86).
#include <cstdint>
#include <emscripten.h>

extern "C" char* d2win_crt_cftoe_18d13(const double* val, char* buf, int32_t ndec, int32_t caps);
extern "C" char* d2win_crt_cftof_18e1f(const double* val, char* buf, int32_t ndec);
extern "C" char* d2win_sub_18E86(const double* dbl, char* buf, uint32_t n, int32_t caps);

// 1.13c D2Win+0x18F25
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_18F25(const double* arg, char* buf, uint32_t fmtChar, int32_t prec, int32_t caps) {
    if (fmtChar == 0x65 || fmtChar == 0x45) { d2win_crt_cftoe_18d13(arg, buf, prec, caps); return; }
    if (fmtChar == 0x66) { d2win_crt_cftof_18e1f(arg, buf, prec); return; }
    d2win_sub_18E86(arg, buf, (uint32_t)prec, caps);
}
