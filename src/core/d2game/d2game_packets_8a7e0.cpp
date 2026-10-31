// 1.13c D2Game packet senders not covered by d2game_packets_8a810.cpp: 0x8A7E0, 0x8AB20, 0x8ACD0, 0x8BA90, 0x8BEB0,
// 0x8BEE0, 0x8BF20, 0x8C1B0, 0x8C340, 0x8C3E0, 0x8C4F0, 0x8C630, 0x8D010. Same conventions as that file: the client
// (retail: EAX or the stack) is an explicit argument, register inputs are named by their register, bytes the
// retail frame never writes go out as zero.
#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

static inline void q16(uint8_t* b, uint32_t o, uint16_t v) { memcpy(b + o, &v, 2); }
static inline void q32(uint8_t* b, uint32_t o, uint32_t v) { memcpy(b + o, &v, 4); }

// 1.13c D2Game+0x8A7E0: opcode 0x8B; bytes 1..4 = EAX, byte 5 = CL (the decompile shows only the opcode; the spec's
// disassembly check has the register stores). Six bytes.
void D2Game_8A7E0_PacketSend8B(D2GSend send, void* client, uint32_t eax, uint8_t cl)
{
    uint8_t b[6] = { 0x8B };
    q32(b, 1, eax);
    b[5] = cl;
    send(client, b, 6);
}

// 1.13c D2Game+0x8AB20: copy a 7-byte record (dword, word at +4, byte at +6) and send it, length 7.
void D2Game_8AB20_PacketSendRecord7(D2GSend send, void* client, const uint8_t* rec)
{
    uint8_t b[7];
    memcpy(b, rec, 7);
    send(client, b, 7);
}

// 1.13c D2Game+0x8ACD0: copy a 6-byte record (dword + word at +4) and send it, length 6.
void D2Game_8ACD0_PacketSendRecord6(D2GSend send, void* client, const uint8_t* rec)
{
    uint8_t b[6];
    memcpy(b, rec, 6);
    send(client, b, 6);
}

// 1.13c D2Game+0x8BA90: opcode 0xA3; byte 1 = a1, word 2 = a2, (a3 dropped), word 4 = a4, byte 6 = a5, dword 7 = a6,
// byte 11 = a7, dword 12 = a8, dword 16 = a9, dword 20 = a10. Length 0x18.
void D2Game_8BA90_PacketSendA3(D2GSend send, void* client, uint8_t a1, uint16_t a2, uint32_t a3, uint16_t a4, uint8_t a5,
                               uint32_t a6, uint8_t a7, uint32_t a8, uint32_t a9, uint32_t a10)
{
    (void)a3;
    uint8_t b[0x18] = { 0xA3 };
    b[1] = a1;
    q16(b, 2, a2);
    q16(b, 4, a4);
    b[6] = a5;
    q32(b, 7, a6);
    b[11] = a7;
    q32(b, 12, a8);
    q32(b, 16, a9);
    q32(b, 20, a10);
    send(client, b, 0x18);
}

// 1.13c D2Game+0x8BEB0: nothing is sent when al == 3. Otherwise byte 0 = cl, byte 1 = al, bytes 2..5 = edx; length 6.
void D2Game_8BEB0_PacketSendIfNot3(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx)
{
    if (al == 3) return;
    uint8_t b[6];
    b[0] = cl;
    b[1] = al;
    q32(b, 2, edx);
    send(client, b, 6);
}

// 1.13c D2Game+0x8BEE0: byte 0 = al, 1 = cl, 2..5 = edx, 6 = s1 (byte), 7..8 = s2, 9..10 = s3. Length 0xB.
void D2Game_8BEE0_PacketSendB(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx, uint8_t s1, uint16_t s2, uint16_t s3)
{
    uint8_t b[11];
    b[0] = al;
    b[1] = cl;
    q32(b, 2, edx);
    b[6] = s1;
    q16(b, 7, s2);
    q16(b, 9, s3);
    send(client, b, 11);
}

// 1.13c D2Game+0x8BF20: opcode 0x08; words at 1 (ax) and 3 (cx), byte 5 = stack byte. Length 6.
void D2Game_8BF20_PacketSend08(D2GSend send, void* client, uint16_t ax, uint16_t cx, uint8_t s1)
{
    uint8_t b[6] = { 0x08 };
    q16(b, 1, ax);
    q16(b, 3, cx);
    b[5] = s1;
    send(client, b, 6);
}

