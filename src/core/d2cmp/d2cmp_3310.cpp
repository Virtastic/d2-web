// 1.13c D2CMP+0x3310 (_write_string, VS2003 CRT): instruction-identical to D2Common+0x394E. Delegates to that port.
#include "core/d2common/output_3985.h"

// 1.13c D2CMP+0x3310
extern "C" void d2cmp_3310(D2CommonStringStream2814* s, const D2CommonOutputCalls3985* calls, int* count,
                           const char* str, int n) {
    D2Common_394E_WriteString(s, calls, count, str, n);
}
