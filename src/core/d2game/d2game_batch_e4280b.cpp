// Implementation batch e4280b: D2Game functions the e4280 batch did not take (E57D0 E9600 EBE70 ECB50
// ECBD0 ECF30 ED880 ED960 EF570 F0090 F0FF0 F12C0 F18B0 F1FF0 F21E0 F42D0), written from the cached 1.13c
// decompiles. RE-DONE only, not linked into the engine: nothing live references these symbols, so the wasm
// link drops them. Retail callees are declared by RVA (r_HEX, D2Game base 0x6FC20000) or ordinal (oNNNNN);
// their signatures are not pinned, so they are variadic and return a widest value. Register and frame
// slots the decompile shows as unaff_* / in_EAX / iStack_* / piStack_* are explicit parameters or locals.
#include <cstdint>
#include <cstdio>
#include <cstring>

using i32 = int32_t; using u32 = uint32_t; using i16 = int16_t; using u16 = uint16_t; using u8 = uint8_t;
using i8 = int8_t; using u64 = uint64_t;
using P = intptr_t;   // a retail address held as a host value
using R = intptr_t;   // a callee's EAX, widest form

template <class T> static inline T ld(P p) { T v; std::memcpy(&v, (const void*)p, sizeof v); return v; }
template <class T> static inline void st(P p, T v) { std::memcpy((void*)p, &v, sizeof v); }
static inline i32 rd(P p, i32 off) { return ld<i32>(p + off); }
static inline P rp(P p, i32 off) { return (P)ld<u32>(p + off); }        // 32-bit retail pointer field
static inline u32 ru(P p, i32 off) { return ld<u32>(p + off); }
static inline u16 rw(P p, i32 off) { return ld<u16>(p + off); }
static inline u8 rb(P p, i32 off) { return ld<u8>(p + off); }
static inline void wr(P p, i32 off, i32 v) { st<i32>(p + off, v); }
static inline void wp(P p, i32 off, P v) { st<u32>(p + off, (u32)v); }
static inline void ww(P p, i32 off, u16 v) { st<u16>(p + off, v); }
static inline void wb(P p, i32 off, u8 v) { st<u8>(p + off, v); }
#define AD(x) ((P)&(x))

// Retail data the decompile names. The retail image provides these; here they are placeholders for the link.
extern u8 DAT_6fd19ff9[];        // error-format string
extern u8 DAT_6fd228d0[];        // 8-byte stride table: u32 at +0, i32 at +4
extern u32 gdwBitMasks_exref[];
extern u8 sgptDataTables_exref[];  // holds the data-table base pointer (u32)
static inline P sdtBase() { return (P)ld<u32>((P)sgptDataTables_exref); }

[[noreturn]] void d2_exit(int);
// IsBadCodePtr stand-in: the retail call probes a code pointer; here only null is caught.
static inline bool is_bad_code_ptr(P p) { return p == 0; }
u32 GetTickCount(void);

// Retail callees (variadic: signatures not pinned).
// Retail callees (variadic: signatures not pinned).
R o10001(...);
R o10003(...);
R o10024(...);
R o10057(...);
R o10066(...);
R o10123(...);
R o10126(...);
R o10127(...);
R o10128(...);
R o10140(...);
R o10141(...);
R o10158(...);
R o10174(...);
R o10259(...);
R o10265(...);
R o10268(...);
R o10286(...);
R o10324(...);
R o10331(...);
R o10387(...);
R o10422(...);
R o10448(...);
R o10454(...);
R o10459(...);
R o10494(...);
R o10587(...);
R o10630(...);
R o10658(...);
R o10671(...);
R o10750(...);
R o10775(...);
R o10826(...);
R o10867(...);
R o10887(...);
R o10913(...);
R o10920(...);
R o10945(...);
R o10973(...);
R o10990(...);
R o11005(...);
R o11056(...);
R o11067(...);
R o11090(...);
R o11103(...);
R o11153(...);
R o11156(...);
R r_211D0(...);
R r_33880(...);
R r_33920(...);
R r_34130(...);
R r_35EB0(...);
R r_36470(...);
R r_366A0(...);
R r_38620(...);
R r_48BA0(...);
R r_49A20(...);
R r_49B40(...);
R r_54810(...);
R r_54D90(...);
R r_75070(...);
R r_75E10(...);
R r_7A3A0(...);
R r_8CF80(...);
R r_8EA80(...);
R r_96680(...);
R r_98120(...);
R r_9BEB0(...);
R r_9C160(...);
R r_AA3E0(...);
R r_AF8D0(...);
R r_C1030(...);
R r_D51A0(...);
R r_DA750(...);
R r_DC0B0(...);
R r_DCF20(...);
R r_DE0C0(...);
R r_DFA60(...);
R r_DFA90(...);
R r_DFCC0(...);
R r_E0040(...);
R r_E0110(...);
R r_E0F40(...);
R r_E1250(...);
R r_E1D90(...);
R r_E20A0(...);
R r_E41D0(...);
R r_E660(...);
R r_ED710(...);
R r_EEF10(...);
R r_F02F0(...);
R r_F0640(...);
R r_F0720(...);
R r_F4260(...);

