// src/core/d2game/d2game_batch_8BA50.cpp -- twelve 1.13c D2Game.dll functions (RE-DONE stage A)
// transcribed from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c) and
// pinned against retail 1.13c D2Game.dll disassembly (image base 0x6FC20000). The set is the
// packet-send helpers 0x8BA50/0x8A6C0/0x8AAE0/0x8D040/0x8D5F0/0x8D710/0x8D210, the inventory
// callback walker 0x37C90, the missile-event packet 0x6F740, and the unit/stat helpers
// 0xCD2C0/0xCD0E0/0x8E030. Register-only arguments (Ghidra's in_EAX/in_ECX/unaff_EDI) are explicit
// parameters named at each function. Unmapped callees are seams on D2GameBatch16Env in retail
// argument order. The fatal path is Fog Ordinal 10265(code) -> Ordinal 10024(text) -> _exit(-1);
// only the exit is observable and is modelled. Bodies that rest on a seam whose retail semantics
// the decompile does not show say so in a one-line comment at the seam.
// Prior art: D2MOO 1.10f CLIENTS_GetPlayerFromClient (0x4AD10), ITEMS/INVENTORY iteration
// (#10460/#10464/#11147), and the 0x8A3E0 packet-append helper.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>
#include <D2BasicTypes.h>
#include <Server.h>

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Unit-list heads indexed by unit type 0..4 (retail 0x6FD1xxxx statics); bucket = head + (id&0x7F)*4,
// chain next at +0xE4, node id at +0xC. Same layout used by the earlier D2Game batch files.
static const uint32_t kUnitListHead[5] = { 0x1120, 0x1320, 0x1520, 0x1920, 0x1720 };

