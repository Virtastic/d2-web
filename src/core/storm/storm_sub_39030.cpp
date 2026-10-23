// 1.13c Storm+0x39030 (sub_39030): SCODE emit -- normalises one operation's operand words and appends a node
// to the list at this+0xC. Transcribed from build-wasm/ghidra-storm/decomp/0x39030.c, gotos kept.
// 0x1290 (list node alloc) is a host seam; 0x3B2D0 (strchr) is inlined. The decompile stores
// (strchr result - 0x6fc354ec) into node[4]; the string sits at 0x6fc354ec, so that is the op index
// (0 - 0x6fc354ec when the op is absent, as in retail).
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32; typedef uint8_t u8;

struct StormEmitEnv {
    i32* (*alloc_node)(i32* list, i32 a, i32 b);  // 0x1290
};

#define B(x, i) ((u8)((x) >> (8 * (i))))
#define SETB1(x, v) ((x) = ((x) & 0xFFFF00FFu) | ((u32)(u8)(v) << 8))

void Storm_Sub39030(void* self, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, const StormEmitEnv& env) {
    u32 uVar1, uVar5;
    u8 cVar6 = (u8)p4;
    if (B(p2, 1) == 0 && B(p3, 1) == 0 && B(p4, 1) == 0) {
        if (p1 == 1) goto L5e;
        goto Le0;
    } else {
        p1 = 1;
        if (B(p2, 1) == 0) goto L5e;
        goto Laa;
    }
L5e:
    if (B(p2, 2) == 0 && B(p2, 3) == 0) {
        if ((u8)p2 == (u8)p3 && B(p3, 1) != 0) SETB1(p2, B(p3, 1));
        else {
            if ((u8)p2 == (u8)p4) {
                SETB1(p2, B(p4, 1));
                if (B(p4, 1) != 0) goto Laa;
            }
            SETB1(p2, 1);
        }
    }
Laa:
    if (B(p3, 1) == 0 && B(p3, 2) == 0 && B(p3, 3) == 0) SETB1(p3, 1);
    if ((u8)p4 != 0) {
        if (B(p4, 1) == 0 && B(p4, 2) == 0 && B(p4, 3) == 0) SETB1(p4, 1);
        goto Le0;
    }
    goto L10e;
Le0:
    if (cVar6 == 0 || (u8)p5 == 0 || p2 != p4 || p2 == p3) goto L10e;
    uVar5 = p4;
    goto Lswap;
L10e:
    uVar1 = p3;
    if (((u8)p3 != 8 && (u8)p3 != 9) || cVar6 == 8 || cVar6 == 9) goto L138;
    uVar5 = p4;
Lswap:
    p4 = (p4 & 0xFFFFFF00u) | (u8)p3;
    cVar6 = (u8)p4;
    uVar1 = uVar5;
    p4 = p3;
L138:
    p3 = uVar1;
    uVar5 = p3;
    if (cVar6 != 0 && (u8)p5 != 0 && p2 != p3 && p2 != p4) {
        p3 = 0;
        Storm_Sub39030(self, p1, p2, uVar5, 0, 0, env);
        p3 = p2;
    }
    i32* n = env.alloc_node((i32*)((char*)self + 0xc), 0, 0);
    n[2] = (i32)p2;
    n[3] = (i32)(cVar6 == 0 ? p3 : p4);
    n[5] = (i32)p1;
    if ((u8)p5 == 0) { n[4] = 0; return; }
    static const char ops[] = "=&|^+-~@";
    i32 idx = (i32)(0u - 0x6fc354ecu);
    for (int i = 0; ops[i]; ++i) if (ops[i] == (char)p5) { idx = i; break; }
    n[4] = idx;
}
