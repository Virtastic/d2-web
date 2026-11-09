// Seam table of the twelve-function item/monster batch in d2game_batch_13A30.cpp (retail argument order at each call site).
// Shared so the FN probes build exactly the struct the ports read.
#pragma once
#include <cstdint>

struct D2GameBatchB87Env {
    // D2Common data tables: the dword at 0x6FD1829C dereferenced. +0xC0C/+0xC10 are the tier table and
    // its row count (0x13A30, stride 0x128); +0xC18/+0xC1C the drop table and count (0x13C20, stride 0x1B8).
    void*    dataTableBase;
    // 0x6FD22CE4: the dword array 0xCB950 indexes with tableIndex*12.
    void*    table6fd22ce4;
    // 0x6FD05D20 / 0x6FD03A80: the two state callbacks 0xE73C0 invokes through 0x6FCFFCC0. Retail
    // checks the literal address for null and fatals when it is; a null member models that.
    void*    cbStateChange;
    void*    cbAfterState;
    // 0x8A3E0: append `len` bytes of `buf` to the client's outgoing queue (client in EAX, buf/len pushed).
    void     (*send8A3E0)(void* client, const void* buf, uint32_t len);
    // 0x6FCFFCC0: event dispatch; EAX=0, EBX=callback, EDI=unit, one stack argument.
    void     (*invokeCallback)(uint32_t eaxArg, void* callback, uint32_t stackArg, void* ediArg);
    // 0xE73C0's sprintf("changing state to %d"-shaped retail text, state) + trace when the log global is
    // set. The retail format string is retail content and is not reproduced; only the state passed on.
    void     (*traceState)(void* unit, uint32_t state);
    // 0x8D3D0: EBX=client, ESI=item, stack (nAction, dwFlag, bGamble); ret 0xC.
    void     (*sub8D3D0)(void* client, void* item, uint32_t nAction, uint32_t dwFlag, uint32_t bGamble);
    // 0x101D0: ESI=unit, EDI=playerUnit; returns the party/owner word, 1 for the unit itself, else 0.
    uint16_t (*sub101D0)(void* unit, void* playerUnit);
    // 0xCBBE0: EAX=unit; returns the monster's display word used in the 0x75 packet.
    uint32_t (*subCBBE0)(void* unit);
    // D2Common ordinals. Signatures follow the retail stack order seen at each call site.
    uint32_t (*ordinal10086)(void* item);                                    // #10086 item level, min 1
    int32_t  (*ordinal10110)(void* unit, void* entry);                       // #10110 entry filter
    int32_t  (*ordinal10174)(uint32_t questPtr, uint32_t a, uint32_t b);     // #10174 quest bit read
    void*    (*ordinal10220)(void);                                          // #10220 fixed table struct
    void*    (*ordinal10411)(void* item);                                    // #10411 item seed pointer
    uint32_t (*ordinal10513)(void* pSeed);                                   // #10513 seed low dword
    void     (*ordinal10536)(void* item, uint32_t value);                    // #10536 set item +0x28
    uint32_t (*ordinal10587)(void* item, uint32_t stat, uint32_t sub);       // #10587 stat get
    void     (*ordinal10614)(void* pSeed, uint32_t value);                   // #10614 reseed (ECX,EDX)
    void*    (*ordinal10695)(uint32_t unitId);                               // #10695 item data record
    int32_t  (*ordinal10711)(void* item);                                    // #10711 item flag
    uint32_t (*ordinal10718)(void* item);                                    // #10718 item format word
    int32_t  (*ordinal10744)(void* item, uint32_t itemTypeId);               // #10744 item type check
    uint32_t (*ordinal10750)(void* rec);                                     // #10750 word at rec+6
    int32_t  (*ordinal10865)(void* item);                                    // #10865 item flag
    uint32_t (*ordinal10867)(void* rec);                                     // #10867 word at rec+2
    void     (*ordinal10887)(void* item, uint32_t stat, uint32_t value, uint32_t flag); // #10887 set stat
    uint32_t (*ordinal10910)(void* item, uint32_t stat, uint32_t sub);       // #10910 stat base get
    void*    (*ordinal10920)(void* unit);                                    // #10920 player data
    void*    (*ordinal10926)(void);                                          // #10926 fixed table ptr
    uint32_t (*ordinal10973)(void* item, uint32_t stat, uint32_t sub);       // #10973 stat get
    void     (*ordinal10977)(void* item, uint32_t bit, uint32_t set);        // #10977 item flag bit
    void     (*ordinal10996)(uint32_t a, uint32_t b, void* item, uint32_t c,
                             uint32_t d, uint32_t e);                        // #10996 item update
    uint32_t (*ordinal11027)(void* unit);                                    // #11027 unit word
    // DAT_6fd31c20 (D2Game+0x111C20): when nonzero 0xE73C0 also calls the trace function 0xCBE70. Last member: probes that declare a prefix never read it.
    uint32_t traceGlobal;
};
