// src/core/d2client/d2client_batch_tiny_1019i.cpp -- five UNMAPPED 1.13c D2Client.dll rows
// reimplemented from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane,
// OBJECTIVE v8.3: implementation + marker, no spec, no vector). Base 0x6FAB0000; every RVA below is
// retail's, and every global this file owns is a static named at its retail RVA.
//
// Rows: ___initmbctable 0xB627, __fptrap 0xBB2B, sub_14940, sub_2B360, sub_2E330. Callees already
// recovered elsewhere are called directly: __amsg_exit 0x46DA (d2client_batch_04860.cpp). The
// remaining callees are import thunks (D2CMP!#10065, Fog!#10043, D2Common!#10237) and the unrecovered
// CRT __setmbcp 0xB4D7, so they are host seams AFTER the retail arguments, which keep their order
// (the src/core/d2client/d2client_batch_1e70.cpp pattern).
//
// Three of the five take their argument in a register, not on the stack (retail __usercall): 0x14940
// reads AL and ECX, 0x2B360 reads ESI, 0x2E330 reads ESI. The signatures below name those registers.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientBatch04860Env;    // d2client_batch_04860.cpp
extern "C" void d2_client_46da(int32_t code, const D2ClientBatch04860Env* env);   // 0x046DA

static uint8_t g_mbctableInit;   // 0x124AD8 DAT_6fbd4ad8: ___initmbctable's one-shot latch
static uint8_t g_byte_11c390;    // 0x11C390 DAT_6fbcc390: the byte 0x2E330 passes to D2Common!#10237

struct D2ClientBatch1019iEnv {
    // 0xB4D7 __setmbcp: retail sets the multibyte code-page tables; the observable here is the latch.
    int32_t (*setmbcp)(int32_t codepage);
    const D2ClientBatch04860Env* crt;   // 0x46DA's env
    // 0x143E0: the packet throttle that sends len bytes from buf to D2Net (that row is STALLED).
    void    (*send_packet)(const void* buf, uint32_t len);
    // Import thunks: D2CMP.dll!#10065 (0xC18A) and Fog.dll!#10043 (0xBF86).
    void    (*close_cel_file)();
    void    (*fog_mem_dealloc)(uint32_t mem, uint32_t file, uint32_t line, uint32_t zero);
    // Import thunk: D2Common.dll!#10237 (0xC26E), a lookup returning one of D2Common's records.
    const void* (*d2common_10237)(uint32_t index);
};

// 1.13c D2Client+0x0B627 (___initmbctable): one-shot; on the first call set the multibyte code page
// with __setmbcp(-3) and latch. Always returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_b627(const D2ClientBatch1019iEnv* env) {
    if (g_mbctableInit == 0) {
        env->setmbcp(-3);
        g_mbctableInit = 1;
    }
    return 0;
}

// 1.13c D2Client+0x0BB2B (__fptrap): a floating-point exception trap -- __amsg_exit(2). Never returns
// in retail (the exit leg is the host seam inside 0x46DA).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_bb2b(const D2ClientBatch1019iEnv* env) {
    d2_client_46da(2, env->crt);
}

// 1.13c D2Client+0x14940: build the 5-byte packet {AL, ECX little-endian at +1} in a stack buffer and
// hand it to the 0x143E0 throttle with the length 5 it loads into EBX. AL is the packet opcode and
// ECX the payload.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_14940(uint8_t al, uint32_t ecx, const D2ClientBatch1019iEnv* env) {
    uint8_t packet[5];
    packet[0] = al;
    std::memcpy(packet + 1, &ecx, 4);
    env->send_packet(packet, 5);
}

// 1.13c D2Client+0x2B360: close the current cel file (D2CMP!#10065), then -- only when ESI, the
// caller's buffer, is non-null -- release it through Fog!#10043 with the file string at 0x0DB0B0 and
// line 0x7A.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_2b360(uint32_t esi, const D2ClientBatch1019iEnv* env) {
    env->close_cel_file();
    if (esi != 0) env->fog_mem_dealloc(esi, 0x0DB0B0u, 0x7Au, 0);
}

// 1.13c D2Client+0x2E330: look up the record D2Common!#10237 returns for the byte at 0x11C390 and
// divide ESI (the caller's frame delta) by its signed dword at +0x1C -- returning ESI unchanged when
// that divisor is not positive. Signed 32-bit division, exactly the retail idiv.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_2e330(uint32_t esi, const D2ClientBatch1019iEnv* env) {
    const uint8_t* rec = (const uint8_t*)env->d2common_10237(g_byte_11c390);
    int32_t divisor;
    std::memcpy(&divisor, rec + 0x1C, 4);
    if (divisor <= 0) return (int32_t)esi;
    return (int32_t)esi / divisor;
}
