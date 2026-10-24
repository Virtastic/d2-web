// D2Multi batch n49: BC00 open-URL launcher, 11A20/11DA0 control teardown, 11E20 game-info line, 11F80 game-list refresh,
// 12120 game-list add, 13280 game detail text. Retail's ordinals, globals and register args are weak seams / parameters.
#include <cstdint>
#include <cstdio>
#include <cstring>
#define W __attribute__((weak))
extern void Ordinal_10050(void*) W; extern void q_d2multi_batch_n49_Ordinal_10031(void*) W; extern void q_d2multi_batch_n49_Ordinal_10053(void*) W;
extern void Ordinal_10106() W; extern void q_d2multi_batch_n49_Ordinal_10167(const char*) W; extern int Ordinal_10037() W;
extern void q_d2multi_batch_n49_Ordinal_10185(int) W; extern int Ordinal_10030() W; extern void* Ordinal_10114() W;
extern int Ordinal_10054() W; extern int Ordinal_10087() W; extern int Ordinal_10186(int, int) W; extern int Ordinal_10079() W;
extern void q_d2multi_batch_n49_Ordinal_10022(int, const char*) W; extern int Ordinal_10003() W;
extern int d2multi_unicode2Win(char* dst, const void* src, int n) W;
extern bool d2multi_9000(const char* key, const char* val, uint8_t* out, int n) W;       // registry read
extern const char* d2multi_urlArg() W;                                                   // (*(DAT_6fa0917c+0xa4))()
extern void d2multi_hostLaunch(const char* cmd) W;                                       // CreateProcessA seam
extern void d2multi_acb0() W; extern void d2multi_onDone() W;                            // FUN_ACB0, (*(DAT_6fa0917c+0x6c))()
extern void* DAT_6fa09cec W, *DAT_6fa09cf0 W, *DAT_6fa09cf4 W, *DAT_6fa09cf8 W, *DAT_6fa09cfc W, *DAT_6fa09d00 W, *DAT_6fa09d04 W,
    *DAT_6fa09d08 W, *DAT_6fa09d2c W, *DAT_6fa09d30 W, *DAT_6fa09d34 W, *DAT_6fa09d38 W, *DAT_6fa09d3c W, *DAT_6fa09d28 W,
    *DAT_6fa09d48 W, *DAT_6fa09d40 W, *DAT_6fa09d4c W, *DAT_6fa09d50 W, *DAT_6fa09d54 W, *DAT_6fa09d58 W, *DAT_6fa09d5c W,
    *DAT_6fa09d60 W, *DAT_6fa09d44 W, *DAT_6fa09d14 W, *DAT_6fa09d18 W, *DAT_6fa09d1c W, *DAT_6fa09d20 W, *DAT_6fa09d24 W,
    *DAT_6fa09d0c W, *DAT_6fa09d10 W, *DAT_6fa09160 W, *DAT_6fa0a0d0 W;
extern void* DAT_6fa09cc8 W, *DAT_6fa09cd4 W, *DAT_6fa09cd8 W, *DAT_6fa09cc0 W, *DAT_6fa09cc4 W, *DAT_6fa09ccc W,
    *DAT_6fa09cd0 W, *DAT_6fa09cdc W, *DAT_6fa09ce0 W, *DAT_6fa09ce4 W, *DAT_6fa09ce8 W;
extern int32_t DAT_6fa09dcc W, DAT_6fa09dc8 W;
extern uint8_t DAT_6f9ebf20[] W, DAT_6f9fa598[] W;   // 0x3b-byte game records (live, snapshot)
extern const char DAT_6f9e74d8[2] W;

