// Seams of D2Client 0x6D010 (the per-frame pump over the group list of the self unit's room set).
#pragma once
#include <cstdint>
typedef uintptr_t U;
struct D2Client6d010Env {
    uint32_t (*ld32)(U base, uint32_t off);
    U        (*ldp)(U base, uint32_t off);
    uint32_t (*g04c)();                                           // dword D2Client+0x11C04C (busy-wait gate)
    uint32_t (*g054)();                                           // dword D2Client+0x11C054
    U        self;                                                // dword D2Client+0x11BBFC (self unit), read once at entry
    uint32_t (*fc1f0)(U self);                                    // thunk 0xC1F0
    void     (*fc3dc)(uint32_t h, U* arr, uint32_t* cnt);         // thunk 0xC3DC (out: array of unit pointers, count)
    uint32_t (*fc11e)();                                          // thunk 0xC11E (limit probe)
    void     (*fc1760)(uint32_t one);                             // 0xc1760
    uint32_t (*fc65e)(U node);                                    // thunk 0xC65E
    void     (*fc658)(U node, uint32_t* type, uint32_t* idx, uint32_t* aux); // thunk 0xC658 (file "..." line 0x7AA)
    int32_t  (*limit)();                                          // [[D2Client+0xCE474]] + 0xA80
    uint32_t (*recWord)(uint32_t idx);                            // dword at [D2Client+0x1154E4] + idx*160 + 0x30
    void     (*f6c3f0)(U node, U arg);                            // 0x6C3F0 (EAX = node, stack arg)
    void     (*f6c3b0)(U node, U arg);                            // 0x6C3B0 (EAX = node, stack arg)
};
// 1.13c D2Client+0x6D010: EAX = arg. Returns the EAX retail leaves: 0 when 0xC1F0 gives 0, the
// group count when the walk ends, and 0xC11E's value when the limit probe stops it.
uint32_t d2c_6d010(const D2Client6d010Env& e, U arg);
