// Prior art used: D2Client row (no D2MOO implementation).
// 1.13c cached decompiles cross-checked against x86 instructions, especially register
// arguments omitted by Ghidra. Unit buffers retain retail offsets and 32-bit links;
// resolve translates those links to native pointers (never truncates a wasm64 pointer).
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <emscripten.h>

#include "d2client_batch_24680_env.h"

namespace {
uint32_t word(const void* p, uint32_t o) {
    uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v;
}
void put(void* p, uint32_t o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }
void* link(const void* p, uint32_t o, const D2ClientBatch24680Env* e) {
    return e->resolve(word(p, o));
}
const uint8_t* record(void* unit, const D2ClientBatch24680Env* e) {
    return static_cast<const uint8_t*>(link(link(unit, 0x14, e), 0, e));
}
[[noreturn]] void fatal(uint32_t line, const D2ClientBatch24680Env* e) {
    e->assert10265_10024(line); std::abort();
}
uint32_t roll(void* u) {
    uint64_t n = uint64_t(word(u, 0x20)) * 0x6ac690c5u + word(u, 0x24);
    put(u, 0x20, uint32_t(n)); put(u, 0x24, uint32_t(n >> 32)); return uint32_t(n);
}
void idle(void* u, const D2ClientBatch24680Env* e) {
    if (!word(u, 0xc8)) { put(u, 0xd4, roll(u) & 63); put(u, 0xc8, 1); }
    if (!(word(u, 0xd4) & 63) && word(u, 0x10) != 1) {
        put(u, 0x10, 1); e->refresh(u);
    }
    uint32_t mode = word(u, 0x10);
    put(u, 0xd4, word(u, 0xd4) + 1);
    if (mode == 2) {
        uint32_t random = roll(u);
        put(u, 0xd4, word(u, 0xd4) + (random & 63)); put(u, 0x10, 0); e->refresh(u);
    }
}
void* find(void* const* buckets, uint32_t id, const D2ClientBatch24680Env* e) {
    void* u = buckets[id & 127];
    while (u && word(u, 0xc) != id) u = link(u, 0xe4, e);
    return u;
}
}

// 1.13c D2Client+0x24870
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_24870(void* unit, const D2ClientBatch24680Env* e) {
    idle(unit, e); return 1; // retail returns AL; upper EAX is unspecified
}
// 1.13c D2Client+0x24AE0
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_24ae0(void* unit, const D2ClientBatch24680Env* e) {
    idle(unit, e); e->advance(unit); return 1;
}
// 1.13c D2Client+0x24CD0
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_24cd0(void* unit, const D2ClientBatch24680Env* e) {
    if (word(unit, 0x10) == 1 && word(unit, 0x44) == uint32_t(record(unit, e)[0x12a]) * 256) {
        put(unit, 0x10, 2); put(unit, 0x44, 0); e->animate(unit, 0); e->refresh(unit);
    }
    return 1;
}
// 1.13c D2Client+0x24D10
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_24d10(void* unit, const D2ClientBatch24680Env* e) {
    const uint32_t now = e->tick();
    if (!word(unit, 0x10)) {
        if (word(unit, 0xd4) < now) {
            put(unit, 0xd4, now + 3000); put(unit, 0x44, record(unit, e)[0x12a]);
            e->set_mode(unit, 1); e->animate(unit, 0);
        }
    } else if (word(unit, 0xd4) < now) {
        uint32_t delay = e->random_range(static_cast<uint8_t*>(unit) + 0x20, 7500, 2500);
        put(unit, 0xd4, delay + now); e->set_mode(unit, 0); e->animate(unit, 0);
    }
    return 1;
}
// 1.13c D2Client+0x24680
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_24680(void* unit, const D2ClientBatch24680Env* e) {
    const uint8_t type = e->object_record(word(unit, 4))[0x1b4];
    if (type >= 19) fatal(0x546, e);
    // The x86 body is a computed tail jump, not the large inlined switch Ghidra
    // prints. Preserve the target's AL and all effects of its actual native call.
    switch (type) {
    case 0: case 1: case 11: return 1;
    case 2: return d2_client_24870(unit, e);
    case 3: return d2_client_24ae0(unit, e);
    case 10: return d2_client_24d10(unit, e);
    case 16: return d2_client_24cd0(unit, e);
    default: return e->case_target[type](unit);
    }
}
// 1.13c D2Client+0x246F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_246f0(void* unit, const D2ClientBatch24680Env* e) {
    const uint8_t* data = e->object_data(unit);
    if (!data) fatal(0x37a, e);
    if (int32_t(*data) >= e->object_type_count) fatal(0x37b, e);
    auto cb = e->object_dispatch[*data].initialize;
    if (cb) cb(unit, data);
}
// 1.13c D2Client+0x247A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_247a0(const uint8_t* packet, const D2ClientBatch24680Env* e) {
    if (!packet) fatal(0x556, e);
    void* unit = find(e->object_buckets, word(packet, 3), e);
    if (!unit) return;
    if (word(unit, 0) != 2) fatal(0x5a, e);
    if (word(unit, 4) != 59 && word(unit, 4) != 60) fatal(0x560, e);
    static_cast<uint8_t*>(link(unit, 0x14, e))[4] = packet[2];
    uint8_t flags = e->get_flags(unit);
    if (packet[1] & 2) flags |= 2;
    if (packet[1] & 1) flags |= 1;
    e->set_flags(unit, flags);
}
// 1.13c D2Client+0x24990
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_24990(void* unit, const D2ClientBatch24680Env* e) {
    e->detach(unit);
    if (e->remove_enabled) {
        e->remove10408(unit); e->remove11016(unit);
        if (word(unit, 0x64)) { e->destroy(link(unit, 0x64, e)); put(unit, 0x64, 0); }
    }
    e->free10043(link(unit, 0x14, e), 0xd6c94, 0xfe, 0);
}
// 1.13c D2Client+0x24A70
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_24a70(void* source, void* target, const D2ClientBatch24680Env* e) {
    const uint8_t* data = e->object_data(source);
    if (!data) fatal(0x34d, e);
    if (int32_t(*data) >= e->object_type_count) fatal(0x34e, e);
    uint32_t id = e->object_dispatch[*data].sound_id;
    if (id) e->sound(id, target, 0, 0, 0);
}
// 1.13c D2Client+0x24B90
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_24b90(void* source, const uint32_t* target_id, const D2ClientBatch24680Env* e) {
    const uint8_t* data = nullptr;
    uint8_t type;
    if (!e->is_object(source)) type = e->object_record(source ? word(source, 4) : 0xffffffffu)[0x16f];
    else { data = e->object_data(source); type = *data; }
    void* target = find(e->player_buckets, *target_id, e);
    if (!target) return;
    if (word(target, 0)) fatal(0x5a, e);
    if (type && int32_t(type) < e->object_type_count) {
        auto cb = e->object_dispatch[type].apply;
        if (cb) cb(source, target, data);
    }
    d2_client_24a70(source, target, e);
}