// Error exit as the decompile does it: format the code, print it, exit.
static inline void d2_fatal(i32 code) { R m = o10265(code); o10024((P)DAT_6fd19ff9, m); d2_exit(-1); }

// 1.13c D2Game+0xE57D0 (ecx = game, edx = record; eax and edi are the decompile's register inputs).
u32 D2GAME_E57D0(P game, P rec, i32 edi, u32 eax)
{
    P st_ = rp(rec, 0x18);                                   // iVar4
    u32 piVar2 = (eax & 0xFFFFFF00u) | rb(st_, 0x0C);        // CONCAT31(eax >> 8, byte +0xC)
    i8 cStack_14 = 0; i32 iStack_10 = 0; u8 auStack_c[12] = {};
    if (rb(st_, 0x0C) == 0) {
        r_DFCC0(game, 0);                                     // FUN_6fcffcc0(param_1, 0)
        wb(st_, 0x0C, 1);
        wb(st_, 0x0D, 0);
        if (game == 0) d2_fatal(0x2D3);
        piVar2 = (u32)rd(st_, 0x10) & 0x7F;
        P iVar5 = rp(game, (i32)piVar2 * 4 + 0x1320);
        if (iVar5 != 0) {
            while (ru(iVar5, 0x0C) != ru(st_, 0x10)) {
                iVar5 = rp(iVar5, 0xE4);
                if (iVar5 == 0) return 1;
            }
            P g2 = rp(game, 0x10F4);
            if (g2 == 0) d2_fatal(0x1FB);
            u64 lVar1 = (u64)ru(g2, 0x18) * 0x6AC690C5ull + ru(g2, 0x1C);
            st<u64>(g2 + 0x18, lVar1);
            R iVar4 = o10331(iVar5);
            piVar2 = (u32)o10141(iVar5, AD(auStack_c));
            for (u32 uVar3 = (u32)lVar1 & 1; uVar3 < 8; uVar3++) {
                u32 uVar6 = ld<u32>((P)DAT_6fd228d0 + uVar3 * 8);
                i32 iv5 = ld<i32>((P)DAT_6fd228d0 + 4 + uVar3 * 8);
                r_34130(iVar4, AD(cStack_14), 3, 0x3F11);     // out slot: decompile's stack0xffffffe4
                piVar2 = 0;
                if (edi != 0) piVar2 = (u32)r_E1D90(2, 0x83, cStack_14 + (i8)uVar6, iStack_10 + iv5, game);
            }
        }
    }
    return (piVar2 & 0xFFFFFF00u) | 1;                       // CONCAT31(piVar2 >> 8, 1)
}