// Host seams for the callees the cached decompiles name but src/ does not implement (yet). Each is
// in retail argument order; register-only arguments are documented on the member.
struct D2GameBatch16Env {
    // 0x8A3E0: append `len` bytes of `buf` to the client's outgoing block queue (client in EAX,
    // buf and len pushed; the function itself is ret 8).
    void     (*send8A3E0)(void* client, const void* buf, uint32_t len);
    // 0x8AA10: fixed 0x20-byte packet sender; buffer in EAX, client pushed (ret 4). Copies 32
    // bytes to a stack block and hands it to 0x8A3E0.
    void     (*send8AA10)(const void* buf32, void* client);
    // D2Common direct lookup 0x6FD003A0: ECX=game, EAX=unit type, EDX=unit id; returns the unit or
    // null.
    void*    (*getUnit)(void* game, uint32_t unitType, uint32_t unitId);
    // 0x4AD10 CLIENTS_GetPlayerFromClient(client, ignoreDeath); client in EAX/ESI.
    void*    (*getPlayerFromClient)(void* client, int32_t ignoreDeath);
    // D2Common #10331 UNITS_GetRoom(unit); unit in ECX, returns the room or null.
    void*    (*getRoom)(void* unit);
    // D2Common #10383: the unit array of a room; args (room, void** out, uint32_t* count). Retail
    // returns the count in EAX. The out pointer is null when the room has no unit array.
    uint32_t (*getRoomUnitArray)(void* room, void** out, uint32_t* count);
    // D2Common #10920: the unit's player-data block (unit in ECX); character name at +0, player id
    // at +0x9C.
    void*    (*getPlayerData)(void* unit);
    // D2Common #10181: room link of a unit node; node in ECX. Returns null or a record whose
    // +4 is the linked room/unit id (see 0x8D040).
    void*    (*ordinal10181)(void* node);
    // D2Common #10258 UNITS_GetObjectPortalFlags(unit) -- stdcall with one stack argument.
    uint32_t (*ordinal10258)(void* unit);
    // D2Common #10587: (unit, opcode, 0) -> stat value used by 0xCD0E0. The decompile shows no
    // visible arguments; the call site is modelled with the two live registers plus a zero word.
    uint32_t (*ordinal10587)(void* unit, int32_t opcode, uint32_t zero);
    // D2Common #10698: for a type-4 (item) unit returns (int8_t)pItemData[0x69], else 0.
    int32_t  (*ordinal10698)(void* item);
    // D2Common #10737: current missile frame (missile pointer on the stack).
    uint32_t (*ordinal10737)(void* missile);
    // D2Common #10764: missile sub_34AA0 (missile pointer on the stack).
    uint32_t (*ordinal10764)(void* missile);
    // D2Common #10973: sub_38B70(unit, a, b) -- stdcall with three stack arguments. Its body is
    // UNMAPPED; only the return value is used here.
    uint32_t (*ordinal10973)(void* unit, uint32_t a, uint32_t b);
    // D2Common #10983: sub_34AB0(missile pointer on the stack).
    uint32_t (*ordinal10983)(void* missile);
    // D2Common #11029 MISSILE_GetLevel(unit).
    uint32_t (*ordinal11029)(void* unit);
    // D2Common #11146 SKILLS_GetBonusSkillLevelFromSkillId(unit, skillId).
    uint32_t (*ordinal11146)(void* unit, uint32_t skillId);
    // D2Common #10191: value read into the 0x73 packet at +1 (missile pointer passed on the
    // stack). Sequence-only in this row; the returned 16-bit value is copied verbatim.
    uint32_t (*ordinal10191)(void* missile);
    // D2Common #10460: first item of an inventory.
    void*    (*invGetFirstItem)(void* inventory);
    // D2Common #10464: next item in the inventory chain.
    void*    (*invGetNextItem)(void* item);
    // D2Common #11147 INVENTORY_UnitIsItem(unit) -- side effect only at the 0x37C90 call site.
    void     (*unitIsItem)(void* item);
    // 0x21100 (unit in ECX): unit position/key block for the 0x73 packet.
    uint32_t (*sub21100)(void* unit);
    // 0x21130 (unit in ECX): second unit key block for the 0x73 packet.
    uint32_t (*sub21130)(void* unit);
    // 0x21260 (unit in ECX): room/owner lookup used when 0x8D5F0 has no target.
    void*    (*sub21260)(void* unit);
    // 0x8B0F0(owner, code, client, a4, a5): item-to-owner action used by the inventory walker.
    void     (*sub8B0F0)(void* owner, uint32_t code, void* client, uint32_t a4, uint32_t a5);
    // 0x8DF70(client, owner, 0): item type 1 branch of the inventory walker.
    void     (*sub8DF70)(void* client, void* owner, uint32_t zero);
    // 0x8DE50(client, owner, 0): item type 2 branch of the inventory walker.
    void     (*sub8DE50)(void* client, void* owner, uint32_t zero);
    // 0x8D120(node in EAX, player unit in EBX): re-send the unit's 0x82 record.
    void     (*sub8D120)(void* node, void* playerUnit);
    // 0x8D3D0(client in EBX, a, b, c): the 2/arg/0 action issued by 0x8E030.
    void     (*sub8D3D0)(void* client, uint32_t a, uint32_t b, uint32_t c);
    // 0xCB950(ecx, edx, unit, a4).
    void     (*subCB950)(uint32_t ecxArg, void* edxArg, void* unit, uint32_t a4);
    // 0xCC2F0(unit, opcode, value, target): write one stat back to the client.
    void     (*subCC2F0)(void* unit, uint32_t opcode, int32_t value, void* target);
    // Storm #501 SStrCopy(dst, src, maxLen); returns nothing observable here.
    void     (*strCopy)(char* dst, const char* src, uint32_t maxLen);
    // Retail data: level record table (0x6FD1829C -> +0xB64 records, +0xB6C count, stride 0x1A4)
    // and the bit-mask block (0x6FD18BE0 -> byte +0x14).
    const uint8_t* levelRecords;
    uint32_t levelRecordCount;
    const uint8_t* gdwBitMasks;
    // Stat opcode table at 0x6FD22DD0: int32 entries terminated by -1.
    const int32_t* statOpcodes;
    // 0x8AA10's client block is always 0x20 bytes; the packet built by 0x6F740 relies on that.
};

// client+0x3D4 bit0 = "unit cached in +0x174"; otherwise +0x16C/+0x170 identify the unit. Shared by
// 0xCD2C0/0xCD0E0/0x8D710 (and 0x8D3D0). A failed re-lookup clears the cache.
static void* d2game_current_unit(void* client, const D2GameBatch16Env* env) {
    if (rd8(client, 0x3D4) & 1) return rdptr(client, 0x174);
    if (!rdptr(client, 0x174)) return nullptr;
    void* u = env->getUnit(rdptr(client, 0x1A8), rd32(client, 0x16C), rd32(client, 0x170));
    if (!u) wrptr(client, 0x174, nullptr);
    return u;
}

