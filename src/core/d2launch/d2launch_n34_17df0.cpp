// D2Launch batch 34 -- transcribed from build-wasm/ghidra-d2launch/decomp (base 0x6FA40000).
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
extern "C" {
int32_t d2launch_ord_10098(const void* name);      // Fog/D2Win resource-by-name loader
int32_t d2launch_ord_10227(void);
int32_t d2launch_ord_10165(void);
void    d2launch_ord_10147(void);
void    d2launch_ord_10018(void);
void    d2launch_ord_10022(int32_t);
int32_t d2launch_ord_10035(void);
void    d2launch_ord_10045(void);
int32_t d2launch_ord_10265(int32_t);
void    d2launch_ord_10024(const char*, int32_t);
int     d2launch_ord_578(char*, int, const char*, ...);
void    d2launch_sub_5B310(void);   // 0x1B310
void    d2launch_sub_53D60(void);   // 0x13D60
void    d2launch_dbg(const char*);  // OutputDebugStringA
extern uint8_t* g_d2launch_state;   // DAT_6fa65abc
extern int32_t* g_d2launch_bnet;    // DAT_6fa656c4 (vtable object)
extern int32_t  g_d2launch_bnetOff; // DAT_6fa65eb0
extern int32_t  g_d2launch_resCnt;  // DAT_6fa65e30
extern int32_t  g_d2launch_resTab[64]; // DAT_6fa652b8
extern int32_t  g_d2launch_res551c; // DAT_6fa6551c
extern const char kRes_1100[], kRes_1130[], kRes_10D0[], kRes_1160[], kRes_1280[], kRes_1250[],
    kRes_12B0[], kRes_1220[], kRes_1190[], kRes_11C0[], kRes_11F0[], kRes_4310[];
}

static int32_t* vt(int off) { return (int32_t*)((uint8_t*)g_d2launch_bnet + off); }
static void push(const char* n) {
    if (g_d2launch_resCnt > 0x3f) {  // overflow is fatal (error 0x624)
        d2launch_ord_10024("", d2launch_ord_10265(0x624));
        _exit(-1);
    }
    g_d2launch_resTab[g_d2launch_resCnt++] = d2launch_ord_10098(n);
}

// 1.13c D2Launch+0x17DF0 -- leave-BNET/rebuild-menu-resources: disconnects, then loads the menu resource set.
extern "C" void d2launch_sub_17DF0(void) {
    uint8_t* st = g_d2launch_state;
    if (g_d2launch_bnet) {
        g_d2launch_bnetOff = 1;
        st[0x35d] = 0;
        st[0x37d] = 0;
        d2launch_dbg("Disconnect from BNET\n");
        d2launch_ord_10018();
        (*(void (**)(void))vt(0x10))();
    }
    g_d2launch_state[0x33] = 0;
    d2launch_sub_5B310();
    if (d2launch_ord_10227() == 0) {
        push(kRes_1100);
        push(kRes_1130); g_d2launch_res551c = g_d2launch_resTab[g_d2launch_resCnt - 1];
        d2launch_sub_53D60();
        push(kRes_10D0);
        push(kRes_1160);
    } else {
        push(kRes_1280); g_d2launch_res551c = g_d2launch_resTab[g_d2launch_resCnt - 1];
        d2launch_sub_53D60();
        push(kRes_1250);
        push(kRes_12B0);
        push(kRes_1220);
    }
    if (d2launch_ord_10165() != 0) { d2launch_ord_10147(); d2launch_ord_10147(); d2launch_ord_10147(); }
    push(kRes_1190);
    push(kRes_11C0);
    push(kRes_11F0);
    push(kRes_4310);
    if (!g_d2launch_bnet) {  // error 0x9fc
        d2launch_ord_10024("", d2launch_ord_10265(0x9fc));
        _exit(-1);
    }
    char ver[32];
    int32_t v = (*(int32_t (**)(void))vt(0x5c))();
    if (g_d2launch_state[0x21f] == 0) d2launch_ord_578(ver, 0x20, "v %d.%02d", v / 100 + 1, v % 100);
    else d2launch_ord_578(ver, 0x20, "v %d.%02d %d", v / 100 + 1, v % 100, (int)(char)(*(int32_t (**)(void))vt(0xb0))());
    d2launch_ord_10022(0);
    if (d2launch_ord_10035() != 0) d2launch_ord_10045();
}
