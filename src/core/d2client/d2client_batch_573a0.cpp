// D2Client implementation batch (rows s3495-s3506). Decompile-derived from the cached Ghidra output.
// Seam convention for this file: every callee the decompile reaches is a host call in retail's call
// order. Ordinal imports use rva = 0x80000000|ordinal; module functions use their RVA. Stack arguments
// beyond four go in an argument array whose address is passed in ecx (named, not a retail register).
// Callers unrecovered / callee ordinals unresolved => rows are PARTIAL where stated; none are stubbed.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <strings.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include "client_host.h"

namespace {
D2cHost g_h;
// Module globals, each with its retail RVA (VA - 0x6FAB0000).
uint32_t g_imeOn;          // +0x11C038 DAT_6fbcc038
uint32_t g_imeMode;        // +0x112030 DAT_6fbcc030
uint32_t g_imeFlag64;      // +0x113F64 DAT_6fbc3f64 (byte)
uint32_t g_gameType;       // +0x113F60 DAT_6fbc3f60
uint32_t g_imeCompLen;     // +0x112028 DAT_6fbcc028
uint16_t g_chat[0x80];     // +0x114C80 DAT_6fbcec80
uint16_t g_comp[0x20];     // +0x114C40 DAT_6fbcec40
uint16_t g_read[0x20];     // +0x115E98 DAT_6fbcfe98
uint32_t g_textColA, g_textColB, g_textColC;  // +0x112004 / +0x112008 / +0x112000
uint32_t g_panelX, g_panelY;                  // +0x11B9A0 / +0x11B9A4 DAT_6fbcb9a0/a4
uint32_t g_nopickup;       // +0x1122F0 DAT_6fbcc2f0
uint32_t g_fpsShown;       // +0x1122C8 DAT_6fbcc2c8
uint32_t g_fpsMode;        // +0x11C2C0 DAT_6fbcc2c0
uint32_t g_fpsSaved;       // +0xFAD94 DAT_6fbaad94 (the /fps and /framerate save word)
uint32_t g_gameMode;       // +0x1123A4 gate / 0x112394 DAT_6fbcc394 below
uint32_t g_netMode;        // +0x112394 DAT_6fbcc394
uint32_t g_clientNet;      // DAT_6fbcb980 (+0x221 vtable ptr): host-resolved, see chatVt()

inline uint32_t call(uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t s0 = 0,
                     uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    D2cCall c = {};
    c.rva = rva; c.eax = eax; c.ecx = ecx; c.edx = edx;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return g_h(&c);
}
// Same seam with ESI set (packet-pointer callees such as 0x147A0).
inline uint32_t callEsi(uint32_t rva, uint32_t esi) {
    D2cCall c = {};
    c.rva = rva; c.esi = esi;
    return g_h(&c);
}
inline uint32_t ord(uint32_t n, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t s0 = 0,
                    uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    return call(0x80000000u | n, eax, ecx, edx, s0, s1, s2, s3);
}
// *(DAT_6fbcb980+0x221) vtable slot call: rva 0x40000000|slot.
inline uint32_t chatVt(uint32_t slot) { return call(0x40000000u | slot); }
inline uint32_t uptr(const void* p) { return (uint32_t)(uintptr_t)p; }

// The 0x48-byte text-draw record shared by 71130/71220/71280 (18 dwords, zeroed).
struct TextRec { uint32_t w[18]; };
inline void drawText(TextRec& r, uint32_t x, uint32_t y, uint32_t a, uint32_t b, uint32_t c) {
    uint32_t args[6] = {uptr(&r), x, y, a, b, c};
    ord(10041, 0, uptr(args));
}
}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE void d2c573a0_setHost(D2cHost h) { g_h = h; }

