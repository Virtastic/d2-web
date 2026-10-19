#include "d2gfx_cinit.h"
// 1.13c D2Gfx+0x013DE -- __cinit. Module callback tables are supplied explicitly;
// native callback pointers are never serialized as retail's four-byte addresses.
extern "C" int32_t d2gfx_cinit_state(int32_t argument, const D2GfxCinitState* state) {
    if (state->floating_init) state->floating_init(argument);
    int32_t result = 0;
    for (auto initialize : state->c_initializers) {
        if (result != 0) return result;
        if (initialize) result = initialize();
    }
    if (result != 0) return result;
    state->register_exit(0x1ae6u);
    for (auto initialize : state->cpp_initializers)
        if (initialize) initialize();
    return 0;
}
