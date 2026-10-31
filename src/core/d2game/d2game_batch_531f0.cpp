// src/core/d2game/d2game_batch_531f0.cpp -- 1.13c D2Game.dll helpers from the cached decompiles.
// Pointer-bearing rows take their pointer walks through injected seams (see D2GameBatch531F0Env),
// so the retail 32-bit field offsets are never read as wasm64 pointers here.
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
