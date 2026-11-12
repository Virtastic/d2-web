// 1.13c D2Client+0xCD5B0 -- unwind thunk: tail-jumps to the shared frame handler with EAX = its descriptor (0xD7AAC).
#include "cl_tbl.h"
extern "C" uint32_t d2c_cd5b0(const cl::Env& e) { return e.H(0xcae73, 0, 0, 0, 0, 0, 0x6FB87AACu); }