// 1.13c D2Game+0xE9600 (ecx = self, edx = event record; si and edi are the decompile's register inputs).
void D2GAME_E9600(P self, P rec, i32 si, P edi)
{
    i32 sv = (i16)rd(rec, 0x14);                             // (short)param_2[5]
    P iVar3 = rp(rec, 0);                                    // *param_2: the owner record
    P iVar2 = (P)o10920(rp(rec, 0x0C));                      // Ordinal_10920(param_2[3])
    P u = rp(iVar2, 0x10 + rb(iVar3, 0x6D) * 4);
    if (o10174(u, 0x13, 0) == 1) return;
    if (sv == 0xFF) {
        if (si == 0x24B) {
            if (r_E660(0x20333367) != 0 && o10174(u, 0x13, 5) != 0 && o10174(u, 0x13, 8) == 0) {
                wb(rp(self, 0x18), 7, 1);
                r_366A0(rp(rec, 0));                         // FUN_6fc566a0(*param_2)
                wr(rp(edi, 0x18), 0x1C, rd(rp(edi, 0x18), 0x1C) - 1);
                o10003(u, 0x13, 6);
                i16 sv2 = (i16)r_DFA60();
                if (sv2 != -1) r_9C160((P)&r_E41D0);         // FUN_6fcbc160(FUN_6fd041d0)
                r_DFCC0(0, 0);                               // FUN_6fcffcc0(this, 0): this (extraout_ECX) not pinned
                self = edi;                                  // the decompile reassigns param_1 here
            }
            if (rb(self, 9) != 0) r_33920(0x13, 0xD);
            wb(self, 0x14, 0);
            wb(self, 0x0B, 0xD);
            if (rb(self, 9) == 0) return;
            if (rb(self, 0x0C) == 5) return;
            r_35EB0();
            if (is_bad_code_ptr(rp(edi, 0xF0))) d2_fatal(0x26E);
            reinterpret_cast<void (*)()>(rp(edi, 0xF0))();   // (**(code **)(unaff_EDI + 0xf0))()
            return;
        }
        if (si != 0x251) return;
        if (o10174(u, 0x13, 6) == 0) return;
        if (o10174(u, 0x13, 8) != 0) return;
        o10003(u, 0x13, 8);
        r_36470(0x206E6972, rp(rec, 0x0C), rp(rec, 0));       // FUN_6fc56470(0x206e6972, param_2[3], *param_2)
        if (o10174(u, 0x13, 7) == 0) return;
        o10003(u, 0x13, 0xD);
        o10003(u, 0x13, 0);
        r_33880();                                           // both arms of the decompile end in FUN_6fc53880
        return;
    }
    if (sv == 0xFD) {
        if (si != 0x23B) return;
        r_35EB0();
        wb(rp(edi, 0x18), 1, 1);
        return;
    }
    if (sv != 0xFC) return;
    if (si != 0x24D) return;
    if (o10174(u, 0x13, 7) != 0) return;
    o10003(u, 0x13, 7);
    r_C1030(rp(self, 4), rp(rec, 0x0C), 0xFC);
    if (o10174(u, 0x13, 8) == 0) return;
    o10003(u, 0x13, 0xD);
    o10003(u, 0x13, 0);
    r_33880();
}

// 1.13c D2Game+0xEBE70 (iStack_8 = unit, param_2 = cursor slot, param_3 = end, piStack_14 = length out).
u32 D2GAME_EBE70(P unit, P cursorRef, P endPtr, P lenOut, u32 unaffEbp)
{
    P puVar9 = rp(cursorRef, 0) + 2;                         // (undefined2 *)*param_2 + 1
    if (endPtr < puVar9) return 1;
    ww(rp(cursorRef, 0), 0, 0x6667);                         // *(undefined2 *)*param_2 = 0x6667
    P puStack_102c = puVar9;
    P local_1020[3] = {puVar9, 0, 0};
    u8 local_1014[8] = {};
    o10126(AD(local_1014));
    u8 stk[0x3200];                                          // auStack_100c / auStack_1008 frame block
    std::memset(stk, 0, sizeof stk);
    i32 iVar7 = 0, iVar3 = 0;
    P list = rp(unit, 0x5C);
    if (list != 0 && (iVar3 = o10658(list, (P)stk + 0x100C, 0x200)) > 0) {
        iVar7 = 0;
        do {
            u16 uVar6 = ld<u16>((P)stk + 0x1006 + iVar7 * 8);          // *(ushort*)(auStack_1008 + k*8 - 2)
            u32 uVar2 = ld<u32>((P)stk + 0x1008 + iVar7 * 8);          // auStack_1008[k*2]
            P iVar8 = 0; u8 bVar1 = 0;
            if ((i32)uVar6 < rd(sdtBase(), 0xBD4) &&
                (iVar8 = (P)(u32)uVar6 * 0x144 + rp(sdtBase(), 0xBCC)) != 0 &&
                (bVar1 = rb(iVar8, 10)) != 0) {
                u32 uVar5 = uVar2;
                if (bVar1 < 0x20) {
                    u32 uVar4;
                    if ((((u8)gdwBitMasks_exref[0x14]) & rb(iVar8, 5)) == 0) {
                        uVar4 = (1u << (bVar1 & 0x1F)) - 1;
                        uVar5 = uVar2 & (u32)(((i32)uVar2 < 1) - 1);
                        if ((i32)uVar4 <= (i32)uVar5) uVar5 = uVar4;     // goto LAB_6fd0bf99
                    } else {
                        uVar5 = (u32)-1 << ((bVar1 - 1) & 0x1F);
                        uVar4 = (1u << ((bVar1 - 1) & 0x1F)) - 1;
                        if ((i32)uVar5 < (i32)uVar2) uVar5 = uVar2;
                        if ((i32)uVar4 <= (i32)uVar5) uVar5 = uVar4;
                    }
                }
                o10128(AD(local_1020[0]), uVar6, 9);
                if (rb(iVar8, 0x0B) != 0) o10128(AD(puStack_102c), (i32)(unaffEbp & 0xFFFF), rb(iVar8, 0x0B));
                o10128(AD(puStack_102c), uVar5, rb(iVar8, 10));
            }
            iVar7++;
            puVar9 = puStack_102c;
        } while (iVar7 < iVar3);
    }
    o10128(AD(local_1020[0]), 0x1FF, 9);
    iVar3 = o10127(AD(puStack_102c));
    wr(lenOut, 0, iVar3 + (i32)puVar9);                      // *piStack_14 = iVar3 + (int)puVar9
    return 0;
}

