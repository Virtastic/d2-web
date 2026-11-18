// 1.13c Storm+0x391E0 (sub_391E0): SCODE expression compiler (parses a "<regs> <ops>" string such as
// "W=D+1" into a node list, orders it, and emits it through two code-gen vtable slots).
// Transcribed from build-wasm/ghidra-storm/decomp/0x391E0.c, gotos kept. 32-bit guest layout:
// words are int32, guest links are converted with G()/H(). The unmapped callees (0x1290, 0x4390,
// 0x4D60+0x1B960, 0x1C580, 0x39030, the code-gen vtable) are host seams on StormScodeEnv. The leaves
// 0x38F10 (max over list), 0x38F60 (clear list), 0x3AFD0/0x3B2D0 (strlen/strchr) are inlined below.
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32; typedef uint8_t u8;
static inline i32* G(i32 v) { return (i32*)(uintptr_t)(u32)v; }
static inline i32 H(const void* p) { return (i32)(u32)(uintptr_t)p; }

struct StormScodeEnv {
    void (*set_last_error)(u32 code);                     // DAT_6fc45388 = code; SetLastError(code)
    void (*free_node)(i32* node);                         // 0x4D60 then 0x1B960(node,"__AUINST__",-2)
    i32* (*alloc_node)(i32* list, i32 a, i32 b);          // 0x1290
    void (*move_node)(i32* self, i32* node, i32 mode);    // 0x4390
    i32* (*mem_alloc)(u32 size, u32 line);                // 0x1C580(size, SCODE.CPP, line, 0)
    void (*emit)(i32* self, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5);  // 0x39030 (__thiscall)
    i32 (*vt0)(i32* self, i32 acc, i32* node);            // (**(code**)*param_1)
    i32 (*vt1)(i32* self, i32 acc);                       // (*(code**)(*param_1+4))
    u32 dat_4535c;                                        // DAT_6fc4535c (0 in the file image)
};

#define B(x, i) ((u8)((x) >> (8 * (i))))
static const char T[] = " WDSTABC01";  // 0x6fc354e0
static bool in(const char* set, char c) { for (; *set; ++set) if (*set == c) return true; return false; }
// (char)pcVar9 + 0x20 with pcVar9 = strchr-style scan of T; 0x20 when absent
static u8 idx(char c) { for (int i = 0; T[i]; ++i) if (T[i] == c) return (u8)i; return 0x20; }
static i32* first(i32 v) { return G(v < 1 ? 0 : v); }  // (v<1)-1 & v

static void clear_list(const StormScodeEnv& e, i32* lst) {
    for (i32 p; (p = lst[1]) > 0;) e.free_node(G(p));
}
// 0x38F10: max of node[5] over nodes whose side (byte 8/12) == ch with byte+2 == 0 and byte+3 != 0
static i32 max_of(i32* lst, u8 ch) {
    i32 r = 0;
    for (i32* n = first(lst[1]); (i32)H(n) > 0; n = G(n[1])) {
        u8* b = (u8*)n;
        if (((b[8] == ch && b[10] == 0 && b[11] != 0) || (b[12] == ch && b[14] == 0 && b[15] != 0)) && r < n[5]) r = n[5];
    }
    return r;
}
static u32 slen(const char* s) { u32 n = 0; while (s[n]) ++n; return n; }

