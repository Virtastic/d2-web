// 1.13c D2Client+0x49000 -- NPC menu override: in an expansion game the record gets three entries, the third being
// the fixed handler with string 0x58D2. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_49000(const cl::Env& e) {
    if (!pa4::G(e, 0x119854)) return 1;
    return pa4::menuRec(e, 0xf3ed7, 3, 0x6FAF8810u, 0x58d2);
}
