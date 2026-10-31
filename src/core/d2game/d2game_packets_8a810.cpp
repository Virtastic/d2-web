// 1.13c D2Game packet senders 0x8A810..0x8B540 (fifteen rows, one file). Each builds one server->client packet in a
// stack buffer and hands it to the 0x8A3E0 append helper, which is passed in as `send` (retail: EAX = client, stdcall
// src/count). The client is not in the decompile's signature (it rides in EAX at the call), so every sender takes it
// explicitly. Layouts come from the decompile's frame: the lowest local is byte 0 and local_NN sits at byte (base - NN).
// Bytes the decompile never writes are sent as zero (retail sends stack garbage there; not reproducible).
#include <cstdint>
#include <cstring>
#include <vector>

typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

static inline void pk_w16(uint8_t* b, uint32_t o, uint16_t v) { memcpy(b + o, &v, 2); }
static inline void pk_w32(uint8_t* b, uint32_t o, uint32_t v) { memcpy(b + o, &v, 4); }

// 1.13c D2Game+0x8A810 (EAX = NUL-terminated name, stack: len, p2 unused, p3, p4, payload): opcode 0x88, the name copied
// unbounded from byte 1 with its NUL, p3 at 5, len at 7, p4 at 9, payload from 11. Sends len + 11 bytes.
void D2Game_8A810_PacketSend88(D2GSend send, void* client, const char* name, uint16_t len, uint32_t p2, uint16_t p3, uint16_t p4, const void* payload)
{
    (void)p2;
    size_t nl = strlen(name) + 1;
    std::vector<uint8_t> b((size_t)len + 11 > 1 + nl ? (size_t)len + 11 : 1 + nl);
    b[0] = 0x88;
    memcpy(b.data() + 1, name, nl);
    pk_w16(b.data(), 5, p3);
    pk_w16(b.data(), 7, len);
    pk_w16(b.data(), 9, p4);
    if (len) memcpy(b.data() + 11, payload, len);
    send(client, b.data(), (uint32_t)len + 11);
}

// 1.13c D2Game+0x8A890 (EAX unused, p2 = source, p3 unused, p4 = count): opcode 0x84, then p4 bytes from byte 2. Sends p4 + 2.
void D2Game_8A890_PacketSend84(D2GSend send, void* client, const void* src, uint16_t count)
{
    std::vector<uint8_t> b((size_t)count + 2, 0);
    b[0] = 0x84;
    if (count) memcpy(b.data() + 2, src, count);
    send(client, b.data(), (uint32_t)count + 2);
}

// 1.13c D2Game+0x8A8E0 (same frame as 0x8A810): opcode 0x83, name at 1 with NUL, p3 at 5, len at 7, p4 at 9, payload at 11.
void D2Game_8A8E0_PacketSend83(D2GSend send, void* client, const char* name, uint16_t len, uint32_t p2, uint16_t p3, uint16_t p4, const void* payload)
{
    (void)p2;
    size_t nl = strlen(name) + 1;
    std::vector<uint8_t> b((size_t)len + 11 > 1 + nl ? (size_t)len + 11 : 1 + nl);
    b[0] = 0x83;
    memcpy(b.data() + 1, name, nl);
    pk_w16(b.data(), 5, p3);
    pk_w16(b.data(), 7, len);
    pk_w16(b.data(), 9, p4);
    if (len) memcpy(b.data() + 11, payload, len);
    send(client, b.data(), (uint32_t)len + 11);
}

// 1.13c D2Game+0x8A960: opcode 0x7E, five bytes (bytes 1..4 unset in retail).
void D2Game_8A960_PacketSend7E(D2GSend send, void* client)
{
    uint8_t b[5] = { 0x7E, 0, 0, 0, 0 };
    send(client, b, 5);
}

// 1.13c D2Game+0x8AA40: opcode 0x65, seven bytes (bytes 1..6 unset in retail).
void D2Game_8AA40_PacketSend65(D2GSend send, void* client)
{
    uint8_t b[7] = { 0x65, 0, 0, 0, 0, 0, 0 };
    send(client, b, 7);
}