u32 Storm_Sub391E0(i32* param_1, const char* param_2, u32* param_3, const StormScodeEnv& env) {
    i32 arr[20] = {0};  // local_50
    u8 local_6c[8] = {0};
    i32* const lst = param_1 + 4;
    const char* c;
    const char* p; const char* pw;
    const char* s = param_2;
    u32 local_8c = 0, local_94 = 0, local_88 = 0, local_98, local_90, uVar10, uVar12, uVar13, local_60;
    u32* pword = nullptr; u32* local_70 = nullptr;
    i32 local_54, local_58 = 0, local_5c, local_78, iVar11, iVar23;
    int local_7c;
    u8 bVar7 = 0, bVar20, local_99 = 0, pos;
    char cVar8, opch = 0;
    bool bVar2, bVar3, bVar4, bVar24;
    const char* local_64;
    i32* n; i32* m; i32* p14; i32* p17;
    u8 *a, *b;

    if (param_3) *param_3 = 0;
    if (!param_2) { env.set_last_error(0x57); return 0; }
    if (slen(s) < 3) { if (param_3) *param_3 = (u32)H(s); clear_list(env, lst); return 0; }
    local_90 = 0;
    do {
        bVar4 = false; bVar24 = false; local_7c = 0; bVar2 = false;
        for (p = s; *p; ++p) {
            if (*p == T[local_90 + 1]) {
                char nx = p[1];
                if (nx == '=' || (nx && in("1234", nx) && p[2] == '=')) { bVar4 = true; bVar2 = true; }
                else { bVar4 = true; if (!bVar24) local_7c = 1; }
            }
            bVar3 = bVar2;
            if (*p == ' ') { bVar3 = false; bVar24 = bVar2; }
            bVar2 = bVar3;
        }
        switch (local_90) {
        case 0: arr[1] = 1; break;
        case 1: case 2: case 3: arr[local_90 + 1] = bVar4 ? 3 : 0; break;
        case 7: case 8: arr[local_90 + 1] = 2; break;
        default: arr[local_90 + 1] = bVar4 ? (local_7c != 0) + 1 : 0;
        }
        ++local_90;
    } while ((i32)local_90 < 9);
    clear_list(env, lst);
    local_64 = s; local_88 = 0; local_94 = 0; opch = 0; local_90 = 4;
    c = s;
    do {
        local_64 = c;
        cVar8 = *c;
        if (cVar8 == 0) { L29493: local_54 = 1; }
        else {
            if (c == s || c[-1] == ' ') if (in("124", cVar8)) { local_90 = (u32)(cVar8 - 0x30); goto L29a35; }
            if (in("&|^+-", cVar8)) { opch = cVar8; goto L29a35; }
            if (cVar8 == ' ') goto L29493;
            if (!in("WSDTABC01", cVar8)) {
                if (param_3) *param_3 = (u32)H(c);
                clear_list(env, lst); return 0;
            }
            local_54 = 0;
        }
        bVar7 = 0; local_8c = 0;
        if (local_54 == 0) {
            bVar7 = idx(cVar8);
            local_8c = bVar7 | ((u32)(arr[bVar7] == 3) << 24);
        L294e8:
            cVar8 = c[1]; bVar24 = false;
            if (cVar8 && in("WABC", cVar8)) {
                pos = idx(cVar8);
                ++c;
                local_8c = (local_8c & 0xff000000u) | ((u32)pos << 16) | (local_8c & 0xffff);
                local_64 = c; bVar24 = true;
            }
            cVar8 = c[1];
            if (cVar8 && in("1234", cVar8)) {
                ++c;
                local_8c = (local_8c & 0xffff0000u) | ((u32)(u8)(cVar8 - 0x30) << 8) | (local_8c & 0xff);
                local_64 = c; goto L29552;
            }
            if (!bVar24) goto L295a6;
        L29552:
            if (bVar7 == 8 || bVar7 == 9) {
                if (param_3) *param_3 = (u32)H(c);
                clear_list(env, lst); return 0;
            }
            goto L294e8;
        }
    L295a6:
        if (B(local_88, 0) == 0) {
            if (c[1] == '=') {
                if (bVar7 == 8 || bVar7 == 9) {
                    if (param_3) *param_3 = (u32)H(c);
                    clear_list(env, lst); return 0;
                }
                ++c; local_88 = local_8c;
            L295cf:
                local_94 = 0; goto L29a30;
            }
            if (local_54 == 0) {
                if (param_3) *param_3 = (u32)H(c + 1);
                clear_list(env, lst); return 0;
            }
        } else {
            if (B(local_94, 0) == 0) local_94 = local_8c;
            else {
                if (local_54 == 0 || arr[B(local_88, 0)] == 3) {
                    local_7c = 0;
                    do {
                        local_5c = 0;
                        do {
                            iVar23 = local_5c;
                            pword = local_7c == 0 ? &local_94 : &local_8c;
                            local_70 = pword;
                            switch (local_5c) {
                            default: uVar10 = 2; iVar11 = 3; break;
                            case 1: uVar10 = 3; iVar11 = 3; break;
                            case 2: uVar10 = 4; iVar11 = 3; break;
                            case 3: uVar10 = 8; iVar11 = 2;
                            }
                            if (B(*pword, 0) == uVar10 && arr[uVar10] == iVar11 &&
                                (local_5c == 0 || local_5c == 1 || local_5c == 2 || opch == 0 || !in("&|^+-", opch))) {
                                if (local_6c[iVar23] == 0 || B(*pword, 2) != 0) {
                                    cVar8 = c[1]; p = c; local_58 = 0;
                                    while (cVar8) {
                                        if (cVar8 == T[uVar10] && p[2] != '=') {
                                            if (B(*pword, 2) == 0) local_58 = 1;
                                            break;
                                        }
                                        cVar8 = p[2]; ++p;
                                    }
                                    local_99 = 0;
                                    if (local_58 == 0 && B(local_94, 0) != 1) {
                                    L29855:
                                        if (B(local_88, 2) == 0 && B(local_88, 1) == 0 && B(local_88, 3) == 0) {
                                            if (arr[B(local_88, 0)] == 1 && B(local_94, 0) != B(local_88, 0)) {
                                                cVar8 = *c; p = c;
                                                while (cVar8 && cVar8 != ' ') {
                                                    if (cVar8 == T[B(local_88, 0)] && p[1] != '=') goto L298b6;
                                                    cVar8 = *++p;
                                                }
                                                local_99 = B(local_88, 0); goto L298c6;
                                            }
                                        }
                                    L298b6:
                                        if (B(local_94, 0) == 1) {
                                            if (param_3) *param_3 = (u32)H(c);
                                            clear_list(env, lst); return 0;
                                        }
                                        local_99 = 1;
                                    } else {
                                        uVar10 = 9; bVar7 = 9;
                                        for (;;) {
                                            bVar20 = (u8)uVar10;
                                            if (bVar7 != 1) {
                                                if (arr[uVar10] != 0) {
                                                    if (arr[uVar10] != 1) goto L2978c;
                                                    cVar8 = *c; p = c;
                                                    if (cVar8) {
                                                        while (cVar8 != T[uVar10]) { cVar8 = *++p; if (!cVar8) goto L297a7; }
                                                        goto L2978c;
                                                    }
                                                }
                                            L297a7:
                                                local_99 = bVar20;
                                                if (bVar20 != 0) goto L298c6;
                                                break;
                                            }
                                        L2978c:
                                            bVar7 = bVar20 - 1; uVar10 = bVar7;
                                            if (bVar7 == 0) break;
                                        }
                                        bVar7 = 9; local_78 = 9; p17 = arr + 9;
                                        do {
                                            if (*p17 == 4) {
                                                uVar10 = 0;
                                                for (iVar23 = 0; iVar23 < 5; ++iVar23)
                                                    if (local_6c[iVar23] == bVar7) uVar10 = idx("DST01"[iVar23]);
                                                if ((char)uVar10 != 0) {
                                                    cVar8 = *c; p = c;
                                                    while (cVar8) {
                                                        if (cVar8 == T[uVar10] && p[1] != '=') { local_99 = 0; goto L29832; }
                                                        cVar8 = *++p;
                                                    }
                                                    local_99 = (u8)local_78;
                                                }
                                            }
                                        L29832:
                                            --bVar7; --p17; --local_78;
                                        } while (bVar7 != 0);
                                        pword = local_70;
                                        if (local_99 == 0) goto L29855;
                                    }
                                L298c6:
                                    local_60 = local_99; uVar10 = local_60;
                                    env.emit(param_1, local_90, local_60, *pword, env.dat_4535c, 0);
                                    if (local_58 != 0 && local_99 != 1) { local_6c[local_5c] = local_99; arr[local_99] = 4; }
                                    *pword = uVar10;
                                    iVar23 = local_5c;
                                } else {
                                    *(u8*)pword = local_6c[iVar23];
                                    ((u8*)pword)[3] = 0;
                                }
                            }
                            local_5c = iVar23 + 1;
                        } while (iVar23 + 1 < 4);
                        ++local_7c;
                    } while (local_7c < 2);
                }
                uVar10 = local_88; local_98 = 1;
                if (local_54 == 0) {
                    cVar8 = c[1]; pw = c;
                    while (cVar8 && cVar8 != ' ') {
                        if (cVar8 == T[B(local_88, 0)] && pw[2] != '=') goto L29a4e;
                        cVar8 = pw[2]; ++pw;
                    }
                    if (arr[B(local_88, 0)] == 3 || B(local_88, 2) != 0 || B(local_88, 3) != 0) {
                    L29a4e:
                        if (((B(local_94, 0) != 1 || B(local_94, 2) != 0) || (uVar10 = local_94, B(local_94, 3) != 0)) &&
                            ((B(local_8c, 0) != 1 || B(local_8c, 2) != 0) || (uVar10 = local_8c, B(local_8c, 3) != 0))) {
                            local_7c = 0;
                            do {
                                pword = local_7c == 0 ? &local_94 : &local_8c;
                                if (B(*pword, 2) == 0 && B(*pword, 3) == 0) {
                                    uVar10 = B(*pword, 0);
                                    iVar23 = arr[uVar10];
                                    if (iVar23 == 1 || iVar23 == 4) {
                                        if (iVar23 == 4)
                                            for (iVar23 = 0; iVar23 < 5; ++iVar23)
                                                if (local_6c[iVar23] == B(*pword, 0)) uVar10 = idx("DST01"[iVar23]);
                                        cVar8 = local_64[1]; pw = local_64;
                                        while (cVar8) {
                                            if (cVar8 == T[uVar10] && pw[2] != '=') goto L29b3a;
                                            cVar8 = pw[2]; ++pw;
                                        }
                                        local_98 = *pword;
                                    }
                                }
                            L29b3a:
                                ++local_7c; c = local_64; uVar10 = local_98;
                            } while (local_7c < 2);
                        }
                    }
                    env.emit(param_1, local_90, uVar10, local_94, local_8c, (u32)(u8)opch);
                    local_94 = uVar10;
                } else {
                    bVar24 = B(local_88, 0) == B(local_94, 0);
                    uVar10 = local_88; local_88 = 0;
                    if (bVar24 && B(uVar10, 1) == B(local_94, 1)) goto L295cf;
                    env.emit(param_1, local_90, uVar10, local_94, env.dat_4535c, 0);
                    local_94 = 0;
                }
            }
        L29a30:
            opch = 0;
        }
    L29a35:
        cVar8 = *c; ++c; local_64 = c;
    } while (cVar8 != 0);

    if (lst[1] < 1) return 0;
    uVar10 = (u32)max_of(lst, 3); uVar12 = (u32)max_of(lst, 4); uVar13 = (u32)max_of(lst, 2);
    if (uVar10) env.emit(param_1, uVar10, 3, env.dat_4535c, env.dat_4535c, 0x2b);
    if (uVar12) env.emit(param_1, uVar12, 4, env.dat_4535c, env.dat_4535c, 0x2b);
    if (uVar13) env.emit(param_1, uVar13, 2, env.dat_4535c, env.dat_4535c, 0x2b);

    for (n = first(lst[1]); H(n) > 0; n = G(n[1])) {
        local_78 = 0;
        do {
            if (local_78 == 0) { a = (u8*)(n + 3); b = (u8*)(n + 2); }
            else { a = (u8*)(n + 2); b = (u8*)(n + 3); }
            if (2 < a[1]) {
                bVar7 = a[0];
                if (bVar7 == b[2] || (bVar7 == b[0] && b[1] < 3)) {
                    if (param_3) *param_3 = slen(s) + (u32)H(s);
                    clear_list(env, lst); return 0;
                }
                iVar23 = 0; local_90 = bVar7;
                do {
                    i32* w = env.alloc_node(param_1 + 3, 0, 0);
                    w[2] = (i32)local_90; w[3] = (i32)env.dat_4535c; w[4] = 7; w[5] = 4;
                    env.move_node(n, w, (iVar23 == 0) + 1);
                    ++iVar23;
                } while (iVar23 < 2);
                a[1] -= 2;
                if (b[0] == a[0]) b[1] -= 2;
            }
            ++local_78;
        } while (local_78 < 2);
        if (!n) n = lst;
    }
    for (n = first(lst[1]); H(n) > 0; n = G(n[1])) {
        if (n[4] == 7 && H(m = first(n[1])) > 0) {
            while (m[4] != 7 || (char)m[2] != (char)n[2]) {
                cVar8 = (char)n[2];
                if ((char)m[2] == cVar8 || (char)m[3] == cVar8 || *((char*)m + 10) == cVar8 ||
                    *((char*)m + 0xe) == cVar8 || (m = G(m[1]), H(m) < 1)) goto L29e87;
            }
            env.free_node(m); env.free_node(n);
            n = G(lst[1]);
            if (H(n) < 1) goto L29e8b;
        }
    L29e87:
        if (!n) { L29e8b: n = lst; }
    }
    n = first(lst[1]);
    for (;;) {
        if (H(n) < 1) {
            p14 = first(lst[1]);
            p17 = G(p14[1]);
            if (H(p17) > 0) {
                do {
                    if (((char)p17[3] != 0 && (char)p17[3] == (char)p14[2]) ||
                        (*((char*)p17 + 0xe) != 0 && *((char*)p17 + 0xe) == (char)p14[2]) ||
                        (((char)p17[2] != 0 && (char)p17[2] == (char)p14[2]) && p17[4] != 0)) {
                        i32 t[20] = {0};  // local_50[0..19]
                        p14 = p17;
                        while (H(p14) > 0) {
                            u8* q = (u8*)p14;
                            if (H(p17) < H(p14) && (q[12] == 0 || t[q[12] + 10] == 0) &&
                                (q[14] == 0 || t[q[14] + 10] == 0) && (q[8] == 0 || (t[q[8] + 10] == 0 && t[q[8]] == 0))) {
                                i32 iv = p14[0];
                                if (iv != 0) {
                                    u32 uv = (u32)p14[1];
                                    i32* pl = ((i32)uv < 0) ? G((i32)~uv) : G(H(p14) + (i32)(uv - (u32)G(iv)[1]));
                                    *pl = iv;
                                    G(p14[0])[1] = p14[1];
                                    p14[0] = 0; p14[1] = 0;
                                }
                                iv = p17[0];
                                p14[0] = iv; p14[1] = G(iv)[1];
                                G(iv)[1] = H(p14);
                                p17[0] = H(p14);
                                break;
                            }
                            t[q[8] + 10] = 1; t[q[12]] = 1;
                            u8 q14 = q[14];
                            p14 = G(p14[1]);
                            t[q14] = 1;
                        }
                    }
                    m = G(p17[1]);
                } while (H(m) > 0 && (p14 = p17, p17 = m, m != nullptr));
            }
            if (param_1[1] == 0) param_1[1] = H(env.mem_alloc(0x400, 0x29d));
            if (param_1[6] == 0) param_1[6] = H(env.mem_alloc(0x400, 0x29f));
            iVar23 = param_1[1];
            for (p17 = first(lst[1]); H(p17) > 0; p17 = G(p17[1])) {
                iVar23 = env.vt0(param_1, iVar23, p17);
                if (!p17) p17 = lst;
            }
            param_1[2] = iVar23 - param_1[1];
            param_1[7] = env.vt1(param_1, param_1[6]) - param_1[6];
            clear_list(env, lst);
            return 1;
        }
        cVar8 = 0;
        if ((char)n[3] != 8) {
            if ((char)n[3] == 9) {
                iVar23 = n[4];
                if (iVar23 == 0) goto caseD_2;
                if (iVar23 == 2) { cVar8 = 1; goto caseD_0; }
                if (iVar23 == 3) { n[4] = 6; n[3] = (i32)env.dat_4535c; }
            }
            goto def;
        }
        switch (n[4]) {
        case 0: case 1:
        caseD_0:
            local_90 = (u8)(cVar8 + 8);
            n[4] = 0; n[3] = (i32)local_90;
        default:
        def:
            n = G(n[1]);
            break;
        case 2: case 3: case 4: case 5:
        caseD_2: {
            i32 nx = n[1];
            env.free_node(n);
            n = first(nx);
        }
        }
    }
}
