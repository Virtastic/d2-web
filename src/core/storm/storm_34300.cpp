// 1.13c Storm+0x34300: the SEVT window procedure. Locals handled/result start 0; 0x341D0 routes the message; when it returns nonzero and set
// handled, the result is returned, else the message goes to DefWindowProcA. The callees are seams so FUNCTION mode can record them.
#include <cstdint>

extern "C" uint32_t Storm_Sub34300Env(uint32_t hwnd, uint32_t msg, uint32_t wp, uint32_t lp,
                                      int32_t (*f_341d0)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t*, int32_t*),
                                      uint32_t (*f_defwndproc)(uint32_t, uint32_t, uint32_t, uint32_t)) {
    int32_t handled = 0, result = 0;
    int32_t ok = f_341d0(hwnd, msg, wp, lp, &handled, &result);
    if (ok != 0 && handled != 0) return (uint32_t)result;
    return f_defwndproc(hwnd, msg, wp, lp);
}
