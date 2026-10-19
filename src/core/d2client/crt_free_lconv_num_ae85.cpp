// 1.13c D2Client+0x0AE85: ___free_lconv_num, the VS 2003 CRT helper for the three numeric
// lconv strings. Prior art: no D2Client implementation exists in D2MOO.
// describes the linked CRT context, while the local 1.13c decompile establishes this row's behavior.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

struct D2ClientFreeLconvNumEnv {
    const uint32_t* default_numeric; // through PTR_PTR_6fb8ba0c, RVA 0x0DBA0C, fields +0/+4/+8
    const uint32_t* static_numeric;  // PTR_DAT_6fb8b9dc/e0/e4, RVAs 0x0DB9DC/0x0DB9E0/0x0DB9E4
    D2cHost host;                    // _free at D2Client+0x04891
};

extern "C" void d2_client_4891(uint32_t p, D2cHost host);

// 1.13c D2Client+0x0AE85. [esp+4] is the three-dword numeric lconv table. For each field in order,
// release its string unless it aliases either the corresponding default-locale field or the matching
// static CRT string. Retail does not special-case a NULL field; if NULL matches neither sentinel, it
// calls _free(NULL), which the recovered _free handles as a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_ae85(uint32_t numeric, const D2ClientFreeLconvNumEnv* env) {
    if (numeric == 0) return;
    const uint32_t* fields = (const uint32_t*)(uintptr_t)numeric;
    for (uint32_t i = 0; i < 3; ++i) {
        const uint32_t p = fields[i];
        if (p != env->default_numeric[i] && p != env->static_numeric[i])
            d2_client_4891(p, env->host);
    }
}
