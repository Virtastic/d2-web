// src/core/d2game/d2game_batch_531f0.cpp -- 1.13c D2Game.dll helpers from the cached decompiles.
// Pointer-bearing rows take their pointer walks through injected seams (see D2GameBatch531F0Env),
// so the retail 32-bit field offsets are never read as wasm64 pointers here.
// Retail handles (quest records, rooms, handles stored in struct fields) are uint32_t; object pointers
// are void* and their fields are read through rd8/rd16/rd32/ptr, which take retail offsets.
#include <cstdint>
// Hex_ByteToChars: writes the two hex digits of the low byte of eax into out[0..1] (high nibble first).
// 1.13c D2Game+0x531F0 (fastcall: eax = byte, ecx = out)
void d2game_531F0(uint32_t byteValue, uint8_t* out) {
    static const char digits[] = "0123456789abcdef";
    out[0] = (uint8_t)digits[(byteValue & 0xff) >> 4];
    out[1] = (uint8_t)digits[byteValue & 0xf];
}

// Hex_DumpToString: writes "0x" then two hex digits per byte, then a NUL.
// Returns NULL when n < 1, otherwise a pointer to the terminating NUL.
// 1.13c D2Game+0x533D0 (fastcall: ecx = n, edx = src, eax = out)
uint8_t* d2game_533D0(int n, const uint8_t* src, uint8_t* out) {
    static const char digits[] = "0123456789abcdef";
    if (n < 1) return nullptr;
    out[0] = '0';
    out[1] = 'x';
    uint8_t* p = out + 2;
    do {
        uint8_t b = *src++;
        p[0] = (uint8_t)digits[b >> 4];
        p[1] = (uint8_t)digits[b & 0xf];
        p += 2;
    } while (--n != 0);
    *p = 0;
    return p;
}

// RemoveIdFromSmallList: list A (selector 0) is int[] at +0 with a ushort count at +0x30;
// list B (selector nonzero) is int[] at +0x34 with a ushort count at +0x64. Removal swaps in the
// last element. Returns 1 when the id was found and removed, 0 otherwise.
// 1.13c D2Game+0x602A0 (fastcall: eax = list base, ecx = selector, edx = id)
int d2game_602A0(uint8_t* base, int selector, int id) {
    if (selector == 0) {
        uint32_t count = *(uint16_t*)(base + 0x30);
        for (uint32_t i = 0; i < count; ++i) {
            int32_t* items = (int32_t*)base;
            if (items[i] == id) {
                if (i != count - 1) items[i] = items[count - 1];
                *(uint16_t*)(base + 0x30) = (uint16_t)(count - 1);
                return 1;
            }
        }
        return 0;
    }
    uint32_t count = *(uint16_t*)(base + 0x64);
    int32_t* items = (int32_t*)(base + 0x34);
    for (uint32_t i = 0; i < count; ++i) {
        if (items[i] == id) {
            if (i != count - 1) items[i] = items[count - 1];
            *(uint16_t*)(base + 0x64) = (uint16_t)(count - 1);
            return 1;
        }
    }
    return 0;
}

// ACT5_Counter_Bump seams: idx from the object, and the counter block the object points at.
// One env struct serves every row in this file. The play path assigns the seams; they stay null until then.
struct D2GameBatch531F0Env {
    int32_t   (*counterIndex)(void* obj);          // FUN_6fc78260
    uint8_t*  (*counterBlock)(void* obj);          // obj->+0x18 (native pointer walk stays in the seam)
    void      (*logStarted)(void* obj);            // LogStartedQuestDebug body (Ordinal/log seam)
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
    void      (*c34C00)(void);                             // D2Game+0x34C00
    void      (*c35EB0)(void);                             // D2Game+0x35EB0
    void      (*c5BF70)(void* game);                       // D2Game+0x5BF70
    void      (*c5E0E0)(void* game, void* node, void* sub);// D2Game+0x5E0E0
    void      (*c8EA80)(void* game, void* p, int32_t sel, uint32_t arg, int32_t z1, int32_t z2, int32_t z3); // +0x8EA80
    void      (*c64940)(void* ecx);                        // D2Game+0x64940
    void      (*c354F0)(void* client, void* game);         // D2Game+0x354F0
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
};
D2GameBatch531F0Env g_d2game531F0Env{};  // seams are assigned by the play path; zero until wired

// ACT5_Counter_Bump: idx = FUN_6fc78260(obj); if idx >= 0, the counter at block+0x114+idx*4 goes up by one,
// and when it passes 4 the flag byte at block+0x108+idx is set.
// 1.13c D2Game+0x59360
void d2game_59360(void* obj) {
    auto& E = g_d2game531F0Env;
    int32_t idx = E.counterIndex(obj);
    if (idx < 0) return;
    uint8_t* blk = E.counterBlock(obj);
    int32_t v;
    __builtin_memcpy(&v, blk + 0x114 + idx * 4, 4);
    v += 1;
    __builtin_memcpy(blk + 0x114 + idx * 4, &v, 4);
    if (v > 4) blk[0x108 + idx] = 1;
}

