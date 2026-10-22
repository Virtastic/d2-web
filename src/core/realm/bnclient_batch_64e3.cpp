// src/core/realm/bnclient_batch_64e3.cpp -- 1.13c BNClient.dll row 0x64E3 (alpha lane, no spec/vector), from
// build-wasm/ghidra-bnclient/decomp/0x64E3.c. MSVC CRT _setmbcp: selects the multibyte code page, filling the
// 257-byte ctype table (0x3F980) and page state from the built-in 5-entry table (0x3D8E8, 0x30 B each).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct BnSetMbcpState {  // DAT_3FA84, 3F960, 3F958, 3FA90..98
    uint32_t cp, mb_flag, mb_aux, extra[3];
};
struct BnSetMbcpEnv {
    // builtin[i]: cp(4) | extra[3](12) | ranges 4x8 bytes (lo,hi pairs) ... as at 0x3D8E8+0x30*i; flags4 = 0x3D8E0[0..3]
    const uint8_t* builtin;   // 5 * 0x30 bytes at 0x3D8E8
    const uint8_t* flags4;    // 4 bytes at 0x3D8E0
    // GetCPInfo: 1 ok; maxcharsize out; leadbytes out[12]
    int32_t (*get_cp_info)(uint32_t cp, uint32_t* maxcharsize, uint8_t lead[12]);
    // FUN_62f0 (value -> mb_aux, ECX -> mb_flag when called from the GetCPInfo path)
    uint32_t (*sys_aux)(uint32_t* ecx_out);
    int32_t crt_has_default;  // DAT_3E65C
    void (*set_sbcs)();       // setSBCS
    void (*after)();          // FUN_6348 (bn_setsbuplow caller)
};

// 1.13c BNClient+0x064E3 (404 B, caller 0x6677). Returns 0 or -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_setmbcp(uint32_t cp, uint8_t* ctype /*257*/, BnSetMbcpState* st,
                                        const BnSetMbcpEnv* env) {
    if (cp != 0) {
        for (uint32_t i = 0; i < 5; i++) {
            const uint8_t* e = env->builtin + i * 0x30;
            uint32_t ecp; memcpy(&ecp, e, 4);
            if (ecp != cp) continue;
            memset(ctype, 0, 257);
            for (uint32_t j = 0; j < 4; j++)
                for (const uint8_t* p = e + 0x10 + j * 8; p[0] && p[1]; p += 2)
                    for (uint32_t c = p[0]; c <= p[1]; c++) ctype[c + 1] |= env->flags4[j];
            uint32_t ecx = 0;
            st->cp = cp; st->mb_flag = 1; st->mb_aux = env->sys_aux(&ecx);
            memcpy(st->extra, e + 4, 12);
            env->after();
            return 0;
        }
        uint32_t maxc = 0; uint8_t lead[12] = {0};
        if (env->get_cp_info(cp, &maxc, lead) == 1) {
            memset(ctype, 0, 257);
            st->cp = cp; st->mb_aux = 0;
            if (maxc < 2) st->mb_flag = 0;
            else {
                for (int k = 0; k < 12 && lead[k]; k += 2) {
                    if (!lead[k + 1]) break;
                    for (uint32_t c = lead[k]; c <= lead[k + 1]; c++) ctype[c + 1] |= 4;
                }
                for (uint32_t c = 1; c < 0xff; c++) ctype[c + 1] |= 8;
                uint32_t ecx = 0;
                st->mb_aux = env->sys_aux(&ecx); st->mb_flag = ecx;
            }
            memset(st->extra, 0, 12);
            env->after();
            return 0;
        }
        if (env->crt_has_default == 0) return -1;
    }
    env->set_sbcs();
    env->after();
    return 0;
}
