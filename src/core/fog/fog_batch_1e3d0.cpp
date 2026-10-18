// src/core/fog/fog_batch_1e3d0.cpp -- six 1.13c Fog.dll alpha-lane rows from the D2QSSocket wing
// (Source/Fog/Src/D2QSSocket.cpp), reimplemented from the decompiles in
// build-wasm/ghidra-fog/decomp/0x<RVA>.c. Their 1.13c calling conventions were recovered from the
// disassembly of retail/D2/D2SE/CORES/1.13c/Fog.dll: every one is __stdcall with stack arguments.
//
// The module-internal callees are separate, still UNMAPPED Fog rows (+0x1B560 alloc, +0x1B0C0 free,
// +0xE5C0 teardown, +0x67B60 flush, +0x67020 close, +0x66EB0 send, +0x6EC90 thread start, +0x61690
// CS validate, +0xFC20 capture-ip, +0xCD8E stack dump, +0x10F30 fatal, +0x684A0 abort, +0x1BA9
// _exit) plus the Win32 thread/event primitives; each is a seam on the env struct passed first.
//
// The socket object is 0x194 bytes: thread handle/id at +0x00/+0x04, name[0x104] at +0x08, a value
// at +0x10C, the connection pointer at +0x110, flags at +0x118/+0x11C, tag[0x40] at +0x128, the
// protocol block at +0x170 and its critical section at +0x174.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <Windows.h>   // CRITICAL_SECTION, Enter/Leave/InitializeCriticalSection (host no-ops)

namespace {

constexpr const char* kQssocketFile = "..\\Source\\Fog\\Src\\D2QSSocket.cpp";

struct FogQSSocket {
    uint32_t thread;       // +0x000 CreateThread result
    uint32_t threadId;     // +0x004
    char name[0x104];      // +0x008
    uint32_t value;        // +0x10C
    void* connection;      // +0x110
    void* event;           // +0x114 CreateEventA result
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
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, threadId) == 0x004, "threadId");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, name) == 0x008, "name");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, value) == 0x10C, "value");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, connection) == 0x110, "connection");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, event) == 0x114, "event");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, at118) == 0x118, "at118");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, tag) == 0x128, "tag");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, protocol) == 0x170, "protocol");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, critical) == 0x174, "critical");
static_assert(sizeof(void*) != 4 || offsetof(FogQSSocket, at18c) == 0x18C, "at18c");
static_assert(sizeof(void*) != 4 || sizeof(FogQSSocket) == 0x194, "FogQSSocket");

struct FogQSSocketApi {
    void* (*alloc)(uint32_t size, uint32_t zero, const char* file, uint32_t line);   // +0x1B560
    void (*free_block)(void** slot, const char* file, uint32_t line);                // +0x1B0C0
    void (*teardown)(void* socket);                                                  // +0x1E5C0
    void (*flush)(void* protocol);                                                   // +0x67B60
    void (*close_conn)(void* connection);                                            // +0x67020
    uint16_t (*send_conn)(void* connection, int32_t param, uint16_t length);         // +0x66EB0
    void (*validate_cs)(void* cs, uint32_t line);                                    // +0x11690
    uint32_t (*capture_ip)(void);                                                    // +0xFC20
    void (*stack_dump)(void);                                                        // +0xCD8E
    void (*fatal)(int level, const char* file, uint32_t line, const char* format, ...); // +0x10F30
    void (*abort)(void);                                                             // +0x684A0
    void (*exit_process)(int code);                                                  // +0x1BA9
    void* (*thread_proc)(void* param);                                               // +0x6EC90
    void* (*create_event)(void);                                                     // CreateEventA
    void* (*create_thread)(void* (*proc)(void*), void* param, uint32_t* id);         // CreateThread
    uint32_t (*wait_object)(void* handle, uint32_t ms);                              // WaitForSingleObject
    void (*sleep_ms)(uint32_t ms);                                                   // Sleep
};