// LogStartedQuestDebug: when the debug flag is set, log "Started quest" against the unit.
// 1.13c D2Game+0x635B0
void d2game_635B0(void* obj) {
    auto& E = g_d2game531F0Env;
    if (E.debugEnabled()) E.logStarted(obj);
}

// QuestNodeType4FlagCheck: walks the quest node list at game+0x10f4 for the first node of type 4 and
// returns whether its flag byte (node[6]+0x93) is set. An empty list or no type-4 node returns false.
// 1.13c D2Game+0x60390
bool d2game_60390(void* game) {
    auto& E = g_d2game531F0Env;
    if (E.questControl(game) == nullptr) return false;
    for (void* node = E.firstQuestNode(game); node != nullptr; node = E.nextQuestNode(node)) {
        if (E.nodeType(node) == 4) return E.nodeFlag93(node) != 0;
    }
    return false;
}

// 1.13c D2Game+0x545D0 (the decompile lost in_EAX; eaxArg carries it: the object whose +0xc is read in case 6)
uint32_t d2game_545D0(const D2GameBatch531F0Env& E, void* p1, void* p2, int32_t p3, void* eaxArg) {
    uint32_t sel = 0;
    if (p1 != nullptr) {
        sel = E.rd32(p1, 0x10);
        if (sel > 6) return 0xc;
    }
    int32_t r = 0;
    switch (sel) {
    case 1:
        E.ord11090(p1, 4);
        {
            uint32_t scratch = 0;
            uint8_t b = E.ord10417(p1, 1, &scratch);
            r = E.c60D0(p2, p3, b, p1, 1);
        }
        break;
    case 2:
        E.ord11090(p1, 4);
        {
            int32_t v = E.c1100(p1);
            uint32_t scratch = 0;
            r = E.c4E80(p2, p3, v, 0, &scratch);
        }
        break;
    case 3:
    case 5:
        return 0xc;
    case 4:
        E.ord11090(p1, 4);
        E.ord10466(E.rd32(p2, 0x60), p1);
        r = E.c1650();
        break;
    case 6:
        E.ord11090(p1, 4);
        if (eaxArg == nullptr) return 0xc;
        {
            uint32_t scratch = 0;
            r = E.c4A70(p3, p2, E.rd32(eaxArg, 0xc), &scratch);
        }
        return r != 0 ? 0 : 0xc;
    default: {
        E.ord11090(p1, 4);
        uint32_t id = (p1 == nullptr) ? 0xffffffffu : E.rd32(p1, 0xc);
        E.c1130(p1);
        E.c1100(p1);
        r = E.c76A0(p3, p2, id);
        break;
    }
    }
    return r != 0 ? 0 : 0xc;
}

// 1.13c D2Game+0x56E00 (__thiscall: obj = this, rec = param_2 quest record handle, out = param_3)
// Return value keeps the decompile's CONCAT31(..., 1): the low byte is always 1.
static inline uint32_t d2game_bool31(int32_t v) { return ((uint32_t)v & 0xffffff00u) | 1u; }
uint32_t d2game_56E00(const D2GameBatch531F0Env& E, void* obj, uint32_t rec, uint8_t* out) {
    int32_t iv;
    *out = 0;
    if (E.ord10174(rec, 0x23, 1) == 0) {
        if (E.ord10174(rec, 0x23, 0xd) == 0) {
            iv = 0;
            if (E.rd8(obj, 9) != 0) {
                iv = E.ord10174(rec, 0x23, 0);
                if (iv == 0) {
                    iv = E.ord10174(rec, 0x23, 0xe);
                    if (iv != 0) {
                        *out = 0xc;
                        return d2game_bool31(iv);
                    }
                    iv = 0;
                    if (E.rd8(obj, 0xc) < 5) {
                        *out = E.rd8(obj, 0xb);
                        return d2game_bool31(1);
                    }
                }
            }
            return d2game_bool31(iv);
        }
    }
    iv = E.ord10174(rec, E.rd32(obj, 0xe0), 5);
    if (iv != 0) {
        *out = 4;
        return d2game_bool31(iv);
    }
    *out = 3;
    return d2game_bool31(0);
}

