// 1.13c Fog+0x1A4A0 -- expression compiler: token stream -> postfix byte code (shunting yard).
// Callees are unported module-internal functions, so each is a seam on an env struct.
// ponytail: the switched token-type byte is read by retail from a stack slot (esp+0x13) the
// tokenizer fills; modelled as the tokenizer's *type out-param. Verify at first live vector.
#include <cstdint>

struct FogExprEnv {
    // Fog+0x197B0: next token; returns nonzero while tokens remain, fills type and value.
    int (*next)(uint8_t* opstack, int* val, uint8_t* type, void* p4, void* p6);
    // Fog+0x19CF0: append opcode, returns new write pointer or null.
    uint8_t* (*emit)(uint8_t* buf, int cap, uint8_t op, void*, int);
    uint8_t* (*lit_a)(uint8_t* buf, int val, uint8_t* out);   // Fog+0x19680
    uint8_t* (*lit_b)(uint8_t* buf, int val, uint8_t* out);   // Fog+0x195E0
    uint32_t (*len)(uint8_t* buf, int n, void*, int, int, int); // Fog+0x19E90
};

// 1.13c Fog+0x1A4A0
int fog_expr_compile_1a4a0(const FogExprEnv* e, uint8_t* out, int cap, void* p4,
                           int (*callback)(int), void* p6)
{
    uint8_t ty[64 + 4];          // operator-type stack, ty[1..]
    int val[65];
    int sp = 0, depth = 0, n = 0, have_lit = 0;
    bool prevlit = false;
    uint8_t* w = out;
    int tv; uint8_t tt;
    while (e->next(ty + 1, &tv, &tt, p4, p6)) {
        switch (tt) {
        case 0: if (n > 0x3f) goto fail; ty[n + 1] = 2; sp++; break;
        case 1: {
            if (n < 1) goto fail;
            n = (int)(w - out);
            while (ty[sp + 1] != 2) {
                int c, op = ty[sp + 1];
                if (op == 1) { if (!callback) c = 2; else c = callback(val[sp]); }
                else if (op >= 10 && op <= 20) c = 2;
                else if (op == 0x15) c = 1;
                else if (op == 0x16) c = 3;
                else c = 0;
                if (depth < c || cap <= n) goto fail;
                *w = (uint8_t)op; uint8_t* nx = w + 1; n++; depth += 1 - c;
                if (op == 1) {
                    if (cap <= (int)(nx - out)) goto fail;
                    *nx = (uint8_t)val[sp]; w += 2; break;
                }
                sp--; w = nx;
                if (sp < 1) goto fail;
            }
            if (sp < 1 || (ty[sp + 1] != 2 && ty[sp + 1] != 1)) goto fail;
            sp--; break;
        }
        case 2: w = e->emit(out, cap, 3, 0, 0); break;
        case 3: w = e->emit(out, cap, 0x10, 0, 0); have_lit = 0; prevlit = false; break;
        case 4: w = e->emit(out, cap, prevlit ? 0x11 : 0x15, 0, 0); prevlit = false; break;
        case 5: w = e->emit(out, cap, 0x12, 0, 0); prevlit = false; break;
        case 6: w = e->emit(out, cap, 0x13, 0, 0); prevlit = false; break;
        case 7: w = e->emit(out, cap, 0x16, 0, 0); break;
        case 8: w = e->emit(out, cap, 0x17, 0, 0); break;
        case 9: w = e->emit(out, cap, 0x14, 0, 0); prevlit = false; break;
        case 10: w = e->emit(out, cap, 0xc, 0, 0); prevlit = false; break;
        case 11: w = e->emit(out, cap, 10, 0, 0); prevlit = false; break;
        case 12: w = e->emit(out, cap, 0xb, 0, 0); prevlit = false; break;
        case 13: w = e->emit(out, cap, 0xd, 0, 0); prevlit = false; break;
        case 14: w = e->emit(out, cap, 0xe, 0, 0); prevlit = false; break;
        case 15: w = e->emit(out, cap, 0xf, 0, 0); prevlit = false; break;
        case 16: w = e->lit_a(out, tv, w); prevlit = true; break;
        case 17: w = e->lit_b(out, tv, w); prevlit = true; have_lit = 1; break;
        case 18:
            if (n < 0x40) { ty[n + 1] = 1; val[sp + 1] = tv; sp++; prevlit = true; have_lit = 1; }
            else { *out = 0; w = nullptr; prevlit = true; have_lit = 1; }
            break;
        }
        if (!w) goto fail;
        n = sp;
    }
    {
        uint8_t* end = e->emit(out, cap, 0, 0, 0);
        if (end) {
            if (!have_lit) {
                uint32_t l = e->len(reinterpret_cast<uint8_t*>(out), (int)(end - out), 0, 0, 0, 0);
                e->lit_a(out, (int)l, out);
                end = e->emit(out, cap, 0, 0, 0);
            }
            return (int)(end - out);
        }
    }
fail:
    *out = 0;
    return 0;
}
