// 1.13c D2CMP+0x3347 (_output, VS2003 CRT): instruction-identical to D2Common+0x3985 once addresses
// are normalised (1946 bytes); delegates to that port.
#include "core/d2common/output_3985.h"
// 1.13c D2CMP+0x3347
extern "C" int d2cmp_3347(D2CommonStringStream2814* s, const char* fmt, const D2CommonOutputArgs3985* args,
                          const D2CommonOutputCalls3985* calls) {
    return D2Common_3985_Output(s, fmt, args, calls);
}