// 1.13c D2Game+0x57750 (fastcall: game = param_1, client = param_2)
// Quest 0x27 gets 2 or 3 from the type-0x23 node's byte at +0xC, once the 0x27 flag is not 1 and the
// client's counter at ordinal 10587 (0xC,0) reaches (difficulty+1)*0x14.
uint32_t d2game_57750(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t diff = E.rd8(game, 0x6d);
    uint32_t rec = E.questRec(client, diff);
    if (E.ord10174(rec, 0x27, 0) == 1) return 0;
    if (E.ord10587(client, 0xc, 0) < (int32_t)((diff + 1) * 0x14)) return 0;
    if (E.ptr(game, 0x10f4) == nullptr) return 0;
    void* node = E.ptr(E.ptr(game, 0x10f4), 0);
    if (node == nullptr) return 0;
    while (E.rd32(node, 0) != 0x23) {
        node = E.ptr(node, 0xf4);
        if (node == nullptr) return 0;
    }
    if (E.rd8(node, 0xc) == 2) {
        E.ord10003(rec, 0x27, 2);
        return 0;
    }
    if (E.rd8(node, 0xc) == 3) E.ord10003(rec, 0x27, 3);
    return 0;
}

// 1.13c D2Game+0x57900 (fastcall: game = param_1, client = param_2)
// MISMATCH risk: the decompile reuses one stack slot (iStack_28) for the difficulty index and the Ordinal_10383
// output array. The index is taken from the game, as in 57750; a null type-0x22 node returns 0 (retail crashes).
uint32_t d2game_57900(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t room = E.ord10331(client);
    if (room == 0) return 0;
    uint32_t diff = E.rd8(game, 0x6d);
    uint32_t rec = E.questRec(client, diff);
    if (E.ord10174(rec, 0x26, 0xe) != 0) return 0;
    if (E.ord10174(rec, 0x26, 0) != 0) return 0;
    if (E.ptr(game, 0x10f4) == nullptr) return 0;
    void* node = E.ptr(E.ptr(game, 0x10f4), 0);
    while (node != nullptr && E.rd32(node, 0) != 0x22) node = E.ptr(node, 0xf4);
    if (node == nullptr) return 0;
    void* sub = E.ptr(node, 0x18);
    uint32_t want = E.rd32(sub, 0x8c);
    bool match = (room == want);
    if (!match) {
        uint32_t* arr = nullptr;
        uint32_t cnt = 0;
        E.ord10383(room, &arr, &cnt);
        for (uint32_t i = 0; !match && i < cnt; ++i) {
            if (arr[i] == want) match = true;
        }
    }
    rec = E.questRec(client, diff);
    if (match && (E.ord10174(rec, 0x25, 0) != 0 || E.ord10174(rec, 0x25, 1) != 0) &&
        E.ord10174(rec, 0x26, 0) == 0 && E.ord10174(rec, 0x26, 1) == 0) {
        E.ord10003(rec, 0x26, 1);
        E.ord10003(rec, 0x26, 0xd);
    }
    return 0;
}

// 1.13c D2Game+0x57A60 (fastcall: obj = param_1 (this), msg = param_2)
// Ordinal_10174(uVar1) has its arguments lost in the decompile; (0x25,0) is assumed, by analogy with 0x57900.
void d2game_57A60(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    void* game = E.ptr(obj, 4);
    void* client = E.ptr(msg, 0xc);
    uint32_t rec = E.questRec(client, E.rd8(game, 0x6d));
    E.ord10174(rec, E.rd32(obj, 0xe0), 0);
    E.ord10174(rec, E.rd32(obj, 0xe0), 0xd);
    void* m8 = E.ptr(msg, 8);
    int32_t kind = (m8 == nullptr) ? -1 : (int32_t)E.rd32(m8, 4);
    if (E.ord10174(rec, 0x25, 0) == 0 && E.ord10174(rec, 0x25, 1) == 0) return;
    uint32_t target = E.rd32(msg, 0x14);
    if (kind == 0x200 && E.rd8(obj, 0xc) == 1) {
        if (E.ord10174(rec, E.rd32(obj, 0xe0), 1) == 0 && E.ord10174(rec, E.rd32(obj, 0xe0), 0) == 0) {
            E.c33EB0(obj, target, 0x200);
        }
        return;
    }
    if (E.ord10174(rec, E.rd32(obj, 0xe0), 1) != 0) {
        E.c33EB0(obj, target, kind);
        return;
    }
    void* m12 = E.ptr(msg, 0xc);
    int32_t id = (m12 == nullptr) ? -1 : (int32_t)E.rd32(m12, 0xc);
    uint32_t n = E.rd16(obj, 0x9c);
    for (uint32_t i = 0; i < n; ++i) {
        if ((int32_t)E.rd32(obj, 0x1c + i * 4) == id) {
            E.c33EB0(obj, target, kind);
            return;
        }
    }
    uint8_t cls = E.rd8(obj, 0xc);
    if (cls < 4 && E.rd8(obj, 9) != 0) {
        uint32_t t = E.tableCaa8(cls);
        if (t != 0xffffffffu && t < 8) E.c33EB0(obj, target, kind);
    }
}

