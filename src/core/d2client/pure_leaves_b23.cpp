// Pure D2Client leaves, batch b23. Structs by 1.13c byte offsets (32-bit pointer fields), so they work on wasm64 blobs.
#include <cstdint>
#include <cstring>

static uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

// 1.13c D2Client+0x3060: eax=link{node*,offs}. node!=0: target = offs<0 ? ~offs : link+(offs-node[1]); *target=node; node[1]=offs; link zeroed.
void D2Client_RelinkNode(uint8_t* link) {
    uint32_t node = rd32(link);
    if (!node) return;
    uint32_t offs = rd32(link + 4);
    // Subtract in retail uint32 arithmetic, then sign-extend the wrapped displacement.
    uintptr_t t = (int32_t)offs < 0 ? (uintptr_t)(uint32_t)~offs
                                    : (uintptr_t)link + (intptr_t)(int32_t)(uint32_t)(offs - rd32((const uint8_t*)(uintptr_t)node + 4));
    std::memcpy((void*)t, &node, 4);
    uint32_t n2 = rd32(link), o2 = rd32(link + 4);   // retail re-reads the link after the store (target may alias it)
    std::memcpy((uint8_t*)(uintptr_t)n2 + 4, &o2, 4);
    std::memset(link, 0, 8);
}

// 1.13c D2Client+0xB150: eax=ANSI codepage -> LANGID (932,936,949,950 -> 0x411,0x804,0x412,0x404), else 0.
uint32_t D2Client_CodepageLangId(int32_t cp) {
    switch (cp) { case 0x3a4: return 0x411; case 0x3a8: return 0x804; case 0x3b5: return 0x412; case 0x3b6: return 0x404; }
    return 0;
}

// 1.13c D2Client+0x4C0D0 / +0x4C110: ecx=unit. dwType(+0)==1 and pData(+0x14)!=0 -> bit 4 / bit 6 of byte pData+0x16, else 0.
static uint32_t pdataBit(const uint8_t* u, int bit) {
    if (!u || rd32(u) != 1) return 0;
    uint32_t p = rd32(u + 0x14);
    return p ? (((const uint8_t*)(uintptr_t)p)[0x16] >> bit) & 1 : 0;
}
uint32_t D2Client_PDataBit4(const uint8_t* u) { return pdataBit(u, 4); }
uint32_t D2Client_PDataBit6(const uint8_t* u) { return pdataBit(u, 6); }

// 1.13c D2Client+0x69850: edx=obj. obj && obj->pBuf(+0x54): zero bytes 0,2,..,38 of pBuf. ecx unused.
void D2Client_ClearEvenBytes(uint8_t* obj) {
    if (!obj) return;
    uint32_t b = rd32(obj + 0x54);
    if (!b) return;
    for (uint32_t i = 0; i < 0x28; i += 2) ((uint8_t*)(uintptr_t)b)[i] = 0;
}
