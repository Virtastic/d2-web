// src/core/d2client/d2client_27d0.cpp -- 1.13c D2Client+0x027D0, the base constructor of the
// 0x22C0/0x2670 GAMEDATA object. The three internal callees are
// host seams after the retail argument, as in d2client_batch_2190.cpp. Every field is a 32-bit word:
// the object is the retail 0x28-byte layout, not a host struct.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatch27d0Env {
    uint32_t vtable_27d0;                        // &PTR_FUN_6fb860e0
    void (*f_28a0)(uint32_t* field);             // 0x28A0  init a three-word list header
    void (*f_2960)(uint32_t* vec, uint32_t n);   // 0x2960  EAX = vector, EDI = new element count
    void (*f_30a0)(uint32_t list);               // 0x30A0  EDI = list header, clears its nodes
};

struct D2Client27d0Object {
    uint32_t vtable;        // +0x00
    uint32_t list_size;     // +0x04
    uint32_t list_next;     // +0x08
    uint32_t list_prev;     // +0x0C
    uint32_t field_10;      // +0x10
    uint32_t vec_capacity;  // +0x14
    uint32_t vec_size;      // +0x18
    uint32_t vec_data;      // +0x1C
    uint32_t vec_alloc;     // +0x20
    uint32_t count;         // +0x24
};
static_assert(offsetof(D2Client27d0Object, list_size) == 0x04, "");
static_assert(offsetof(D2Client27d0Object, field_10) == 0x10, "");
static_assert(offsetof(D2Client27d0Object, vec_capacity) == 0x14, "");
static_assert(offsetof(D2Client27d0Object, vec_data) == 0x1C, "");
static_assert(offsetof(D2Client27d0Object, count) == 0x24, "");
static_assert(sizeof(D2Client27d0Object) == 0x28, "");

// The "empty circular list" header retail builds inline in both places: three words [size][next][prev],
// with next = &next and prev = ~next. Run only when the size word is not already the expected value; the
// clear (0x30A0) runs first, exactly as retail orders it.
static void d2c13_init_list(uint32_t p, uint32_t size, uint32_t expected,
                            const D2ClientBatch27d0Env* env) {
    if (*(const uint32_t*)(uintptr_t)p == expected) return;
    env->f_30a0(p);
    uint32_t next = p + 4;
    *(uint32_t*)(uintptr_t)next = next;
    *(uint32_t*)(uintptr_t)p = size;
    *(uint32_t*)(uintptr_t)(p + 8) = ~next;
}

// 1.13c D2Client+0x027D0 (207 B, ret 4): construct the base object. Install the vtable, initialise the
// three-word list at +0x04 through 0x28A0, zero +0x10 and the four-word vector header at +0x14..+0x20,
// then (unless 0x28A0 left 0x0C at +0x04) clear and rebuild the empty list {size 0x0C, self, ~self}.
// Set +0x24 = 3, resize the vector to 4 elements of 0x0C bytes, and give each element a fresh empty list
// {size 4, self, ~self}. The vector data word and the +0x24 bound are re-read every pass, as in retail.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_27d0(void* self, const D2ClientBatch27d0Env* env) {
    D2Client27d0Object* o = (D2Client27d0Object*)self;
    o->vtable = env->vtable_27d0;
    env->f_28a0(&o->list_size);
    o->vec_capacity = 0;
    o->vec_size = 0;
    o->vec_data = 0;
    o->vec_alloc = 0;
    o->field_10 = 0;
    d2c13_init_list((uint32_t)(uintptr_t)&o->list_size, 0xC, 0xC, env);
    o->count = 3;
    env->f_2960(&o->vec_capacity, 4);
    uint32_t i = 0;
    uint32_t offset = 0;
    do {
        uint32_t entry = o->vec_data + offset;
        d2c13_init_list(entry, 4, 4, env);
        i++;
        offset += 0xC;
    } while (i <= o->count);
    return self;
}