// Storm #501 SStrCopy: copy at most max-1 bytes, always terminate.
void sstr_copy(char* dest, const char* src, uint32_t max) {
    if (max == 0) return;
    uint32_t i = 0;
    while (i + 1 < max && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// The unrecoverable-internal-error tail shared by the wing: capture ip, stack dump, report level 6
// at the given line with the IP, abort, _exit(-1).
[[noreturn]] void qssocket_fatal(const FogQSSocketApi* api, uint32_t line) {
    const uint32_t ip = api->capture_ip();
    api->stack_dump();
    api->fatal(6, "", line, "Unrecoverable internal error %08x", ip);
    api->abort();
    api->exit_process(-1);
    __builtin_unreachable();
}

}  // namespace

// 1.13c Fog+0x1E3D0 (ordinal 10068)
// Create a socket object: a NULL name or one longer than 0x102 characters is unrecoverable at
// D2QSSocket.cpp:0x242/:0x243. Allocate 0x194 zeroed bytes; copy name (SStrCopy, 0x104) to +0x08,
// copy the tag text to +0x128 with limit 4 (NULL tag: "UNKNOWN", limit 0x40), store the value at
// +0x10C, clear +0x170/+0x18C/+0x190, initialise the critical section at +0x174 and set +0x118 = 2.
// Allocation failure reports level 2 at :0x245 and returns NULL.
extern "C"
FogQSSocket* fog_sub_1E3D0(const FogQSSocketApi* api, const char* name, int32_t value,
                           const char* tag) {
    if (name == nullptr) qssocket_fatal(api, 0x242);
    uint32_t length = 0;
    while (name[length] != '\0') ++length;
    if (length > 0x102) qssocket_fatal(api, 0x243);

    FogQSSocket* socket = static_cast<FogQSSocket*>(api->alloc(0x194, 0, kQssocketFile, 0x245));
    if (socket == nullptr) {
        api->fatal(2, kQssocketFile, 0x245, "");
        return nullptr;
    }
    std::memset(socket, 0, 0x194);
    sstr_copy(socket->name, name, 0x104);
    sstr_copy(socket->tag, tag != nullptr ? tag : "UNKNOWN", tag != nullptr ? 4 : 0x40);
    socket->value = static_cast<uint32_t>(value);
    socket->at18c = 0;
    socket->at190 = 0;
    *reinterpret_cast<uint32_t*>(socket->protocol) = 0;
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(socket->critical));
    socket->at11c = 0;
    socket->at118 = 2;
    return socket;
}

// 1.13c Fog+0x1E940 (ordinal 10069): terminate a socket object.
// Tear the protocol layer down, flush +0x170, close and clear the connection at +0x110 when it is
// live (its +0x24 flag is set), then release the block (D2QSSocket.cpp:0x27C).
extern "C"
void fog_sub_1E940(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket == nullptr) return;
    api->teardown(socket);
    api->flush(socket->protocol);
    void* conn = socket->connection;
    if (conn != nullptr && *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(conn) + 0x24) != 0) {
        api->close_conn(conn);
        socket->connection = nullptr;
    }
    void* slot = socket;
    api->free_block(&slot, kQssocketFile, 0x27C);
}

// 1.13c Fog+0x1E9A0 (ordinal 10075): the protocol-layer half of the teardown.
extern "C"
void fog_sub_1E9A0(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket != nullptr) api->teardown(socket);
}

// 1.13c Fog+0x1E800 (ordinal 10071)
// Send: a NULL object, a missing connection or a NULL connection head reads false. Otherwise
// validate and enter the connection's critical section at +0xC, send arg2/arg3 when both are
// non-zero, leave, and report whether the returned count equals arg3.
extern "C"
bool fog_sub_1E800(const FogQSSocketApi* api, FogQSSocket* socket, int32_t param, uint16_t length) {
    if (socket == nullptr) return false;
    uint8_t* conn = static_cast<uint8_t*>(socket->connection);
    if (conn == nullptr || *reinterpret_cast<uint32_t*>(conn) == 0) return false;

    api->validate_cs(conn + 0xC, 0x1DF);
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(conn + 0xC));
    uint16_t sent = 0;
    if (param != 0 && length != 0) {
        sent = api->send_conn(socket->connection, param, length);
    }
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
        static_cast<uint8_t*>(socket->connection) + 0xC));
    return length == sent;
}

// 1.13c Fog+0x1ECD0 (ordinal 10074): start the object's thread if it has none.
// Clear +0x124, create the event at +0x114, create the thread on the +0x6EC90 routine with the
// object as parameter and its id at +0x04, store the handle at +0x00 and return it (NULL when the
// object is null or already has a thread).
extern "C"
void* fog_sub_1ECD0(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket == nullptr || socket->thread != 0) return nullptr;
    socket->at124 = 0;
    socket->event = api->create_event();
    void* thread = api->create_thread(api->thread_proc, socket, &socket->threadId);
    socket->thread = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(thread));
    return thread;
}

// 1.13c Fog+0x1ED20 (ordinal 10073): start the thread and wait until it is running.
// The same creation as +0x1ECD0, then WaitForSingleObject(handle, 0) with a 10 ms Sleep between
// retries until the wait answers 0.
extern "C"
void fog_sub_1ED20(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket == nullptr || socket->thread != 0) return;
    socket->at124 = 0;
    socket->event = api->create_event();
    void* thread = api->create_thread(api->thread_proc, socket, &socket->threadId);
    socket->thread = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(thread));
    uint32_t state = api->wait_object(thread, 0);
    while (state != 0) {
        api->sleep_ms(10);
        state = api->wait_object(thread, 0);
    }
}
