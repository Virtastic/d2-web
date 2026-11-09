// D2Client+0xAEB10: unit event dispatch (ECX=this unit, EAX=event, stack=param_1; ESI passed through); env-seam style.
#include <cstdint>
#include <emscripten.h>
#include "unit_event_aeb10.h"
extern "C" {
// 1.13c D2Client+0xAEB10
EMSCRIPTEN_KEEPALIVE void d2c_aeb10(const D2cAEB10Env* e, void* self, int32_t ev, int32_t p1) {
    if (!self) return;
    switch (ev) {
    case 0: case 2: e->f32FE0(self); return;
    case 6:
        if (p1 != 0 && *(int32_t*)((char*)self + 0x10) == 0x11) { e->f2A6C0(self); e->fA6210(self); }
        return;
    case 0xc:
        e->fAE3F0(e->curUnit);
        if (self == e->curUnit) { e->fAC970(self); e->f32FE0(self); }
    }
}
}
