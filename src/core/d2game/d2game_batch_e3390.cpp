// Stage A leaves checked against cached 1.13c decompiles and retail bytes.
// Prior art: VC7.1 CRT mantissa/string/PMD conventions. Not a parity verdict.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
// Three sequential dword transfers, including forward-overlap behaviour.
// 1.13c D2Game+0xF66D3
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f66d3(void* dst,const void* src) {
    for (unsigned i=0;i<3;++i) {
        uint32_t word;
        std::memcpy(&word,static_cast<const uint8_t*>(src)+i*4,4);
        std::memcpy(static_cast<uint8_t*>(dst)+i*4,&word,4);
    }
}
// 1.13c D2Game+0xF66EE
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_f66ee(const void* src) {
    for (unsigned i=0;i<3;++i) {
        uint32_t word; std::memcpy(&word,static_cast<const uint8_t*>(src)+i*4,4);
        if(word) return 0;
    }
    return 1;
}
// Register inputs EAX=string, EDI=signed displacement; include the terminator.
// 1.13c D2Game+0xF617B
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f617b(char* str,int32_t displacement) {
    if(displacement) std::memmove(str+displacement,str,std::strlen(str)+1);
}
// PMD uses three signed retail dwords. The object's vbptr is a native pointer
// in the relocated host image, whereas the table entries remain signed dwords.
struct D2GamePMD { int32_t member, vbptr, vbentry; };
static_assert(sizeof(D2GamePMD)==12);
// 1.13c D2Game+0xF5919
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_f5919(void* object,const D2GamePMD* pmd) {
    auto* base=static_cast<uint8_t*>(object);
    auto* result=base+pmd->member;
    if(pmd->vbptr>=0) {
        const uint8_t* table;
        std::memcpy(&table,base+pmd->vbptr,sizeof table);
        int32_t adjustment; std::memcpy(&adjustment,table+pmd->vbentry,4);
        result+=adjustment+pmd->vbptr;
    }
    return result;
}
