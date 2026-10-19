#pragma once
#include <cstdint>
struct D2GfxCinitState {
    void (*floating_init)(int32_t);
    int32_t (*c_initializers[5])();
    int32_t (*register_exit)(uint32_t);
    void (*cpp_initializers[3])();
};
extern "C" int32_t d2gfx_cinit_state(int32_t argument, const D2GfxCinitState* state);
