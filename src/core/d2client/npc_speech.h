// D2Client NPC Talk-box speech selection (npc_speech.cpp). Pure logic: the hero seed and quest state come in through Env.
#pragma once
#include <cstdint>
namespace d2client_npcspeech {
constexpr int kRecordCount = 46;
struct Msg { uint16_t id; uint8_t flag; int32_t want; int32_t quest; int32_t cls; };   // entry +0, +2, +3, +7, +0xB
struct Rec { int32_t cls; uint8_t noIntro; const Msg* msgs; int32_t count; };          // record +0, +0x14, +5, +9
struct Env { uint32_t (*rand)(void*, uint32_t); int (*quest)(void*, int32_t); void* ctx; int32_t playerClass; };
struct State { bool rolled; int32_t idx[kRecordCount]; };
int recordIndex(int32_t npcClass);
const Rec& record(int index);
void reset(State& st);
bool hasIntroduction(int32_t npcClass);
int32_t introductionId(int32_t npcClass, int32_t playerClass);
int32_t gossipId(State& st, int32_t npcClass, const Env& env);
}
