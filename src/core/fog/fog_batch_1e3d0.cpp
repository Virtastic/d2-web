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

#include <Windows.h>   // CRITICAL_SECTION, InitializeCriticalSection (host no-op)

#include "fog_qssocket.h"

namespace {

constexpr const char* kQssocketFile = "..\\Source\\Fog\\Src\\D2QSSocket.cpp";

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
// Tear the protocol layer down (ESI = socket), flush +0x170 (ESI), close and clear the connection at +0x110
// when it is live (its +0x24 flag is set; ECX = connection), then release the block through a stack slot
// holding the socket (ECX = &slot, EAX = 0, D2QSSocket.cpp:0x27C).
extern "C"
void fog_sub_1E940(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket == nullptr) return;
    api->teardown(socket);
    api->flush(socket->protocol);
    uint8_t* conn = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(socket->connection));
    uint32_t live = 0;
    if (conn != nullptr) std::memcpy(&live, conn + 0x24, 4);
    if (conn != nullptr && live != 0) {
        api->close_conn(conn);
        socket->connection = 0;
    }
    void* slot = socket;
    api->free_block(&slot, kQssocketFile, 0x27C);
}

// 1.13c Fog+0x1E9A0 (ordinal 10075): the protocol-layer half of the teardown (ESI = socket).
extern "C"
void fog_sub_1E9A0(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket != nullptr) api->teardown(socket);
}

// 1.13c Fog+0x1E800 (ordinal 10071)
// Send: a NULL object, a missing connection or a NULL connection head reads false. Otherwise
// validate (ECX = cs, EDX = 0x1DF) and enter the connection's critical section at +0xC, send
// (ECX = connection reread from +0x110, EDX = param, push length) when param and the low 16 bits of
// length are both non-zero, leave the section at the reread connection +0xC, and report whether the
// low 16 bits of the sent count equal length.
extern "C"
bool fog_sub_1E800(const FogQSSocketApi* api, FogQSSocket* socket, int32_t param, uint16_t length) {
    if (socket == nullptr) return false;
    uint8_t* conn = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(socket->connection));
    uint32_t head = 0;
    if (conn != nullptr) std::memcpy(&head, conn, 4);
    if (conn == nullptr || head == 0) return false;

    api->validate_cs(conn + 0xC, 0x1DF);
    api->enter_cs(conn + 0xC);
    uint16_t sent = 0;
    if (param != 0 && length != 0) {
        sent = api->send_conn(reinterpret_cast<void*>(static_cast<uintptr_t>(socket->connection)), param, length);
    }
    api->leave_cs(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(socket->connection)) + 0xC);
    return length == sent;
}

// 1.13c Fog+0x1ECD0 (ordinal 10074): start the object's thread if it has none.
// Clear +0x124, CreateEventA(0, 0, 0, 0) into +0x114, CreateThread(0, 0, +0x1EC90, object, 0, &object+0x04)
// into +0x00 and return the handle (NULL when the object is null or already has a thread).
extern "C"
uint32_t fog_sub_1ECD0(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket == nullptr || socket->thread != 0) return 0;
    socket->at124 = 0;
    socket->event = api->create_event(0, 0, 0, 0);
    const uint32_t thread = api->create_thread(0, 0, api->thread_proc, socket, 0, &socket->threadId);
    socket->thread = thread;
    return thread;
}

// 1.13c Fog+0x1ED20 (ordinal 10073): start the thread and wait until it is running.
// The same creation as +0x1ECD0, then WaitForSingleObject(handle, 0) with a 10 ms Sleep between
// retries until the wait answers 0.
extern "C"
void fog_sub_1ED20(const FogQSSocketApi* api, FogQSSocket* socket) {
    if (socket == nullptr || socket->thread != 0) return;
    socket->at124 = 0;
    socket->event = api->create_event(0, 0, 0, 0);
    const uint32_t thread = api->create_thread(0, 0, api->thread_proc, socket, 0, &socket->threadId);
    socket->thread = thread;
    uint32_t state = api->wait_object(thread, 0);
    while (state != 0) {
        api->sleep_ms(10);
        state = api->wait_object(thread, 0);
    }
}
