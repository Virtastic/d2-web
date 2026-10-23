// D2Client+0xAEB10: unit event dispatch (ECX=this unit, EAX=event, stack=param_1; ESI passed through); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cAEB10Env {
    void (*f32FE0)(void* unit);   // D2Client+0x82FE0
    void (*f2A6C0)();             // D2Client+0x2A6C0 (ESI)
    void (*fA6210)();             // D2Client+0xA6210 (ESI)
    void (*fAE3F0)(void* unit);   // D2Client+0xAE3F0 (EAX=unit)
    void (*fAC970)();             // D2Client+0xAC970 (EBX)
    void* curUnit;                // global DAT 0xCBBFC (6fbcbbfc)
};
extern "C" {
// 1.13c D2Client+0xAEB10
EMSCRIPTEN_KEEPALIVE void d2c_aeb10(const D2cAEB10Env* e, void* self, int32_t ev, int32_t p1) {
    if (!self) return;
    switch (ev) {
    case 0: case 2: e->f32FE0(self); return;
    case 6:
        if (p1 != 0 && *(int32_t*)((char*)self + 0x10) == 0x11) { e->f2A6C0(); e->fA6210(); }
        return;
    case 0xc:
        e->fAE3F0(self);
        if (self == e->curUnit) { e->fAC970(); e->f32FE0(self); }
    }
}
}
