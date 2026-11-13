// Seams shared by the edit-box ports D2Client 0x37F80 (key handler) and 0x38470 (draw): the box is one packed struct
// (dword fields at unaligned offsets); every callee goes through cl::Env::H (the call log). The two CRT pieces the key handler
// calls unhooked (0x3FD3 isdigit, 0x3146 sprintf with the "%d" at 0x6FB8057C) are separate proven rows and ride in this seam.
#pragma once
#include <cstdint>
#include "cl_env.h"
struct D2cEditSeam {
    void* ctx;
    int32_t (*isdigit)(void* ctx, int32_t c);
    int32_t (*sprintf_d)(void* ctx, char* dst, uint32_t v);
};
extern "C" uint32_t d2c_37f80(uint32_t self, uint32_t msg, const cl::Env& e, const D2cEditSeam& s);
extern "C" void d2c_38470(uint32_t self, const cl::Env& e);
