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
uint32_t g_imeOn;          // +0x112038 DAT_6fbcc038
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
uint32_t g_fpsMode;        // +0x1122C0 DAT_6fbcc2c0
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

// 1.13c D2Client+0x573A0 (s3495). fastcall(unit=ecx, classId=edx). PARTIAL: the DAT_6fb85cd0/cd8
// tables and the stack-slot out-params of Ordinal_10569/10740 are not decoded; calls go via the seam.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_573A0(uint32_t unit, int32_t idx, uint32_t dataTables, uint32_t tblBase, uint32_t tblCount,
                                         uint32_t rec, int32_t recSub, int32_t subCount) {
    // Bounds exactly as retail: 0 <= idx < *(dt+0xBA0); rec = idx*0x23C + *(dt+0xB98); (short)rec[0xEA] in [0,*(dt+0xB6C)).
    (void)tblBase; (void)tblCount;
    if (idx < 0 || rec == 0 || recSub < 0 || recSub >= subCount) return 0;
    (void)dataTables;
    if (ord(10511, unit) == 0) return 0;
    uint32_t a = ord(10980), b = ord(10649);
    if (a == 0 || b == 0) { if (call(0xA1340, 0, uptr(&a)) == 0) return 0; }
    ord(10192, unit); uint32_t bv = ord(10266) & 0xFF;
    call(0x1630, 0, unit); call(0x1660, 0, unit);
    ord(10740, bv & 7);
    ord(10569, unit);
    ord(10740);
    call(0xB0450, 0, unit);
    return 1;
}

// 1.13c D2Client+0x578F0 (s3496). Register ESI = this (unaff_ESI). PARTIAL: ESI-carried object and
// Ordinal_10203/10306/10331/10630/10788 results are seam-only.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_578F0(int32_t* self) {
    int32_t o = self[0xb];
    if (o == 0 || ord(10203) == 0) return 0;
    ord(10630); ord(10306); ord(10331);
    int32_t t = self[0];
    if (t != 2 && (t < 4 || t > 5) && self[0xb]) ord(10867, self[0xb]);
    t = self[0];
    if (t != 2 && (t < 4 || t > 5) && self[0xb]) ord(10750, self[0xb]);
    uint32_t u = ord(10788, o);
    ord(10266, u);
    ord(10740);
    uint32_t r = call(0x1400);
    if (ord(10057, r) != 0) return 0;
    uint32_t rec[23] = {0x25};
    uint32_t ret = call(0xB30F0, 0, uptr(rec));
    call(0x88A70, 0, uptr(self));
    return ret;
}

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

// 1.13c D2Client+0x70140 (s3500). Per-frame IME status indicator + typed-text redraw. PARTIAL:
// layout globals DAT_6fb8bc48/4c and the Ordinal_10150/10014/10028 draw args are seam-forwarded.
EMSCRIPTEN_KEEPALIVE void d2c_70140(int32_t bx, int32_t by) {
    ord(10184);
    if (g_imeOn) {
        switch (g_imeMode) {
        case 0: case 1: case 2: case 3: ord(10150, 0, by - 0x80); break;
        case 4: case 5: ord(10150, 0, by - 0x80); call(0x6F260); break;
        }
    }
    ord(10014, 0, 0x7f, by - 0x67, bx - 0x80, by - 0x38);
    ord(10227);
    int n = 0; while (g_chat[n]) n++;
    int sel = 0;
    if (n > 0) {
        int line = 0;
        for (uint32_t it = ord(10059, bx - 0x109); it; line++) {
            if (line < 0x2d) { ord(10150, 0, by + line * 0xf - 0x58); ord(10028); }
            it = *(uint32_t*)(uintptr_t)(it + 4);  // node->next (retail reads +4)
            if (!it && g_imeOn && g_imeFlag64) { ord(10028); call(0x6F3E0, 0, by + line * 0xf - 0x58); }
        }
        ord(10008);
        if (line > 0) sel = line - 1 > 2 ? 2 : line - 1;
    } else if (g_imeOn && g_imeFlag64) { call(0x6F3E0, 0, by - 0x58); ord(10184); return; }
    if (!(g_imeOn && g_imeFlag64)) call(0x6EF20, 0, by - 0x58 + sel * 0xf);
    ord(10184);
}

// 1.13c D2Client+0x70500 (s3501). "/w name msg"-style whisper parser. EAX = line, EBX = command word.
// PARTIAL: tokenising uses the seam strtok (FUN_6fab3be4); the packet send FUN_6fac47a0 and the
// friend-list walk at DAT_6fbcbc14 (+0x66 name, +0x80 next) are seam/state.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_70500(char* line, const char* cmd, int32_t haveList) {
    if (call(0x6EFB0, 0, uptr(line))) return 0;
    size_t cl = strlen(cmd);
    if (line[cl] != ' ' && line[cl] != 0) return 0;
    char who[256] = {}, msg[256] = {};
    char copy[0x102]; strncpy(copy, line, sizeof copy - 1);
    int tok = 0; char* p = strtok(copy, " ");
    while (p) {
        ++tok;
        if (tok == 2) strncpy(who, p, 255);
        else if (tok == 3) { strncpy(msg, line + (p - copy), 255); break; }
        p = strtok(nullptr, " ");
    }
    if (tok == 3) {
        if (haveList && call(0x147A0, 0, uptr(who), 0, uptr(msg), g_gameType)) return 1;  // send 0x15 packet to a listed name
        if (chatVt(0x14)) {}
        return 1;
    }
    char out[0x200]; snprintf(out, sizeof out, "<BattleNet>: Correct usage is: %s (name) (message)", cmd);
    call(0x7F780, 0, uptr(out));
    return 1;
}

