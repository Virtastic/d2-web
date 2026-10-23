// 1.13c Fog+0x19E90 -- evaluator for the postfix byte code the Fog+0x1A4A0 compiler emits.
// 64-slot int stack; pushes onto a full stack are dropped, pops from an empty one yield 0.
#include <cstdint>

struct FogExprFn { uint32_t (*fn)(uint32_t arg, uint32_t ctx); int32_t argc; };  // 8-byte table entry

// 1.13c Fog+0x19E90
uint32_t fog_expr_eval_19e90(const uint8_t* code, int len, uint32_t (*cb)(),
                             const FogExprFn* tbl, int ntbl, uint32_t ctx)
{
    if (len < 1) return 0;
    uint32_t st[64]; int sp = 0;
    auto push = [&](uint32_t v) { if (sp < 64) st[sp++] = v; };
    auto pop = [&]() -> uint32_t { return sp > 0 ? st[--sp] : 0; };
    const uint8_t* p = code;
    do {
        const uint8_t* n = p + 1;
        switch (*p) {
        case 1: {
            uint32_t i = *n; n = p + 2;
            if ((int)i >= ntbl) { push(0); break; }
            uint32_t a = 0;
            switch (tbl[i].argc) {
            case 3: a = pop(); pop(); pop(); break;
            case 2: pop(); pop(); break;
            case 1: pop(); break;
            case 0: break;
            default: push(0); goto next;
            }
            push(tbl[i].fn(a, ctx));
            break;
        }
        case 4: case 5: case 6: {
            if (!cb || len <= n - code) return 0;
            uint32_t v = cb(); push(v);
            n = p + (*p == 4 ? 2 : *p == 5 ? 3 : 5);
            break;
        }
        case 7: if (len <= n - code) return 0; push((int32_t)(int8_t)*n); n = p + 2; break;
        case 8: if (len <= n - code) return 0; push((int32_t)(int16_t)(n[0] | n[1] << 8)); n = p + 3; break;
        case 9: if (len <= n - code) return 0;
                push(n[0] | n[1] << 8 | n[2] << 16 | (uint32_t)n[3] << 24); n = p + 5; break;
        case 0x13: { uint32_t b = pop(), a = pop(); push(b ? (uint32_t)((int32_t)a / (int32_t)b) : 0); break; }
        case 0x14: { uint32_t b = pop(), a = pop(), r = 1;
                     if ((int32_t)b >= 1) { r = a; for (int32_t k = (int32_t)b - 1; k > 0; --k) r *= a; }
                     push(r); break; }
        case 0x15: push(-pop()); break;
        case 0x16: { uint32_t a = pop(), b = pop(); uint32_t r = a;
                     if (sp > 0 && pop() != 0) r = b;
                     push(r);
                     break; }
        case 10: case 11: case 12: case 13: case 14: case 15:
        case 0x10: case 0x11: case 0x12: {
            uint32_t t = pop(), s = pop(); int32_t T = (int32_t)t, S = (int32_t)s;
            switch (*p) {
            case 10: push(S < T); break;       case 11: push(T < S); break;
            case 12: push(S <= T); break;      case 13: push(T <= S); break;
            case 14: push(s == t); break;      case 15: push(s != t); break;
            case 0x10: push(s + t); break;     case 0x11: push(s - t); break;
            default: push(s * t); break;
            }
            break;
        }
        default: return sp > 0 ? st[sp - 1] : 0;
        }
        next: p = n;
    } while (p - code < len);
    return 0;
}