// 0x8BA50: 7-byte packet 0xAB to the client in EAX. Bytes 1..5 carry ECX/EDX (bit fields), byte 6
// the low byte of the stack argument.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8ba50(void* client, uint32_t ecxIn, uint32_t edxIn, uint32_t param4, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8BA50
    uint8_t b[7];
    b[0] = 0xAB;
    b[1] = (uint8_t)(ecxIn & 0xFF);
    wr32(b, 2, edxIn);
    b[6] = (uint8_t)(param4 & 0xFF);
    env->send8A3E0(client, b, 7);
}

// 0x8A6C0: 7-byte packet 0x98 to the client in EAX; bytes 1..4 = ECX, 5..6 = DX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8a6c0(void* client, uint32_t ecxIn, uint16_t dxIn, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8A6C0
    uint8_t b[7];
    b[0] = 0x98;
    wr32(b, 1, ecxIn);
    wr16(b, 5, dxIn);
    env->send8A3E0(client, b, 7);
}

// 0x8AAE0: 7-byte packet 0x60. Retail prologue is `sub $8; push %esi`, and the pushed ESI is the
// argument of #10258 (UNITS_GetObjectPortalFlags, stdcall 1), so after its ret the buffer base is
// E-8 and the client is the first stack argument [E+4].
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8aae0(void* client, void* unit, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8AAE0
    uint8_t b[7];
    b[0] = 0x60;
    b[1] = (uint8_t)env->ordinal10258(unit);
    b[2] = rd8(rdptr(unit, 0x14), 4);
    wr32(b, 3, rd32(unit, 0xC));
    env->send8A3E0(client, b, 7);
}

// 0xCD2C0: forward to 0xCB950, then if EDX equals the client's current unit send stat 0xAF of that
// unit back to it (#10973 supplies the value, 0xCC2F0 the packet).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_cd2c0(uint32_t ecxIn, void* edxIn, void* unit, uint32_t a4, const D2GameBatch16Env* env) { // 1.13c D2Game+0xCD2C0
    env->subCB950(ecxIn, edxIn, unit, a4);
    if (!unit) d2game_fatal(0xFB);
    void* cur = d2game_current_unit(unit, env);
    if (edxIn == cur) {
        uint32_t v = env->ordinal10973(edxIn, 0xAF, 0);
        env->subCC2F0(edxIn, 0xAF, (int32_t)v, edxIn);
    }
}

// 0x37C90: inventory walker. Client in EDI, owner unit in EBP. For every item: type 3 issues the
// 0x8B0F0 action; type 1/2 reach the client-side action only when the client's current unit is the
// owner. #10698 is read twice per item, matching the two retail call sites.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_37c90(void* client, void* owner, const D2GameBatch16Env* env) { // 1.13c D2Game+0x37C90
    void* inv = rdptr(owner, 0x60);
    if (!inv) d2game_fatal(0x186);
    for (void* item = env->invGetFirstItem(inv); item; item = env->invGetNextItem(item)) {
        env->unitIsItem(item);
        int32_t t = env->ordinal10698(item);
        if (t == 3) {
            env->sub8B0F0(owner, 6, client, 0, 0);
            continue;
        }
        if (!client) d2game_fatal(0xFB);
        if (d2game_current_unit(client, env) == owner && t == 1) {
            env->sub8DF70(client, owner, 0);
            continue;
        }
        if (d2game_current_unit(client, env) == owner && env->ordinal10698(item) == 2) {
            env->sub8DE50(client, owner, 0);
            continue;
        }
    }
}

// 0x6F740: 0x73 missile-event packet, always 0x20 bytes via 0x8AA10. Game in ECX, missile unit in
// ESI. The stack argument feeds the packet's future fields (it is non-zero on the client path) and
// is also the send target; the second stack argument is unused (ret 8).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_6f740(void* game, void* unit, void* client, uint32_t unusedArg2, const D2GameBatch16Env* env) { // 1.13c D2Game+0x6F740
    (void)unusedArg2;
    if (!unit || rd32(unit, 0) != 3) return;
    if (!((rd32(unit, 0xC8) >> 10) & 1)) return;
    void* u = env->getUnit(game, rd32(unit, 0x94), rd32(unit, 0x98));
    if (!u) return;
    int32_t lvl = (int32_t)rd32(unit, 4);
    if (lvl < 0 || (uint32_t)lvl >= env->levelRecordCount) return;
    const uint8_t* rec = env->levelRecords + (uint32_t)lvl * 0x1A4;
    if (!(rd8(env->gdwBitMasks, 0x14) & rd8(rec, 4))) return;
    uint8_t b[0x20];
    std::memset(b, 0, sizeof b);
    b[0] = 0x73;
    wr16(b, 0x01, (uint16_t)env->ordinal10191(unit));
    wr32(b, 0x03, env->sub21100(unit));
    wr32(b, 0x07, env->sub21130(unit));
    if (client) {
        wr32(b, 0x0B, env->ordinal10983(rdptr(unit, 0x2C)));
        wr32(b, 0x0F, env->ordinal10764(rdptr(unit, 0x2C)));
    }
    wr16(b, 0x13, (uint16_t)env->ordinal10737(unit));
    b[0x15] = (uint8_t)rd32(u, 0);
    wr32(b, 0x16, rd32(u, 0xC));
    b[0x1A] = (uint8_t)env->ordinal11029(unit);
    env->send8AA10(b, client);
}