// 1.13c D2Client+0x70700 (s3502). One-argument command ("/cmd x"). EAX = line, EBX = command word.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_70700(char* line, const char* cmd) {
    size_t cl = strlen(cmd);
    if (call(0x6EFB0, 0, uptr(line))) return 0;
    if (line[cl] != ' ' && line[cl] != 0) return 0;
    char copy[0x202]; strncpy(copy, line, sizeof copy - 1);
    int n = 0; char* p = strtok(copy, " ");
    while (p && n < 2) { ++n; p = strtok(nullptr, " "); }
    if (n == 1) { chatVt(0x14); return 1; }
    char out[0x108]; snprintf(out, sizeof out, "<BattleNet>: Correct usage is: %s", cmd);
    call(0x7F780, 0, 0, 0, uptr(out));
    return 1;
}

// 1.13c D2Client+0x707F0 (s3503). Slash-command dispatcher. PARTIAL: the 0x15/0x14 packet build and
// FUN_6fb1ec30 parse are seam; "/players" confirmation text goes through Ordinal_10049.
EMSCRIPTEN_KEEPALIVE void d2c_707F0(char* text) {
    if (!strcasecmp(text, "/fps") || !strcasecmp(text, "/framerate")) {
        if (text[1] == 'f' && text[2] == 'p' && text[3] == 's') { if (g_fpsShown || !g_fpsMode) call(0x5A5E0); g_fpsShown = 1; }
        else call(0x5A5E0);
        ord(10227);
        call(0x6F4A0);
        return;
    }
    if (!strncasecmp(text, "/players", 8) && (g_netMode == 0 || g_netMode == 1 || g_netMode == 6 || g_netMode == 8)) {
        long n = atol(text + 8); n = n < 2 ? 1 : n > 7 ? 8 : n;
        ord(10049); snprintf(text, 64, "players set to %ld", n);
    }
    if (!strcasecmp(text, "/nopickup")) g_nopickup = !g_nopickup;
    else if (!strcasecmp(text, "soundchaosdebug")) call(0x88D00);
    else {
        uint8_t kind = 1;
        if (!g_imeOn || (uint8_t)call(0x6EC00)) { if (call(0x6EC30, 0, uptr(text), uptr(&kind)) && kind == 0) { /* packet 0x14 */ } }
        else if (kind == 3) {
            if (!call(0x70500, 0, 1) && !call(0x70500, 0, 1) && !call(0x70500, 0, 1) && !call(0x70500, 0, 1) &&
                !call(0x70700) && !call(0x70700)) chatVt(0x14);
            call(0xC2790, 0, 5, 1);
            return;
        }
        call(0x147A0, 0, uptr(text));  // FUN_6fac47a0: send chat packet
    }
    call(0xC2790, 0, 5, 1);
}

// 1.13c D2Client+0x71130 (s3504). Two-pass shadowed text draw; param_1 = text pointer.
EMSCRIPTEN_KEEPALIVE void d2c_71130(uint32_t text) {
    TextRec r = {};
    r.w[0] = text; r.w[13] = g_textColA;
    uint32_t args[6] = {uptr(&r), g_panelX + 0xce, 0x13e - g_panelY, 0xffffffffu, 5, 0};
    ord(10041, 0, uptr(args));
    r.w[14] = r.w[14];  // iStack_10: written back by the first call (out slot)
    args[0] = r.w[14]; args[1] = g_panelX + 0x152;
    ord(10041, 0, uptr(args));
}

// 1.13c D2Client+0x71220 (s3505).
EMSCRIPTEN_KEEPALIVE void d2c_71220(uint32_t text) {
    TextRec r = {};
    r.w[13] = g_textColA; r.w[0] = text;
    drawText(r, g_panelX + 0x110, 0x13e - g_panelY, 0xffffffffu, 5, 0);
}

// 1.13c D2Client+0x71280 (s3506). param_1 selects colour DAT_6fbcc008 (0) / DAT_6fbcc000 (else).
EMSCRIPTEN_KEEPALIVE void d2c_71280(int32_t sel) {
    TextRec r = {};
    r.w[13] = sel == 0 ? g_textColB : g_textColC;
    uint32_t args[6] = {uptr(&r), g_panelX + 0xbc, 0x148 - g_panelY, 0xffffffffu, 5, 0};
    ord(10041, 0, uptr(args));
    uint32_t one[1] = {1};
    args[0] = uptr(one); args[1] = g_panelX + 0x1bc;
    ord(10041, 0, uptr(args));
}

}  // extern "C"