// 1.13c D2Game+0x8C340: byte 0 = al, 1 = cl, 2..5 = edx, 6/7/8 = three stack bytes. Length 9.
void D2Game_8C340_PacketSend9(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx, uint8_t s1, uint8_t s2, uint8_t s3)
{
    uint8_t b[9];
    b[0] = al;
    b[1] = cl;
    q32(b, 2, edx);
    b[6] = s1;
    b[7] = s2;
    b[8] = s3;
    send(client, b, 9);
}

// 1.13c D2Game+0x8C3E0: byte 0 = al, 1 = cl, 2..5 = edx, word at 6, dword at 8. Length 0xC.
void D2Game_8C3E0_PacketSendC(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx, uint16_t w6, uint32_t d8)
{
    uint8_t b[12];
    b[0] = al;
    b[1] = cl;
    q32(b, 2, edx);
    q16(b, 6, w6);
    q32(b, 8, d8);
    send(client, b, 12);
}

// 1.13c D2Game+0x8C1B0: sixteen-byte packet, byte 0 = cl, 1 = al, 2..5 = edx, 6 = s1, 7..8 = s2, 9..10 = s3, 11 = s4,
// 12..13 = s5, 14..15 = s6. After the buffer is filled and before the send, al >= 6 is fatal: error 0x166 is logged
// through `fatal` (retail: 0xA2DA/0xA2C2 and 0x2A3C(-1)) and the packet is not sent.
struct D2Game8C1B0Env { void (*fatal)(uint32_t code); };
void D2Game_8C1B0_PacketSendUnitState(const D2Game8C1B0Env* e, D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx,
                                      uint8_t s1, uint16_t s2, uint16_t s3, uint8_t s4, uint16_t s5, uint16_t s6)
{
    uint8_t b[16];
    b[0] = cl;
    b[1] = al;
    q32(b, 2, edx);
    b[6] = s1;
    q16(b, 7, s2);
    q16(b, 9, s3);
    b[11] = s4;
    q16(b, 12, s5);
    q16(b, 14, s6);
    if (al >= 6) {
        e->fatal(0x166);
        std::exit(-1);
    }
    send(client, b, 16);
}

// 1.13c D2Game+0x8C4F0: 53-byte packet, byte 0 = 0xB2, bytes 1..16 = a 16-byte copy of `src` (retail: ordinal 0xA280 copy
// helper), words at 49 (cx) and 51 (ax). Delivered through `deliver` (retail: ordinal 0xA35E with (0, stackArg, buf, 0x35)).
typedef void (*D2GDeliverB2)(uint32_t zero, uint32_t stackArg, const void* buf, uint32_t len);
void D2Game_8C4F0_PacketSendB2(D2GDeliverB2 deliver, uint32_t stackArg, const uint8_t* src, uint16_t cx, uint16_t ax)
{
    uint8_t b[0x35] = { 0xB2 };
    memcpy(b + 1, src, 16);
    q16(b, 49, cx);
    q16(b, 51, ax);
    deliver(0, stackArg, b, 0x35);
}

// 1.13c D2Game+0x8C630: opcode 0x8C, dword 1 = ecx, dword 5 = edx, word 9 = stack word. Length 11. The target is
// `target`, unless `unit` is non-null with *unit == 0, when it is `resolveClient(unit)` (retail: ordinal 10920's result + 0x9C).
void D2Game_8C630_PacketSend8CForUnit(D2GSend send, void* (*resolveClient)(const uint32_t* unit), void* target, const uint32_t* unit,
                                      uint32_t ecx, uint32_t edx, uint16_t w)
{
    uint8_t b[11] = { 0x8C };
    q32(b, 1, ecx);
    q32(b, 5, edx);
    q16(b, 9, w);
    if (unit && *unit == 0) target = resolveClient(unit);
    send(target, b, 11);
}

// 1.13c D2Game+0x8D010: opcode 0x8C, dword 1 = eax, dword 5 = ecx, word 9 = dx. Length 11, handed to 0xDFC10
// (`dispatch`, EBX = the per-client send callback at 0x6FCAC690).
void D2Game_8D010_PacketSend8CVia(void (*dispatch)(const void* buf), uint32_t eax, uint32_t ecx, uint16_t dx)
{
    uint8_t b[11] = { 0x8C };
    q32(b, 1, eax);
    q32(b, 5, ecx);
    q16(b, 9, dx);
    dispatch(b);
}
