// Batch 7A020: 12 D2Game functions from the cached 1.13c decompile. Register inputs
// (unaff_*/in_EAX/stack) are explicit parameters; every such argument is marked GUESS.
// Unmapped Ordinal_/FUN_ callees are reached through a hook table `H` (GUESS: names by ordinal).
#include <cstdint>

typedef int32_t i32; typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
struct U { i32 t; i32 f[0x60]; };  // unit view: [0]=type, [1]=id, [0x31]=flags (GUESS size)

struct H {  // hooks, GUESS: each is the Ordinal/FUN named in the field
    int (*o10648)(U*); int (*o10541)(U*); int (*o10830)(U*); int (*o10666)(U*, U*);
    int (*o10341)(U*); int (*o10239)(U*); int (*o10973)(U*, int, int); int (*o10913)(U*);
    int (*o11027)(U*); int (*f1900)(); int (*f18A0)(); int (*f1850)(); int (*f213b0)(int);
    bool (*f42a80)(int); void* (*o10045)(const char*); void (*o10158)();
    int (*dataTableMax)();  // *(sgptDataTables+0xa80)
    int (*bitmask12)();     // gdwBitMasks[0xc]
    int (*f21220)();        // returns record ptr as int
    int (*rec_d)(int);      // *(u8*)(rec+0xd)
};

static inline bool isUnit(U* a, U* b) { return a != b && a && a->t == 1; }
static inline bool hasF1(U* u) { return (u->f[0x30] >> 1) & 1; }   // param_3[0x31]>>1&1
static inline bool inRange(const H& h, int v) { return v >= 0 && v < h.dataTableMax(); }

struct Msg { u32 next; u8 type; u8 b9; u16 w; u32 a; u32 b; };
static void enqueue(const H& h, int* client, u32* m) {  // shared tail of 7A020/7A4C0
    if (client[0xec/4] == 0) client[0xec/4] = (int)(intptr_t)m;
    if (client[0xf0/4]) *(u32*)(intptr_t)client[0xf0/4] = (u32)(intptr_t)m;
    client[0xf0/4] = (int)(intptr_t)m;
    h.o10158();
}

// superseded by d2game_7a020.cpp: D2Game+0x7A020   GUESS: client (ESI), arg (retaddr slot)
void D2GAME_7A020(const H& h, int* client, u32 arg) {
    u32* m = (u32*)h.o10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp");
    ((u8*)m)[0xe] = 0x2f; ((u8*)(m + 1))[0] = 0x23; m[0] = 0; ((u16*)(m + 3))[0] = 0; m[2] = arg;
    enqueue(h, client, m);
}

// superseded by d2game_7a4C0.cpp: D2Game+0x7A4C0   GUESS: client (ESI), b11 (retaddr slot)
void D2GAME_7A4C0(const H& h, int* client, u32 p1, u8 p2, u8 b11) {
    u32* m = (u32*)h.o10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp");
    *(u16*)(m + 2) = 0x281; ((u8*)m)[10] = 0; ((u8*)m)[0xb] = b11; ((u8*)(m + 1))[0] = 0x99;
    m[0] = 0; m[3] = p1; ((u8*)(m + 4))[0] = p2;
    enqueue(h, client, m);
}

// 1.13c D2Game+0x7B510   EAX = game (+0x20 = seed), stack arg = table base (slot idx*4+0xF0 -> record)
struct B510H { int (*o10331)(int); int (*o10826)(int); int (*f1780)(int, void*); };
int D2GAME_7B510(const B510H& h, int game, int* esi) {
    int idx = h.o10826(h.o10331(game));
    int* e = (int*)(intptr_t)esi[0xf0/4 + idx];
    if (e[0x2d4/4] < 0) {
        if (idx < 0x28) { e[0x2d4/4] = 0; return 0; }
        e[0x2d4/4] = h.f1780(3, (void*)(intptr_t)(game + 0x20));
    }
    return e[0x2d4/4];
}

// 1.13c D2Game+0x7B860, 0x7B8E0, 0x7BA40 and 0x7BB00 moved to d2game_7b860.cpp .. d2game_7bb00.cpp (retail register and stack arguments).

// 1.13c D2Game+0x7BC70, 0x7BD00 and 0x7BD50 moved to d2game_7bc70.cpp (retail register and stack arguments).

// 1.13c D2Game+0x7BE10 and 0x7BF50 moved to d2game_7be10.cpp and d2game_7bf50.cpp (retail register and stack arguments).
