// 1.13c D2Client+0x391E0: map a (a, b) pair through the 118 x 10-byte record table to a
// string id and fetch it with D2Lang#10003, or fall back to a two-dword scratch result.
#include <cstdint>
#include <cstddef>
#include <cstring>

static const uint16_t kId[0xDF] = {0,3763,3764,3765,3766,0,0,0,3790,3791,0,0,3792,3793,0,0,3794,3795,3796,3797,3798,3771,0,3772,3773,3774,0,3775,3776,3777,3778,3779,3799,3800,3801,3802,3803,3780,3781,3782,3783,3784,3804,3785,3805,3806,3807,3808,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3786,3787,3788,0,0,3809,3810,3811,3812,3813,3814,3815,3816,3817,3818,3819,3820,3821,3822,3823,3824,3825,3826,3827,3828,3829,3830,3831,3832,3833,3834,3835,3836,3837,3838,3839,3840,3841,3842,3843,3844,3845,3846,3847,3848,0,0,0,0,0,0,0,0,3789,3849,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3850,3851,3852,3853,3854,3855,3856,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3857,3858,3859,3860};
static const uint16_t kId100[5] = {3766,3767,3768,3769,3770};
struct LangRec { uint32_t a; uint16_t code; uint32_t b; };   // 10 bytes, packed, at D2Client+0x11A2E0
// tbl: the 118 records; out: the 8-byte scratch at +0x1223C8. lang(id): D2Lang#10003. Returns lang's result, or out.
uint32_t d2client_sub_391E0(uint32_t esi, uint32_t edx, const uint8_t* tbl, uint32_t* out, uint32_t (*lang)(uint32_t), uint32_t outTag) {
    int i = 0;
    for (;; ++i) {
        if (i == 118) return lang(0xEB2);   // no match: mov ecx,0xEB2 (3762)
        uint32_t a, b; memcpy(&a, tbl + i * 10, 4); memcpy(&b, tbl + i * 10 + 6, 4);
        if (a == esi && b == edx) break;
    }
    uint16_t code; memcpy(&code, tbl + i * 10 + 4, 2);
    if (code == 0xFFFF) return lang(0xEB2);
    if (code >= 0x100 && code <= 0x104) return lang(kId100[code - 0x100]);
    if (code < 0xDF && kId[code]) return lang(kId[code]);
    out[0] = code & 0xFFFF00FFu; out[1] = 0;
    return outTag;
}
