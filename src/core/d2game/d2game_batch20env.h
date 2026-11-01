#pragma once
#include <cstdint>

// Host seams for the callees the cached decompiles name but src/ does not implement here.
struct D2GameBatch20Env {
    // Ordinal_10920 UNITS_GetPlayerData(unit).
    void*    (*getPlayerData)(void* unit);
    // Ordinal_10174 QUESTRECORD_GetQuestState(pQuestFlags, nQuest, nFlag) -> 0/1.
    int32_t  (*getQuestState)(void* pQuestFlags, int32_t nQuest, int32_t nFlag);
    // 0x33830 QUESTS_FastRemovePlayerGUID(pGUIDs, dwGUID).
    int32_t  (*fastRemovePlayerGUID)(void* pGUIDs, int32_t dwGUID);
    // 0x33F30 QUESTS_GetQuestData(pGame, nQuestId).
    void*    (*getQuestData)(void* pGame, int32_t nQuestId);
    // Ordinal_10331 UNITS_GetRoom(unit).
    void*    (*getRoom)(void* unit);
    // Ordinal_10826 DUNGEON_GetLevelIdFromRoom(room).
    uint32_t (*getLevelIdFromRoom)(void* room);
    // Ordinal_10459 level/act table lookup, returning the act byte.
    uint32_t (*levelLookup)(uint32_t levelId);
    // Ordinal_10119 BITMANIP_GetBitState(pBuffer, bit).
    int32_t  (*getBitState)(void* pBuffer, uint32_t bit);
    // Ordinal_10118 BITMANIP_SetBitState(pBuffer, bit).
    void     (*setBitState)(void* pBuffer, uint32_t bit);
    // 0x8A3E0 D2GAME_PACKETS_SendPacket(EAX = client word, packet, size).
    void     (*sendPacket)(uint32_t client, const void* packet, uint32_t size);
    // IsBadCodePtr(fn): nonzero when fn is not callable.
    int32_t  (*isBadCodePtr)(void* fn);
    // 0x34510's call *0xA0(unit, 5*4): __fastcall handler(unit in ECX, event record in EDX).
    void     (*dispatchHandler)(void* unit, const uint32_t* event);
    // 0x3502's destructor callback argument of __ArrayUnwind.
    void     (*arrayUnwindOne)(void* element);
    // Ordinal_10003 QUESTRECORD_SetQuestState(pQuestFlags, nQuest, nFlag).
    void     (*setQuestState)(void* pQuestFlags, int32_t nQuest, int32_t nFlag);
    // Ordinal_10579 (0xA43C) QUESTRECORD_Export(pQuestFlags, pBuffer, nBytes, bFlag): fills nBytes at pBuffer.
    void     (*exportQuestFlags)(void* pQuestFlags, void* pBuffer, uint32_t nBytes, uint32_t bFlag);
    // 0xDFCC0 (ECX = value, stack 0), the quest "trigger FX" callee of 0x35E90.
    void     (*triggerFx)(uint32_t value);
    // 0xDFFD0 (stack: value, quest node), called by 0x35A20.
    void     (*questNotify)(uint32_t value, void* quest);
    // 0x15D0 (EAX = value, EDX = flag index) -> nonzero when the flag is set.
    uint32_t (*checkFlag)(uint32_t value, uint32_t index);
    // 0x49DB0 (EAX = client word) -> act byte in AL.
    uint32_t (*actOfClient)(uint32_t client);
    // call a quest handler word (retail code address) with ECX = node, EDX = arg2 and stack (unit, flags, arg2); returns AL.
    uint32_t (*callQuestHandler)(uint32_t fn, void* node, uint32_t unit, uint32_t flags, uint32_t arg2);
    // 0x8A770 (EAX = packet, stack = client word): queue a packet whose size the packet table gives.
    void     (*sendQueued)(uint32_t client, const void* packet);
};