// 1.13c D2Game+0x586C0 (fastcall: game = param_1, msg = param_2). Ordinal_10920 arg lost, taken as msg.
uint32_t d2game_586C0(const D2GameBatch531F0Env& E, void* game, void* msg) {
    uint32_t rec = E.questRec(msg, E.rd8(game, 0x6d));
    uint32_t room = E.ord10331(msg);
    if (room == 0 || E.ord10174(rec, 0x23, 0) != 0) return 0;
    if (E.ptr(game, 0x10f4) == nullptr) return 0;
    void* node = E.ptr(E.ptr(game, 0x10f4), 0);
    while (node != nullptr && E.rd32(node, 0) != 0x1f) node = E.ptr(node, 0xf4);
    if (node == nullptr) return 0;
    void* sub = E.ptr(node, 0x18);
    uint32_t want = E.rd32(sub, 4);
    bool match = (room == want);
    if (!match) {
        uint32_t* arr = nullptr;
        uint32_t cnt = 0;
        E.ord10383(room, &arr, &cnt);
        for (uint32_t i = 0; !match && i < cnt; ++i)
            if (arr[i] == want) match = true;
    }
    rec = E.questRec(msg, E.rd8(game, 0x6d));
    if (match) {
        E.ord10003(rec, 0x23, 1);
        E.ord10003(rec, 0x23, 0xd);
    }
    return 0;
}

// 1.13c D2Game+0x587C0 (fastcall: obj = param_1, msg = param_2). The decompile's one-argument
// Ordinal_10174 call has its result unused and is a read-only query, so it is dropped.
void d2game_587C0(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    void* game = E.ptr(obj, 4);
    uint32_t rec = E.questRec(E.ptr(msg, 0xc), E.rd8(game, 0x6d));
    int32_t iv3 = E.ord10174(rec, E.rd32(obj, 0xe0), 0);
    uint32_t e0 = E.rd32(obj, 0xe0);
    int32_t iv4 = E.ord10174(rec, e0, 1);
    if (iv4 == 0) {
        if (iv3 == 1) return;
        if (E.rd8(obj, 9) == 0) return;
        uint32_t t = E.tableB2d8(E.rd8(obj, 0xc));
        if (t == 0xffffffffu) return;
        if (t > 7) return;
    } else {
        if (E.ord10174(rec, e0, 5) != 0) return;
    }
    E.c33EB0(obj, E.rd32(msg, 0x14), (int32_t)e0);
}

// 1.13c D2Game+0x596B0 (fastcall: obj = param_1, msg = param_2). The matching arms all make the
// same call, so the transcription makes it once, after the 0x69590 gate.
void d2game_596B0(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    void* a8 = E.ptr(msg, 8);
    int32_t kind = (a8 == nullptr) ? -1 : (int32_t)E.rd32(a8, 4);
    switch (kind) {
    case 0x200:
    case 0x208:
    case 0x201:
    case 0x202:
    case 0x203:
        break;
    default:
        return;
    }
    if (E.c89590() == 0) E.c33EB0(obj, E.rd32(msg, 0x14), kind);
}

// 1.13c D2Game+0x5E3C0 (no arguments in the decompile: game = unaff_ESI, unit = the Ordinal_10331 argument,
// taken as the parameter here).
void d2game_5E3C0(const D2GameBatch531F0Env& E, void* game, void* unit) {
    uint32_t room = E.ord10331(unit);
    if (E.ord10826(room) != 0x78) return;
    if (E.ptr(game, 0x10f4) == nullptr) return;
    void* node = E.ptr(E.ptr(game, 0x10f4), 0);
    while (node != nullptr && E.rd32(node, 0) != 0x23) node = E.ptr(node, 0xf4);
    if (node == nullptr) return;
    void* sub = E.ptr(node, 0x18);
    if (E.rd8(sub, 0) == 0) {
        E.wr32(sub, 0xc, E.rd32(sub, 0xc) + 1);
        E.wr8(sub, 0x11, 0);
        E.wr8(sub, 0x10, 0);
        E.c5E0E0(game, node, sub);
    }
}

// 1.13c D2Game+0x5E7F0 (thiscall: game = this, p = param_1)
void d2game_5E7F0(const D2GameBatch531F0Env& E, void* game, void* p) {
    if (E.ptr(game, 0x10f4) == nullptr) return;
    void* node = E.ptr(E.ptr(game, 0x10f4), 0);
    while (node != nullptr && E.rd32(node, 0) != 0x22) node = E.ptr(node, 0xf4);
    if (node == nullptr) return;
    if (E.rd8(E.ptr(node, 0x18), 0x87) != 0)
        E.c8EA80(game, p, 7, E.rd32(game, 0xa8) + 0xc, 0, 0, 0);
}

