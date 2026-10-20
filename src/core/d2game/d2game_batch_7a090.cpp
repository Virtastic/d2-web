// Alpha batch D2Game+0x7A090, 0x7A440 (SUnitMsg queue append). From the cached 1.13c decompile; no test runner run.
// GUESS = arg Ghidra dropped (register-passed). Offsets are the 1.13c 32-bit layout, as decompiled.
#include <cstdint>
#include <cstring>

namespace ordns_7a090 { __attribute__((weak, import_module("env"), import_name("Ordinal_10045"))) void* Ordinal_10045(const char*, int, int); } using namespace ordns_7a090;
namespace ordns_7a090 { __attribute__((weak, import_module("env"), import_name("Ordinal_10158"))) void Ordinal_10158(void*); } using namespace ordns_7a090;

static void* msgAlloc(int line) { return Ordinal_10045("..\\Source\\D2Game\\UNIT\\SUnitMsg.cpp", line, 0); }

// Link node at client+0xEC (head) / +0xF0 (tail), then notify (Ordinal_10158).
static void msgQueue(uint8_t* client, uint8_t* n)
{
    void** head = (void**)(client + 0xec);
    void** tail = (void**)(client + 0xf0);
    if (!*head) *head = n;
    if (*tail) **(void***)tail = n;
    *tail = n;
    Ordinal_10158(client); // GUESS: arg is the client (ESI)
}

// 1.13c D2Game+0x7A090
extern "C" void D2Game_7A090(uint8_t* client) // GUESS: client = unaff_ESI
{
    uint8_t* n = (uint8_t*)msgAlloc(799);
    *(uint32_t*)n = 0;
    n[4] = 0xa4;
    *(uint16_t*)(n + 8) = 799; // GUESS: uVar2 is the line number, as decompiled
    msgQueue(client, n);
}

// 1.13c D2Game+0x7A440
extern "C" void D2Game_7A440(uint8_t* client, uint16_t retaddrWord, uint16_t p1, uint8_t p2) // GUESS: client=ESI, retaddrWord=unaff_retaddr
{
    uint8_t* n = (uint8_t*)msgAlloc(0x298); // GUESS: line arg 0x298 matches the stored word
    *(uint16_t*)(n + 8) = 0x298;
    n[10] = 0;
    *(uint16_t*)(n + 12) = retaddrWord;
    n[4] = 0x9a;
    *(uint32_t*)n = 0;
    *(uint16_t*)(n + 14) = p1;
    n[16] = p2;
    msgQueue(client, n);
}