// 1.13c D2Game+0xECB50 (edx = op, this = *ebx, in_EAX = end; edi = the unaff_EDI source).
u32 D2GAME_ECB50(u32 op, P src, P edi, P eax, P ebx, P errOut)
{
    P cur = rp(ebx, 0);                                      // this = *unaff_EBX
    u32 out = 0;                                             // param_3 (output size)
    i32 iVar1;
    if (op == 0x47) {
        iVar1 = r_54D90(cur, src, edi, eax - cur, 0, AD(out));
    } else if (op < 0x48 || op > 0x60) {
        iVar1 = 1;
    } else {
        iVar1 = r_54810(src, edi, op, eax - cur, 0, AD(out));
    }
    if (iVar1 == 0) {
        wp(ebx, 0, cur + (P)out);                            // *unaff_EBX = param_3 + this
        return 0;
    }
    if (errOut != 0) wr(errOut, 0, iVar1);
    return 0x14;
}

// 1.13c D2Game+0xECBD0 (__thiscall: this = flag, param_1 = cursor slot, param_3 = error out; in_EAX = end, esi = unit).
u32 D2GAME_ECBD0(P self, P cursor, u32 op, P errOut, P eax, P esi)
{
    P puVar2 = cursor;
    P psVar1 = rp(cursor, 0) + 2;                            // (short*)*param_1 + 1
    if (rd(esi, 0x70) != 0 && psVar1 <= eax) {
        if (rw(rp(cursor, 0), 0) != 0x666A) {
            if (errOut != 0) wr(errOut, 0, 0x65);
            return 0x16;
        }
        if (self != 0) {
            P local_p1 = 0;                                  // param_1 local, the cursor for FUN_6fd0cb50
            i32 iVar3 = (i32)D2GAME_ECB50(op, esi, 0, eax, AD(local_p1), AD(local_p1));
            if (iVar3 != 0) {
                if (errOut != 0) wr(errOut, 0, (i32)local_p1);
                return 0x16;
            }
            r_D51A0(esi, self);
            if (r_DFA90() == 0) {
                R uVar4 = o10913(self, 0);
                o10887(self, 6, uVar4);
            }
        }
        wp(puVar2, 0, psVar1);                               // *puVar2 = (uint)psVar1
    }
    return 0;
}

