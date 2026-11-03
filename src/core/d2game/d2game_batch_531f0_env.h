// Seams of the 0x531F0 batch ports (d2game_batch_531f0.cpp): retail-layout field walks and callees, injected so a FUNCTION-mode probe can record them.
#pragma once
#include <cstdint>
struct D2GameBatch531F0Env {
    int32_t   (*counterIndex)(void* obj);          // FUN_6fc78260
    uint8_t*  (*counterBlock)(void* obj);          // obj->+0x18 (native pointer walk stays in the seam)
    void      (*cBE70)(uint32_t id, uint32_t msgRva, int32_t n);  // D2Game+0xCBE70 (edx = message at module rva msgRva, stack = id, n; ret 8)
    int       (*debugEnabled)(void);               // DAT_6fd31c20 != 0
    void*     (*firstQuestNode)(void* game);       // **(game+0x10f4)
    void*     (*nextQuestNode)(void* node);        // node+0xF4
    int32_t   (*nodeType)(void* node);             // node[0]
    uint8_t   (*nodeFlag93)(void* node);           // node[6]+0x93
    void*     (*questControl)(void* game);         // game+0x10f4 (nonzero check)

    // Field walks on retail-layout objects: offsets are retail 32-bit offsets.
    uint8_t   (*rd8)(void* obj, uint32_t off);
    uint16_t  (*rd16)(void* obj, uint32_t off);
    uint32_t  (*rd32)(void* obj, uint32_t off);
    void*     (*ptr)(void* obj, uint32_t off);     // pointer-valued field

    // D2Common quest/unit ordinals, retail argument order.
    int32_t   (*ord10174)(uint32_t rec, int32_t a, int32_t b);  // quest flag get
    void      (*ord10003)(uint32_t rec, int32_t a, int32_t b);  // quest flag set
    uint32_t  (*questRec)(void* client, uint32_t diff);         // rd32(Ordinal_10920(client)+0x10+diff*4)
    int32_t   (*ord10587)(void* client, int32_t a, int32_t b);
    uint32_t  (*ord10331)(void* unit);                          // unit room handle
    void      (*ord10383)(uint32_t room, uint32_t** arr, uint32_t* count);
    void      (*ord11090)(void* obj, int32_t n);
    uint8_t   (*ord10417)(void* obj, int32_t n, uint32_t* out);
    void      (*ord10466)(uint32_t handle, void* obj);
    uint32_t  (*tableCaa8)(uint32_t idx);                       // DAT_6fd2caa8[idx]

