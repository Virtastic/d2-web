// 1.13c D2Multi+0xBC00 (stdcall, ret 4): open the URL from vtable [3917c]+0xa4 in the shell-registered .html handler. The formatted registry keys and the
// handler command line live in stack buffers (invisible to the vector); GetDesktopWindow / SetForegroundWindow / CreateProcessA / CloseHandle are imports.
// gv: +0 [39184], +4 URL present, +8/+12 the two registry-read answers, +16 the CreateProcessA result (+4..+16 are the probe's script, not globals).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_bc00(const D2mEnv& e, uint32_t* gv) {
    const uint32_t url = d2m_call(e, 0x7c08, 0, gv[0]);
    if (url == 0) return 1;
    uint8_t key[0x40] = {0}, cls[0x40] = {0}, cmd[0x40] = {0};
    const uint32_t ksz = (uint32_t)(uintptr_t)key, csz = (uint32_t)(uintptr_t)cls;
    uint32_t size = 0;
    uint32_t ebp = 0;
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, ksz, D2M_MOD(0x17a00), D2M_MOD(0x17a0c));
    if (d2m_call(e, 0x9000, 0, ksz, 0, 0, (uint32_t)(uintptr_t)&size, 0, csz, 0x400) != 0) {
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, ksz, D2M_MOD(0x179e4), D2M_MOD(0x17a0c));
        if (d2m_call(e, 0x9000, 0, ksz, 0, 0, (uint32_t)(uintptr_t)&size, 0, (uint32_t)(uintptr_t)cmd, 0x400) != 0 && gv[4] != 0) ebp = 1;
    }
    if (ebp == 0) {
        const uint32_t text = d2m_call(e, 0x7ea8, 0, 0x14dc);
        d2m_call(e, 0xacb0, 0, 0, 0, 0, 0, text);
    }
    d2m_call(e, 0x7bf6, 0, gv[0], ebp == 0);
    return 1;
}