// 1.13c D2Game+0xECF30 (ecx = unit, edx = flag pointer, cursor and end on the stack, ebx = config, esi = game).
u32 D2GAME_ECF30(P self, P flagPtr, P cursorRef, P endPtr, P errOut, P ebx, P esi, P piStack_c)
{
    P psVar6 = rp(cursorRef, 0);                             // (short*)*param_3
    i32 iVar3 = 0;
    if (flagPtr == 0 || rd(flagPtr, 0) != 0 || (iVar3 = o10920(), rd((P)iVar3, 0x9C) == 0)) {
        if (errOut != 0) wr(errOut, 0, 0x65);
        return 0x15;
    }
    if (endPtr < psVar6 + 4) {                               // psVar6 + 2 shorts
        if (errOut != 0) wr(errOut, 0, 0x66);
        return 0x15;
    }
    if (rw(psVar6, 0) == 0x4D4A) {
        u16 uVar1 = rw(psVar6, 2);
        psVar6 += 4;                                         // psVar6 + 2 shorts
        if (uVar1 > 1) {
            if (errOut != 0) wr(errOut, 0, 0x68);
            return 0x15;
        }
        u32 uStack_10 = 0;
        if (uVar1 != 0) {
            do {
                psVar6 += 12;                                // psVar6 + 6 shorts
                if (endPtr < psVar6) {
                    if (errOut != 0) wr(errOut, 0, 0x69);
                    return 0x15;
                }
                u32 uVar5 = (u32)rd(flagPtr, 4);             // param_2[1]
                P puVar4 = 0;
                if (uVar5 > 6 || (puVar4 = o11153(), puVar4 == 0)) {
                    if (errOut != 0) wr(errOut, 0, 0x6A);
                    return 0x15;
                }
                wr(puVar4, 0, 0);
                wr(puVar4, 4, (i32)uVar5);
                wp(puVar4, 8, rp(self, 0x1C));
                wp(puVar4, 0x80, self);                      // puVar4[0x20] = param_1
                if (rd(self, 0x70) == 0) uVar5 = ru(puVar4, 0xC8) & 0xFDFFFFFF;
                else uVar5 = ru(puVar4, 0xC8) | 0x2000000;
                wr(puVar4, 0xC8, (i32)uVar5);
                wr(puVar4, 0xC8, (i32)(uVar5 | 0x4000000));
                o10826();
                u8 bVar2 = (u8)o10459();
                wb(puVar4, 0x18, bVar2);
                wp(puVar4, 0x1C, rp(self, (i32)bVar2 * 4 + 0xBC));
                wr(puVar4, 0x74, 0);
                wr(puVar4, 0xC4, rd(puVar4, 0xC4) | 0x10);
                wr(puVar4, 0xD0, 0xB);
                i32 iVar3b = rd(self, 0x90) + 1;
                if (iVar3b == -1) iVar3b = 1;
                wr(self, 0x90, iVar3b);
                r_38620(iVar3b);
                r_E0F40(0, puVar4, 0, self);
                if (rd(puVar4, 0x10) == 0x11 || rd(puVar4, 0x10) == 0) {
                    o10775();
                    o10671(puVar4, 5);
                    o11056(rp(puVar4, 0x2C), 0x8000);
                }
                wr(puVar4, 0xC4, rd(puVar4, 0xC4) & ~4);
                r_E0040();
                r_49A20();
                o10140();
                r_E0110();
                o10945(puVar4, 7, 1);
                wr(puVar4, 0xC4, rd(puVar4, 0xC4) & ~0x10);
                o10268(rp(puVar4, 0x60), rp(ebx, 0x0C));
                i32 iVar7 = 0;
                u32 stk_d0 = 0;
                iVar3 = (i32)D2GAME_ECB50(uStack_10, self, 0, endPtr, ebx, AD(stk_d0));
                if (iVar3 != 0) {
                    if (piStack_c != 0) wr(piStack_c, 0, iVar7);
                    return 0x15;
                }
                o10990(rp(esi, 0x60), rd(puVar4, 0x0C), 0, 1);
                r_8CF80(1);
                uStack_10++;
            } while ((i32)uStack_10 < (i32)uVar1);
        }
        wp(cursorRef, 0, psVar6);                            // *param_3 = (uint)psVar6
        return 0;
    }
    if (errOut != 0) wr(errOut, 0, 0x67);
    return 0x15;
}

// 1.13c D2Game+0xED880 (in_EAX = unit, unaff_EBX = end, unaff_ESI = game; param_1 = cursor slot).
u32 D2GAME_ED880(P param_1, P unit, P end, P game, P retaddr)
{
    P psVar1 = rp(param_1, 0);                               // (short *)*param_1
    P local_4 = 0;
    if (rd(unit, 0x70) != 0 && psVar1 + 2 <= end) {          // psVar1 + 1 shorts
        if (rw(psVar1, 0) != 0x666B) return 0x17;
        local_4 = psVar1 + 3;                                // (int)psVar1 + 3
        if (rb(psVar1, 2) != 0) {                            // (char)psVar1[1]
            i32 iVar3 = o10630();
            if (iVar3 == 0) return 0x17;
            o11103();
            u32 stk_f4 = 0;
            iVar3 = r_ED710(game, AD(local_4), AD(stk_f4), end, retaddr);
            P piVar2 = local_4;
            if (iVar3 == 0) return 0x17;
            iVar3 = 3;
            o11090(local_4);
            i32 iVar4 = (piVar2 == 0) ? -1 : rd(piVar2, 0x0C);     // piVar2[3]
            if (game != 0 && rd(game, 0) == 0) {
                o10920();
                r_49B40(iVar4);
                wp(param_1, 0, iVar3);                       // *param_1 = iVar3
                return 0;
            }
            r_49B40(iVar4);
            local_4 = game;
        }
        wp(param_1, 0, local_4);                             // *param_1 = (int)local_4
    }
    return 0;
}

