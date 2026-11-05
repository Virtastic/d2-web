// src/core/d2game/d2game_batch_43450.h -- the callee env of d2game_batch_43450.cpp, shared with its FN probes (src/core/fn/d2game_<rva>.cpp).
#pragma once
#include <cstdint>
#include <cstring>

struct D2GameBatch43450Env {
    const uint8_t* dataTables;          // *sgptDataTables_exref
    // Fog/D2Common ordinals (by number)
    void*    (*o10331)(void* unit);                       // unit -> skill-ish record
    uint32_t (*o10826)(void* p);
    uint8_t* (*o10014)(uint32_t id);
    int      (*o10686)(void* unit, uint32_t ebp, uint32_t esi, uint32_t flags);
    void*    (*o10392)(uint32_t id);                      // unit lookup by id
    void     (*o10786)(void* unit, uint32_t a, uint32_t b);
    uint32_t (*o10203_id)(uint32_t id);
    int      (*o10680_3)(void* p, int stat, int z);                        // 3 stack args
    void*    (*f21420s)(int skillId);                                      // 0x1420: EAX=skillId
    uint32_t (*o10786_4)(void* unit, uint32_t a, uint32_t b, uint32_t c);   // 4 stack args, result in EAX
    void     (*fD007A0u)(void* unit, void* game);                          // 0xE07A0: EAX=unit, ECX=game
    void     (*o10637)(void* missile, uint32_t v);
    void     (*o10019)(void* missile, uint32_t v);
    void*    (*o10203)(void* unit);
    void*    (*o10871)(void* unit, int a);
    int      (*o10680)(void* p);
    uint32_t (*o10057)(void* p);
    int      (*o10983)(uint32_t id);
    int      (*o10764)(uint32_t id);
    uint32_t (*o10867)(uint32_t id);
    uint32_t (*o10750)(uint32_t id);
    uint32_t (*o10945)(void* unit, int a, int b);
    void*    (*o11013)(uint32_t a, int b, uint32_t c, uint32_t d, uint32_t e);
    void     (*o10785)(void* p, int v);
    void     (*o10911)(void* p);
    void     (*o10807)(void* a, void* b, int c);
    void     (*o11043)(void* p, uint32_t v);
    uint32_t (*o10653)();
    // D2Game unmapped / mapped RVAs
    int      (*f9FA30)(void* game, int* out);
    int      (*f9FA30u)(void* unit, void* game, int* outRegX, int* outStackY);   // 0x9FA30: eax=unit, ecx=game, ebx=&x, stack &y
    uint32_t (*fE19C0_d019c0)(uint32_t y, uint32_t x, uint32_t z, void* game, void* unit, uint32_t z2, uint32_t z3); // 0xD019C0: eax=y ecx=x edx=0, 4 stack args
    void*    (*f_d003a0)(void* game, uint32_t id);        // 0xE03A0
    void*    (*f_d003a0t)(void* game, uint32_t type, uint32_t id); // 0xE03A0: ecx=game, eax=owner type, edx=owner id
    uint32_t (*f_d00a60)(void* a, void* game);            // 0xE0A60
    uint32_t (*f_d00a60u)(void* u, void* src, void* game); // 0xE0A60: eax=u, ecx=src, stack game
    void*    (*f21420id)(uint32_t id);                    // 0x1420: eax=skill id
    uint8_t* (*f21250id)(uint32_t classId);               // 0x1250: eax=class id
    void*    (*f21420)(void* p);
    uint8_t* (*f21250)(void* p);
    uint32_t (*f21100)(void* unit);                       // 0x1100 unit x
    uint32_t (*f21130)(void* unit);                       // 0x1130 unit y
    uint32_t (*f21280)(void* room, uint32_t x, uint32_t y);   // 0x1280 room at (x, y): eax = room, ebx = x, edi = y
    void*    (*f8F930)(void* game, uint32_t* params);     // 0x6F930 CreateMissile
    void*    (*fCC28F0)(void* game, uint32_t a, void* unit, uint32_t b, uint32_t c, int d, int e, int f, int g, int h);
    void     (*fD007A0)(void* game);
    bool     (*fCC0630)(uint32_t game, void* unit);       // 0xA0630
    void     (*fAEA80)(void* game, void* unit, int a, uint32_t b, int c, int d, int e);
    void     (*fAE6F0)(void* item);                       // 0x8E6F0
    int      (*fB1340)(void* p);                          // 0x91340
    uint32_t (*fFE5A0)(void* game, uint32_t a, int b);    // 0xDE5A0
    void     (*fBF210)();                                 // 0x9F210, seam: reads ambient regs
    void     (*fBE9B0)(uint32_t ecx, int v);              // 0x9E9B0
    void     (*fFDDE0)(int v, void* a, void* b);          // 0xDDDE0
    void     (*fFED30)(void* a, void* b, void* c);        // 0xDED30
    void     (*f648E0)(void* a, void* b, uint32_t cb, void* c); // 0x448E0
    void     (*fCC11B0)(int skillId, void* game, void* unit, void* target, uint32_t* blk, int z); // 0xA11B0, seam: block is the 0x70-byte local
    void     (*fFD450)(void* game, void* unit, void* target, uint32_t* blk, int z, uint8_t srcDam); // 0xDD450
    void     (*f214D0)(int v, void* game);               // 0x14D0
    // 0x45CE0 / 0x46560 seams (ordinals whose arg order is taken from the decompile)
    uint32_t (*o10511)(void* unit);                       // used-skill frame / flags
    int      (*o10601)();                                 // skill id from ambient skill reg
    void     (*f21180)(void* p);                          // 0x1180 on unit+0x20
    uint32_t (*f21c20)();                                 // 0x1C20
    void     (*fAE900)(int v);                            // 0x8E900
    void     (*o10188)(void* it, int stat, int val, int layer);
    void     (*o10243)(int a, int b);
    uint32_t (*o10786r)(void* unit, uint32_t calc, uint32_t a, uint32_t b); // formula eval, returns value
    int      (*fBEE50)(int z, int* out, void* unit, int z2, int skillId, uint32_t lvl); // 0xFBEE50
    int      (*fBFA30)(void* game, int* out);             // 0xBFA30
    int*     (*fC3610)(void* game);                       // 0xC3610
    void     (*fD0FE40)(void* game);                      // 0x10FE40
    void     (*f6E2A0)(void* n, uint32_t calc, int* p, uint32_t q); // 0x6E2A0
    void     (*f6F970)(uint32_t calc, void* unit, int* p, int id, uint32_t q); // 0x6F970
    uint32_t bitMask4;                                    // gdwBitMasks[4]
    // 0x4A030 globals
    int      clientsInit;                                 // DAT_6fd31c18
    void     (*enterCS)();                                // Ordinal 10050
    void     (*leaveCS)();
    void**   clientBuckets;                               // DAT_6fd307b8, 0x100 entries
};

// entry points with FN probes (src/core/fn/d2game_<rva>.cpp)
uint32_t sub_434E0(D2GameBatch43450Env& e, void* game, void* src, void* tgt, uint32_t p3, uint32_t p4, int p5);
uint32_t sub_43610(D2GameBatch43450Env& e, void* game, int* unit, int skillId, uint32_t arg);
uint32_t sub_437D0(D2GameBatch43450Env& e, void* game, int* unit, int skillId, uint32_t arg);
void sub_439A0(D2GameBatch43450Env& e, int* unit, void* game);
