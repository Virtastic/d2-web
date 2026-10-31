// Batch aea40b: D2Game functions at RVA 0xAEDC0-0xB3D60, transcribed from the cached 1.13c decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Same shape as d2game_batch_aea40.cpp: retail 32-bit struct
// layout read as raw bytes off a base address (R32/S16/U16/R8 at byte offsets; the decompile's int* index
// arithmetic is converted to byte offsets), register inputs as explicit parameters, callees as weak externs
// (Ord<N> for D2Common ordinals, D2GAME_<RVA> for D2Game internals) so the file links standalone.
// Our implementations are d2_fn_d2game_<RVA>.
#include <cstdint>
#include <cstdlib>

typedef intptr_t I;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))
#define S16(b, o) (*(int16_t*)((uintptr_t)(b) + (o)))
#define U16(b, o) (*(uint16_t*)((uintptr_t)(b) + (o)))

// *sgptDataTables_exref is the tables block (+0xc4 unit-table count, +0xbcc/+0xbd4 skill-table base/count,
// +0xb98/+0xba0 record base/count). Same convention as d2game_batch_aea40.cpp.
extern "C" __attribute__((weak)) char sgptDataTables_exref[];
#define DTB() ((uintptr_t)R32(sgptDataTables_exref, 0))

O(10328); O(10331); O(10392); O(10393); O(10428); O(10459); O(10540); O(10551); O(10585); O(10680);
O(10695); O(10785); O(10807); O(10826); O(10830); O(10839); O(10871); O(10887); O(10911); O(10913);
O(10916); O(10920); O(10945); O(10959); O(10973); O(11013); O(11043); O(10057); O(10188); O(10024);
O(10265); O(10510); O(10689); O(10830);

F(1100); F(1130); F(1180); F(11D0); F(1450); F(4EA30); F(7A260); F(8C3B0); F(8E6F0); F(8EA80);
F(9F7A0); F(B0A20); F(B17B0); F(C24C0); F(DFA90); F(DFFD0); F(E0460); F(E1570); F(E07A0); F(386F0);
F(37480); F(48560); F(2E4F0); F(5E3C0); F(A0AD0); F(AADE); F(DE0C0); F(DCF20); F(9F210); F(A24C0);
F(A42A); F(A83E); F(ACE2); F(A598); F(B7D30);