// 1.13c D2Game+0xED960 (src = the 8-dword record at in_EAX; the decompile's uStack_*/sStack_*/pvStack_28
// slots are explicit parameters here, see the MISMATCH note in the hand-back).
P D2GAME_ED960(P param_1, P src, P pvStack_28, u32 uStack_2c, u16 uStack_22, i16 sStack_24, u32 uStack_4)
{
    if (rd(src, 0x0C) == 0 && rd(src, 0x04) == 0 && rw(src, 0x08) == 0) return 0;
    u8 local_20[32];
    for (int i = 0; i < 32; i++) local_20[i] = rb(src, i);   // 8 dwords copied into the frame block
    u16 local_16 = rw(AD(local_20), 8);
    u32 uVar5 = 0;
    u32 uVar12 = ru(param_1, 0x70);
    i32 iVar6 = o11156(uVar12, local_16, 1);
    if (iVar6 == 0) return 0;
    u16 uVar8 = (u16)(sStack_24 + rw(iVar6, 0x114));
    if (rw(iVar6, 0x116) < uVar8) uVar8 = rw(iVar6, 0x114);
    u32 uVar7 = uStack_2c & 0x10000;
    P piVar1 = r_E1250((u16)uStack_4, ((u32)uStack_22 << 16) | uVar8, (i16)(P)pvStack_28,
                       ((u32)ld<u16>(AD(local_20)) << 16) | uStack_22, rd(iVar6, 0x08));
    if (piVar1 == 0) return 0;
    r_96680();
    o10973(uStack_4, 0x0C, 0);
    u32 uVar2 = o10066(0);
    i32 iVar11 = 0x0D;
    iVar6 = 1;
    i32 piVar10 = (i32)piVar1;
    u32 uVar3 = o10973(piVar1, 0x0D, 0);
    if (uVar3 < uVar7) o10887(piVar1, 0x0D, uVar7, 0);
    r_7A3A0();
    i32 iVar4 = o11156(rd(pvStack_28, 0x70), uVar5 >> 0x10, 1, uVar7);
    while (iVar4 != 0) {
        iVar6++;
        if (piVar10 < iVar6 || (uVar5 = o10448(iVar6, rd(iVar4, 0x20)), uVar12 < uVar5)) {
            r_48BA0(pvStack_28, uStack_22, piVar1);
            if (iVar11 != 0) {
                r_98120(pvStack_28, uStack_22);
                wr(piVar1, 0xC4, rd(piVar1, 0xC4) | 0x10000);   // piVar1[0x31]
                r_DA750();
                o11090(piVar1, 0x0C);
                r_AF8D0();
            }
            if (rd(piVar1, 0x60) == 0) o10387(0, piVar1);       // piVar1[0x18]
            return piVar1;
        }
        iVar4 = o11156(rd(pvStack_28, 0x70), uVar2 >> 0x10, iVar6);
    }
    r_E20A0(pvStack_28, piVar1);
    return 0;
}

// 1.13c D2Game+0xEF570 (__fastcall: ecx unused, edx = record).
void D2GAME_EF570(P unused, P param_2)
{
    (void)unused;
    P piStack_30 = param_2;
    o10331();
    i32 iVar1 = rd(param_2, 0);
    if (iVar1 != 2 && ((iVar1 < 4 || 5 < iVar1) && rd(param_2, 0x2C) != 0)) o10867(rd(param_2, 0x2C));
    iVar1 = rd(param_2, 0);
    if (iVar1 != 2 && (iVar1 < 4 || 5 < iVar1)) {
        if (rd(param_2, 0x2C) == 0) {
            P slot = 0;
            r_EEF10(AD(slot));
            return;
        }
        o10750(rd(param_2, 0x2C));
        r_EEF10(AD(piStack_30));
        return;
    }
    P slot = 0;
    r_EEF10(AD(slot));
}

// 1.13c D2Game+0xF0090 (in_EAX = unit, unaff_EDI = unit being compared, unaff_retaddr = return value to store).
void D2GAME_F0090(P in_EAX, P unaff_EDI, P unaff_retaddr)
{
    P iVar2 = o10920();
    i32 iVar4 = (in_EAX == 0) ? -1 : rd(in_EAX, 0x0C);
    if (unaff_EDI != in_EAX) {
        for (P piVar1 = rp(iVar2, 0x38); piVar1 != 0; piVar1 = rp(piVar1, 0x10)) {
            if (rd(piVar1, 0) == iVar4) {
                wp(piVar1, 0x08, unaff_retaddr);
                break;
            }
        }
    }
    iVar2 = o10920();
    if (iVar2 != 0) wr(iVar2, 0x160, (i32)GetTickCount());
}

