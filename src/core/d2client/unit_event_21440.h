#pragma once
#include <cstddef>
#include <cstdint>
namespace d2client_21440 {
// These are retail addresses, including callback addresses. Never cast them to
// native pointers: the caller resolves retail storage and dispatches callbacks.
struct Event {
    uint32_t kind, key1, key2, arg, callback, data, prev, next;
};
struct Association {
    uint32_t type;
    int32_t classId, id, owner;
    uint32_t value, x, y, percent, pending, extra[3], next;
};
static_assert(sizeof(Event) == 0x20 && offsetof(Event, callback) == 0x10);
static_assert(offsetof(Event, prev) == 0x18 && offsetof(Event, next) == 0x1c);
static_assert(sizeof(Association) == 0x34 && offsetof(Association, next) == 0x30);
static_assert(offsetof(Association, percent) == 0x1c && offsetof(Association, pending) == 0x20);
struct UnitLayout {
    uint32_t type, classId, pool, id;
    uint8_t rest[0x80];
    uint32_t eventHead;
};
static_assert(offsetof(UnitLayout, pool) == 8 && offsetof(UnitLayout, id) == 0xc);
static_assert(offsetof(UnitLayout, eventHead) == 0x90);
struct State {
    void* context;
    uint8_t* (*resolve)(void*, uint32_t);
    // Fog #10045/#10046 and #10042/#10043 respectively. Allocators return
    // retail addresses; the native allocation and address mapping belong to caller.
    uint32_t (*poolAlloc)(void*, uint32_t pool, uint32_t size, const char*, uint32_t line, uint32_t flags);
    void (*poolFree)(void*, uint32_t pool, uint32_t address, const char*, uint32_t line, uint32_t flags);
    uint32_t (*alloc)(void*, uint32_t size, const char*, uint32_t line, uint32_t flags);
    void (*free)(void*, uint32_t address, const char*, uint32_t line, uint32_t flags);
    uint32_t (*findOwner)(void*, int32_t id); // D2Client+0x9D630, ECX unused
    int16_t (*ownerGroup)(void*, uint32_t owner); // D2Client+0x9CE80, EAX input
    uint32_t (*compareUnit)(void*, uint32_t unit, int32_t owner, uint32_t mode); // +0x4FD90
    const char* (*errorText)(void*, uint32_t code); // Fog #10265
    void (*printError)(void*, const char* format, const char* text); // Fog #10024
    void (*exitProcess)(void*, int32_t code);
    uint32_t head;        // DAT_6fbcc4d4
    uint32_t currentUnit; // DAT_6fbcbbfc
    uint32_t tables;      // *sgptDataTables
    uint32_t bitMasks;    // gdwBitMasks
    uint32_t returnAddress; // caller return address observed in the retail stack frame
};
inline constexpr const char* eventFile = "..\\Source\\D2Client\\UNIT\\CUnitEvent.cpp";
inline constexpr const char* associationFile = "..\\Source\\D2Client\\GAME\\RosterPets.cpp";
uint32_t addEvent(State&, uint32_t unit, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t w4, uint32_t w5);
void removeEvents(State&, uint32_t unit, uint32_t key1, uint32_t key2);
void clearEvents(State&, uint32_t unit);
void removeOtherTypeSeven(State&, int32_t id);
void clearAssociations(State&);
void setAssociation(State&, int32_t id, int32_t classId, int32_t owner, uint32_t type);
} // namespace d2client_21440
