// 1.13c D2Client+0x2D810 (197 B, ret 4, EDI = dst; stack v): ids fetched in order A=str(0x5300), B=str(0x5304), C=str(0x5305), D=str(0xF9B);
// local = "%d" of v widened. strcat dst B, local, D, A, C, str(0xF9E); returns 1.
#include "d2client_ustr_env.h"
#include "d2client_wfmt.h"
extern "C" uint32_t d2c_port_2d810(uint16_t* dst, int32_t v, const D2ClientUStrEnv& e) {
    using namespace d2client_wfmt;
    const uint16_t* A = e.str(0x5300); const uint16_t* B = e.str(0x5304); const uint16_t* C = e.str(0x5305); const uint16_t* D = e.str(0xF9B);
    Buf o; putInt(o, v); uint16_t local[32] = {}; widen(o, local);
    e.strcat(dst, B); e.strcat(dst, local); e.strcat(dst, D); e.strcat(dst, A); e.strcat(dst, C);
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