// 1.13c D2Game+0xF0FF0 (in_EAX = the first unit, param_1 = mode, param_2 = the second unit).
void D2GAME_F0FF0(P param_1, P param_2, P in_EAX)
{
    if (in_EAX != 0 && rd(in_EAX, 0x80) != 0 && rd(in_EAX, 0) == 0) {
        i16 sVar1 = (i16)r_9BEB0();
        if (sVar1 != -1) {
            i32 iVar3 = o10587(param_2, 0x0C, 0);
            if (8 < iVar3) r_9C160((P)0x6FD10CD0);           // LAB_6fd10cd0
            return;
        }
    }
    if (param_2 != 0 && rd(param_2, 0) == 0) {
        r_F02F0(param_2);
        r_F0640(param_2, param_1);
        if (in_EAX != 0 && rd(in_EAX, 0) == 0) {
            r_F02F0(in_EAX);
            r_F0640(in_EAX, param_1);
            if (rd(in_EAX, 0) == 0) {
                P iVar3 = o10920();
                r_F0720(rd(iVar3, 0x9C), 3);
                return;
            }
            r_F0720(0, 3);
            return;
        }
    }
    d2_fatal(0x24);
}

// 1.13c D2Game+0xF12C0 (param_1 = unit; unaff_EDI = the second coordinate; uStack_10 = the unset frame slot, 0 here).
u32 D2GAME_F12C0(P param_1, P unaff_EDI)
{
    u32 uStack_10 = 0;
    P iVar6 = o10331(param_1);
    if (iVar6 == 0) return 0;
    i32 iVar10 = rd(param_1, 0);
    if (iVar10 != 2 && (iVar10 < 4 || 5 < iVar10) && rd(param_1, 0x2C) != 0) o10867(rd(param_1, 0x2C));
    iVar10 = rd(param_1, 0);
    if (iVar10 != 2 && (iVar10 < 4 || 5 < iVar10) && rd(param_1, 0x2C) != 0) o10750(rd(param_1, 0x2C));
    P puVar1 = param_1 + 0x20;                               // (ulonglong *)(param_1 + 8)
    u32 uVar7 = o10123(param_1);
    u32 uVar8 = o10259(rd(param_1, 0x2C));
    for (iVar10 = 0; iVar10 < 10; iVar10++) {
        u64 uVar4 = (u64)ru(puVar1, 0) * 0x6AC690C5ull + (u64)ru(param_1, 0x24);
        wr(puVar1, 0, (i32)(u32)uVar4);
        i32 iVar2 = (i32)param_1 + (i32)((u32)(uVar4 & 0xFFFFFFFFu) % 0x28) - 0x14;
        uVar4 = (uVar4 & 0xFFFFFFFFu) * 0x6AC690C5ull + (uVar4 >> 0x20);
        st<u64>(puVar1, uVar4);
        i32 iVar3 = (i32)unaff_EDI - 0x14 + (i32)((u32)(uVar4 & 0xFFFFFFFFu) % 0x28);
        i16 sVar5 = (i16)o10001(uStack_10, iVar2, iVar3, uVar7, uVar8);
        if (sVar5 == 0) {
            o10286(rd(iVar6, 0x2C), iVar2, iVar3);
            return 1;
        }
    }
    return 0;
}

// 1.13c D2Game+0xF18B0 (__thiscall: this = flag, param_1 = item, param_2 = the value to show; in_EAX = unit,
// unaff_EBX = the game/owner record).
void D2GAME_F18B0(P self, P param_1, u16 param_2, P in_EAX, P unaff_EBX)
{
    if (param_2 == 0) return;
    if (self != 0) {
        if (in_EAX != 0 && rd(in_EAX, 0) == 0) o10920();
        u8 local_50[40] = {};                                // the 40-byte record the decompile copies
        local_50[0] = 0x27;
        i32 v = (param_1 == 0) ? -1 : rd(param_1, 0x0C);     // local_50._2_4_
        std::memcpy(&local_50[2], &v, 4);
        local_50[1] = 1;
        local_50[6] = 1;                                     // uStack_4a
        local_50[8] = 3;                                     // uStack_48
        std::memcpy(&local_50[10], &param_2, 2);             // uStack_46
        r_AA3E0((P)local_50, 0x28);
        return;
    }
    char local_5c[12];
    std::sprintf(local_5c, "%d", (unsigned)param_2);
    if (param_1 != 0 && rd(param_1, 0xA4) != 0) o10422(rd(unaff_EBX, 0x1C), rd(param_1, 0xA4));
    i32 iVar2 = o10454(rd(unaff_EBX, 0x1C), AD(local_5c), rd(unaff_EBX, 0xA8));
    if (iVar2 != 0) {
        if (param_1 != 0) wr(param_1, 0xA4, iVar2);
        o10158(param_1);
        if (param_1 != 0) wr(param_1, 0xC4, rd(param_1, 0xC4) | 0x100);
        u32 uVar1 = (u32)o11067(iVar2);
        r_8EA80(unaff_EBX, param_1, 6, uVar1, 0, 0, 0);
    }
}

