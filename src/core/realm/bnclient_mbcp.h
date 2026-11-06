// src/core/realm/bnclient_mbcp.h -- the BNClient CRT multibyte code-page state, named at its retail RVAs, and the seams of
// the _setmbcp pair (0x64E3 worker, 0x6677 __setmbcp) and ___initmbctable (0x67C7).
#pragma once
#include <cstdint>

struct BnMbcpGlobals {
    uint8_t* ctype;               // 0x1F980: 257 bytes (the ctype table and the byte after it)
    uint32_t* cp;                 // 0x1FA84: the current code page
    uint32_t* mb_flag;            // 0x1F960: 1 = a multibyte code page
    uint32_t* lang;               // 0x1F958: the language id for the code page
    uint32_t* extra;              // 0x1FA90: three dwords (read back as five u16 by __setmbcp)
    uint32_t* has_default;        // 0x1E65C: a pseudo code page (-2/-3/-4) was asked for
};
struct BnSetMbcpSeams {
    const uint8_t* flags4;        // 0x1D8E0: the four class flags of the built-in lead ranges
    const uint8_t* builtin;       // 0x1D8E8: five 0x30-byte rows (cp, three extra dwords, four 8-byte lead-range lists)
    int32_t (*get_cp_info)(uint32_t cp, uint8_t* cpinfo20);   // GetCPInfo (IAT 0x19130)
    void (*set_sbcs)();           // 0x631F setSBCS
    void (*setsbuplow)();         // 0x6348 the case tables
};
struct BnMbcpWrapSeams {
    void (*lock)(int32_t);        // 0x3163 __lock
    void (*unlock)();             // 0x67BE (unlock 0xD)
    uint32_t (*oemcp)();          // GetOEMCP (IAT 0x1912C)
    uint32_t (*acp)();            // GetACP (IAT 0x19128)
    void* (*alloc)(uint32_t);     // 0x2544 _malloc
    void (*release)(void*);       // 0x3CF4 _free
    int32_t (*setmbcp)(uint32_t); // 0x64E3
};
struct BnMbcpSaved {
    void** saved;                 // 0x1F95C: the 0x220-byte snapshot block, or null
    const uint8_t* tbl256;        // 0x1FAA0: 256 bytes
    const uint32_t* default_cp;   // 0x1E6A8: the -4 code page
};

extern "C" int32_t bn_setmbcp(uint32_t cp, const BnMbcpGlobals* g, const BnSetMbcpSeams* s);
extern "C" int32_t bn_setmbcp_wrap(int32_t cp, const BnMbcpGlobals* g, const BnMbcpSaved* sv, const BnMbcpWrapSeams* s);
extern "C" int32_t bn_initmbctable(int32_t* flag, void (*setmbcp)(int32_t));
