// 1.13c D2Launch+0xD140  sub_D140: stdcall, one ignored stack word (ret 4). D050 with EAX = the module string at 0x25758, then BDA0; returns 1.
#include <cstdint>
#include "d2launch_env.h"
extern "C" uint32_t d2l_d140(const D2lSeams* s) {
    s->d050(0x6FA65758u);
    s->bda0();
    return 1;
}
