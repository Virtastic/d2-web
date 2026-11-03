// Shared G/Env of the D2Client 0x804C0..0x82490 batch (d2client_batch_804c0.cpp) and the FN probes of its rows.
#pragma once
#include <cstdint>
struct D2cB8G {                 // globals; retail RVA noted
    uint8_t* player;            // +0x10BBFC DAT_6fbcbbfc
    uint8_t* ctx;               // +0x10BF68 DAT_6fbcbf68 (+0x110 count, +0x114 live, +0x11C particle array)
    int32_t  rainMode;          // +0x10BF78
    int32_t  flag7c;            // +0x10BF7C
    int32_t  flag6c;            // +0x10BF6C
    int32_t  cellW, cellH;      // +0x0DBC48 DAT_6fb8bc48, +0x0DBC4C DAT_6fb8bc4c
    int32_t  levelId;           // +0x10C3B8 DAT_6fbcc3b8
    int32_t  tickDX, tickDY;    // +0x10C408 DAT_6fbcc408, +0x10C40C DAT_6fbcc40c
    int32_t  frame;             // +0x1237440 DAT_6fbd3440 (frame counter)
    int32_t  maxLive;           // +0x113934 DAT_6fbc3934
    int32_t  trigHandle;        // +0x113918 DAT_6fbc3918
    const uint8_t* tab84a19;    // DAT_6fb84a19 stride 2
    const uint8_t* tab3940;     // DAT_6fbc3940
    const uint8_t* tab38e0;     // DAT_6fbc38e0
    const uint8_t* tab38d4;     // DAT_6fbc38d4
    const uint8_t* tab38a0;     // DAT_6fbc38a0
    const uint8_t* tab38f8;     // DAT_6fbc38f8
    int32_t  cnt393c, flag38c0, v38bc, v91380, cnt2ac, mode414; // DAT_6fbc393c/38c0/38bc, 6fb91380, 6fbcc2ac, 6fbcc414
    uint32_t s97f8, s985c;      // DAT_6fbc97f8 / DAT_6fbc985c
    uint8_t* dt;                // *sgptDataTables
    uint8_t* hashA;             // &DAT_6fbbae08  128 x ptr chain heads (chain next at +0xE4)
    uint8_t* hashB;             // &DAT_6fbba608  stride 0x80
};

struct D2cB8Env {
    int      (*ord10078)(void);
    uint32_t (*ord10331)(uint32_t);
    uint32_t (*ord10826)(uint32_t);
    uint32_t (*ord11044)(int32_t, int32_t);
    uint32_t (*rnd)(uint64_t* seed);          // FUN_6fab18d0
    int32_t  (*ftol)(void);                   // FUN_6fb7acc4
    void     (*fatal)(uint32_t code);         // Ordinal_10265+10024+_exit(-1)
    uint32_t (*ord10064)(void);
    void     (*ord10065)(void);
    void     (*ord10083)(uint32_t);
    void     (*ord10084)(uint32_t);
    char     (*ord10215)(uint32_t);
    void     (*ord10499)(uint32_t);
    void     (*ord10014)(int,int,int,int,int,int,int,uint32_t);
    void*    (*fn38a70)(int,int,int,int);
    void     (*fn38ba0)(void*,int,int,int);
    void     (*fn2fe80)(void);
    uint8_t* (*ord10695)(int32_t id);         // unit-info lookup
    int      (*ord10106)(void);
    int      (*ord10620)(void);
    int      (*ord10787)(void);
    int      (*ord10744)(void);
    void     (*ord10600)(void);
    uint8_t* (*ord10806)(uint32_t);
    void     (*ord10519)(void);
    void     (*ord10996a)(int);
    void     (*ord10996b)(int,int);
    int      (*ord10202)(void);
    uint32_t (*ord10061)(uint32_t);
    int      (*ord10747)(void);
    void     (*ord10164)(void);
    uint8_t* (*ord10195)(uint8_t* u, int f);
    int      (*ord10680)(uint8_t* it, int a, int b);
    uint8_t* (*ord10900)(uint8_t* it, int f);
    uint8_t* (*ord10460)(uint32_t);
    uint8_t* (*ord10464)(uint8_t*);
    uint8_t* (*ord11147)(uint8_t*);
    void     (*ord11037)(uint8_t*,int,int,int,uint8_t*);
    int      (*ord11145)(uint8_t*,uint8_t*,int,int,int,int,int32_t*);
    uint32_t (*ord10838)(uint8_t*,int);
    int      (*ord10197)(uint32_t);
    void     (*ord10466)(uint32_t,int);
    int      (*ord11017)(uint32_t);
    void     (*fn62d0)(void);
    void     (*fn31aa0)(void);
    void     (*fn56ef0)(uint8_t* tab, uint32_t idx, uint32_t key, int n);
    int      (*ord10708)(uint32_t);
    int      (*ord10828)(void);
    int      (*ord10507)(void);
    void     (*ord10546)(void);
    void     (*ord10978)(void);
    int      (*fn81f60)(uint8_t* unit);
    char     (*ord10417)(void);
    void     (*ord10694)(uint32_t,uint32_t);
    void     (*ord10921)(void);
    void     (*fn19a90)(uint8_t*);
    void     (*ord10605)(void);
    int      (*ord10646)(uint32_t,uint32_t);
    int      (*ord10202b)(int,int,int,const char*);
    void     (*ord11127)(void);
    void     (*ord10608)(uint32_t,char);
    // fidelity seams added for rows 0x805A0 / 0x80700 (retail passes these through registers the first seams dropped)
    uint32_t (*rnd2)(uint64_t* seed, int32_t range, int32_t base);   // FUN_6fab18d0: ECX = seed, ESI = range, EDI = base
    uint32_t (*ord10064c)(uint8_t* ctx);                              // thunk 0xBFEC, ECX = DAT_6fbcbf68
    int32_t  (*ftolk)(int32_t num, int32_t den, uint32_t kRva);       // x87: float(num/den) * float@kRva, truncated (FUN_6fb7acc4); the host owns the FPU
    void     (*fn7fe10)(void);                                        // tail-jump target of 0x805A0 (no arguments)
    void*    (*fn88a70e)(uint32_t ebx, int, int, int, int);           // 0x88A70 with EBX
};
