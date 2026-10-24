// Storm batch b55-sto-a: rows whose decompile is a plain (stack-arg) function.
#include <cstdint>
#include <cstddef>

// 1.13c Storm+0x55A0  _strncmp: unsigned-byte compare of at most n bytes, returns -1/0/1.
extern "C" int storm_b55_sto_a_strncmp(const char* a, const char* b, size_t n) {
    for (; n; --n, ++a, ++b) {
        unsigned char x = (unsigned char)*a, y = (unsigned char)*b;
        if (x != y) return x > y ? 1 : -1;
        if (!x) return 0;
    }
    return 0;
}

// also Storm 0x2F200  append a 12-byte zeroed node {u16 value; u32 hinst; u32 next} to the list at *head.
struct storm_b55_sto_a_node { uint16_t value; uint16_t pad; uint32_t hinst; storm_b55_sto_a_node* next; };
struct storm_b55_sto_a_env {
    storm_b55_sto_a_node** head;                 // DAT_6fc4537c
    uint32_t hinst;                              // DAT_6fc45390
    void* (*alloc_zeroed)(size_t);               // GetProcessHeap/HeapAlloc(HEAP_ZERO_MEMORY)
};
extern "C" void storm_b55_sto_a_list_append(storm_b55_sto_a_env* env, uint16_t value) {
    storm_b55_sto_a_node** link = env->head;
    while (*link) link = &(*link)->next;
    auto* n = (storm_b55_sto_a_node*)env->alloc_zeroed(12);
    *link = n;
    n->value = value;
    n->hinst = env->hinst;
    n->next = nullptr;
}