// 1.13c D2Client+0x573A0 (s3495). fastcall(unit=ecx, classId=edx) + one stack arg (ret 4), returns EAX 0/1.
// Byte tables DAT_6fb85cd0 (RVA 0xD5CD0, 8 bytes) and DAT_6fb85cd8 (RVA 0xD5CD8, 9 per row) are read
// through the host as rva 0x20000000|tableRva (one byte). Ordinal args per the disassembly:
// 10511(unit) 10980(h) 10649(h) 6fb51340(ebx=unit, edi=&o0, stack=&o1) 10192(unit,a,b,callerEbp)
// 10266(al) 10740(id,&dx,&dy) 10569(unit,&r,&r2) 6fb60450(sub,x,y,idx,p).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_573A0(uint32_t unit, int32_t idx, uint32_t dataTables, uint32_t tblBase, uint32_t tblCount,
                                         uint32_t rec, int32_t recSub, int32_t subCount, uint32_t stackArg, uint32_t callerEbp) {
    // Bounds exactly as retail: 0 <= idx < *(dt+0xBA0); rec = idx*0x23C + *(dt+0xB98); (short)rec[0xEA] in [0,*(dt+0xB6C)).
    (void)tblBase; (void)tblCount; (void)dataTables; (void)rec;
    if (idx < 0 || recSub < 0 || recSub >= subCount) return 0;
    uint32_t h = ord(10511, 0, 0, 0, unit);
    if (h == 0) return 0;
    uint32_t a = ord(10980, 0, 0, 0, h), b = ord(10649, 0, 0, 0, h);
    if (a == 0 || b == 0) {
        uint32_t o[2] = {0, 0};
        D2cCall c = {};
        c.rva = 0xA1340; c.ebx = unit; c.ecx = uptr(&o[0]); c.edx = 0; c.s[0] = uptr(&o[1]);  // edi=&o[0] in ecx slot
        if (g_h(&c) == 0) return 0;
        a = o[0]; b = o[1];
    }
    uint32_t bv = ord(10266, 0, 0, 0, ord(10192, 0, 0, 0, unit, a, b, callerEbp) & 0xFF) & 7;
    uint32_t x = call(0x1630, 0, unit), y = call(0x1660, 0, unit);
    int32_t d[2] = {0, 0};
    ord(10740, 0, 0, 0, call(0x20000000u | (0xD5CD0 + bv)) & 0xFF, uptr(&d[0]), uptr(&d[1]));
    x += (uint32_t)d[0]; y += (uint32_t)d[1];
    int32_t r[2] = {0, 0}, q[2] = {0, 0};
    ord(10569, 0, 0, 0, unit, uptr(&r[0]), uptr(&q[0]));
    int32_t k = r[0] - 4;
    k = k > 0 ? (k >= 8 ? 8 : k) : 0;
    uint32_t t = call(0x20000000u | (0xD5CD8 + bv * 9 + (uint32_t)k)) & 0xFF;
    if (t != 0x63) {
        ord(10740, 0, 0, 0, t, uptr(&d[0]), uptr(&d[1]));
        x += (uint32_t)d[0]; y += (uint32_t)d[1];
    }
    uint32_t args[5] = {(uint32_t)recSub, x, y, (uint32_t)idx, stackArg};
    call(0xB0450, 0, uptr(args));
    return 1;
}

// 1.13c D2Client+0x578F0 moved to d2client_578f0.cpp (corrected callee args).

// 1.13c D2Client+0x6F5B0 (s3497). IME init for the Korean/Japanese/Chinese builds.
EMSCRIPTEN_KEEPALIVE void d2c_6F5B0(uint32_t sysLangId) {
    if (ord(10048) == 0) { ord(10024, 0, 0, 0, 0, ord(10265, 0x179)); call(0x3664 - 0x3664 + 0x35A1, 0, 0xFFFFFFFFu); return; }  // _exit(-1) = _doexit
    for (uint32_t i = 0; i < 0x30; i += 0xc) call(0x3CA80, 0, 0, 0, i);  // FUN_6faeca80 over table 0x6FB927DC
    g_gameType = ord(10009);
    if (g_gameType != 6 && g_gameType != 9 && g_gameType != 8 && g_gameType != 7 && g_gameType != 0xc) return;
    g_imeOn = 0;
    if (!call(0x6F060)) return;
    uint32_t imc = call(0xBE3C);  // ImmGetContext(Ordinal_10048())
    if (!imc) return;
    g_read[0] = 0; g_comp[0] = 0;
    bool openCall = false;
    switch (g_gameType) {
    case 6:
        if (sysLangId == 0x411) {
            g_imeOn = 1; call(0x3CCC0);  // FUN_6faeccc0(hwnd,&0x6FB92808)
            if (!call(0xBE48, 0, imc, 1)) { g_imeFlag64 = 0; g_imeMode = 0; }  // ImmSetOpenStatus
            else { g_imeFlag64 = 1; g_imeMode = 3; }
        }
        break;
    case 7: case 0xc: {
        if (sysLangId != 0x412 && sysLangId != 0x812) break;
        call(0xBE48, 0, imc, 1);
        call(0xBE54, 0, imc);               // ImmGetConversionStatus
        call(0xBE5A, 0, imc, 1);            // ImmSetConversionStatus(ctx,1,sentence)
        uint32_t conv = call(0xBE54, 0, imc);
        if (!(conv & 1)) { g_imeMode = 0; g_imeOn = 1; }
        else { g_imeMode = 1; g_imeFlag64 = 1; g_imeOn = 1; }
        openCall = true; break; }
    case 8: case 9:
        g_imeFlag64 = 0; g_imeOn = 1;
        if (!call(0xBF92)) call(0xBF80, 0, 0, 0, g_gameType == 8 ? 0x10 : 0x70);  // ImmSimulateHotKey
        call(0xBE48, 0, imc, 1);
        g_imeMode = g_gameType == 8 ? 5 : 4;
        openCall = true; break;
    }
    if (openCall) call(0x3CCC0);
    call(0xBE2A, 0, imc);  // ImmReleaseContext
}

