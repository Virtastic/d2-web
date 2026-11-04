// src/core/fog/fog_qssocket.h -- the 1.13c Fog D2QSSocket object (0x194 bytes) and the seams its wing calls through
// (src/core/fog/fog_batch_1e3d0.cpp). Pointer fields are 32-bit words, so the layout is retail's on wasm64 too.
#pragma once
#include <cstddef>
#include <cstdint>

struct FogQSSocket {
    uint32_t thread;       // +0x000 CreateThread result
    uint32_t threadId;     // +0x004
    char name[0x104];      // +0x008
    uint32_t value;        // +0x10C
    uint32_t connection;   // +0x110 connection pointer
    uint32_t event;        // +0x114 CreateEventA result
    uint32_t at118;        // +0x118 = 2
    uint32_t at11c;        // +0x11C = 0
    uint32_t at120;        // +0x120
    uint32_t at124;        // +0x124 = 0
    char tag[0x40];        // +0x128
    uint8_t at168[8];      // +0x168
    uint8_t protocol[4];   // +0x170
    uint8_t critical[24];  // +0x174 CRITICAL_SECTION
    uint32_t at18c;        // +0x18C = 0
    uint32_t at190;        // +0x190 = 0
};
static_assert(offsetof(FogQSSocket, threadId) == 0x004, "threadId");
static_assert(offsetof(FogQSSocket, name) == 0x008, "name");
static_assert(offsetof(FogQSSocket, value) == 0x10C, "value");
static_assert(offsetof(FogQSSocket, connection) == 0x110, "connection");
static_assert(offsetof(FogQSSocket, event) == 0x114, "event");
static_assert(offsetof(FogQSSocket, at118) == 0x118, "at118");
static_assert(offsetof(FogQSSocket, tag) == 0x128, "tag");
static_assert(offsetof(FogQSSocket, protocol) == 0x170, "protocol");
static_assert(offsetof(FogQSSocket, critical) == 0x174, "critical");
static_assert(offsetof(FogQSSocket, at18c) == 0x18C, "at18c");
static_assert(sizeof(FogQSSocket) == 0x194, "FogQSSocket");

struct FogQSSocketApi {
    void* (*alloc)(uint32_t size, uint32_t zero, const char* file, uint32_t line);   // +0x1B560
    void (*free_block)(void** slot, const char* file, uint32_t line);                // +0x1B0C0
    void (*teardown)(void* socket);                                                  // +0x1E5C0 (ESI = socket)
    void (*flush)(void* protocol);                                                   // +0x17B60 (ESI = protocol)
    void (*close_conn)(void* connection);                                            // +0x17020 (ECX = connection)
    uint16_t (*send_conn)(void* connection, int32_t param, uint16_t length);         // +0x16EB0 (ECX conn, EDX param, push len)
    void (*validate_cs)(void* cs, uint32_t line);                                    // +0x11690
    uint32_t (*capture_ip)(void);                                                    // +0xFC20
    void (*stack_dump)(void);                                                        // +0xCD8E
    void (*fatal)(int level, const char* file, uint32_t line, const char* format, ...); // +0x10F30
    void (*abort)(void);                                                             // +0x684A0
    void (*exit_process)(int code);                                                  // +0x1BA9
    void* (*thread_proc)(void* param);                                               // +0x6EC90
    uint32_t (*create_event)(uint32_t sa, uint32_t manual, uint32_t initial, uint32_t name);   // CreateEventA
    uint32_t (*create_thread)(uint32_t sa, uint32_t stack, void* (*proc)(void*), void* param, uint32_t flags,
                              uint32_t* id);                                         // CreateThread
    uint32_t (*wait_object)(uint32_t handle, uint32_t ms);                              // WaitForSingleObject
    void (*sleep_ms)(uint32_t ms);                                                   // Sleep
    void (*enter_cs)(void* cs);                                                      // EnterCriticalSection
    void (*leave_cs)(void* cs);                                                      // LeaveCriticalSection
};

