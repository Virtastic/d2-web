// Batch: D2Game+0x534D0, +0x78DD0. Raw-offset transcriptions of the cached 1.13c decompile.
// Ordinals are declared opaquely; register inputs (EAX/EBX) are explicit parameters.
#include <cstdint>
#include <initializer_list>
#include <unistd.h>

extern "C" {
intptr_t Ordinal_10920(void*);
void Ordinal_11042(void*, uint32_t, uint32_t);
void Ordinal_11072(void*, uint32_t, uint32_t);
int32_t Ordinal_10048(void); intptr_t Ordinal_10711(...); intptr_t Ordinal_10910(intptr_t,intptr_t,intptr_t); intptr_t Ordinal_10973(...);
intptr_t Ordinal_10887(...); intptr_t Ordinal_10744(...); intptr_t Ordinal_10516(...); intptr_t Ordinal_10202(...);
intptr_t Ordinal_10905(...); intptr_t Ordinal_11088(intptr_t); intptr_t Ordinal_10417(...); intptr_t Ordinal_11054(...);
intptr_t Ordinal_11090(...); intptr_t Ordinal_10892(...); int Ordinal_10010(int, uint32_t, int);
int Ordinal_11139(int, uint32_t);
}
void D2GAME_8970_Unk(int ebx, int* p, uint32_t eax, int one, uint32_t* out);
int D2GAME_D57F0_Unk(uint32_t eax, uint32_t v);
void D2GAME_78B60_Unk(uint32_t eax, uint32_t p);
void D2GAME_D5740_Unk(int* p, int v);

// 1.13c D2Game+0x534D0
uint32_t D2GAME_534D0(int* param_1, uint32_t param_2, int* param_3, uint32_t* param_4)
{
    *param_4 = 0;
    if (param_1 && *param_1 == 0) {
        uint8_t* base = (uint8_t*)Ordinal_10920(param_1);
        uint8_t* dst = *(uint8_t**)(base + 0x9c);
        if (dst && (*param_4 = 0x30, param_2 > 0x48)) {
            uint8_t* p4 = dst + 0x41e;
            uint8_t* p5 = (uint8_t*)param_1 + 7;
            auto conv = [](uint8_t b) -> uint16_t {
                if (b < 0x9c || b > 0xd8) return b == 0xff ? 0xffff : b;
                return b + 0x41;
            };
            for (int i = 2; i; --i) {
                static const int o[4] = {0, 6, 12, 18};  // p5 stride within a row
                for (int k = 0; k < 4; ++k) {
                    // entry k: id at p5[-7+6k], flag at p5[-6+6k]; dest at p4+8k-2 (u16), p4+8k (u8), p4+8k+2 (u32)
                    *(uint16_t*)(p4 + 8 * k - 2) = conv(p5[-7 + o[k]]);
                    p4[8 * k] = p5[-6 + o[k]];
                    *(uint32_t*)(p4 + 8 * k + 2) = 0xffffffff;
                }
                p5 += 0x18;
                p4 += 0x20;
            }
            if (param_2 > 0x51) {
                uint32_t u = *(uint32_t*)((uint8_t*)param_1 + *param_3 + 4);
                Ordinal_11042(param_1, *(uint32_t*)((uint8_t*)param_1 + *param_3), 0xffffffff);
                Ordinal_11072(param_1, u, 0xffffffff);
                *param_3 += 8;
            }
        }
    }
    return 0;
}

// 1.13c D2Game+0x78DD0
void D2GAME_78DD0(int ebx, int* param_2, int in_EAX)
{
    uint32_t stk = 0;
    int iVar1 = Ordinal_10048();
    if (((iVar1 != 0 || Ordinal_10711() != 0 || Ordinal_10910(0,0,0) != 0)) && Ordinal_10973() < 1) {
        if (Ordinal_10910(0,0,0) != 0) { Ordinal_10887(); return; }
        if (Ordinal_10744() == 0 || Ordinal_10516() == 0) {
            stk = in_EAX;
            iVar1 = Ordinal_10905();
            uint32_t uVar2 = (uint32_t)Ordinal_11088((intptr_t)in_EAX);
            Ordinal_10417();
            Ordinal_11054();
            Ordinal_11090(param_2);
            D2GAME_8970_Unk(ebx, param_2, in_EAX, 1, &stk);
            Ordinal_10892();
            if (iVar1 != 0) {
                for (iVar1 = Ordinal_10010(param_2[0x18], uVar2, 0); iVar1 != 0;
                     iVar1 = Ordinal_10010(param_2[0x18], uVar2, iVar1)) {
                    int a = in_EAX ? *(int*)(in_EAX + 4) : -1;
                    int b = *(int*)(iVar1 + 4);
                    if (a == b && Ordinal_10202(iVar1, 0x100, 0x530, "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp") == 0) {
                        if (a == *(int*)(iVar1 + 4) && Ordinal_11139(param_2[0x18], stk & 0xff) == 0 &&
                            D2GAME_D57F0_Unk(in_EAX, *(uint32_t*)(iVar1 + 0xc)) != 0)
                            return;
                        break;
                    }
                }
            }
            D2GAME_78B60_Unk(in_EAX, (uint32_t)(uintptr_t)param_2);
        } else {
            if (Ordinal_10202(0x100) == 0) D2GAME_D5740_Unk(param_2, 0x2d);
        }
    }
}
