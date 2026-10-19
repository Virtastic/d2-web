// Prior art: D2MOO has no D2Client module; the cached 1.13c decompiles are the
// authority for these leaves and linked MSVC helpers. Native pointers stay native.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "../d2moo/include/Units/Units.h"

struct D2ClientGfxExtension {
    uint32_t reserved;
    int32_t preciseX, preciseY, preciseZ;
    uint32_t middle[9];
    uint32_t value34, value38, value3c;
};
static_assert(offsetof(D2ClientGfxExtension, preciseZ) == 0xc);
static_assert(offsetof(D2ClientGfxExtension, value34) == 0x34);
// The extension slot follows the 0x30-byte gfx header. Its pointer is native,
// rather than a truncated retail address. D2GfxDataStrc is otherwise opaque here.
struct D2ClientGfxExtensionHeader {
    uint32_t header[12];
    D2ClientGfxExtension* extension;
};
static_assert(offsetof(D2ClientGfxExtensionHeader, extension) == 0x30);
static const D2ClientGfxExtension* extension(const D2UnitStrc* unit) {
    if (!unit || !unit->pGfxData) return nullptr;
    D2ClientGfxExtension* result;
    std::memcpy(&result, reinterpret_cast<const uint8_t*>(unit->pGfxData) +
                offsetof(D2ClientGfxExtensionHeader, extension), sizeof(result));
    return result;
}
// 1.13c D2Client+0xC7F20
extern "C" uint32_t d2_client_c7f20(const D2UnitStrc* u) { auto p=extension(u); return p ? p->value3c : 0; }
// 1.13c D2Client+0xC7F40
extern "C" uint32_t d2_client_c7f40(const D2UnitStrc* u) { auto p=extension(u); return p ? p->value38 : 0; }
// 1.13c D2Client+0xC7F60
extern "C" uint32_t d2_client_c7f60(const D2UnitStrc* u) { auto p=extension(u); return p ? p->value34 : 0; }
// C++20 specifies arithmetic right shift for negative signed values.
// 1.13c D2Client+0xC7EC0
extern "C" int32_t d2_client_c7ec0(const D2UnitStrc* u) { auto p=extension(u); return p ? p->preciseZ >> 11 : 0; }
// 1.13c D2Client+0xC7EE0
extern "C" int32_t d2_client_c7ee0(const D2UnitStrc* u) { auto p=extension(u); return p ? p->preciseY >> 11 : 0; }
// 1.13c D2Client+0xC7F00
extern "C" int32_t d2_client_c7f00(const D2UnitStrc* u) { auto p=extension(u); return p ? p->preciseX >> 11 : 0; }
// 1.13c D2Client+0xCC0B8
extern "C" int32_t d2_client_cc0b8(const uint32_t* mantissa) {
    for (unsigned i=0; i<3; ++i) if (mantissa[i]) return 0;
    return 1;
}
// Forward word copies preserve retail's behaviour even for partially overlapping mantissas.
// 1.13c D2Client+0xCC09D
extern "C" void d2_client_cc09d(uint32_t* destination, const uint32_t* source) {
    for (unsigned i=0; i<3; ++i) destination[i]=source[i];
}
struct D2ClientPmd { int32_t member, vbptr, vbentry; };
static_assert(sizeof(D2ClientPmd)==12);
// 1.13c D2Client+0xCB7CD
extern "C" void* d2_client_cb7cd(void* object, const D2ClientPmd* pmd) {
    auto base=static_cast<uint8_t*>(object);
    auto result=base+pmd->member;
    if (pmd->vbptr>=0) {
        const uint8_t* table;
        std::memcpy(&table,base+pmd->vbptr,sizeof(table));
        int32_t displacement;
        std::memcpy(&displacement,table+pmd->vbentry,sizeof(displacement));
        result+=static_cast<ptrdiff_t>(displacement)+pmd->vbptr;
    }
    return result;
}
extern "C" uint32_t d2_client_6ff0(const char*);
extern "C" void* d2_client_31a0(void*,const void*,uint32_t);
// 1.13c D2Client+0xCB2A4
extern "C" void d2_client_cb2a4(char* text,int32_t distance) {
    if (distance) d2_client_31a0(text+distance,text,d2_client_6ff0(text)+1u);
}
struct D2ClientGetptdEnv;
extern "C" void* d2_client_72f7(const D2ClientGetptdEnv*);
// 1.13c D2Client+0xCB76D
// Retail calls getptd twice on the positive path. The counter at +0x84 is
// a signed word (Ghidra's CRT type misleadingly calls this slot _purecall).
extern "C" void d2_client_cb76d(const D2ClientGetptdEnv* env) {
    auto p=static_cast<uint8_t*>(d2_client_72f7(env));
    int32_t counter;
    std::memcpy(&counter,p+0x84,4);
    if (counter>0) {
        p=static_cast<uint8_t*>(d2_client_72f7(env));
        uint32_t value;
        std::memcpy(&value,p+0x84,4);
        --value;
        std::memcpy(p+0x84,&value,4);
    }
}