    // D2Game internal callees, named by RVA.
    int32_t   (*c1130)(void* obj);
    int32_t   (*ord10057)(uint32_t room);                  // Ordinal_10057: nonzero for a town room
    uint32_t  (*cA0000)(void* a, int32_t a1, int32_t a3, int32_t a4, int32_t a2, int32_t xb, int32_t ya); // D2Game+0xA0000, esi = a
    int32_t   (*c1100)(void* obj);
    int32_t   (*c76A0)(int32_t p3, void* p2, uint32_t id);       // D2Game+0xD76A0
    int32_t   (*c60D0)(void* p2, int32_t p3, uint8_t b, void* p1, uint32_t one);  // +0xD60D0
    int32_t   (*c4E80)(void* p2, int32_t p3, int32_t v, uint32_t zero, uint32_t* scratch); // +0xD4E80
    int32_t   (*c1650)(void);                                    // +0xD1650
    int32_t   (*c4A70)(int32_t p3, void* p2, uint32_t id, uint32_t* scratch);  // +0xD4A70
    void      (*c33EB0)(void* obj, uint32_t handle, int32_t kind);             // +0x33EB0
    // Second batch: field writes, more ordinals, and callees named by RVA.
    void      (*wr8)(void* obj, uint32_t off, uint8_t v);
    void      (*wr16)(void* obj, uint32_t off, uint16_t v);
    void      (*wr32)(void* obj, uint32_t off, uint32_t v);
    uint32_t  (*tableB2d8)(uint32_t idx);                  // DAT_6fd2b2d8[idx]
    void      (*ord10593)(uint32_t rec, int32_t a, int32_t b);
    int32_t   (*ord10826)(uint32_t room);                  // room -> class/level id
    uint8_t   (*ord10459)(int32_t cls);
    int32_t   (*ord10802)(uint32_t handle);
    void      (*fatal)(uint32_t line);                     // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    void      (*c34C00)(void* client, void* game);         // D2Game+0x34C00 (eax = client, stack = game, ret 4)
    void      (*c35EB0)(uint32_t mode);                    // D2Game+0x35EB0, mode in EAX (disassembly)
    void      (*c5BF70)(void* game, void* client);         // D2Game+0x5BF70, client in EAX
    void      (*c5E0E0)(void* game, void* node, void* sub);// D2Game+0x5E0E0
    void      (*c8EA80)(void* game, void* p, int32_t sel, uint32_t arg, int32_t z1, int32_t z2, int32_t z3); // +0x8EA80
    void      (*c64940)(void* ecx);                        // D2Game+0x64940
    void      (*c354F0)(uint32_t al, void* client, void* game);  // D2Game+0x354F0 (al, ecx = client, stack = game, ret 4)
    int32_t   (*c89590)(void);                             // D2Game+0x69590
    int16_t   (*cFA60)(void);                              // D2Game+0xDFA60
    void      (*cDFFD0)(int32_t a, void* client);          // D2Game+0xDFFD0
    void      (*cBC160)(uint32_t token);                   // D2Game+0x9C160: callback registration by RVA token
    int16_t   (*cBBEB0)(void);                             // D2Game+0x9BEB0
    int32_t   (*c2E660)(uint32_t id);                      // D2Game+0x2E660
    void*     (*c53F30)(uint32_t handle, void* game);      // D2Game+0x33F30 (nullable), eax = game
    bool      (*badCodePtr)(void* obj, uint32_t off);      // IsBadCodePtr(obj+off)
    uint32_t  (*callCodePtr)(void* obj, uint32_t off);     // (*(code*)(obj+off))()
    void      (*cEEF10)(void* rec);                        // D2Game+0xEEF10 (spawn record)
    void      (*c34130)(uint32_t room, int32_t* coord, int32_t n, uint32_t flags); // D2Game+0x34130
    // Third batch: remaining callees and ordinals.
    void      (*ord10141)(void* msg, uint32_t* out);       // Ordinal_10141(msg, out)
    void*     (*c36820)(int32_t n);                        // D2Game+0x36820 (nullable)
    void      (*cFFCC0)(void* obj, int32_t n);             // D2Game+0xFFCC0, object form
    void      (*cFFCC0Token)(uint32_t rva, int32_t n);     // D2Game+0xFFCC0, callback-token form
    void      (*c5EC30)(void* obj);                        // D2Game+0x5EC30
    void      (*c33830)(void* obj, uint32_t off);   // D2Game+0x33830, address of obj+off
    int32_t   (*c58260)(void* obj);                 // D2Game+0x58260, counter index (-1 if none)
    void      (*ord10346)(uint32_t room);
    uint32_t  (*ord10331Sel)(void* unit, int32_t sel);     // Ordinal_10331(unit, sel)
    void      (*ord10383Ptrs)(uint32_t room, void*** arr, uint32_t* cnt);  // Ordinal_10383, object array
    int32_t   (*ord10522)(void* node, void* unit);
    // Player-block load (0x55E10): stat ordinals and the section readers it chains.
    int32_t   (*statGet)(void* unit, int32_t id, int32_t layer);               // Ordinal_10973
    void      (*statSet)(void* unit, int32_t id, int32_t v, int32_t layer);    // Ordinal_10887
    int32_t   (*statMaxHp)(void* unit);                                        // Ordinal_10049
    int32_t   (*statMaxMana)(void* unit);                                      // Ordinal_11060
    int32_t   (*ord10399)(int32_t a, int32_t b);
    void*     (*playerData)(void* unit);                                       // Ordinal_10920
    void      (*cA2100)(void* unit, int32_t sel, void* item, int32_t z);       // D2Game+0xA2100, ecx = unit, edx = sel
    uint32_t  (*c55320)(void* data, int32_t p1, void* p2, void** outUnit, uint32_t* size, uint32_t* o14, uint32_t* o8,
                        uint32_t* o4, uint32_t* o10, uint32_t* oc, uint32_t* used);        // D2Game+0x55320, edx = data
    uint32_t  (*c55840)(int32_t p1, void* p2, void** out);                  // D2Game+0x55840, edi = p1
    uint32_t  (*c530D0)(void* data, void* unit, uint32_t* used);               // edx = data
    uint32_t  (*c53040)(void* unit, void* data, uint32_t* used);               // eax = unit, edx = data
    uint32_t  (*c53340)(void* data, void* unit, uint32_t* used);               // eax = data, ebx = unit
    uint32_t  (*c53640)(void* data, int32_t p1, int32_t n, uint32_t* used, void* unit); // eax = data, edi = unit
    uint32_t  (*c53D40)(void* unit, int32_t p2, void* data, uint32_t* used);   // esi = unit
    uint32_t  (*c55050)(uint32_t v18, int32_t p1, void* unit, void* data, uint32_t* used, uint32_t rem); // edx = v18, esi = &used, edi = 0
    uint32_t  (*c55A10)(int32_t rem, void* data, int32_t p1, void* unit, uint32_t v18, uint32_t* used); // eax = rem, ecx = data
    uint32_t  (*c55AD0)(int32_t p1, void* unit, void* data, uint32_t v18, int32_t rem, uint32_t* used);
    void      (*c534D0)(void* unit, void* data, uint32_t v18, uint32_t* used);
    void      (*c8A670)(void* p2);
    // Disassembly-true seams (the cached decompile loses register arguments; sm-56950 rows reviewed 2026-11-02 use these):
    void      (*c53EB0)(uint32_t mode, void* obj, uint32_t a, int32_t b);      // 0x53EB0: EAX mode, ECX obj, stack a, b
    int32_t   (*c69590g)(void* game);                                          // 0x69590: stack game
    void*     (*cDFCC0)(uint32_t eax, uint32_t cbRva, void* game, uint32_t s1);  // 0xDFCC0: EAX unit class, EBX callback RVA, EDI game, stack s1
    void*     (*c36820r)(uint32_t eaxWord, int32_t n, uint32_t* esiBuf, void* game, void* msg);
    uint16_t  (*gTblW)(uint32_t off);                      // word at (*DAT_6fd1829c)+off
    void      (*logStarted)(void* obj);            // LogStartedQuestDebug body (Ordinal/log seam)
};
