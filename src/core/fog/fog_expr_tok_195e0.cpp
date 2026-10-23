// Fog+0x195E0 / 0x19680 / 0x197B0 / 0x19CF0 / 0x143D0 -- expression compiler helpers.
// Retail passes several args in registers (ESI/EDI/EAX/EBX); modelled as explicit params.
#include <cstdint>
#include <cstring>
#include <ctype.h>

struct FogExprState {            // 0x148 bytes; EDI/ESI/param_1 in retail
    uint8_t  op[0x40];           // +0x00 operator stack
    int32_t  val[0x40];          // +0x40
    int32_t  top;                // +0x140
    int32_t  depth;              // +0x144
};

// shared body: op1/op2/op3 = opcodes for 8/16/32-bit immediates
static uint8_t* emit_imm(FogExprState* st, uint8_t* buf, int v, uint8_t* w, int lim,
                         uint8_t o8, uint8_t o16, uint8_t o32)
{
    if (v >= -0x80 && v <= 0x7f) {
        if ((int)(w - buf) < lim) { w[0] = o8; w[1] = (uint8_t)v; st->depth++; return w + 2; }
    } else if (v >= -0x8000 && v <= 0x7fff) {
        if ((int)(w + 3 - buf) < lim) { w[0] = o16; memcpy(w + 1, &v, 2); st->depth++; return w + 3; }
    } else if ((int)(w + 5 - buf) < lim) { w[0] = o32; memcpy(w + 1, &v, 4); st->depth++; return w + 5; }
    *buf = 0; return nullptr;
}
// 1.13c Fog+0x195E0
uint8_t* fog_expr_imm_195e0(FogExprState* st, int lim, uint8_t* buf, int v, uint8_t* w) { return emit_imm(st, buf, v, w, lim, 4, 5, 6); }
// 1.13c Fog+0x19680
uint8_t* fog_expr_imm_19680(FogExprState* st, int lim, uint8_t* buf, int v, uint8_t* w) { return emit_imm(st, buf, v, w, lim, 7, 8, 9); }

// 1.13c Fog+0x19CF0 -- push op `nt` with precedence reduction; emits popped ops at `w`.
static const uint8_t kPrec[23] = {23,1,2,3,9,9,9,9,9,9,15,15,15,15,15,15,17,17,19,19,20,21,22};
uint8_t* fog_expr_push_19cf0(FogExprState* st, uint8_t* buf, int cap, uint8_t nt,
                             int (*argc1)(), int32_t nv, uint8_t* w)
{
    int n = (int)(w - buf);
    while (st->top > 0) {
        uint8_t b = st->op[st->top - 1];
        if (b == 1 || nt == 1 || b > 0x16 || kPrec[b] < nt) break;
        int c;
        switch (b) {
        case 1: c = argc1 ? argc1() : 2; break;
        case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: c = 2; break;
        case 0x15: c = 1; break;
        case 0x16: c = 3; break;
        default: c = 0;
        }
        if (st->depth < c) goto fail;
        if (cap <= n) { *buf = 0; return nullptr; }
        *w++ = b; n++; st->top--; st->depth += 1 - c;
    }
    if (nt != 0x17 && nt != 3) {
        if (nt == 0) goto fin;
        if (st->top > 0x3f) { *buf = 0; return nullptr; }
        st->op[st->top] = nt; st->val[st->top] = nv; st->top++;
    }
    if (nt != 0) return w;
fin:
    if (w != buf) {
        if ((int)(w - buf) < cap) { *w = 0; return w + 1; }
fail:   *buf = 0;
    }
    return nullptr;
}

// 1.13c Fog+0x197B0 -- tokenizer. lookup(type,val,name) < 0 means unknown; call(name) >= 0 = function id.
// esi = retail's ESI flag (type 0x10 vs 0x11 for resolved names).
const char* fog_expr_token_197b0(FogExprState* st, const char* p, int* val, uint8_t* type, int* val2,
                                 int esi, int (*call)(const char*), int (*lookup)(uint8_t, int32_t, const char*))
{
    char nm[256]; int i;
    *val = 0;
    while (*p && (isspace((unsigned char)*p) || *p == '"')) p++;
    auto cur = [&]() { return lookup(st->op[st->top - 1], st->val[st->top - 1], nm); };
    if (isdigit((unsigned char)*p)) {
        *type = 0x10; *val = *p - '0';
        while (isdigit((unsigned char)p[1])) { p++; *val = *val * 10 + (*p - '0'); }
        return p + 1;
    }
    if (*p == '\'') {
        nm[0] = p[1]; const char* q = p + 2; i = 1;
        for (; *q; q++) { if (*q == '\'') { q++; break; } if (i < 0xff) nm[i++] = *q; }
        nm[i] = 0; int r = cur();
        if (r < 0) { *type = 0x10; *val = 0; } else { *type = (esi == 0) + 0x10; *val = r; }
        return q;
    }
    if (isalpha((unsigned char)*p)) {
        nm[0] = *p; i = 1; const char* q = p + 1;
        while (isalnum((unsigned char)*q)) { if (i < 0xff) nm[i++] = *q; q++; }
        nm[i] = 0;
        const char* s = q; while (*s && isspace((unsigned char)*s)) s++;
        if (*s == '(') { int f = call(nm); if (f >= 0) { *type = 0x12; *val = f; return s + 1; } }
        int r = cur();
        if (r < 0) { *type = 0x10; *val2 = 0; } else { *type = (esi == 0) + 0x10; *val2 = r; }
        return s;
    }
    switch (*p) {
    case '!': if (p[1] == '=') { *type = 0x0f; return p + 2; } break;
    case '=': if (p[1] == '=') { *type = 0x0e; return p + 2; } break;
    case '(': *type = 0; return p + 1;
    case ')': *type = 1; return p + 1;
    case ',': *type = 2; return p + 1;
    case '+': *type = 3; return p + 1;
    case '-': *type = 4; return p + 1;
    case '*': *type = 5; return p + 1;
    case '/': *type = 6; return p + 1;
    case '?': *type = 7; return p + 1;
    case ':': *type = 8; return p + 1;
    case '^': *type = 9; return p + 1;
    case '<': if (p[1] == '=') { *type = 10; return p + 2; } *type = 11; return p + 1;
    case '>': if (p[1] == '=') { *type = 13; return p + 2; } *type = 12; return p + 1;
    case '.': {
        nm[0] = p[1]; const char* q = p + 2; i = 1;
        while (isalnum((unsigned char)*q)) { if (i < 0xff) nm[i++] = *q; q++; }
        nm[i] = 0; int r = cur();
        if (r >= 0) { *type = 0x10; *val2 = r; return q; }
    }
    }
    return nullptr;
}

// 1.13c Fog+0x143D0 -- strip the exe path in `buf` (filled by GetModuleFileNameA) to its directory.
// Returns 1 and truncates after the last '\\' '/' (or ':' kept) separator; 0 if none/empty.
int fog_app_dir_143d0(char* buf)
{
    int n = (int)strlen(buf);
    if (n <= 0) return 0;
    while (buf[n] != '\\' && buf[n] != '/' && buf[n] != ':') if (--n < 1) return 0;
    if (buf[n] == ':') n++;
    buf[n] = 0; return 1;
}
