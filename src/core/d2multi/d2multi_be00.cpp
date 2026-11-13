// superseded port of D2Multi 0xBE00: UI teardown. gv: +0 [39dc0], +4 [39160], +8 [39f20], +12 [39f88], +16 [3a03c]; gv is read and written back.
#include "d2multi_env.h"

extern "C" void d2multi_be00(const D2mEnv& e, uint32_t* gv) {
    d2m_call(e, 0x11a20);
    d2m_call(e, 0x11da0);
    d2m_call(e, 0xb900);
    d2m_call(e, 0x8790);
    d2m_call(e, 0x11510);
    gv[4] = 0;
    d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x3a0d8));
    d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x3a0cc));
    if (gv[0] == 1) {
        d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x39dbc));
        d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x39db0));
        d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x39db4));
        d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x39db8));
        gv[0] = 0;
        d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, gv[1]);
    }
    if (gv[2] == 3) gv[2] = 0;
    if (gv[3] != 0) d2m_call(e, 0x7c20, 0, gv[3], 1);
}
