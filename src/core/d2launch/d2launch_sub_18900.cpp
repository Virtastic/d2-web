// 1.13c D2Launch+0x18900 realm-select screen setup: reads Aux Battle.net IP, classifies, loads 8 resources
#include <cstdint>
#define WK __attribute__((weak))
extern "C" {
WK extern int DAT_6fa65e30; WK extern uint32_t DAT_6fa652b8[];
WK int32_t Ordinal_10098(const void*); WK uint32_t Ordinal_10265(uint32_t); WK void Ordinal_10024(const void*, uint32_t);
WK void _exit(int);
WK void FUN_6fa5b310();
}
static void d2l_fail() { uint32_t e = Ordinal_10265(0x624); Ordinal_10024("", e); _exit(-1); }
static uint32_t d2l_slot(const void* name) { uint32_t h = Ordinal_10098(name); DAT_6fa652b8[DAT_6fa65e30++] = h; return h; }

extern "C" {
WK uint32_t Ordinal_10023(int32_t); WK void Ordinal_10014(void*) __asm__("d2launch_Ordinal_10014"); WK intptr_t Ordinal_10061(intptr_t); WK int32_t Ordinal_10003(); WK intptr_t Ordinal_10020(intptr_t); WK void q_d2launch_sub_18900_Ordinal_10147(void);
WK int Ordinal_422(...); WK int Ordinal_425(...); WK int32_t Ordinal_10227(void);
WK int _sprintf(char*, const char*, ...); WK int _sscanf(const char*, const char*, ...);
WK void q_d2launch_sub_18900_FUN_6fa567b0();
WK extern char DAT_6fa61880[], DAT_6fa618b0[], DAT_6fa618e0[], DAT_6fa61910[], DAT_6fa61940[], DAT_6fa61970[], DAT_6fa619a0[], DAT_6fa619d0[];
}
extern "C" uint32_t FUN_6fa58900() {
    char ip[100]; uint32_t a = 0, b = 0, c = 0, d = 0x22; char cls;
    Ordinal_10023(0); Ordinal_10014(0); Ordinal_10061(0); FUN_6fa5b310();
    if (Ordinal_422("Diablo II", "Aux Battle.net", 0, ip, 100) == 0) {
        cls = 1; _sprintf(ip, "%d.%d.%d.%d", 0xd8, 0x94, 0xf6, 0x22);
        Ordinal_425("Diablo II", "Aux Battle.net", 0, ip);
    } else {
        _sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);
        if (d & 1) cls = 7; else if ((int8_t)d < 0) cls = 6; else if (d & 0x10) cls = 5;
        else if (d & 8) cls = 4; else if (d & 0x40) cls = 3; else cls = (d & 4) ? 2 : 1;
    }
    if (Ordinal_10227() != 0) { q_d2launch_sub_18900_FUN_6fa567b0(); return 1; }
    const char* n[] = {DAT_6fa61880, DAT_6fa618b0, DAT_6fa618e0, DAT_6fa61910, DAT_6fa61940, DAT_6fa61970, DAT_6fa619a0, DAT_6fa619d0};
    for (int i = 0; i < 8; i++) {
        if (DAT_6fa65e30 >= 0x40) { d2l_fail(); return 0; }
        d2l_slot(n[i]);
        if (i == 1) { Ordinal_10003(); Ordinal_10020(0); }
        if (i == 6) switch (cls) { case 5: case 6: case 7: break; default: q_d2launch_sub_18900_Ordinal_10147(); /* fallthrough */ case 2: q_d2launch_sub_18900_Ordinal_10147(); /* fallthrough */ case 3: q_d2launch_sub_18900_Ordinal_10147(); /* fallthrough */ case 4: q_d2launch_sub_18900_Ordinal_10147(); }
    }
    return 1;
}