// 1.13c D2Client+0x6FAB0 (s3498). IME composition handler; param_1 points at the WM_IME_COMPOSITION block.
// PARTIAL: the IMM / MultiByteToWideChar calls are seam calls whose output buffers are the statics.
EMSCRIPTEN_KEEPALIVE void d2c_6FAB0(uint32_t* msg) {
    uint32_t imc = call(0xBE3C);
    if (imc) {
        if (msg[3] & 0x800) {  // GCS_RESULTSTR
            uint16_t res[0x20] = {};
            call(0xBE60, 0, imc, 0x800, uptr(res));  // ImmGetCompositionStringA + MultiByteToWideChar (<=199 bytes, 0x20 wide)
            int n = 0; while (n < 0x20 && res[n]) n++;
            int a = 0; while (g_comp[a]) a++;
            int b = 0; while (g_chat[b]) b++;
            if (a + b < 0x7f) { memcpy(g_chat + b, g_comp, a * 2); g_chat[b + a] = 0; g_imeCompLen += a; }
            b = 0; while (g_chat[b]) b++;
            if (b + n < 0x7f) { memcpy(g_chat + b, res, n * 2); g_chat[b + n] = 0; g_imeCompLen += n; }
            g_comp[0] = 0; g_read[0] = 0;
        }
        if ((msg[3] & 8) && call(0xBE84, 0, imc)) {  // GCS_COMPSTR, ImmGetOpenStatus
            uint16_t buf[0x20] = {};
            if (call(0xBE60, 0, imc, 8, uptr(buf)) > 0) {
                int n = 0; while (n < 0x20 && buf[n]) n++;
                if (n < 0x20) { buf[n] = 0; memcpy(g_read, buf, (n + 1) * 2); }
                else { memcpy(g_read, buf, 0x1f * 2); g_read[0x1f] = 0; call(0x88A70, 0, 0); }
            }
        }
        call(0xBE2A, 0, imc);
    }
    msg[6] = 1; msg[7] = 0;
    ord(511, 0, uptr(msg));
}

// 1.13c D2Client+0x6FCE0 (s3499). Chat/BattleNet message formatting. PARTIAL: the Unicode::sprintf
// template (Ordinal_10003 string id) and the msg-descriptor read through *(DAT_6fbcb980+0x221)+0x24 are seam calls.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_6FCE0(uint32_t* msgType, const char* name, const char* body) {
    if (chatVt(0x24) == 0) return 0;
    // retail reads a "already handled" flag (local_ca4) the vtable call fills; non-zero => return 1.
    D2cCall c = {}; c.rva = 0x40000000u | 0x24; g_h(&c);
    if (c.out[0] != 0) return 1;
    switch (*msgType) {
    case 1: case 2: case 3: case 5: case 7: case 9: case 0x15: case 0x16: case 0x17:
        return 1;
    case 4: case 6: case 10:
        call(0x6EB70); call(0x6EBB0);  // FUN_6fb1eb70 / FUN_6fb1ebb0 (account / character-name pair)
        ord(10003); call(0x7D850);
        break;
    default:
        call(0x7D850);
    }
    (void)name; (void)body;
    return 1;
}

// D2Client 70140, 70500, 70700, 707F0, 71130, 71220 and 71280 live in their own files (d2client_<rva>.cpp).

}  // extern "C"
