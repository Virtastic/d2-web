// 1.13c Fog+0x1E910 (ordinal 10260), from build-wasm/ghidra-fog/decomp/0x1E910.c and the retail disassembly.
// stdcall(socket, buf, len) ret 0xC: a NULL socket, a NULL connection at +0x110 or a connection whose first dword is
// zero answers -1; otherwise tail-calls +0x1E880 with EAX = the connection, stack (buf, len zero-extended from 16 bits)
// and returns its result.
#include <cstdint>
#include <cstring>

extern "C" int32_t fog_sub_1E910(const uint8_t* socket, uint32_t buf, uint16_t len,
                                 int32_t (*send_1e880)(uint8_t* conn, uint32_t buf, uint32_t len)) {
    if (socket == nullptr) return -1;
    uint32_t c; std::memcpy(&c, socket + 0x110, 4);
    uint8_t* conn = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(c));
    if (conn == nullptr) return -1;
    uint32_t head; std::memcpy(&head, conn, 4);
    if (head == 0) return -1;
    return send_1e880(conn, buf, len);
}
