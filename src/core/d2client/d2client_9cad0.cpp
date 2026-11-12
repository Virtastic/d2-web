// 1.13c D2Client+0x9CAD0 (EAX = unit): the NPC greeting record of the unit's class (unit+4), NULL for a unit of no
// listed class or no unit. A record is six dwords at D2Client+0xDF0DC + 0x18*k: {row, row kind 0, row base + cycle,
// row kind 2, last pick, its tick}; 0x9CCE0 writes the last two, so the records are the module's mutable copy.
#include <cstdint>
#include <cstring>

namespace {
int32_t g_rec[29][6] = {
    { 3480, 3482, 3485, 3484 }, { 3523, 3525, 3528, 3527 }, { 3571, 3573, 3576, 3575 }, { 3613, 3615, 3618, 3617 },
    { 3644, 3647, 3649, 3644 }, { 3737, 3739, 3743, 3742 }, { 3776, 3779, 3782, 3781 }, { 3811, 3814, 3816, 3811 },
    { 3844, 3847, 3850, 3849 }, { 3877, 3880, 3883, 3882 }, { 3910, 3913, 3917, 3916 }, { 3949, 3951, 3954, 3953 },
    { 3989, 3991, 0, 3989 },    { 3992, 3993, 3995, 3994 }, { 4043, 4045, 0, 4043 },    { 4046, 4048, 0, 4046 },
    { 4049, 4051, 4054, 4053 }, { 4077, 4079, 4082, 4081 }, { 4118, 4120, 4123, 4122 }, { 4150, 4152, 4155, 4154 },
    { 4217, 4219, 4221, 4220 }, { 4248, 4250, 4252, 4251 }, { 4303, 4305, 4306, 4303 }, { 0, 4320, 4324, 4323 },
    { 0, 4382, 4386, 4385 },    { 0, 4563, 4567, 4566 },    { 0, 4489, 4493, 4492 },    { 0, 4449, 4453, 4452 },
    { 0, 4533, 4537, 4536 },
};
int32_t slotOf(uint32_t cls) {   // record index = (address - 0xDF0DC) / 0x18
    switch (cls) {
        case 0x92: case 0xF4: case 0xF5: case 0xF6: case 0x109: case 0x208: return 4;
        case 0x93: return 10;  case 0x94: return 0;   case 0x96: return 17;  case 0x9A: return 5;
        case 0x9B: case 0xAF: return 23;
        case 0xB0: return 3;   case 0xB1: return 6;   case 0xB2: return 8;   case 0xC6: return 11;
        case 0xC7: return 7;   case 0xC8: return 9;   case 0xC9: return 16;  case 0xCA: return 18;
        case 0xD2: return 19;  case 0xFB: case 0x209: return 22;
        case 0xFC: return 2;   case 0xFD: return 13;  case 0xFE: return 1;   case 0xFF: return 21;
        case 0x101: return 12; case 0x129: return 20; case 0x195: return 14; case 0x1FF: return 27;
        case 0x200: return 24; case 0x201: return 26; case 0x202: return 28; case 0x203: return 25;
    }
    return -1;
}
}
// 1.13c D2Client+0x9CAD0
uint8_t* d2client_9cad0(uint32_t cls) {
    const int32_t k = slotOf(cls);
    return k < 0 ? nullptr : reinterpret_cast<uint8_t*>(g_rec[k]);
}
void d2client_9cad0_reset() {
    for (auto& r : g_rec) r[4] = r[5] = 0;
}
