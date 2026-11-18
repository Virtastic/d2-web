// Fog+0x1A4A0 -- expression compiler: infix text -> postfix byte code (shunting yard), stdcall (ret 0x18):
//   (const char* str, uint8_t* out, int cap, callcb, arity, lookup)
// callcb / lookup are the tokenizer's callbacks (Fog+0x197B0); arity(v) = argument count of the call entry with value v
// (fastcall ECX = v; null = 2). The callees (tokenizer, 19CF0 push/flush, 195E0/19680 immediates, 19E90 evaluator) are our ports.
// Retail keeps the working state (FogExprState) in its frame; only top and depth are initialised.
#include <cstdint>
#include <cstring>

struct FogExprState { uint8_t op[0x40]; int32_t val[0x40]; int32_t top; int32_t depth; };
const char* fog_expr_token_197b0(FogExprState* st, const char* p, uint32_t* val, uint8_t* type,
                                 int (*call)(const char*), int (*lookup)(const char*, int32_t*, uint32_t, uint32_t));
uint8_t* fog_expr_push_19cf0(FogExprState* st, uint8_t* buf, int cap, uint8_t nt, int (*argc1)(), int32_t nv, uint8_t* w);
uint8_t* fog_expr_imm_195e0(FogExprState* st, int lim, uint8_t* buf, int v, uint8_t* w);
uint8_t* fog_expr_imm_19680(FogExprState* st, int lim, uint8_t* buf, int v, uint8_t* w);
struct FogExprFn;
uint32_t fog_expr_eval_19e90(const uint8_t* code, int len, uint32_t (*cb)(uint32_t, uint32_t), const FogExprFn* tbl, int ntbl, uint32_t ctx);

// 1.13c Fog+0x1A4A0
int fog_expr_compile_1a4a0(const char* str, uint8_t* out, int cap, int (*callcb)(const char*),
                           int (*arity)(int32_t), int (*lookup)(const char*, int32_t*, uint32_t, uint32_t))
{
    FogExprState S; memset(&S, 0, sizeof S);
    int prevlit = 0, havelit = 0;
    uint8_t tt = 0; uint32_t tv = 0;
    uint8_t* w = out;
    const char* p = fog_expr_token_197b0(&S, str, &tv, &tt, callcb, lookup);
    auto push = [&](int nt) { w = fog_expr_push_19cf0(&S, out, cap, (uint8_t)nt, nullptr, 0, w); };
    while (p) {
        switch (tt) {
        case 0:
            if (S.top >= 0x40) goto fail;
            S.op[S.top++] = 2; break;
        case 1: {
            if (S.top <= 0) goto fail;
            int n = (int)(w - out);
            for (;;) {
                int t = S.top; uint8_t bl = S.op[t - 1];
                if (bl == 2) break;
                int32_t v = S.val[t - 1]; int c;
                if (bl == 1) c = arity ? arity(v) : 2;
                else if (bl >= 10 && bl <= 20) c = 2;
                else if (bl == 0x15) c = 1;
                else if (bl == 0x16) c = 3;
                else c = 0;
                if (S.depth < c || n >= cap) goto fail;
                S.depth += 1 - c; *w++ = bl; n++;
                if (bl == 1) {
                    if ((int)(w - out) >= cap) goto fail;
                    *w++ = (uint8_t)v; break;
                }
                if (--S.top <= 0) goto fail;
            }
            if (S.top <= 0) goto fail;
            uint8_t a = S.op[S.top - 1];
            if (a != 2 && a != 1) goto fail;
            S.top--; break;
        }
        case 2: push(3); break;
        case 3: push(0x10); prevlit = 0; break;
        case 4: push(prevlit ? 0x11 : 0x15); prevlit = 0; break;
        case 5: push(0x12); prevlit = 0; break;
        case 6: push(0x13); prevlit = 0; break;
        case 7: push(0x16); break;
        case 8: push(0x17); break;
        case 9: push(0x14); prevlit = 0; break;
        case 10: push(0xc); prevlit = 0; break;
        case 11: push(0xa); prevlit = 0; break;
        case 12: push(0xb); prevlit = 0; break;
        case 13: push(0xd); prevlit = 0; break;
        case 14: push(0xe); prevlit = 0; break;
        case 15: push(0xf); prevlit = 0; break;
        case 16: w = fog_expr_imm_19680(&S, cap, out, (int)tv, w); prevlit = 1; break;
        case 17: w = fog_expr_imm_195e0(&S, cap, out, (int)tv, w); prevlit = 1; havelit = 1; break;
        case 18:
            if (S.top >= 0x40) { out[0] = 0; w = nullptr; }
            else { S.op[S.top] = 1; S.val[S.top] = (int32_t)tv; S.top++; }
            prevlit = 1; havelit = 1; break;
        default: break;
        }
        if (!w) goto fail;
        tt = 0;
        p = fog_expr_token_197b0(&S, p, &tv, &tt, callcb, lookup);
    }
    push(0);
    if (!w) goto fail;
    if (!havelit) {
        uint32_t l = fog_expr_eval_19e90(out, (int)(w - out), nullptr, nullptr, 0, 0);
        w = fog_expr_imm_19680(&S, cap, out, (int)l, out);
        push(0);
    }
    return (int)((uint32_t)(uintptr_t)w - (uint32_t)(uintptr_t)out);
fail:
    out[0] = 0;
    return 0;
}