// 1.13c D2Game+0x5EE30 (no arguments in the decompile: game = unaff_EBX, obj = unaff_EDI).
// Ordinal_11090 has no arguments in the decompile; (obj, 4) is assumed from its other call sites.
void d2game_5EE30(const D2GameBatch531F0Env& E, void* game, void* obj) {
    int32_t id = (obj == nullptr) ? -1 : (int32_t)E.rd32(obj, 3 * 4);
    void* sub = E.ptr(game, 0x18);
    int32_t i = 0;
    for (;; ++i) {
        if (i > 2) return;
        if ((int32_t)E.rd32(sub, 0xec + i * 4) == id) break;
    }
    int32_t kind = (obj == nullptr) ? 0 : (int32_t)E.rd32(obj, 4 * 4);
    bool skipOrd = false;
    if (kind != 1) {
        if (kind == 2) {
            if (E.rd8(sub, 0x108 + i) == 1) E.wr32(sub, 0xfc + i * 4, E.rd32(sub, 0xfc + i * 4) + 1);
            if ((int32_t)E.rd32(sub, 0xfc + i * 4) > 5) E.wr8(sub, 0x10b + i, 1);
            if (E.rd8(sub, 0x10b + i) == 0) skipOrd = true;
        } else if (kind != 3) {
            skipOrd = true;
        }
    }
    if (!skipOrd) E.ord11090(obj, 4);
    E.c8EA80(E.ptr(game, 4), obj, 7, E.rd32(E.ptr(game, 4), 0xa8) + 0x19, 0, 0, 0);
}

// 1.13c D2Game+0x627A0 (fastcall: game = param_1, client = param_2)
uint32_t d2game_627A0(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t rec = E.questRec(client, E.rd8(game, 0x6d));
    if (E.ord10174(rec, 5, 0) == 1) return 0;
    uint32_t room = E.ord10331(client);
    if (room == 0) return 0;
    if (E.ord10826(room) != 0x19) {
        E.ord10003(rec, 5, 0xe);
        return 0;
    }
    E.ord10003(rec, 5, 0xd);
    E.ord10003(rec, 5, 0);
    E.ord10593(rec, 5, 1);
    E.cDFFD0(0x25, client);
    if (E.ptr(game, 0x10f4) == nullptr) return 0;
    void* node = E.ptr(E.ptr(game, 0x10f4), 0);
    while (node != nullptr && E.rd32(node, 0) != 5) node = E.ptr(node, 0xf4);
    if (node == nullptr) return 0;
    void* sub = E.ptr(node, 0x18);
    uint32_t v = (client == nullptr) ? 0xffffffffu : E.rd32(client, 0xc);
    uint16_t n = E.rd16(sub, 100);
    if (n < 0xc) {
        E.wr32(sub, 0x34 + (uint32_t)n * 4, v);
        E.wr16(sub, 100, (uint16_t)(n + 1));
    }
    return 0;
}

// 1.13c D2Game+0x62870 (fastcall: game = param_1, client = param_2)
uint32_t d2game_62870(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t rec = E.questRec(client, E.rd8(game, 0x6d));
    if (E.ord10174(rec, 5, 0) != 0 && E.ord10174(rec, 5, 0xd) == 0 && E.ord10174(rec, 5, 0xe) == 0) return 0;
    E.c354F0(client, game);
    return 0;
}

// 1.13c D2Game+0x629B0 (fastcall: game = param_1, client = param_2)
void d2game_629B0(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t rec = E.questRec(client, E.rd8(game, 0x6d));
    if (E.ord10174(rec, 4, 0) == 1) return;
    if (E.ord10174(rec, 4, 1) == 1) return;
    uint32_t room = E.ord10331(client);
    if (room == 0) return;
    int32_t cls = E.ord10826(room);
    if (cls == 0) return;
    if (E.ord10459(cls) == 0) {
        E.ord10003(rec, 4, 0xd);
        E.ord10003(rec, 4, 1);
        E.c34C00();
    }
}

// 1.13c D2Game+0x648B0 (fastcall: game = param_1, client = param_2)
uint32_t d2game_648B0(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t rec = E.questRec(client, E.rd8(game, 0x6d));
    if (E.ord10174(rec, 4, 0) == 1) return 0;
    if (E.ord10174(rec, 4, 1) == 1) return 0;
    uint32_t room = E.ord10331(client);
    if (room == 0) return 0;
    if (E.ord10826(room) != 0x26) return 0;
    E.ord10003(rec, 4, 0xd);
    E.ord10003(rec, 4, 1);
    E.c34C00();
    if (E.cFA60() != -1) E.cBC160(0x629B0);
    return 0;
}

// 1.13c D2Game+0x654A0 (fastcall: obj = param_1, msg = param_2)
void d2game_654A0(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    if (E.rd8(obj, 9) == 0 || E.rd8(obj, 0xc) == 6) return;
    void* sub = E.ptr(obj, 0x18);
    int32_t id = E.ord10802(E.rd32(msg, 8));
    if (id == 0x20646b62 || id == 0x20736b62) E.wr32(sub, 0x7c, E.rd32(sub, 0x7c) - 1);
    if (E.rd32(sub, 0x7c) == 0 && E.rd8(obj, 0xc) < 6 && E.rd8(sub, 0x4f) != 1 && E.rd8(sub, 0x47) == 1) {
        E.c35EB0();
        E.c64940(obj);  // ECX lost in the decompile (extraout_ECX); obj assumed
    }
}