// 1.13c D2Multi+0xBC00
uint32_t d2multi_BC00(void) {
    const char* url = d2multi_urlArg();
    if (!url) return 1;
    uint8_t cls[0x400] = {0}, cmd[0x400 + 2] = {0};
    char key[0x400];
    snprintf(key, sizeof key, "%s\\.html", "SOFTWARE\\Classes");
    if (d2multi_9000(key, nullptr, cls, 0x400)) {
        snprintf(key, sizeof key, "%s\\%s\\shell\\open\\command", "SOFTWARE\\Classes", (char*)cls);
        if (d2multi_9000(key, nullptr, cmd, 0x400)) {
            size_t n = strlen((char*)cmd);
            memcpy(cmd + n, DAT_6f9e74d8, 2);          // separator word, then the URL
            n = strlen((char*)cmd);
            strncat((char*)cmd + n, url, sizeof cmd - n - 1);
            d2multi_hostLaunch((char*)cmd);
            d2multi_onDone();
            return 1;
        }
    }
    Ordinal_10003(); d2multi_acb0();
    d2multi_onDone();
    return 1;
}

// 1.13c D2Multi+0x11A20
void d2multi_11A20(void) {
    void** p[] = {&DAT_6fa09cec,&DAT_6fa09cf0,&DAT_6fa09cf4,&DAT_6fa09cf8,&DAT_6fa09cfc,&DAT_6fa09d00,&DAT_6fa09d04,&DAT_6fa09d08,
        &DAT_6fa09d2c,&DAT_6fa09d30,&DAT_6fa09d34,&DAT_6fa09d38,&DAT_6fa09d3c,&DAT_6fa09d28,&DAT_6fa09d48,&DAT_6fa09d40,
        &DAT_6fa09d4c,&DAT_6fa09d50,&DAT_6fa09d54,&DAT_6fa09d58,&DAT_6fa09d5c,&DAT_6fa09d60,&DAT_6fa09d44,&DAT_6fa09d14,
        &DAT_6fa09d18,&DAT_6fa09d1c,&DAT_6fa09d20,&DAT_6fa09d24,&DAT_6fa09d0c,&DAT_6fa09d10};
    for (void** q : p) Ordinal_10050(q);
    q_d2multi_batch_n49_Ordinal_10031(DAT_6fa09160);
    if (DAT_6fa0a0d0) q_d2multi_batch_n49_Ordinal_10053(DAT_6fa0a0d0);
}

// 1.13c D2Multi+0x11DA0
void d2multi_11DA0(void) {
    void** p[] = {&DAT_6fa09cc8,&DAT_6fa09cd4,&DAT_6fa09cd8,&DAT_6fa09cc0,&DAT_6fa09cc4,&DAT_6fa09ccc,&DAT_6fa09cd0,
        &DAT_6fa09cdc,&DAT_6fa09ce0,&DAT_6fa09ce4,&DAT_6fa09ce8};
    for (void** q : p) Ordinal_10050(q);
    q_d2multi_batch_n49_Ordinal_10031(DAT_6fa09160);
}

// 1.13c D2Multi+0x11E20 (index passed in EAX)
void d2multi_11E20(int idx) {
    if (!DAT_6fa09cc4) return;
    int off = idx * 0x3b;
    Ordinal_10106();
    int len = (int)strlen((const char*)DAT_6f9ebf20 + 0x11 + off) + 1;
    if (len != 1) { q_d2multi_batch_n49_Ordinal_10167((const char*)DAT_6f9ebf20 + 0x11 + off); q_d2multi_batch_n49_Ordinal_10167(nullptr); }
    Ordinal_10037();
    q_d2multi_batch_n49_Ordinal_10185(len); q_d2multi_batch_n49_Ordinal_10053(nullptr);
}

