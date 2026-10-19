// D2Client 1.13c unit events and client unit associations.
// Prior art: D2MOO has no D2Client module; community pointer tables have no names
// for this batch. Rules come from the cached 1.13c routines, with register and
// stack arguments checked against retail instructions (not Ghidra's prototypes).
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>

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
static constexpr const char* eventFile = "..\\Source\\D2Client\\UNIT\\CUnitEvent.cpp";
static constexpr const char* associationFile = "..\\Source\\D2Client\\GAME\\RosterPets.cpp";
static uint32_t read(State& s, uint32_t addr) {
    uint32_t value; std::memcpy(&value, s.resolve(s.context, addr), 4); return value;
}
static void write(State& s, uint32_t addr, uint32_t value) {
    std::memcpy(s.resolve(s.context, addr), &value, 4);
}
static Event& event(State& s, uint32_t addr) { return *reinterpret_cast<Event*>(s.resolve(s.context, addr)); }
static Association& association(State& s, uint32_t addr) { return *reinterpret_cast<Association*>(s.resolve(s.context, addr)); }
static uint32_t find(State& s, int32_t id) {
    for (uint32_t p = s.head; p; p = association(s, p).next)
        if (association(s, p).id == id) return p;
    return 0;
}
static int32_t unitId(State& s, uint32_t unit) { return unit ? static_cast<int32_t>(read(s, unit + 0xc)) : -1; }

// 1.13c D2Client+0x21440
void removeEvents(State& s, uint32_t unit, uint32_t key1, uint32_t key2) {
    if (!unit) return;
    for (uint32_t p = read(s, unit + 0x90), next; p; p = next) {
        Event& e = event(s, p); next = e.next;
        if (e.key1 != key1 || e.key2 != key2) continue;
        if (e.prev) event(s, e.prev).next = next;
        else write(s, unit + 0x90, next);
        if (next) event(s, next).prev = e.prev;
        s.poolFree(s.context, read(s, unit + 8), p, eventFile, 0x80, 0);
    }
}
// 1.13c D2Client+0x214B0
uint32_t addEvent(State& s, uint32_t unitEsi, uint32_t param1, uint32_t param2,
                  uint32_t param3) {
    if (!unitEsi) return 0;
    uint32_t p = s.poolAlloc(s.context, read(s, unitEsi + 8), 0x20, eventFile, 0x48, 0);
    Event& e = event(s, p);
    e = {};
    // Retail uses ESI for the unit and consumes exactly three stack arguments.
    // The fourth record word is the caller return address, not a callback arg.
    e.kind = 0x48;
    e.key1 = param2;
    e.key2 = param3;
    e.callback = s.returnAddress;
    e.data = param1;
    e.next = read(s, unitEsi + 0x90);
    if (e.next) event(s, e.next).prev = p;
    write(s, unitEsi + 0x90, p);
    return p;
}
// 1.13c D2Client+0x21540
void clearEvents(State& s, uint32_t unit) {
    if (!unit) return;
    for (uint32_t p = read(s, unit + 0x90), next; p; p = next) {
        next = event(s, p).next;
        s.poolFree(s.context, read(s, unit + 8), p, eventFile, 0x2f, 0);
    }
    write(s, unit + 0x90, 0);
}
// 1.13c D2Client+0x21580
uint32_t percent(State& s, int32_t id) { uint32_t p = find(s, id); return p ? association(s, p).percent : 0; }
// 1.13c D2Client+0x215B0
void removeOtherTypeSeven(State& s, int32_t id) {
    uint32_t previous = 0;
    for (uint32_t p = s.head; p; p = association(s, p).next) {
        Association& a = association(s, p);
        // Retail tests +0, even though insertion's class ID is at +4.
        if (a.type == 7 && a.id != id) {
            if (previous) association(s, previous).next = a.next;
            else s.head = a.next;
            s.free(s.context, p, associationFile, 0x59, 0);
            return; // Only the first match is removed.
        }
        previous = p;
    }
}
// 1.13c D2Client+0x21630
void clearAssociations(State& s) {
    for (uint32_t p = s.head, next; p; p = next) {
        next = association(s, p).next;
        s.free(s.context, p, associationFile, 0x2f, 0);
    }
    s.head = 0;
}
// 1.13c D2Client+0x21670
int32_t classId(State& s, int32_t id) { uint32_t p = find(s, id); return p ? association(s, p).classId : 0; }
// 1.13c D2Client+0x216D0
uint32_t type(State& s, int32_t id) { uint32_t p = find(s, id); return p ? association(s, p).type : 0; }
// 1.13c D2Client+0x217E0
uint32_t count(State& s, uint32_t unit, int32_t classId) {
    const int32_t owner = unitId(s, unit);
    uint32_t result = 0;
    for (uint32_t p = s.head; p; p = association(s, p).next) {
        Association& a = association(s, p);
        if (a.classId == classId && a.owner == owner) ++result;
    }
    return result;
}
// 1.13c D2Client+0x218C0
void setAssociation(State& s, int32_t id, int32_t classId, int32_t owner, uint32_t type) {
    if (classId == 7) removeOtherTypeSeven(s, id);
    uint32_t p = find(s, id);
    if (p) {
        Association& a = association(s, p);
        a.type = type; a.owner = owner; a.classId = classId; a.pending = 0;
        return;
    }
    p = s.alloc(s.context, 0x34, associationFile, 0x7a, 0);
    Association& a = association(s, p);
    a = {}; a.type = type; a.id = id; a.owner = owner;
    a.classId = classId; a.percent = 100; a.next = s.head;
    s.head = p;
}
// 1.13c D2Client+0x219F0
uint32_t relation(State& s, int32_t id) {
    uint32_t p = find(s, id);
    if (!p) return 0;
    int32_t classId = association(s, p).classId;
    if (classId < 0 || classId >= static_cast<int32_t>(read(s, s.tables + 0xbf0))) return 3;
    // x86 table address computation wraps at 32 bits, even on wasm64.
    uint32_t row = read(s, s.tables + 0xbe8) + static_cast<uint32_t>(classId) * 0xe0u;
    if (!row || !(s.resolve(s.context, row + 4)[0] & s.resolve(s.context, s.bitMasks + 0x10)[0])) return 3;
    const int32_t current = unitId(s, s.currentUnit), owner = association(s, p).owner;
    if (owner == current) return 1;
    uint32_t found = s.findOwner(s.context, current);
    int16_t group = s.ownerGroup(s.context, found);
    if (group == -1) return 0;
    found = s.findOwner(s.context, owner);
    return group == s.ownerGroup(s.context, found) ? 2 : 0;
}
// 1.13c D2Client+0x21B10
uint32_t compareOwner(State& s, uint32_t unit) {
    uint32_t p = find(s, unitId(s, unit));
    int32_t owner = p ? association(s, p).owner : -1;
    if (!s.currentUnit || unitId(s, s.currentUnit) == -1) {
        s.printError(s.context, "", s.errorText(s.context, 0x225));
        s.exitProcess(s.context, -1);
        std::abort(); // Retail _exit cannot return.
    }
    return owner == -1 ? 1 : s.compareUnit(s.context, s.currentUnit, owner, 8);
}
} // namespace d2client_21440
