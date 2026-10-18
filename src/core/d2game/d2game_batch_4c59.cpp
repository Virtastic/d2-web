// src/core/d2game/d2game_batch_4c59.cpp -- 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Stage A alpha lane (OBJECTIVE v9):
// implementation + marker only, no spec, no vector.
//
// Records are opaque host objects addressed by retail byte offsets. Register-only arguments
// (Ghidra's in_EAX/unaff_EBX) are explicit parameters. Callees outside this batch are seam pointers
// on D2GameBatch4c59Env, in retail argument order. The fatal path Fog #10265(id) -> #10024(text) ->
// _exit(-1) is modelled as d2game_fatal(id): only the exit is observable.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION, EnterCriticalSection

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees src/ does not implement, in retail argument order.
struct D2GameBatch4c59Env {
    void     (*crtLock)(int32_t locknum);                              // CRT __lock, 0x402B
    void     (*appendPacket)(const void* data, uint32_t len);          // 0x8A3E0
    // 0x37B10 seams (retail Fog/D2Common ordinal imports).
    void     (*ord11049)(void* self, uint32_t arg2, uint32_t arg3);    // 0xA4A2
    void     (*ord10075)(void* pUnit, void* self, uint32_t a, uint32_t b); // 0xAB8C
    void     (*ord11090)(const void* unitHandle);                      // 0xAF34
    void     (*ord10945)(void* pUnit, uint32_t mode, uint32_t val);    // 0xB2A6
    int32_t  (*ord11134)(uint32_t container);                          // 0xA3CA
    void     (*listNodeHandler)(void* pUnit, void* pNode);             // 0x8E6F0
    void     (*sendRequest8E)(const uint8_t* req);                     // 0xDFC10
    // 0x9BB10 seams.
    void*    (*nextUnit)(void* container);                             // Ordinal #10170
    uint32_t (*unitId)(void* unit);                                    // Ordinal #10727
    void*    (*nextUnitAfter)(void* container);                        // Ordinal #10691
    void     (*removeUnit)(void* client, void* unit);                  // 0x9B630
    int32_t  (*ord11104)(void* client);                                // Ordinal #11104
    void     (*ordDF51A0)(void* pGame, void* client);                  // 0xDF51A0
};

// 1.13c D2Game+0x04C59
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_4c59(void* _File, const D2GameBatch4c59Env* env)
{
    // __lock_file: FILE objects inside the CRT's _iob window (0x6FD1F340..0x6FD1F599, stride 0x20)
    // take a CRT lock number (_iob index + _LOCK_FIRST_FILE 0x10); any other FILE locks through its
    // own critical section at sizeof(FILE)=0x20. Ghidra folds the _iob base into the index
    // arithmetic: (File - 0x6FD1F340) >> 5 + 0x10.
    const uintptr_t p = (uintptr_t)_File;
    if (p > 0x6FD1F337u && p < 0x6FD1F599u) {
        env->crtLock((int32_t)((p - 0x6FD1F340u) >> 5) + 0x10);
        return;
    }
    EnterCriticalSection((LPCRITICAL_SECTION)((uint8_t*)_File + 1));
}

// 1.13c D2Game+0x37B10
// Unit lookup + client-side unit update. `self` is the client, pGame the game, nUnitId the unit to
// find in the game unit hash table (pGame+0x1120, 0x80 buckets, chain link +0xE4, id +0x0C).
// The pair arrives in EAX (explicit here) and holds two dwords passed to ordinal #10075 and re-used
// in the 0x8E request. pUnit+0xDC is a list of nodes (type byte +0x00, next +0x24); the types 3, 4
// and 6 are replayed to 0x8E6F0. When ordinal #11134 reports a container (-1 = none), an 8-byte
// 0x8E/1 request {u32 container, u32 pair[0]} goes to 0xDFC10.
// Ghidra reconstructs overlapping call frames, so the stack stores are transcribed in call order.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_37b10(void* self, void* pGame, uint32_t nUnitId,
                                             const uint32_t* pPair,
                                             const D2GameBatch4c59Env* env)
{
    if (self == nullptr) d2game_fatal(0);       // Fog #10265 id not recovered by Ghidra
    env->ord11049(self, 2, 0x9C19);
    if (pGame == nullptr) d2game_fatal(0);

    void* pUnit = rdptr(pGame, 0x1120 + (nUnitId & 0x7F) * 4);
    if (pUnit == nullptr) return;
    while (rd32(pUnit, 0x0C) != nUnitId) {
        pUnit = rdptr(pUnit, 0xE4);
        if (pUnit == nullptr) return;
    }

    env->ord10075(pUnit, self, pPair[0], pPair[1]);

    uint8_t unitHandle[8];
    wrptr(unitHandle, 0, pUnit);
    wr32(unitHandle, 4, 0x11);
    wr32(pUnit, 0xC4, rd32(pUnit, 0xC4) | 2);
    env->ord11090(unitHandle);
    wr32(pUnit, 0xC4, rd32(pUnit, 0xC4) & 0xFFFFFFF7u);
    env->ord10945(pUnit, 7, 1);

    static const uint8_t kTypes[3] = { 3, 4, 6 };
    for (uint32_t t = 0; t < 3; ++t) {
        void* pNode = rdptr(pUnit, 0xDC);
        while (pNode != nullptr) {
            const uint8_t nType = rd8(pNode, 0);
            void* pNext = rdptr(pNode, 0x24);
            if (nType == kTypes[t]) env->listNodeHandler(pUnit, pNode);
            pNode = pNext;
        }
    }

    const int32_t nContainer = env->ord11134(rd32(pUnit, 0x60));
    if (nContainer != -1) {
        uint8_t req[8];
        req[0] = 0x8E;
        req[1] = 1;
        wr32(req, 2, (uint32_t)nContainer);
        wr32(req, 4, pPair[0]);
        env->sendRequest8E(req);
    }
}