// 1.13c D2Game+0x5C080 (fastcall: game = param_1, client = param_2)
void d2game_5C080(const D2GameBatch531F0Env& E, void* game, void* client) {
    uint32_t diff = E.rd8(game, 0x6d);
    uint32_t rec = E.questRec(client, diff);
    if (E.ord10174(rec, 0x27, 0) != 0) return;
    if (E.ord10587(client, 0xc, 0) < (int32_t)((diff + 1) * 0x14)) return;
    uint32_t room = E.ord10331(client);
    if (room == 0) return;
    int32_t cls = E.ord10826(room);
    if (cls == 0) return;
    if (E.ord10459(cls) == 4) {
        E.ord10003(rec, 0x27, 0);
        E.ord10003(rec, 0x27, 0xd);
        E.c5BF70(game);
    }
}

// 1.13c D2Game+0x5AFB0 (fastcall: obj = param_1). The result is a bool in AL; the callers read AL only, and
// the disassembly leaves the upper bits of EAX undefined. 0x33F30 takes ecx = obj+0x10 and eax = obj+4 (the
// game); the code pointer called is the one read from the 0x33F30 result at +0xf0 with ecx = that result.
uint32_t d2game_5AFB0(const D2GameBatch531F0Env& E, void* obj) {
    if (E.rd8(obj, 0xc) < 4 && E.rd8(obj, 9) != 0) {
        if (E.rd8(obj, 0xc) == 0) E.c35EB0();
        return 1;
    }
    void* p = E.c53F30(E.rd32(obj, 0x10), E.ptr(obj, 4));
    if (p == nullptr) return 0;
    if (E.badCodePtr(obj, 0xf0)) {
        E.fatal(0xf6);
        return 0;
    }
    return E.callCodePtr(p, 0xf0);
}

// 1.13c D2Game+0x5E420 (cdecl-style: param_1 = obj, param_2 = msg). Ghidra's iStack_4, read as the
// game in the no-slot fallback, is taken as obj's game pointer (obj+4), the same object the rest uses.
void d2game_5E420(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    void* sub = E.ptr(obj, 0x18);
    if (msg == nullptr || E.rd32(msg, 0x10) != 4) {
        if ((int32_t)E.rd32(sub, 0x40) < 3) {
            int32_t id = (msg == nullptr) ? -1 : (int32_t)E.rd32(msg, 4);
            uint32_t slot;
            if (id == 0x1da) slot = 1;
            else if (id == 0x1db) slot = 0;
            else if (id == 0x1dc) slot = 2;
            else return;
            if ((int32_t)E.rd32(sub, 0xc) > 0) {
                E.c5E0E0(E.ptr(obj, 4), obj, sub);
                return;
            }
            uint32_t scratch = 0;
            E.ord10141(msg, &scratch);
            void* piv = E.c36820(2);
            if (piv == nullptr) {
                E.c8EA80(E.ptr(obj, 4), msg, 7, E.rd32(E.ptr(obj, 4), 0xa8) + 10, 0, 0, 0);
                return;
            }
            if (msg == nullptr || E.rd32(msg, 0x10) != 4) {
                E.ord11090(msg, 4);
                E.wr32(sub, 0x30 + slot * 4, 4);
            }
            uint32_t n = E.rd32(sub, 0x40);
            E.wr8(sub, 0x2c + n, 1);
            E.wr32(sub, 0x20 + n * 4, E.rd32(piv, 0xc));
            E.wr32(sub, 0x40, n + 1);
            E.wr32(sub, 0x44, E.rd32(sub, 0x44) + 1);
            E.wr8(sub, 0x48, 1);
        }
    }
}