// 0x8D040: 0x82 party/unit record. Game in EAX, unit id in EDI; stack args are the client and a
// packet field. Walks the type-3 list for the id, follows #10181 to the linked id (0xFFFFFFFF when
// the node has no link), then walks the player list for that id and copies its 16-byte name.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8d040(void* game, uint32_t unitId, void* client, uint32_t field, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8D040
    if (!game) d2game_fatal(0x2D3);
    void* node = rdptr(game, kUnitListHead[3] + (unitId & 0x7F) * 4);
    while (node && rd32(node, 0xC) != unitId) node = rdptr(node, 0xE4);
    if (!node) return;
    void* link = env->ordinal10181(node);
    uint32_t linkedId = link ? rd32(link, 4) : 0xFFFFFFFFu;
    void* plist = rdptr(game, kUnitListHead[0] + (linkedId & 0x7F) * 4);
    while (plist && rd32(plist, 0xC) != linkedId) plist = rdptr(plist, 0xE4);
    if (!plist) return;
    uint8_t b[0x1D];
    std::memset(b, 0, sizeof b);
    b[0] = 0x82;
    wr32(b, 1, rd32(plist, 0xC));
    env->strCopy((char*)b + 5, (const char*)env->getPlayerData(plist), 0x10);
    wr32(b, 0x15, unitId);
    wr32(b, 0x19, field);
    env->send8A3E0(client, b, 0x1D);
}

// 0x8D5F0: 0x21 skill packet. Unit in ESI, target in EAX/EDI, skill id in BX. Without a target the
// sender is resolved from the unit's #21260 record (type 1, record bit 0, player id at +0x9C).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8d5f0(void* target, void* unit, uint16_t skillId, uint8_t arg1, uint8_t arg2, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8D5F0
    if (!unit) return;
    if (!target) {
        if (rd32(unit, 0) != 1) return;
        void* r = env->sub21260(unit);
        if (!r || rd32(r, 0) != 0) return;
        target = (void*)(uintptr_t)rd32(env->getPlayerData(r), 0x9C);
        if (!target) return;
    }
    uint8_t b[0xC];
    std::memset(b, 0, sizeof b);
    b[0] = 0x21;
    b[1] = (uint8_t)rd32(unit, 0);
    b[2] = arg2;
    wr32(b, 3, rd32(unit, 0xC));
    wr16(b, 7, skillId);
    b[9] = arg1;
    b[0xA] = (uint8_t)env->ordinal11146(unit, skillId);
    env->send8A3E0(target, b, 0xC);
}

// 0x8D710: 0x59 chat/string packet to the client in EAX, then a 0x8D120 refresh of the named unit
// when it is present in the client's game. Unit id in EDI, the string is the first stack argument,
// the two trailing words are the last two.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8d710(void* client, uint32_t ecxIn, uint32_t unitId, const char* str, uint16_t arg2, uint16_t arg3, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8D710
    uint8_t b[0x1A];
    std::memset(b, 0, sizeof b);
    b[0] = 0x59;
    wr32(b, 1, unitId);
    b[5] = (uint8_t)ecxIn;
    env->strCopy((char*)b + 6, str, 0x7FFFFFFF);
    wr16(b, 0x16, arg2);
    wr16(b, 0x18, arg3);
    env->send8A3E0(client, b, 0x1A);
    if (!client) d2game_fatal(0xFB);
    (void)d2game_current_unit(client, env);
    void* game = rdptr(client, 0x1A8);
    if (!game) d2game_fatal(0x2D3);
    void* node = rdptr(game, kUnitListHead[0] + (unitId & 0x7F) * 4);
    while (node && rd32(node, 0xC) != unitId) node = rdptr(node, 0xE4);
    if (node) env->sub8D120(node, d2game_current_unit(client, env));
}