// 1.13c D2Game+0x8AB50 (p1 byte, p2, p4, p5, p6 words; p3 unused): opcode 0x57, p1 at 5, words at 6, 8, 10, 12. Fourteen bytes.
void D2Game_8AB50_PacketSend57(D2GSend send, void* client, uint32_t p1, uint16_t p2, uint32_t p3, uint16_t p4, uint16_t p5, uint16_t p6)
{
    (void)p3;
    uint8_t b[14] = { 0x57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    b[5] = (uint8_t)p1;
    pk_w16(b, 6, p2);
    pk_w16(b, 8, p4);
    pk_w16(b, 10, p5);
    pk_w16(b, 12, p6);
    send(client, b, 14);
}

// 1.13c D2Game+0x8ABA0 (p1 dword, p2 word): opcode 0x11, dword at 2, word at 6. Eight bytes (byte 1 unset in retail).
void D2Game_8ABA0_PacketSend11(D2GSend send, void* client, uint32_t p1, uint16_t p2)
{
    uint8_t b[8] = { 0x11, 0, 0, 0, 0, 0, 0, 0 };
    pk_w32(b, 2, p1);
    pk_w16(b, 6, p2);
    send(client, b, 8);
}

// 1.13c D2Game+0x8ABF0 (EAX = source record): copies the 15-byte record (dword, dword, dword, word, byte) and sends it.
void D2Game_8ABF0_PacketCopy15(D2GSend send, void* client, const uint8_t* rec)
{
    uint8_t b[15];
    memcpy(b, rec, 15);
    send(client, b, 15);
}

// 1.13c D2Game+0x8AC30 (EAX = source record): copies the 7-byte record (dword, word, byte) and sends it.
void D2Game_8AC30_PacketCopy7(D2GSend send, void* client, const uint8_t* rec)
{
    uint8_t b[7];
    memcpy(b, rec, 7);
    send(client, b, 7);
}

// 1.13c D2Game+0x8AC60 (EAX = source record): copies the 8-byte record (two dwords) and sends it.
void D2Game_8AC60_PacketCopy8(D2GSend send, void* client, const uint8_t* rec)
{
    uint8_t b[8];
    memcpy(b, rec, 8);
    send(client, b, 8);
}

// 1.13c D2Game+0x8AD80 (EAX = flag; zero selects the opcode 0x4D, nonzero 0x9A; p1 byte at 1, dword p2 at 2, dword p6 at 6,
// byte p8 at 10, words p4 at 11, p5 at 13, p7 at 15). Seventeen bytes. The decompile's ternary is the reverse of the spec's note.
void D2Game_8AD80_PacketSend4D9A(D2GSend send, void* client, uint32_t eax, uint32_t p1, uint32_t p2, uint32_t p3, uint16_t p4, uint16_t p5, uint32_t p6, uint16_t p7, uint8_t p8)
{
    (void)p3;
    uint8_t b[17] = {0};
    b[0] = eax == 0 ? 0x4D : 0x9A;
    b[1] = (uint8_t)p1;
    pk_w32(b, 2, p2);
    pk_w32(b, 6, p6);
    b[10] = p8;
    pk_w16(b, 11, p4);
    pk_w16(b, 13, p5);
    pk_w16(b, 15, p7);
    send(client, b, 17);
}

// 1.13c D2Game+0x8AF60 (EAX = object or null): opcode 0x40, id dword at 1 (object's +0xC, or 0xFFFFFFFF for null), p2 dword at 5,
// p4 dword at 9. Thirteen bytes.
void D2Game_8AF60_PacketSend40(D2GSend send, void* client, const uint8_t* obj, uint32_t p2, uint32_t p4)
{
    uint8_t b[13] = {0};
    uint32_t id = obj ? *(const uint32_t*)(obj + 0xC) : 0xFFFFFFFFu;
    b[0] = 0x40;
    pk_w32(b, 1, id);
    pk_w32(b, 5, p2);
    pk_w32(b, 9, p4);
    send(client, b, 13);
}

// 1.13c D2Game+0x8AFB0 (EAX = object or null; p2 byte, p3 flag): opcode 0x3F, byte 1 = p3 ? 0xFF : p2, words at 2 and 4 are the
// low and high halves of object+0xC (zero for null). Eight bytes (6..7 unset in retail).
void D2Game_8AFB0_PacketSend3F(D2GSend send, void* client, const uint8_t* obj, uint8_t p2, int p3)
{
    uint8_t b[8] = {0};
    b[0] = 0x3F;
    b[1] = p3 ? 0xFF : p2;
    if (obj) {
        uint32_t id = *(const uint32_t*)(obj + 0xC);
        pk_w16(b, 2, (uint16_t)id);
        pk_w16(b, 4, (uint16_t)(id >> 16));
    }
    send(client, b, 8);
}

// 1.13c D2Game+0x8B360: opcode 0x5C, five bytes (bytes 1..4 unset in retail).
void D2Game_8B360_PacketSend5C(D2GSend send, void* client)
{
    uint8_t b[5] = { 0x5C, 0, 0, 0, 0 };
    send(client, b, 5);
}

// 1.13c D2Game+0x8B540 (p2 byte, p4 byte, p5 flag, p6 byte): opcode 0x93, p4 at 5, p2 at 6, byte 7 = p6 when p5 is nonzero else 4.
// Eight bytes (bytes 1..4 unset in retail).
void D2Game_8B540_PacketSend93(D2GSend send, void* client, uint8_t p2, uint8_t p4, int p5, uint8_t p6)
{
    uint8_t b[8] = { 0x93, 0, 0, 0, 0, 0, 0, 0 };
    b[5] = p4;
    b[6] = p2;
    b[7] = p5 ? p6 : 4;
    send(client, b, 8);
}