// 1.13c D2Game+0x5F3F0 (__fastcall: obj = param_1 (ECX), msg = param_2 (EDX)). The lost pvVar6 and
// extraout_ECX values in the later calls are taken as obj.
void d2game_5F3F0(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    int32_t iv5 = (int32_t)E.rd32(msg, 0x14);
    int32_t iv1 = (int32_t)E.rd32(msg, 0x18);
    bool goL495 = false;
    if (iv1 == 0x70) {
        if (!(E.rd8(obj, 9) == 0 || E.rd8(obj, 0xc) != 0)) {
            E.wr8(obj, 0xc, 1);
            goL495 = true;
        }
    } else if ((iv1 == 0x71 || iv1 == 0x72) && E.rd8(obj, 9) != 0) {
        bool gt2 = E.rd8(obj, 0xc) > 2;
        if (!gt2) E.c35EB0();
        if (E.rd8(obj, 0xb) == 0) {
            E.wr8(obj, 0x14, 0);
            E.wr8(obj, 0xb, 1);
            E.cFFCC0Token(0x571d0, 0);
            E.cFFCC0(obj, 0);
            goL495 = true;
        } else if (!gt2) {
            E.cFFCC0(obj, 0);
            goL495 = true;
        } else {
            goL495 = true;
        }
    }
    if (goL495) E.c5EC30(obj);
    if (iv5 == 0x6d) {
        if (E.rd16(obj, 0x9c) != 0) E.c33830(obj, 0x1c);
        if (E.rd8(obj, 0xc) == 2) {
            void* game = E.ptr(msg, 0);
            uint32_t rec = E.questRec(E.ptr(msg, 0xc), E.rd8(game, 0x6d));
            if (E.ord10174(rec, E.rd32(obj, 0xe0), 0) == 1) return;
            if (E.ord10174(rec, E.rd32(obj, 0xe0), 1) == 1) return;
            E.c35EB0();
            if (E.rd8(obj, 0xb) == 0) {
                E.wr8(obj, 0x14, 0);
                E.wr8(obj, 0xb, 1);
                E.cFFCC0(obj, 0);
            }
            E.cFFCC0(obj, 0);
        }
    }
    if (iv1 > 0x78 && iv1 < 0x7d && E.rd8(obj, 9) != 0 && E.rd8(obj, 0xc) < 5) {
        E.cFFCC0(obj, (int32_t)E.rd32(msg, 8));
        E.wr8(obj, 0xc, 6);
        void* sub = E.ptr(obj, 0x18);
        E.wr32(sub, 0x84, 2);
        if (E.rd32(sub, 0x88) == 0) E.wr32(sub, 0x88, 1);
    }
}

// Counter bump shared by both exits of 0x5A310: index from 0x58260, count at sub+0x114, flag at sub+0x108.
static void d2game_5A310_bump(const D2GameBatch531F0Env& E, void* obj, void* sub) {
    int32_t idx = E.c58260(obj);
    if (idx < 0) return;
    uint32_t v = E.rd32(sub, 0x114 + (uint32_t)idx * 4) + 1;
    E.wr32(sub, 0x114 + (uint32_t)idx * 4, v);
    if ((int32_t)v > 4) E.wr8(sub, 0x108 + (uint32_t)idx, 1);
}

// 1.13c D2Game+0x5A310 (__fastcall: obj = param_1, msg = param_2). The Ordinal_10383 out-pointers are
// modelled as an object array; the lost `this` of the 0xDFCC0 call is taken as obj.
void d2game_5A310(const D2GameBatch531F0Env& E, void* obj, void* msg) {
    if (E.rd8(obj, 9) == 0) return;
    void* sub = E.ptr(obj, 0x18);
    void* m8 = E.ptr(msg, 8);
    if (m8 == nullptr || E.rd32(m8, 4) != 0x1b2) {
        int32_t cnt = (int32_t)E.rd32(sub, 0x15c);
        for (int32_t i = 0; i < cnt; ++i) {
            int32_t key = (m8 == nullptr) ? -1 : (int32_t)E.rd32(m8, 0xc);
            if (key == (int32_t)E.rd32(sub, 0x120 + (uint32_t)i * 4)) {
                d2game_5A310_bump(E, obj, sub);
                return;
            }
        }
        E.wr32(sub, 0xa8, E.rd32(sub, 0xa8) + 1);
    } else {
        E.ord10346(E.ord10331Sel(m8, 0));
        uint32_t room = E.ord10331(m8);
        if (room != 0) {
            void** arr = nullptr;
            uint32_t cnt = 0;
            E.ord10383Ptrs(room, &arr, &cnt);
            for (uint32_t idx = 0; idx < cnt; ++idx) {
                for (void* node = E.ptr(arr[idx], 0x74); node != nullptr; node = E.ptr(node, 0xe8)) {
                    if (E.rd32(node, 0) == 1 && E.rd32(node, 4) == 0x216 && E.rd32(node, 0x10) != 0xc &&
                        E.rd32(node, 0x10) != 0 && E.ord10522(node, m8) < 0xf) {
                        E.wr32(sub, 0x120 + E.rd32(sub, 0x15c) * 4, E.rd32(node, 0xc));
                        E.wr32(sub, 0x15c, E.rd32(sub, 0x15c) + 1);
                        E.wr32(sub, 0xac, E.rd32(sub, 0xac) + 1);
                    }
                }
            }
        }
    }
    if ((int32_t)E.rd32(sub, 0xa8) > 4 && E.rd8(obj, 0xb) != 0xc) {
        E.wr8(obj, 0x14, 0);
        E.wr8(obj, 0xb, 0xc);
        E.cFFCC0(obj, 0);
        E.wr8(sub, 0x84, 0);
        E.cFFCC0(E.ptr(msg, 0xc), (int32_t)E.rd32(msg, 0xc));
    }
    d2game_5A310_bump(E, obj, sub);
}

