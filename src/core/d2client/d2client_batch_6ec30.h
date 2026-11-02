// src/core/d2client/d2client_batch_6ec30.h -- the Env of d2client_batch_6ec30.cpp (its FN probes include it).
#pragma once
#include <cstdint>
#include <cstddef>
struct CmdEntry { uint32_t name; uint16_t kind; uint32_t pad; uint32_t flag; } __attribute__((packed)); // 0xE
static_assert(sizeof(CmdEntry) == 14, "CmdEntry");

struct D2ClientBatch6EC30Env {
    // globals
    CmdEntry* cmdTable;      uint32_t cmdCount;   // 0xB285C table, (0xB2885-0xB285C)/14+1 entries
    uint32_t  cmdAllowAll;   // 0xBC394
    uint32_t  curUnit;       // 0xBBBFC DAT_6fbcbbfc
    uint32_t  flagAAE18;     // D2Client+0xFAE18 (0x6FBAAE18)
    uintptr_t unitTable;     // 0xBA608 (stride 0x80)
    uintptr_t dataTables;    // *sgptDataTables
    uint32_t  bitMask8;      // gdwBitMasks[8]
    // callees
    int32_t   (*strlen_)(const char*);                                   // Ordinal_506
    int32_t   (*cmp)(const char*, const char*, int32_t);                 // Ordinal_509 (arg order guessed)
    void      (*fmt)(const char*, char*, int32_t, const char*);          // Ordinal_504 (args guessed)
    void      (*cat)(uint32_t, const char*, int32_t);                    // Ordinal_501
    uintptr_t (*lookup)(uint32_t id, uintptr_t slot, uint32_t idx);      // 0xA4E20
    void      (*fn51B10)(uintptr_t, int32_t, int32_t, int32_t, uint8_t); // 0xA1B10
    void      (*fn52950)(uintptr_t, int32_t, int32_t, int32_t);          // 0xA2950
    void      (*fn79030)(uint32_t);                                      // 0xC9030
    void      (*fn2A1B0)(uint32_t, uint32_t, uint32_t);                  // 0x2A1B0
    void      (*fn2A750)(uint32_t, uintptr_t, const uint32_t*);          // 0x2A750
    void      (*fnC240)(void);                                           // 0xAC240
    void      (*set)(uintptr_t, uint32_t, uint32_t, uint32_t);           // Ordinal_10887
    void      (*fn2A6C0)(void);                                          // 0x2A6C0
    void      (*fn6210)(void);                                           // 0xA6210
    uintptr_t (*fn630F0)(uint32_t*);                                     // 0xB30F0
    void      (*s10069)(uintptr_t, uint8_t, uint32_t);                   // Ordinal_10069
    void      (*s10970)(uintptr_t, uint16_t);                            // Ordinal_10970
    uint32_t  (*fn1380)(uintptr_t);                                      // 0x1380
    void      (*fn50250)(const void* copy7);                             // 0xA0250, ESI = &7-byte packet copy
    void      (*fn4BC80)(const void* copy15);                            // 0x4BC80, ESI = &15-byte packet copy
    uint32_t  (*s10047)(void);                                           // Ordinal_10047
};
extern "C" void d2c_af980(D2ClientBatch6EC30Env* e, uintptr_t p);
extern "C" void d2c_af9b0(D2ClientBatch6EC30Env* e, uintptr_t p);
extern "C" uint32_t d2c_afc10(D2ClientBatch6EC30Env* e);
extern "C" void d2c_aeca0(D2ClientBatch6EC30Env* e, uint16_t v);
