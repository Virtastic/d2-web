// Host seam struct of src/core/d2game/d2game_n66.cpp (shared with the FN probes).
#pragma once
#include <cstdint>
typedef uint8_t* P;

struct N66Host {
    void (*mem_free)(void* p, const char* file, int line, int zero);  // Ordinal_10046
    void (*mem_free_named)(void* p, const char* name, int a, int b);  // Ordinal_403
    void (*unwind)(void* frame, void* target, void* rec, void* ret);  // RtlUnwind
    void (*unlock_fhandle)(int fh);                                   // __unlock_fhandle
    void (*fatal)(const char* msg, int code);                         // Ordinal_10024+_exit
    int32_t dataTablesRoot;                                           // *sgptDataTables_exref
    // unit/item seams
    P (*first_room)(P ptr);                                           // Ordinal_10717
    P (*unit_get_inv)(P unit);                                        // Ordinal_10331
    void (*inv_remove)(P inv, int type, int guid);                    // Ordinal_10885
    void (*unit_a)(P unit);                                           // Ordinal_10408
    void (*unit_b)(P unit);                                           // Ordinal_11016
    void (*unit_unlink)(P unit);                                      // 0xDFC10
    void (*unit_free)(P game, P unit);                                // 0xE20A0
    void (*set_string)(int a, int b);                                 // Ordinal_10786
    P (*lookup)();                                                    // 0xA00B0
    void (*apply)(int a, P b, int idx, int ebx);                      // 0x448E0
    // VirtualQuery family
    int (*reset_stack)();
    // Fog #10046 FOG_FreePool(pool, p, file, line, 0): ECX = pool, EDX = block
    void (*mem_free_pool)(void* pool, void* p, const char* file, int line, int zero);
};
