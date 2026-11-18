// 1.13c D2Client+0x64460 -- options item: the value slot mapped to 0..100 and handed to a second renderer setter in ECX.
#include "cl_cmd.h"
namespace d2client_lerp { uint64_t lerp63620(uint32_t a, uint32_t b, uint32_t n, uint32_t m); }
extern "C" uint32_t d2c_64460(uint32_t it, const cl::Env& e) {
    const uint32_t v = (uint32_t)d2client_lerp::lerp63620(0x64, 0, e.R(it + 0x120), e.R(it + 0x124));
    return e.H(0xd288, 0, 0, 0, 0, 0, 0, v);
}
