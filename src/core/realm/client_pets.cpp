#include "client_pets.h"

#include <vector>

#include "../d2client/cl_env.h"

extern "C" uint32_t d2cl_a62a0(const cl::Env& e, uint32_t A, uint32_t B);

namespace d2::realm {
namespace {
constexpr uint32_t kPetHireling = 7;
constexpr uint32_t kNodeBase = 0x1000, kNodeSize = 0x40;
constexpr uint32_t kRecordA = 0x400, kRecordB = 0x800, kRecordSize = 0x100;
std::vector<ClientPet> g_pets;   // front is the list head
uint32_t g_imageHero = 0xFFFFFFFFu;

uint32_t rd32(const uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}
uint32_t rd16(const uint8_t* p) { return uint32_t(p[0]) | (uint32_t(p[1]) << 8); }

ClientPet* find(uint32_t guid) {
    for (auto& pet : g_pets) if (pet.guid == guid) return &pet;
    return nullptr;
}

// Before a hireling is entered the client drops one other entry. The retail routine tests the entry's first
// field, which holds the class, against 7 and unlinks the first such entry with another guid.
void dropOtherHireling(uint32_t guid) {
    for (auto it = g_pets.begin(); it != g_pets.end(); ++it) {
        if (it->classId != kPetHireling || it->guid == guid) continue;
        g_pets.erase(it);
        return;
    }
}

void upsert(uint32_t type, uint32_t guid, uint32_t owner, uint32_t classId) {
    if (type == kPetHireling) dropOtherHireling(guid);
    if (ClientPet* pet = find(guid)) {
        pet->classId = classId; pet->owner = owner; pet->type = type; pet->dead = 0;
        return;
    }
    g_pets.insert(g_pets.begin(), ClientPet{classId, type, guid, owner, 100, 0, 0, 0});
}

void withdraw(uint32_t guid, uint32_t heroGuid) {
    for (auto it = g_pets.begin(); it != g_pets.end(); ++it) {
        if (it->guid != guid) continue;
        if (it->type == kPetHireling && it->owner == heroGuid) it->dead = 1;
        else g_pets.erase(it);
        return;
    }
}

struct RelationCall {
    const RelationUnit* target;
    const RelationUnit* hero;
    const RelationFacts* facts;
};

uint32_t recordRead(const RelationUnit& unit, uint32_t offset) {
    switch (offset) {
    case 0x00: return unit.type;
    case 0x04: return unit.classId;
    case 0x0c: return unit.guid;
    case 0x10: return unit.mode;
    case 0xc4: return unit.flags;
    default: return 0;   // +0x94, +0x98 and the owner bit of +0xC8 belong to units the server never sends this client
    }
}

uint32_t relationRead(void* ctx, uint32_t address) {
    const auto& call = *static_cast<const RelationCall*>(ctx);
    if (address >= kRecordA && address < kRecordA + kRecordSize) return recordRead(*call.target, address - kRecordA);
    if (address >= kRecordB && address < kRecordB + kRecordSize) return recordRead(*call.hero, address - kRecordB);
    return clientPetImageRead(address);
}

bool unitDead(const RelationUnit& unit) {
    if (unit.flags & 0x10000u) return true;
    if (unit.type == 0) return unit.mode == 0 || unit.mode == 0x11;
    if (unit.type == 1) return unit.mode == 0 || unit.mode == 0x0c;
    return false;
}

uint32_t relationHook(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t, uint32_t,
        uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {
    const auto& call = *static_cast<const RelationCall*>(ctx);
    switch (rva) {
    case 0xc1f0: return 1;                                    // the hero's room: a token the next call receives
    case 0xc208: return call.facts->inTown ? 1 : 0;
    case 0x216a0: return clientPetOwner(ecx);
    case 0xa5440: return unitDead(*call.target) ? 1 : 0;
    case 0x1510: return call.facts->monStats2Flag ? call.facts->monStats2Flag(eax, edx) : 0;
    case 0x4fd90: return call.facts->rosterHostile ? 1 : 0;
    case 0xc6a6: return call.facts->aligned ? 1 : 0;
    // 0x9dc30 (leave to loot a player's corpse) and 0xa51f0 (a selected skill that targets my own pet in
    // town) read roster and skill state this client does not hold: both answer no.
    default: return 0;
    }
}
}  // namespace

void clientPetsClear() { g_pets.clear(); }

bool clientPetPacket81(const uint8_t* packet, int length) {
    if (!packet || length != kPetPacket81Size || packet[0] != 0x81) return false;
    const uint32_t type = packet[1], classId = rd16(packet + 2), owner = rd32(packet + 4), guid = rd32(packet + 8);
    upsert(type, guid, owner, classId);
    if (ClientPet* pet = find(guid)) {
        pet->guid = guid; pet->owner = owner; pet->type = type;
        pet->seed = rd32(packet + 12);
        pet->nameId = rd16(packet + 16);
    }
    return true;
}

bool clientPetPacket7A(const uint8_t* packet, int length, uint32_t heroGuid) {
    if (!packet || length != kPetPacket7ASize || packet[0] != 0x7a) return false;
    const uint32_t type = packet[2], classId = rd16(packet + 3), owner = rd32(packet + 5), guid = rd32(packet + 9);
    if (packet[1]) upsert(type, guid, owner, classId);
    else withdraw(guid, heroGuid);
    return true;
}

const ClientPet* clientPetFind(uint32_t guid) { return find(guid); }
uint32_t clientPetCount() { return static_cast<uint32_t>(g_pets.size()); }
const ClientPet* clientPetAt(uint32_t index) { return index < g_pets.size() ? &g_pets[index] : nullptr; }

uint32_t clientPetOwner(uint32_t guid) {
    const ClientPet* pet = find(guid);
    return pet ? pet->owner : 0xFFFFFFFFu;
}

uint32_t clientPetHirelingNameId(uint32_t guid) {
    const ClientPet* pet = find(guid);
    if (!pet || pet->type != kPetHireling) return 0;
    return pet->nameId == 0x421 ? 0x2b0d : pet->nameId;
}

void clientPetImageHero(uint32_t heroGuid) { g_imageHero = heroGuid; }

uint32_t clientPetImageRead(uint32_t address) {
    if (address == kPetHeadSlot) return g_pets.empty() ? 0 : kNodeBase;
    if (address == kPetHeroRecord + 0xc) return g_imageHero;
    if (address < kNodeBase) return 0;
    const uint32_t index = (address - kNodeBase) / kNodeSize, offset = (address - kNodeBase) % kNodeSize;
    if (index >= g_pets.size()) return 0;
    const ClientPet& pet = g_pets[index];
    switch (offset) {
    case 0x00: return pet.classId;
    case 0x04: return pet.type;
    case 0x08: return pet.guid;
    case 0x0c: return pet.owner;
    case 0x1c: return pet.life;
    case 0x20: return pet.dead;
    case 0x24: return pet.seed;
    case 0x28: return pet.nameId;
    case 0x30: return index + 1 < g_pets.size() ? kNodeBase + (index + 1) * kNodeSize : 0;
    default: return 0;
    }
}

uint32_t clientUnitRelation(const RelationUnit& target, const RelationUnit& hero, const RelationFacts& facts) {
    RelationCall call{&target, &hero, &facts};
    cl::Env env{};
    env.ctx = &call;
    env.h = relationHook;
    env.rd = relationRead;
    env.gl = kPetHeadSlot;
    return d2cl_a62a0(env, target.present ? kRecordA : 0, hero.present ? kRecordB : 0);
}

uint32_t clientRelationRosterGuid(const RelationUnit& target) {
    if (!target.present) return 0xFFFFFFFFu;
    if (target.type == 0) return target.guid;
    return target.type == 1 ? clientPetOwner(target.guid) : 0xFFFFFFFFu;
}

}  // namespace d2::realm
