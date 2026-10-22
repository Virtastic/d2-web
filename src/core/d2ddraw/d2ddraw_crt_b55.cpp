// D2DDraw statically linked MSVC CRT float conversion helpers (batch b55-ddr-a).
// Skipped (register args EAX/EBX/EDI): __shift, __cftoe2, __cftof2 and their callers 9844/9950/99B7.
#include <cstdint>

struct d2ddraw_crt_b55_Env {
    void (*fn9644)();                           // 0x9644
    uint32_t (*msP5MpTestFdiv)();               // 0x9AA7
    void (*setDefaultPrecision)();              // 0x9AF9
    uint32_t* fdivFlag;                         // DAT_6f8d0260
    int (*isZeroMan)(uint32_t* man);            // 0x9BA1
    void (*copyMan)(uint32_t* dst, uint32_t* src); // 0x9C2E
    int (*roundMan)(uint32_t* man, int bits);   // 0x9C13
    void (*shrMan)(uint32_t* man, int n);       // 0x9C47
    const int* tblDouble;                       // DAT_6f8cebe4
    const int* tblFloat;                        // DAT_6f8cebfc
    void (*strToLd12)(uint16_t* ld12, int* outLen, const uint8_t* s, int, int, int, int); // 0xA3AC
};

// 1.13c D2DDraw+0x967C
void d2ddraw_crt_b55___fpmath(const d2ddraw_crt_b55_Env* e, int param_1)
{
    e->fn9644();
    *e->fdivFlag = e->msP5MpTestFdiv();
    if (param_1 != 0) e->setDefaultPrecision();
}

// 1.13c D2DDraw+0x9CC2
uint32_t d2ddraw_crt_b55___ld12cvt(const d2ddraw_crt_b55_Env* e, const uint16_t* param_1,
                                   uint32_t* param_2, const int* param_3)
{
    uint16_t uVar1 = param_1[5];
    uint32_t local_1c[3];
    uint32_t local_10 = *(const uint32_t*)(param_1 + 3);
    uint32_t local_c = *(const uint32_t*)(param_1 + 1);
    int32_t local_8 = (int32_t)((uint32_t)param_1[0] << 16);
    uint32_t uVar4 = uVar1 & 0x7fff;
    int iVar5 = (int)uVar4 - 0x3fff;
    uint32_t uVar3;
    (void)local_8;
    if (iVar5 == -0x3fff) {
        iVar5 = 0;
        if (e->isZeroMan(&local_10) != 0) {
            uVar3 = 0;
            goto done;
        }
        local_10 = 0;
        local_c = 0;
    } else {
        e->copyMan(local_1c, &local_10);
        if (e->roundMan(&local_10, param_3[2]) != 0) iVar5 = (int)uVar4 - 0x3ffe;
        int iVar2 = param_3[1];
        if (iVar5 < iVar2 - param_3[2]) {
            local_10 = 0;
            local_c = 0;
        } else {
            if (iVar2 < iVar5) {
                if (param_3[0] <= iVar5) {
                    local_c = 0;
                    local_8 = 0;
                    local_10 = 0x80000000u;
                    e->shrMan(&local_10, param_3[3]);
                    iVar5 = param_3[5] + param_3[0];
                    uVar3 = 1;
                    goto done;
                }
                local_10 &= 0x7fffffffu;
                iVar5 = param_3[5] + iVar5;
                e->shrMan(&local_10, param_3[3]);
                uVar3 = 0;
                goto done;
            }
            e->copyMan(&local_10, local_1c);
            e->shrMan(&local_10, iVar2 - iVar5);
            e->roundMan(&local_10, param_3[2]);
            e->shrMan(&local_10, param_3[3] + 1);
        }
    }
    iVar5 = 0;
    uVar3 = 2;
done:
    local_10 = ((uint32_t)iVar5 << ((0x1fu - (uint32_t)param_3[3]) & 0x1f)) |
               ((uVar1 & 0x8000) ? 0x80000000u : 0u) | local_10;
    if (param_3[4] == 0x40) {
        param_2[1] = local_10;
        param_2[0] = local_c;
    } else if (param_3[4] == 0x20) {
        param_2[0] = local_10;
    }
    return uVar3;
}

// 1.13c D2DDraw+0x9E1A
uint32_t d2ddraw_crt_b55___ld12tod_a(const d2ddraw_crt_b55_Env* e, const uint16_t* ifp, uint32_t* d)
{
    return d2ddraw_crt_b55___ld12cvt(e, ifp, d, e->tblDouble);
}

// 1.13c D2DDraw+0x9E30
uint32_t d2ddraw_crt_b55___ld12tod_b(const d2ddraw_crt_b55_Env* e, const uint16_t* ifp, uint32_t* d)
{
    return d2ddraw_crt_b55___ld12cvt(e, ifp, d, e->tblFloat);
}

// 1.13c D2DDraw+0x9E46
void d2ddraw_crt_b55_strtod_a(const d2ddraw_crt_b55_Env* e, uint32_t* param_1, const uint8_t* param_2)
{
    int len;
    uint16_t ld[6];
    e->strToLd12(ld, &len, param_2, 0, 0, 0, 0);
    d2ddraw_crt_b55___ld12tod_a(e, ld, param_1);
}

// 1.13c D2DDraw+0x9E8B
void d2ddraw_crt_b55_strtod_b(const d2ddraw_crt_b55_Env* e, uint32_t* param_1, const uint8_t* param_2)
{
    int len;
    uint16_t ld[6];
    e->strToLd12(ld, &len, param_2, 0, 0, 0, 0);
    d2ddraw_crt_b55___ld12tod_b(e, ld, param_1);
}