// 1.13c D2Game+0x42510 (EAX = unit a, EDI = unit b, then four stack args s1..s4; callee pops 0x10).
// Both units must be non-null and neither unit's room may be a town room. The pair is then handed to
// 0xA0000 with esi = a; the stdcall argument order is (s1, s3, s4, s2, b.x, b.y) where b.x is the
// 0x1100 helper result and b.y the 0x1130 helper result.
uint32_t d2game_42510(const D2GameBatch531F0Env& E, void* a, void* b, int32_t s1, int32_t s2, int32_t s3, int32_t s4) {
    if (a == nullptr || b == nullptr) return 0;
    if (E.ord10057(E.ord10331(a)) != 0) return 0;
    if (E.ord10057(E.ord10331(b)) != 0) return 0;
    int32_t y = E.c1130(b);
    int32_t x = E.c1100(b);
    return E.cA0000(a, s1, s3, s4, s2, x, y);
}

// Player-block load: builds the player unit from a saved block, then reads the sections in order.
// Returns 0 on success, 9 when the block is shorter than 0x82 bytes, or the first section error.
// 1.13c D2Game+0x55E10 (eax = out unit slot, ecx = block base, stack p1, p2, size; callee pops 0x18).
uint32_t d2game_55E10(const D2GameBatch531F0Env& E, void** outUnit, uint8_t* base, int32_t p1, void* p2, uint32_t size) {
    uint32_t used = 0, v18 = 0, o8 = 0, o4 = 0, o10 = 0, oc = 0, flag = size;
    void* unit = nullptr;
    *outUnit = nullptr;
    if (size < 0x82) return 9;
    uint32_t r = E.c55320(base, p1, p2, &unit, &flag, &v18, &o8, &o4, &o10, &oc, &used);
    if (r != 0) return r;
    if (flag != 0) return E.c55840(p1, p2, outUnit);
    *outUnit = unit;
    int32_t rem = (int32_t)size - (int32_t)used;
    uint8_t* d = base + used;
    if ((r = E.c530D0(d, unit, &used)) != 0) return r;
    d += used; rem -= (int32_t)used;
    if ((r = E.c53040(unit, d, &used)) != 0) return r;
    d += used; rem -= (int32_t)used;
    if ((r = E.c53340(d, unit, &used)) != 0) return r;
    rem -= (int32_t)used; d += used;
    if ((r = E.c53640(d, p1, 0x10, &used, unit)) != 0) return r;
    o8 = (uint32_t)E.statGet(unit, 6, 0);
    o4 = (uint32_t)E.statGet(unit, 8, 0);
    int32_t hp = E.statGet(unit, 0xe, 0);
    if ((hp < 0 || hp > E.statMaxHp(unit)) && unit != nullptr) {
        if (E.rd32(unit, 0) == 0) E.statMaxHp(unit);
        E.statSet(unit, 0xe, 0, 0);
    }
    int32_t mp = E.statGet(unit, 0xf, 0);
    if ((mp < 0 || mp > E.statMaxMana(unit)) && unit != nullptr) {
        if (E.rd32(unit, 0) == 0) E.statMaxMana(unit);
        E.statSet(unit, 0xf, 0, 0);
    }
    rem -= (int32_t)used; d += used;
    if ((r = E.c53D40(unit, (int32_t)(intptr_t)p2, d, &used)) != 0) return r;
    if (o10 != 0) E.cA2100(unit, 1, (void*)(uintptr_t)o10, -1);
    if (oc != 0) E.cA2100(unit, 0, (void*)(uintptr_t)oc, -1);
    void* pd = E.playerData(unit);
    E.wr32(pd, 0x74, o10);
    E.wr32(pd, 0x70, oc);
    d += used; rem -= (int32_t)used;
    if (E.c55050(v18, p1, unit, d, &used, (uint32_t)rem) == 0) {
        d += used; rem -= (int32_t)used;
        if (E.c55A10(rem, d, p1, unit, v18, &used) == 0) {
            rem -= (int32_t)used; d += used;
            if (E.c55AD0(p1, unit, d, v18, rem, &used) == 0) {
                d += used; rem -= (int32_t)used;
                used = 0;
                E.c534D0(unit, d, v18, &used);
            }
        }
    }
    E.statSet(unit, 6, (int32_t)o8, 0);
    E.statSet(unit, 8, (int32_t)o4, 0);
    E.statSet(unit, 0x44, 100, 0);
    E.statSet(unit, 0x43, 100, 0);
    E.statSet(unit, 0x45, 100, 0);
    int32_t id = unit == nullptr ? -1 : (int32_t)E.rd32(unit, 4);
    E.statSet(unit, 0x1e, E.ord10399(id, E.ord10587(unit, 0xc, 0)), 0);
    if (E.rd8(p2, 0x45c) != 0) E.c8A670(p2);
    return 0;
}
