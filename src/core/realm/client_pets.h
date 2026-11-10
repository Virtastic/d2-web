// The client's pet list of a realm game and the attack-target relation that reads it.
// No engine dependency: the list is fed the server's pet packets and is read back through the same 32-bit
// record layout the 1.13c client walks, so the ported relation and unit-removal handlers run on it unchanged.
#pragma once
#include <cstdint>

namespace d2::realm {

struct ClientPet {
    uint32_t classId;   // +0x00
    uint32_t type;      // +0x04  pet type; 7 is the hireling
    uint32_t guid;      // +0x08
    uint32_t owner;     // +0x0C  owning player's guid, 0xFFFFFFFF for none
    uint32_t life;      // +0x1C  percent, 100 on insertion
    uint32_t dead;      // +0x20  set when the server withdraws my own hireling: the entry stays
    uint32_t seed;      // +0x24
    uint32_t nameId;    // +0x28  string index of a hireling's name
};

inline constexpr int kPetPacket7ASize = 13;
inline constexpr int kPetPacket81Size = 20;
inline constexpr uint32_t kPetHeadSlot = 0x40;        // image address of the list head pointer
inline constexpr uint32_t kPetHeroRecord = 0x80;      // image address of a record whose +0xC is the hero's guid

void clientPetsClear();
// 0x81: the full entry of a pet (type, class, owner, guid, seed, name id). False on a malformed packet.
bool clientPetPacket81(const uint8_t* packet, int length);
// 0x7A: byte 1 nonzero adds or updates an entry; zero withdraws it. My own hireling is only marked dead.
bool clientPetPacket7A(const uint8_t* packet, int length, uint32_t heroGuid);
const ClientPet* clientPetFind(uint32_t guid);
uint32_t clientPetCount();
const ClientPet* clientPetAt(uint32_t index);         // list order, newest first
uint32_t clientPetOwner(uint32_t guid);               // 0xFFFFFFFF when the unit is not a pet or has no owner
// A hireling's name string index as the client shows it (0 when the unit is not a listed hireling).
uint32_t clientPetHirelingNameId(uint32_t guid);

// The list as 32-bit records: kPetHeadSlot holds the first node's address, a node holds the fields above at
// their offsets and its successor at +0x30; kPetHeroRecord + 0xC reads the guid given to clientPetImageHero.
void clientPetImageHero(uint32_t heroGuid);
uint32_t clientPetImageRead(uint32_t address);

// One side of the relation: what the client's unit record holds for it.
struct RelationUnit {
    bool present = false;
    uint32_t type = 0, classId = 0, guid = 0, mode = 0, flags = 0;
};
struct RelationFacts {
    bool inTown = false;                               // the hero stands in a town room
    bool aligned = false;                              // the two units' alignments match (both evil or both good)
    bool rosterHostile = false;                        // the roster marks the two players hostile
    uint32_t (*monStats2Flag)(uint32_t classId, uint32_t bit) = nullptr;   // MonStats2 flag bit of a class
};
// Whether the hero may take the target as an attack target (1) or not (0).
uint32_t clientUnitRelation(const RelationUnit& target, const RelationUnit& hero, const RelationFacts& facts);
// The player whose roster relation decides the answer above: the target player, or the owner of a target pet;
// 0xFFFFFFFF when no player is involved.
uint32_t clientRelationRosterGuid(const RelationUnit& target);

}  // namespace d2::realm