// 1.13c D2Game+0xF1FF0 (__thiscall: this = unit, param_1 = value; in_EAX = the record; uStack_4 = the stack slot).
void D2GAME_F1FF0(P self, u32 param_1, P in_EAX, u32 uStack_4)
{
    i32 local_20[2] = {0, 0};
    r_75070(in_EAX);
    o11005(rd(self, 0x2C), 1);
    u32 uStack_18 = param_1;
    (void)uStack_18;
    r_75E10(uStack_4, AD(local_20));
}

// 1.13c D2Game+0xF21E0 (param_1..param_3 as the decompile names them; in_EAX = the record).
void D2GAME_F21E0(u32 param_1, u32 param_2, u32 param_3, P in_EAX)
{
    i32 local_20[3] = {0, 0, 0};
    u32 local_14 = param_2;
    u32 local_10 = param_3;
    r_75070(in_EAX);
    (void)local_14; (void)local_10;
    r_75E10(param_1, AD(local_20));
}

// 1.13c D2Game+0xF42D0 (param_1..param_3 as the decompile names them; iStack_4, piStack_40 and piStack_4c are the
// frame slots the decompile reads without setting, so they are explicit parameters here, see the MISMATCH note).
i32 D2GAME_F42D0(u32 param_1, P param_2, u32 param_3, P iStack_4, P piStack_40, P piStack_4c)
{
    o10331();
    o10826();
    i32 iVar2 = o10057();
    if (iVar2 == 0 && (iVar2 = o10324()) == 0) {
        P puVar6 = iStack_4 + 0x20;                          // (ulonglong *)(iStack_4 + 0x20)
        if (param_3 == 0) {
            R m = o10265();
            o10024((P)DAT_6fd19ff9, m);
            d2_exit(-1);
        }
        u32 fr[0x30] = {};                                   // frame block; fr[0] = uStack_c0, fr[1] = uStack_bc,
        fr[3] = 0;                                           // fr[2] = uStack_b8, fr[3] = uStack_b4
        fr[2] = 0x0C;
        fr[1] = param_3;
        fr[0] = 0x6FD14376;
        o10973(param_3, 0x0C, 0);
        fr[0] = 0x6FD14384;
        r_211D0(puVar6);
        fr[0] = 0;
        i32 iVar3 = o10973(param_3, 2);
        u32 uVar5 = fr[2] >> 0x18;
        i32 iVar4 = o10973(param_3, 0x1F, 0);
        iVar3 = (((i32)uVar5 + (iVar3 >> 1) * -5) - iVar2) * 2 - iVar4 + 0x7D;
        if (iVar3 < 0x41) iVar3 = 0x41;
        u64 uVar1 = (u64)ru(puVar6, 0) * 0x6AC690C5ull + (u64)ru(iStack_4, 0x24);
        st<u64>(puVar6, uVar1);
        uVar1 &= 0xFFFFFFFFu;
        if ((i32)(uVar1 % 100) < iVar3) {
            uVar5 = (u32)r_F4260(piStack_40);
            iVar2 = 0;
            if (uVar5 != 0) {
                for (int k = 2; k < 2 + 0x1C; k++) fr[k] = 0; // zero the 28-dword block from uStack_b8
                fr[3] = (fr[3] & 0xFFFF0000u) | 1;
                iVar2 = o10494(param_3, 0x36);
                if (iVar2 == 0) fr[1] |= 4;
                r_DC0B0(piStack_4c, piStack_40, param_3, AD(fr[0]));
                r_DE0C0(piStack_4c, piStack_40, 0, AD(fr[0]));
                iVar2 = (i32)r_DCF20(piStack_4c, piStack_40, AD(fr[0]));
            }
        }
    }
    return iVar2;
}