// 1.13c D2Multi+0x11F80
void d2multi_11F80(void) {
    if (!DAT_6fa09cc0) return;
    char sel[256];
    void* u = Ordinal_10114();
    if (u) d2multi_unicode2Win(sel, u, 0x100);
    Ordinal_10030(); Ordinal_10106(); Ordinal_10106();
    DAT_6fa09dc8 = DAT_6fa09dcc;
    if (!DAT_6fa09dcc) return;
    int iVar3 = Ordinal_10054();
    memcpy(DAT_6f9fa598, DAT_6f9ebf20, (size_t)DAT_6fa09dcc * 0x3b);
    for (int i = 0; i < DAT_6fa09dc8; i++) {
        q_d2multi_batch_n49_Ordinal_10167(nullptr);
        int h = Ordinal_10087();
        if (iVar3) {
            char num[16];
            snprintf(num, sizeof num, "%u", *(uint16_t*)(DAT_6f9fa598 + i * 0x3b + 0x31));
            Ordinal_10186(h, 1);
        }
    }
    if (u)
        for (int i = 0; i < DAT_6fa09dc8; i++) {   // retail's two-bytes-per-step strcmp
            const uint8_t* x = (const uint8_t*)sel; const uint8_t* y = DAT_6f9fa598 + i * 0x3b + 1;
            bool eq = true;
            for (;;) {
                if (x[0] != y[0]) { eq = false; break; }
                if (!x[0]) break;
                if (x[1] != y[1]) { eq = false; break; }
                if (!x[1]) break;
                x += 2; y += 2;
            }
            if (eq) { Ordinal_10079(); break; }
        }
    Ordinal_10037();
}

// 1.13c D2Multi+0x12120 (source record in EDI)
void d2multi_12120(const uint8_t* src) {
    for (int i = 0; i < DAT_6fa09dcc; i++)
        if (*(int16_t*)(DAT_6f9ebf20 + i * 0x3b + 0x33) == *(const int16_t*)(src + 8)) return;
    uint8_t* r = DAT_6f9ebf20 + DAT_6fa09dcc * 0x3b;
    *(int16_t*)(r + 0x33) = *(const int16_t*)(src + 8);
    *(int32_t*)(r + 0x37) = 0;
    *(int16_t*)(r + 0x31) = *(const int16_t*)(src + 0x14);
    strcpy((char*)r + 1, (const char*)src + 0x1c);
    strcpy((char*)r + 0x11, (const char*)src + 0x2c);
    DAT_6fa09dcc++;
}

// 1.13c D2Multi+0x13280 (game record in EBX)
void d2multi_13280(const uint8_t* g) {
    if (!DAT_6fa09cc4) return;
    char b[256];
    Ordinal_10106();
    if (strlen((const char*)g + 0x2c) != 0) { q_d2multi_batch_n49_Ordinal_10167(nullptr); q_d2multi_batch_n49_Ordinal_10167(nullptr); }
    uint32_t t = *(const uint32_t*)(g + 0xc);
    snprintf(b, sizeof b, "Elapsed Time: %d:%02d:%02d", t / 0xe10, (t % 0xe10) / 0x3c, t % 0x3c);
    q_d2multi_batch_n49_Ordinal_10167(b);
    uint8_t lo = g[0x10], d = g[0x11];
    if (d < 100) {
        if (d == 0) snprintf(b, sizeof b, "Level %d", lo);
        else {
            int a = (int)lo - d; if (a < 2) a = 1;
            uint32_t z = (uint32_t)d + lo; if (z > 0x62) z = 99;
            snprintf(b, sizeof b, "Level %d to %d", a, z);
        }
        q_d2multi_batch_n49_Ordinal_10167(b);
    }
    if (g[0x12] && g[0x12] < 8) { snprintf(b, sizeof b, "Up to %d Players", g[0x12]); q_d2multi_batch_n49_Ordinal_10167(b); }
    q_d2multi_batch_n49_Ordinal_10167(nullptr);
    static const char* const cls[] = {"Amazon","Sorceress","Necromancer","Paladin","Barbarian","Druid","Assassin"};
    int n = *(const int32_t*)(g + 0x14);
    for (int i = 0; i < n; i++) {
        if (!g[0x8c + i * 0x10]) continue;
        q_d2multi_batch_n49_Ordinal_10167(nullptr);
        uint16_t c = *(const uint16_t*)(g + 0x4c + i * 2), lv = *(const uint16_t*)(g + 0x6c + i * 2);
        if (c < 7) snprintf(b, sizeof b, "Level %d %s", lv, cls[c]); else snprintf(b, sizeof b, "Level %d", lv);
        q_d2multi_batch_n49_Ordinal_10022(5, b);
    }
    Ordinal_10037();
}
