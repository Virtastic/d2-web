#include "client_pets.h"

#include <cstdlib>
#include <cstring>
#include <vector>

#include "../d2client/cl_env.h"
#include "../d2client/unit_event_21440.h"

extern "C" uint32_t d2cl_a62a0(const cl::Env& e, uint32_t A, uint32_t B);

namespace d2::realm {
namespace {
constexpr uint32_t kPetHireling = 7;
constexpr uint32_t kNodeBase = 0x1000, kNodeSize = 0x40;
constexpr uint32_t kRecordA = 0x400, kRecordB = 0x800, kRecordSize = 0x100;
// The list is the 1.13c client's own: association nodes (D2Client+0x218C0 inserts and updates, +0x215B0 drops the other
// hireling, +0x21630 clears) in a 32-bit image, the head at g_head. The proved bodies run on this image; nothing else
// keeps a copy of the pets.
using d2client_21440::Association;
alignas(8) uint8_t g_mem[kNodeBase + 0x10000];
uint32_t g_head = 0, g_top = kNodeBase;
std::vector<uint32_t> g_freeNodes;
Association* nodeAt(uint32_t address) { return reinterpret_cast<Association*>(g_mem + address); }
d2client_21440::State listState() {
    d2client_21440::State s{};
    s.resolve = [](void*, uint32_t address) { return g_mem + address; };
    s.alloc = [](void*, uint32_t, const char*, uint32_t, uint32_t) -> uint32_t {
        if (!g_freeNodes.empty()) { const uint32_t a = g_freeNodes.back(); g_freeNodes.pop_back(); return a; }
        if (g_top + kNodeSize > sizeof g_mem) std::abort();
        const uint32_t a = g_top; g_top += kNodeSize; return a;
    };
    s.free = [](void*, uint32_t address, const char*, uint32_t, uint32_t) { g_freeNodes.push_back(address); };
    s.head = g_head;
    return s;
}
uint32_t g_imageHero = 0xFFFFFFFFu;
ClientPet g_view;   // the entry clientPetFind / clientPetAt return: valid until the next call

uint32_t rd32(const uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}
uint32_t rd16(const uint8_t* p) { return uint32_t(p[0]) | (uint32_t(p[1]) << 8); }

uint32_t findNode(uint32_t guid) {
    for (uint32_t p = g_head; p; p = nodeAt(p)->next) if (uint32_t(nodeAt(p)->id) == guid) return p;
    return 0;
}
ClientPet viewOf(const Association& a) {
    return ClientPet{a.type, uint32_t(a.classId), uint32_t(a.id), uint32_t(a.owner), a.percent, a.pending, a.extra[0], a.extra[1]};
}
ClientPet* find(uint32_t guid) {
    const uint32_t p = findNode(guid);
    if (!p) return nullptr;
    g_view = viewOf(*nodeAt(p));
    return &g_view;
}

// 0x218C0 with ESI = the packet's type byte (node +4), the stack word = its class word (node +0); a type-7 entry first
// has 0x215B0 drop the one other node whose +0 is 7.
void upsert(uint32_t type, uint32_t guid, uint32_t owner, uint32_t classId) {
    auto s = listState();
    d2client_21440::setAssociation(s, int32_t(guid), int32_t(type), int32_t(owner), classId);
    g_head = s.head;
}

void withdraw(uint32_t guid, uint32_t heroGuid) {
    uint32_t previous = 0;
    for (uint32_t p = g_head; p; previous = p, p = nodeAt(p)->next) {
        Association& a = *nodeAt(p);
        if (uint32_t(a.id) != guid) continue;
        if (a.classId == int32_t(kPetHireling) && uint32_t(a.owner) == heroGuid) { a.pending = 1; return; }
        if (previous) nodeAt(previous)->next = a.next; else g_head = a.next;
        g_freeNodes.push_back(p);
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

void clientPetsClear() {
    auto s = listState();
    d2client_21440::clearAssociations(s);   // 0x21630
    g_head = s.head;
}

bool clientPetPacket81(const uint8_t* packet, int length) {
    if (!packet || length != kPetPacket81Size || packet[0] != 0x81) return false;
    const uint32_t type = packet[1], classId = rd16(packet + 2), owner = rd32(packet + 4), guid = rd32(packet + 8);
    upsert(type, guid, owner, classId);
    if (const uint32_t p = findNode(guid)) {
        nodeAt(p)->extra[0] = rd32(packet + 12);
        nodeAt(p)->extra[1] = rd16(packet + 16);
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
uint32_t clientPetCount() {
    uint32_t n = 0;
    for (uint32_t p = g_head; p; p = nodeAt(p)->next) ++n;
    return n;
}
const ClientPet* clientPetAt(uint32_t index) {
    uint32_t p = g_head;
    for (; p && index; --index) p = nodeAt(p)->next;
    if (!p) return nullptr;
    g_view = viewOf(*nodeAt(p));
    return &g_view;
}

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
    if (address == kPetHeadSlot) return g_head;
    if (address == kPetHeroRecord + 0xc) return g_imageHero;
    if (address < kNodeBase || address + 4 > g_top) return 0;
    uint32_t value;
    std::memcpy(&value, g_mem + address, 4);
    return value;
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
