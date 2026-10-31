// src/core/d2cmp/d2cmp_revive1.cpp -- revived D2CMP rows of 1.13c D2CMP.dll, transcribed from the cached
// Ghidra decompiles of the 1.13c D2CMP.dll. Specs are kept in the private store.
// Guest objects are host memory with the 1.13c field layout; callees and retail log/exit calls come in
// through the env struct, one member per retail call.
#include <cstdint>
#include <cstdlib>

struct D2CmpCelEnv {
    void* (*celDecode)(uint8_t* src, void* a1, void* table, uint32_t a3, uint8_t* obj, uint32_t a5, uint32_t a6); // RVA 0x140A0
    void (*release)(uint32_t arg);                                                                             // RVA 0xD250
    void* table;                                                                                               // DAT_6fe77240
};

// cel-cache callback body. ctx = {+4 src, +8 obj, +0xC a5, +0x10 a6, +0x14 out, +0x18 rel}.
// Sets bit 2 in the obj's +0x94 record flags, decodes through 0x140A0, stores the result at ctx+0x14, clears the bit
// and obj+0 bit 2, releases ctx+0x18, and sets src bit 0 when the decode returned null. Returns 1.
// 1.13c D2CMP+0x0D76E
int32_t d2cmp_d76e(uint8_t* a1, uint32_t a2, uint32_t* ctx, const D2CmpCelEnv* e) {
    uint8_t* src = (uint8_t*)(uintptr_t)ctx[1];
    uint8_t* obj = (uint8_t*)(uintptr_t)ctx[2];
    uint32_t* rec = (uint32_t*)(uintptr_t)*(uint32_t*)(obj + 0x94);
    rec[3] |= 2;
    void* out = e->celDecode(src, a1, e->table, a2, obj, ctx[3], ctx[4]);
    ctx[5] = (uint32_t)(uintptr_t)out;
    rec[3] &= ~2u;
    obj[0] &= 0xFB;
    e->release(ctx[6]);
    if (ctx[5] == 0) src[0] |= 1;
    return 1;
}

struct D2CmpHashEnv {
    void (*assertFn)(const char* file, int32_t line, int32_t flag); // Ordinal 10046
    void (*fatal)(uint32_t text);                                    // Ordinal 10024 with Ordinal 10265(text), then _exit
    uint32_t (*text)(uint32_t id);                                   // Ordinal 10265
};

// unlink a cel-data node from the hash bucket chain. node[0] is the hash key, node+0x28
// is prev, node+0x2C is next, bucket at table+0x800+key*4. A null node is fatal with text 0xB2.
// 1.13c D2CMP+0x012A30
void d2cmp_12a30(uint8_t* table, uint32_t* node, const D2CmpHashEnv* e) {
    if (node == nullptr) {
        e->fatal(e->text(0xB2));
        std::_Exit(-1);
    }
    uint32_t* prev = (uint32_t*)(uintptr_t)node[10];
    uint32_t* next = (uint32_t*)(uintptr_t)node[11];
    if (prev) ((uint32_t*)((uint8_t*)prev + 0x2C))[0] = node[11];
    if (next) ((uint32_t*)((uint8_t*)next + 0x28))[0] = node[10];
    uint32_t* bucket = (uint32_t*)(table + 0x800 + node[0] * 4);
    if (*bucket == (uint32_t)(uintptr_t)node) *bucket = node[11];
    e->assertFn("..\\Source\\D2CMP\\SRC\\CelDataHash.cpp", 0x3A, 0);
}

struct D2CmpGfxHashEnv {
    void (*assertFn)(const char* file, int32_t line, int32_t flag); // Ordinal 10046
    void (*freeFn)(void);                                            // DAT_6fe87ef8, called with no arguments
    void* (*ord10094)(void);                                         // returns the cache block; +0x18 is the list
    void (*ord10097)(void);
    uint32_t (*text)(uint32_t id);                                   // Ordinal 10265
    void (*fatal)(uint32_t text);                                    // Ordinal 10024, then _exit
};

// __thiscall(ECX = table, stack = node). Unlinks the node from its bucket at table+0x8000,
// then, when the node's ctx (+0x20) has the flag at +0xB0 set, walks the ctx's record list (count = byte+0x24 * byte+0x21,
// stride 0x23 + two dword lengths) freeing each live entry through freeFn, and clears +0xB0.
// 1.13c D2CMP+0x016D60
void d2cmp_16d60(uint8_t* table, uint32_t* node, const D2CmpGfxHashEnv* e) {
    uint32_t* bucket = (uint32_t*)(table + 0x8000 + node[0] * 4);
    if (*bucket == (uint32_t)(uintptr_t)node) *bucket = node[11];
    uint8_t* ctx = (uint8_t*)(uintptr_t)node[8];
    if (*(uint32_t*)(ctx + 0xB0) != 0) {
        if (*(uint32_t*)(ctx + 0x28) != 0) {
            uint8_t* blk = (uint8_t*)e->ord10094();
            if (blk == nullptr) {
                e->fatal(e->text(0x5F));
                std::_Exit(-1);
            }
            uint32_t list = *(uint32_t*)(blk + 0x18);
            uint32_t off = 0, count = 0;
            uint32_t total = (uint32_t)ctx[0x24] * (uint32_t)ctx[0x21];
            if (total != 0) {
                do {
                    uint32_t* ent = (uint32_t*)(blk + off + 0x18);
                    if (*ent != 0) {
                        if (e->freeFn) e->freeFn();
                        *ent = 0;
                    }
                    count++;
                    off += 0x23 + *(uint32_t*)(blk + off + 0x1C) + *(uint32_t*)(blk + off + 0x14);
                } while (count < total);
            }
            if (list != 0) e->assertFn("..\\Source\\D2CMP\\SRC\\GfxHash.cpp", 0x82, 0);
        }
        e->ord10097();
        *(uint32_t*)(ctx + 0xB0) = 0;
    }
    e->assertFn("..\\Source\\D2CMP\\SRC\\GfxHash.cpp", 0x8E, 0);
}