// 1.13c D2Game+0x49DB0
// Client act getter: the client arrives in EAX (explicit here); a null client is the 0x7CB fatal.
// Returns the act byte the client is currently in (+0x1AC).
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2game_49db0(void* pClient)
{
    if (pClient == nullptr) d2game_fatal(0x7CB);
    return rd8(pClient, 0x1AC);
}

// 1.13c D2Game+0x8B710
// Packet 0x23 (13 bytes): header byte, one uninitialised gap byte in the retail frame, then the four
// caller fields. Fastcall arguments param_1/param_3 are unused by the body.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8b710(uint32_t param_1, uint32_t param_2, uint32_t param_3,
                                             uint8_t param_4, uint16_t param_5, uint32_t param_6,
                                             const D2GameBatch4c59Env* env)
{
    (void)param_1; (void)param_3;
    uint8_t packet[0xD] = {};
    packet[0] = 0x23;
    wr32(packet, 2, param_2);
    packet[6] = param_4;
    wr16(packet, 7, param_5);
    wr32(packet, 9, param_6);
    env->appendPacket(packet, 0xD);
}

// 1.13c D2Game+0x8BF80
// Packet 0x0B (11 bytes): a leading byte not written by this body, then a byte from param_1 and the
// caller fields. Fastcall argument param_3 is unused by the body.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8bf80(uint32_t param_1, uint32_t param_2, uint32_t param_3,
                                             uint16_t param_4, uint16_t param_5, uint8_t param_6,
                                             const D2GameBatch4c59Env* env)
{
    (void)param_3;
    uint8_t packet[0xB] = {};
    packet[1] = (uint8_t)param_1;
    wr32(packet, 2, param_2);
    wr16(packet, 6, param_4);
    wr16(packet, 8, param_5);
    packet[10] = param_6;
    env->appendPacket(packet, 0xB);
}

// 1.13c D2Game+0x9BB10
// Walks the unit list of `self` (or of the game at client+0x60 when self is null) and removes every
// unit found in the game unit table at pGame+0x1720 (0x80 buckets, id +0x0C, chain +0xE4) through
// 0x9B630. A found unit whose flags (+0xC8) have bit 0 set must carry a container at +0x60 (else the
// 0x108 fatal); that container's units get the same table lookup and 0x9B630 is called on `self`
// directly. The game arrives in EBX (explicit pGame; null is the 0x2D3 fatal) and client+0x60 null is
// the 0xF4 fatal. When ordinal #11104 reports a nonzero value, 0xDF51A0(pGame, client) runs.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9bb10(void* self, int* pClient, void* pGame,
                                             const D2GameBatch4c59Env* env)
{
    if ((void*)pClient[0x18] == nullptr) d2game_fatal(0xF4);
    void* container = (void*)pClient[0x18];
    if (self != nullptr) container = self;

    void* pUnit = env->nextUnit(container);
    if (pUnit != nullptr) {
        do {
            const uint32_t nId = env->unitId(pUnit);
            if (pGame == nullptr) d2game_fatal(0x2D3);

            void* pFound = rdptr(pGame, 0x1720 + (nId & 0x7F) * 4);
            while (pFound != nullptr && rd32(pFound, 0x0C) != nId) pFound = rdptr(pFound, 0xE4);

            if (pFound != nullptr) {
                env->removeUnit(pClient, pUnit);
                if ((rd32(pFound, 200) & 1) != 0) {
                    void* pItems = rdptr(pFound, 0x60);
                    if (pItems == nullptr) d2game_fatal(0x108);
                    for (void* pItem = env->nextUnit(pItems); pItem != nullptr;
                         pItem = env->nextUnitAfter(pItems)) {
                        const uint32_t nItemId = env->unitId(pItem);
                        void* pSlot = rdptr(pGame, 0x1720 + (nItemId & 0x7F) * 4);
                        while (pSlot != nullptr) {
                            if (rd32(pSlot, 0x0C) == nItemId) {
                                env->removeUnit(pClient, self);
                                break;
                            }
                            pSlot = rdptr(pSlot, 0xE4);
                        }
                    }
                }
            }
            pUnit = env->nextUnitAfter(container);
        } while (pUnit != nullptr);

        if (env->ord11104(pClient) != 0) env->ordDF51A0(pGame, pClient);
    }
}
