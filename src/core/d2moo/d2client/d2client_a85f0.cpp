// Mark a light dirty: set its dirty flag and clear its stamp. A null light, or one of kind 2, is fatal.
#include "d2client_light_env.h"
extern "C" void d2c_a85f0(uint32_t obj, const cl::Env& e) {
    if (!obj) { e.H(0xBF92, 0x18D); return; }
    const uint32_t kind = e.R(obj + 0xC);
    e.W(obj + 0x20, 1);
    e.W(obj + 0x1C, 0);
    if (kind == 2) e.H(0xBF92, 0x190);
}
