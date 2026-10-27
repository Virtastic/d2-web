// Prior art used: D2MOO D2WinFont/D2WinButton/D2WinEditBox (UI type contracts),
// d2win_popup.cpp (the existing popup state), and the sibling CRT lock wrappers.
// Logic from cached 1.13c decompiles; register-only arguments checked in retail x86.
// Dependencies below are mandatory, typed calls to DIFFERENT retail callees. No
// default implementation substitutes for the locale, renderer, or UI routines.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <emscripten.h>

struct D2WinSmallEnv {
    void* (*getThreadLocale)();                 // __getptd()->locale
    void** globalLocale;                       // PTR_DAT_6f8ff1cc
    void* (*updateThreadLocale)();             // 0x2A1F
    int32_t (*tolowerMt)(void*, int32_t);       // 0x274D
    int32_t (*wctombMt)(void*, char*, uint16_t);// 0x3FF4
    uint8_t* const* fileBlocks;                // DAT_6f9abb40, native block pointers
    void (*leaveCriticalSection)(void*);
    void (*writeChar)(uint8_t, void*, int32_t*);// 0x1AAE: AL, ECX, ESI
    uint32_t (*errorText)(uint32_t);            // Fog #10265
    void (*reportError)(const char*, uint32_t); // Fog #10024
    void (*setPalette)(void*, uint32_t, uint8_t, uint8_t, uint8_t); // Gfx #10049
    void* palette;                             // DAT_6f922338
    void (*setActive)(uint32_t);                // Gfx #10029
    void (*initialize)(void*, void*);           // 0xE0D0: ESI control, EAX source
    uint32_t (*removeControl)(void*);           // 0x17FC0
};
static uint32_t read32(const void* p, uint32_t off) {
    uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p)+off,4); return v;
}
static void write32(void* p,uint32_t off,uint32_t v) {
    std::memcpy(static_cast<uint8_t*>(p)+off,&v,4);
}
static void* locale(const D2WinSmallEnv* e) {
    void* p=e->getThreadLocale();
    return p==*e->globalLocale ? p : e->updateThreadLocale();
}
[[noreturn]] static void fail(const D2WinSmallEnv* e,uint32_t line) {
    e->reportError("",e->errorText(line)); std::_Exit(-1);
}
extern "C" void d2win_SetPopUpUnicodeText(const uint16_t*,uint32_t,uint32_t,uint32_t,uint32_t);
extern "C" void d2win_popup_set_tail(uint32_t);

// 1.13c D2Win+0x12730 -- popup call completes before the extra module word is changed.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_12730(const uint16_t* text,uint32_t a,
    uint32_t b,uint32_t c,uint32_t d,uint32_t extra) {
    d2win_SetPopUpUnicodeText(text,a,b,c,d); d2win_popup_set_tail(extra);
}
// 1.13c D2Win+0x196B0 -- unsigned addition with carry, result written even on overflow.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_addl(uint32_t a,uint32_t b,uint32_t* out) {
    const uint32_t sum=a+b; *out=sum; return sum<a || sum<b;
}
// 1.13c D2Win+0x2815 -- synchronize per-thread locale before case conversion.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_tolower(int32_t ch,const D2WinSmallEnv* e) {
    return e->tolowerMt(locale(e),ch);
}
// 1.13c D2Win+0x069DC -- descriptor blocks contain 32 records of 0x24 bytes.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_69DC(int32_t fd,const D2WinSmallEnv* e) {
    e->leaveCriticalSection(e->fileBlocks[fd>>5]+0xC+(uint32_t(fd)&31)*0x24);
}
// 1.13c D2Win+0x1AE1 -- EAX=count pointer, stack=(character, repeats, stream).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_write_multi_char(uint8_t ch,int32_t repeats,
    void* stream,int32_t* count,const D2WinSmallEnv* e) {
    while(repeats>0) { --repeats; e->writeChar(ch,stream,count); if(*count==-1) break; }
}
// 1.13c D2Win+0xBAD0 -- assertion helper, tested value arrives in EAX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_BAD0(uint32_t value,const D2WinSmallEnv* e) {
    if(value==0) fail(e,0x3D2);
}
// 1.13c D2Win+0xED70 -- tint the complete 256-entry module palette.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_ED70(uint8_t r,uint8_t g,uint8_t b,const D2WinSmallEnv* e) {
    e->setPalette(e->palette,0x100,r,g,b);
}
// 1.13c D2Win+0x17020 -- deactivate type 2, then clear the two activity words.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_17020(void* p,const D2WinSmallEnv* e) {
    if(read32(p,8)==2) e->setActive(0);
    write32(p,0x18,0); write32(p,0x1C,0);
}
// 1.13c D2Win+0x0e5c0 -- incoming EAX is saved at +34; stack source becomes
// EAX for initialization. The byte store intentionally overlaps the word at +4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_E5C0(uint32_t ecx,uint32_t edx,void* p,
    void* source,uint8_t kind,uint32_t eax,const D2WinSmallEnv* e) {
    write32(p,0x34,eax); static_cast<uint8_t*>(p)[5]=kind;
    write32(p,0x40,ecx); write32(p,0,edx); e->initialize(p,source);
    write32(p,0x44,read32(p,0x44)|2);
}
// 1.13c D2Win+0x10900 -- type-checked access to the control's +50 word.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_10900(const void* p,const D2WinSmallEnv* e) {
    if(read32(p,0)!=2) fail(e,0x50);
    return read32(p,0x50);
}
// 1.13c D2Win+0xF8A0 -- type-check the button and preserve 0x17FC0's EAX result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_F8A0(void* p,const D2WinSmallEnv* e) {
    if(read32(p,0)!=6) fail(e,0x59);
    return e->removeControl(p);
}
// 1.13c D2Win+0x4054 -- wchar is explicitly the retail 16-bit type.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_wctomb(char* out,uint16_t ch,const D2WinSmallEnv* e) {
    return e->wctombMt(locale(e),out,ch);
}
// 1.13c D2Win+0x159E0 -- null -> line 0x6E, non-type-1 -> line 0x70 (fatal); else 0x17FC0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_159E0(void* p,const D2WinSmallEnv* e) {
    if(!p) fail(e,0x6E);
    if(read32(p,0)!=1) fail(e,0x70);
    return e->removeControl(p);
}
// FN probe glue (checked against retail): hooked 0x17FC0 logs (rva, ecx=token) into lg and returns ret.
static uint32_t* fn159Lg; static uint32_t fn159Ret, fn159Tok;
static uint32_t fn159Remove(void*) {
    uint32_t* w=fn159Lg+1+fn159Lg[0]++*10;
    w[0]=0x17FC0; w[1]=0; w[2]=fn159Tok; for(int i=3;i<10;++i) w[i]=0;
    return fn159Ret;
}
extern "C" uint32_t d2win_sub_159E0_fn(void* p,uint32_t* lg,uint32_t tok,uint32_t ret) {
    fn159Lg=lg; lg[0]=0; fn159Tok=tok; fn159Ret=ret;
    D2WinSmallEnv e={}; e.removeControl=fn159Remove;
    return d2win_sub_159E0(p,&e);
}