// ---------------------------------------------------------------------------------------------------
// 1.13c D2Game+0xAEDC0 -- FUN_6fccedc0(undefined4 param_1, int *param_2, int param_3); unit in ESI.
// Walks the three skill-stat slots of the unit (+0x9e) and applies each to the stat list.
// Decompile artifacts: uStack_c and iStack_24 are read before any write; both are kept as locals
// (uStack_c = 0, iStack_24 unused). See MISMATCH notes in the spec once it lands.
extern "C" U d2_fn_d2game_AEDC0(U p1, I p2, I p3, I esi)
{
    (void)p1;
    U uStack_c = 0;
    I unit;
    I uVar7 = (p3 == 0) ? (I)0xffffffff : (I)S32(p3, 4);
    unit = Ord10695(uVar7);
    if (unit == 0) return 0;

    U uVar2 = R32(unit, 0xb0);
    I piVar17 = p2;
    I iVar9 = Ord10428(p2, p3);
    I iVar10 = S16(unit, 0x98);
    I item = 0;
    I base = 0;                       // iStack_2c: unit-base stat delta
    if (0 < iVar9) {
        if (iVar10 < 1) return 0;
        if ((I)R32(DTB(), 0xc4) <= iVar10) return 0;
        item = Ord10871(p2, iVar10);
        if (item != 0) {
            base = Ord10959(item) - S32(esi, 0xa8);
        }
    }
    U uStack_1c = 0;
    bool bVar4 = false;
    I idx = 0;                        // iStack_20
    do {
        I iVar15 = S16(unit, 0x9e + idx * 2);
        if (iVar15 < 0 || (I)R32(DTB(), 0xbd4) <= iVar15) break;
        I iVar14 = iVar15 * 0x144 + (I)R32(DTB(), 0xbcc);
        I iVar13 = item;
        if (iVar14 != 0) {
            I iVar11 = Ord10428(p2, uStack_c, S32(unit, 0xa4 + idx * 4));
            I uVar7b = 0;
            bool doCase = true;
            switch (iVar15) {
            case 0x1a:
                iVar11 = (I)((uint32_t)iVar11 << 8);
                /* fallthrough */
            case 8:
                iVar11 = Ord10393(p2, iVar11);
                uVar7b = 1;
                break;
            case 0x4a:
                iVar11 = (I)((uint32_t)iVar11 << 8);
                /* fallthrough */
            case 6:
                iVar11 = Ord10540(p2, iVar11);
                uVar7b = 3;
                break;
            default:
                doCase = false;
                break;
            }
            if (doCase) {
                I cnt = Ord10973(p2, uVar7b, 0);
                if (0 < cnt) {
                    U uA = (U)D2GAME_11D0((I)((uintptr_t)p2 + 0x20));
                    U uB = (U)D2GAME_1180((I)((uintptr_t)p2 + 0x20));
                    if ((int32_t)uB < (int32_t)(uA >> 1)) iVar11 *= 2;
                }
            }
            // every path reaches the shift (the decompile's default jump lands on this label)
            iVar11 = (I)((uint32_t)iVar11 << (R8(iVar14, 0x18) & 0x1f));
            if (0 < iVar11) {
                if (iVar15 == 6) {
                    bVar4 = true;
                    if (p2 != 0 && R32(p2, 0) == 0) iVar11 = D2GAME_4EA30(iVar11);
                } else if (iVar15 == 0x4a) {
                    bVar4 = true;
                    if (p2 != 0 && R32(p2, 0) == 0) iVar11 = D2GAME_4EA30(iVar11);
                }
                if (iVar9 < 1) {
                    I iVar14b = Ord10973(p2, iVar15, 0);
                    if ((I)U16(item, 0x32) < (I)R32(DTB(), 0xbd4)) {
                        I t = Ord10973(p2, U16(item, 0x32), 0);
                        item = t;                          // decompile reuses iVar18 here
                        if (item < iVar14b + iVar11) iVar11 = item - iVar14b;
                    }
                    Ord10551(p2, iVar15, iVar11, 0);
                } else {
                    U uVar16 = (U)(S32(esi, 0xa8) + base + iVar9);
                    if (item == 0) {
                        I iVar18b;
                        if (p2 == 0) {
                            iVar18b = -1;
                            iVar13 = 6;
                        } else {
                            iVar18b = R32(p2, 0xc);
                            iVar13 = R32(p2, 0);
                        }
                        iVar13 = Ord11013(R32(esi, 0x1c), 2, uVar16, iVar13, iVar18b);
                        if (iVar13 == 0) goto next_slot;
                        item = iVar10;
                        Ord10785(iVar13, iVar10);
                        Ord10945(p2, iVar10, 1);
                        if (p2 == 0) {
                            D2GAME_8EA80(piVar17, p2, 3, R32(piVar17, 0xa8) + 1, 0, 0, 0);
                        } else if (R32(p2, 0) != 0) {
                            U pc = R32(p2, 0xdc);
                            while (pc != 0) {
                                uint8_t cVar1 = R8(pc, 0);
                                pc = R32(pc, 0x24);
                                if (cVar1 == 3) {
                                    D2GAME_8E6F0();
                                    uVar16 = uVar2;
                                }
                            }
                            D2GAME_8EA80(piVar17, p2, 3, R32(piVar17, 0xa8) + 1, 0, 0, 0);
                        }
                        Ord10911(item, (I)&D2GAME_9F7A0);
                        Ord10807(p2, item, 1);
                    }
                    Ord11043(item, uVar16);
                    item = Ord10680(item, iVar13, 0);
                    if (idx == 0) {
                        D2GAME_8EA80(esi, p2, 0xc, uVar16, 0, 0, 0);
                    }
                    if (0 < base + iVar9) {
                        Ord10188(iVar13, iVar15, (item * base + iVar11) / (base + iVar9), 0);
                    }
                }
                uStack_1c = 1;
            }
        }
    next_slot:
        idx = idx + 1;
        item = iVar13;
    } while (idx < 3);

    if (bVar4 && p2 != 0 && R32(p2, 0) == 0) {
        U bVar5 = (U)Ord10973(p2, 0x160, 0) & 0xff;
        I cnt = Ord10973(p2, 6, 0);
        I s = Ord10913(p2) >> 8;
        U bVar6;
        if (s < 1 || s <= (cnt >> 8)) {
            bVar6 = 0x80;
        } else {
            bVar6 = (U)((((cnt >> 8) << 7) / s) & 0xff);
        }
        uStack_c = bVar6;
        I d = (I)bVar5 - (I)bVar6;
        if (d < 0) d = -d;
        if (4 < d) {
            D2GAME_7A260();
            Ord10887(p2, 0x160, bVar6, 0, uStack_c);
        }
    }
    (void)piVar17;
    return uStack_1c;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c D2Game+0xAF450 -- FUN_6fccf450(undefined4 param_1, int *param_2); unaff_EDI is the caller's
// record pointer (its fields at +0xc / +0x0 are read on the tail path).
extern "C" I d2_fn_d2game_AF450(U p1, I p2, I edi)
{
    if (p2 == 0 || R32(p2, 0) != 0) return 0;
    R32(p2, 0xc4) |= 0x40;
    I piStack_1c = p2;
    I iVar5 = Ord10331();
    U uVar6 = (U)Ord10826(iVar5);
    U uVar4 = (U)Ord10459(uVar6) & 0xff;
    I iVar7 = Ord10585(uVar4);
    I iVar8 = Ord10057(iVar5);
    I iVar5r = iVar5;
    if (iVar8 == 0) {
        iVar8 = Ord10826(iVar5);
        if (iVar8 != 0x88) {
            D2GAME_386F0();
            iVar8 = 0x3b;
            I *ppiVar11 = &piStack_1c;   // passed by address, as the decompile does
            piStack_1c = 0;
            U uVar9 = (U)D2GAME_1130(p2);
            I puVar10 = D2GAME_1100(p2);
            iVar5 = D2GAME_A24C0(p1, iVar5r, puVar10, uVar9, iVar7, (I)ppiVar11, iVar8);
            D2GAME_DFFD0(7, 0);
            I piVar3 = piStack_1c;
            if (iVar5 != 0) {
                I iVar7b = (piStack_1c == 0) ? -1 : R32(piStack_1c, 0xc);
                D2GAME_37480(iVar7b);
                if (piVar3 != 0) {
                    D2GAME_2E4F0(R32(p2, 0xc));
                    iVar7 = D2GAME_E1570(p1, piVar3);
                    if (iVar7 != 0) {
                        D2GAME_2E4F0(R32(p2, 0xc));
                        D2GAME_5E3C0();
                    }
                    D2GAME_5E3C0();
                }
            }
            if (edi != 0) {
                U uVar1 = R32(edi, 0xc);
                U uVar2 = R32(edi, 0);
                if (R32(p2, 0) == 0) {
                    Ord10920(p2);
                    D2GAME_8C3B0(uVar2 & 0xff, uVar1);
                    return piStack_1c;
                }
                D2GAME_8C3B0(uVar2 & 0xff, uVar1);
            }
            return iVar5;
        }
    }
    D2GAME_DFFD0(0x18, p2);
    return 0;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c D2Game+0xB1820 -- FUN_6fcd1820(int param_1, undefined4 param_2, int param_3, undefined4 *param_4,
// int *param_5); the receiver is in EAX (in_EAX). Returns 1 with *param_4/*param_5 filled, else 0.
// The decompile's _exit(-1) on a null game pointer is kept as std::abort().
extern "C" U d2_fn_d2game_B1820(I eax, I param_1, U param_2, I param_3, U *param_4, int32_t *param_5)
{
    I in_EAX = eax;
    I piVar5 = 0;
    I iVar3 = 0;
    I iVar4 = 0;
    I piStack_28 = 0;
    I iStack_24 = 0x7fffffff;
    U uStack_20 = 0, uStack_1c = 0, uStack_18 = 0, uStack_14 = 0, uStack_10 = 0;
    U uVar1;
    U uVar2;
    bool haveTarget = false;

    if (in_EAX == 0 || R32(in_EAX, 0) != 1 || R32(in_EAX, 0x14) == 0) return 0;
    iVar3 = (I)R32(R32(in_EAX, 0x14), 0x38);
    if (iVar3 == 0) return 0;

    auto fatal = [&]() {
        Ord10265(0x2d3);
        Ord10024((I)(uintptr_t)0, 0);   // DAT_6fd19ff9 message, retail data not dumped here
        std::abort();
    };

    switch (iVar3) {
    case 1:
        uVar1 = (U)D2GAME_48560(in_EAX);
        if (param_1 == 0) { fatal(); }
        for (piVar5 = (I)R32(param_1, 0x1120 + (uVar1 & 0x7f) * 4);
             piVar5 != 0; piVar5 = (I)R32(piVar5, 0xec)) {
            if (R32(piVar5, 0xc) == uVar1) { haveTarget = true; break; }
        }
        break;
    case 2:
        uVar1 = (U)D2GAME_48560(in_EAX);
        if (param_1 == 0) { fatal(); }
        for (piVar5 = (I)R32(param_1, 0x1320 + (uVar1 & 0x7f) * 4);
             piVar5 != 0; piVar5 = (I)R32(piVar5, 0xec)) {
            if (R32(piVar5, 0xc) == uVar1) { haveTarget = true; break; }
        }
        break;
    case 3: {
        I iVar3b = Ord10830();
        uVar2 = (U)D2GAME_1450(2);
        if (iVar3b == 1 || (uVar2 != 0 && (iVar3b == 0 || iVar3b == 2))) {
            D2GAME_E0460();
        }
        piStack_28 = 0;
        iStack_24 = 0x7fffffff;
        if (param_3 == 0) {
            D2GAME_1130(in_EAX);
            D2GAME_1100(in_EAX);
            I uVar2b = Ord10331();
            uStack_18 = (U)Ord10916(uVar2b);
            uStack_20 = param_2;
            uStack_1c = 0x23;
            uStack_14 = 0;
            uStack_10 = 0x7fffffff;
        } else {
            uStack_20 = 0;
            uStack_1c = 0x7fffffff;
        }
        D2GAME_B17B0(&piStack_28, param_1, in_EAX, 0);
        iVar3 = iStack_24;
        piVar5 = piStack_28;
        D2GAME_E0460();
        goto found_tail;
    }
    case 4:
        uVar1 = (U)D2GAME_48560(in_EAX);
        if (param_1 == 0) { fatal(); }
        for (piVar5 = (I)R32(param_1, 0x1920 + (uVar1 & 0x7f) * 4);
             piVar5 != 0; piVar5 = (I)R32(piVar5, 0xec)) {
            if (R32(piVar5, 0xc) == uVar1) { haveTarget = true; break; }
        }
        break;
    default:
        break;
    }

    if (!haveTarget) goto no_target;
    iVar3 = D2GAME_B0A20(in_EAX);
    if (param_3 != 0 && Ord10839(piVar5) != 0) goto no_target;

found_tail:
    if (piVar5 != 0) {
        I iVar4b = S32(piVar5, 0);
        if (iVar4b > -1) {
            if (iVar4b < 2) {
                iVar4b = D2GAME_DFA90();
                if (iVar4b == 0) {
                    *param_4 = (U)piVar5;
                    *param_5 = (int32_t)iVar3;
                    return 1;
                }
            } else if (iVar4b == 3) {
                *param_4 = (U)piVar5;
                *param_5 = (int32_t)iVar3;
                return 1;
            }
        }
    }
no_target:
    if (R32(in_EAX, 0) == 1 && R32(in_EAX, 0x14) != 0) {
        I iVar3c = (I)R32(in_EAX, 0x14);
        R32(iVar3c, 0x38) = 0;
        R32(iVar3c, 0x34) = 0;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c D2Game+0xB3D60 -- FUN_6fcd3d60(uint param_1, int *param_2, int param_3); unaff_EDI is the
// caller's list pointer, unaff_retaddr the return-address slot passed to FUN_6fcfe0c0's frame.
// The 0x70-byte local block (uStack_70 at +0, local_74 at +4, zeroed from +4) is inferred from the
// decompile's stores; see MISMATCH notes in the spec once it lands.
extern "C" U d2_fn_d2game_B3D60(U param_1, I param_2, I param_3, I edi, I retaddr)
{
    if (param_3 < 0 || param_3 >= (I)S32(DTB(), 0xba0)) return 0;
    I iVar4 = param_3 * 0x23c + (I)R32(DTB(), 0xb98);
    if (iVar4 == 0) return 0;

    if (param_2 != 0) R32(param_2, 0xc4) |= 0x40;
    I unitSave = (I)param_2;   // uStack_84 is written then overwritten in the decompile
    (void)unitSave;
    D2GAME_E07A0(param_1);
    I piVar2 = Ord10392();     // uStack_84 = param_2[0xb] is read by Ord10392 in the decompile
    if (piVar2 == param_2 || piVar2 == 0) return 1;

    uint8_t blk[0x70];
    for (int i = 0; i < 0x70; i++) blk[i] = 0;
    uint16_t *uStack_70 = (uint16_t *)&blk[0];
    uint32_t *local_74 = (uint32_t *)&blk[4];
    *local_74 = param_1;
    *uStack_70 = (uint16_t)(*uStack_70 | 1 | U16(iVar4, 0x12e));
    *local_74 = *local_74 | R32(iVar4, 0x130);
    if (R32(iVar4, 0x134) != 0) { /* iStack_14 = R32(iVar4, 0x134) in the decompile */ }
    U uStack_8c = 1;
    I iStack_90 = param_3;
    I uStack_50 = Ord10510(param_2, retaddr);
    D2GAME_9F210();
    D2GAME_DE0C0(edi, param_2, 1, (I)&blk[0]);
    I iVar3 = Ord10973(piVar2, 6, 0);
    if (iVar3 == 0) uStack_8c |= 2;
    D2GAME_DCF20(edi, param_2, (I)&iStack_90);
    short sVar1 = S16(iVar4, 0x4e);
    if (0 < sVar1 && (int)sVar1 <= (int)S32(DTB(), 0xbc0)) {
        Ord10689(piVar2, (I)sVar1, 0);
    }
    (void)uStack_50;
    return 1;
}