// 0x8E030: assert that the EAX unit is a player whose player-data id equals ECX, then issue the
// 0x8D3D0 action (2, stackArg, 0) with the client in EBX (the EAX unit is the client's unit here).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8e030(void* unit, uint32_t expected, uint32_t arg, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8E030
    int32_t cur = 0;
    if (unit && rd32(unit, 0) == 0) cur = (int32_t)rd32(env->getPlayerData(unit), 0x9C);
    if (cur != (int32_t)expected) d2game_fatal(0x7DD);
    env->sub8D3D0(unit, 2, arg, 0);
}

// 0xCD0E0: stat synchronisation. Client in EAX, unit in EBX. When the client's current unit differs,
// push every opcode in the 0x6FD22DD0 table through #10587 and 0xCC2F0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_cd0e0(void* client, void* unit, const D2GameBatch16Env* env) { // 1.13c D2Game+0xCD0E0
    if (!client) d2game_fatal(0xFB);
    void* cur = d2game_current_unit(client, env);
    if (cur != unit) {
        for (const int32_t* p = env->statOpcodes; *p != -1; ++p) {
            uint32_t v = env->ordinal10587(unit, *p, 0);
            env->subCC2F0(unit, (uint32_t)*p, (int32_t)v, cur);
        }
    }
}

// 0x8D210: unit-presence packet. Client in EAX, a "camera" selector in EBX, then seven stack
// arguments: s1 (opcode modifier byte), s2 (unit id), s3 (unit type byte), s4 (unit id 2), s5
// (word), s6 (word), s7 (byte). Not-found path: 0x4C/0x99 with 0x10 bytes. Found and in another
// room: 0x4D/0x9A with 0x11 bytes including the two missile words.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8d210(void* client, int32_t ebx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7, const D2GameBatch16Env* env) { // 1.13c D2Game+0x8D210
    if (!client) d2game_fatal(0x996);
    uint8_t opcode = (ebx == 0) ? 0x4C : 0x99;
    void* unit = env->getUnit(rdptr(client, 0x1A8), s3 & 0xFF, s4);
    if (!unit) {
        uint8_t b[0x10];
        std::memset(b, 0, sizeof b);
        b[0] = opcode;
        b[1] = (uint8_t)(s1 & 0xFF);
        wr32(b, 2, s2);
        wr16(b, 6, (uint16_t)s5);
        b[8] = (uint8_t)(s7 & 0xFF);
        b[9] = (uint8_t)(s3 & 0xFF);
        wr32(b, 0xA, s4);
        wr16(b, 0xE, (uint16_t)s6);
        env->send8A3E0(client, b, 0x10);
        return;
    }
    bool visible = false;
    void* player = env->getPlayerFromClient(client, 0);
    void* room = player ? env->getRoom(player) : nullptr;
    if (room) {
        void* arr = nullptr;
        uint32_t count = 0;
        env->getRoomUnitArray(room, &arr, &count);
        void* unitRoom = env->getRoom(unit);
        for (uint32_t i = 0; i < count; ++i) {
            if (rd32(arr, i * 4) == (uint32_t)(uintptr_t)unitRoom) { visible = true; break; }
        }
    }
    if (visible) {
        uint8_t b[0x10];
        std::memset(b, 0, sizeof b);
        b[0] = opcode;
        b[1] = (uint8_t)(s1 & 0xFF);
        wr32(b, 2, s2);
        wr16(b, 6, (uint16_t)s5);
        b[8] = (uint8_t)(s7 & 0xFF);
        b[9] = (uint8_t)(s3 & 0xFF);
        wr32(b, 0xA, s4);
        wr16(b, 0xE, (uint16_t)s6);
        env->send8A3E0(client, b, 0x10);
        return;
    }
    uint8_t b[0x11];
    std::memset(b, 0, sizeof b);
    b[0] = (ebx == 0) ? 0x4D : 0x9A;
    b[1] = (uint8_t)(s1 & 0xFF);
    wr32(b, 2, s2);
    wr32(b, 6, s5);
    b[0xA] = (uint8_t)(s7 & 0xFF);
    wr16(b, 0xB, (uint16_t)env->ordinal10983(rdptr(unit, 0x2C)));
    wr16(b, 0xD, (uint16_t)env->ordinal10764(rdptr(unit, 0x2C)));
    wr16(b, 0xF, (uint16_t)s6);
    env->send8A3E0(client, b, 0x11);
}
