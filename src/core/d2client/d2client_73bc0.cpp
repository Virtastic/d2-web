// 1.13c D2Client+0x73BC0 -- missile unit: byte +0x178 of its missile row scaled (x256) and divided by EDI, handed to 0xC2FE.
// ESI = unit (type 3), EDI = divisor.
#include <emscripten.h>
#include "d2c_73930_env.h"
using namespace d2c73;

extern "C" EMSCRIPTEN_KEEPALIVE void d2client_73bc0(const D2c73Env* e, uint32_t unit, int32_t div) {
    if (!unit || rd32(e, unit) != 3 || !div) return;
    const int32_t id = (int32_t)rd32(e, unit + 4);
    if (id < 0 || id >= (int32_t)rd32(e, e->tab + 0xb6c)) return;
    const uint32_t rec = (uint32_t)id * 0x1a4u + rd32(e, e->tab + 0xb64);
    const int32_t v = (int32_t)(rd8(e, rec + 0x178) << 8);
    C(e, 0xc2fe, 0, 0, 0, 0, 0, 0, unit, (uint32_t)(v / div));
}
