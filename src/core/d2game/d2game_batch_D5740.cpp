// Stage A, cached 1.13c decompiles; prior art: D2MOO PlrSave.cpp
// sub_6FC8AF70/sub_6FC8B3D0/sub_6FC8BEE0/sub_6FC8C050 and ItemMode.cpp
// sub_6FC43280/sub_6FC49F80. Retail ordinals below are D2Common unless >= 0x10000
// (D2Game RVA). Calls preserve the recovered order; register arguments become explicit.
// Pointers live in a host relocation map supplied by Env, never in retail 32-bit slots.
// These are callable Stage A implementations, not a parity verdict or client integration.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <initializer_list>
#include <array>

namespace d2game_batch_d5740 {
using Word = uintptr_t;
struct Env {
    Word (*call)(uint32_t, std::initializer_list<Word>);
    void* (*ptr)(const void*, uint32_t);
    void (*setptr)(void*, uint32_t, void*);
};
static uint16_t r16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v,static_cast<const uint8_t*>(p)+o,2); return v; }
static uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4); return v; }
static void w16(void* p,uint32_t o,uint16_t v) { std::memcpy(static_cast<uint8_t*>(p)+o,&v,2); }
static void w32(void* p,uint32_t o,uint32_t v) { std::memcpy(static_cast<uint8_t*>(p)+o,&v,4); }
static Word a(const void* p) { return reinterpret_cast<Word>(p); }
[[noreturn]] static void fatal(Env& e,uint32_t line) { auto text=e.call(10265,{line}); e.call(10024,{text}); std::abort(); }

// 1.13c D2Game+0xD1950
int d2game_D1950(const int32_t* x,const int32_t* y) {
    if ((x[0]&&y[2]) || (x[2]&&y[0]) || (x[1]&&y[3]) || (x[3]&&y[1])) return 1;
    const bool oneHandX=x[5]&&!x[6], oneHandY=y[5]&&!y[6];
    return (oneHandX&&y[4]) || (oneHandY&&x[4]) ||
           (oneHandX&&oneHandY&&x[7]&&y[7]) || (x[9]&&y[9]);
}

// 1.13c D2Game+0xD1A30
int d2game_D1A30(Env& e,void* item) { auto quality=e.call(10787,{a(item)}); return quality!=2&&quality!=7; }

// 1.13c D2Game+0x52A20
int32_t d2game_52A20(int16_t id,int32_t level) {
    switch(id) {
    case 0:case 1:return level<25?100:level<49?200:400;
    case 2:case 3:return level<49?220:440;
    case 4:case 5:case 12:case 13:case 14:case 21:case 23:return 480;
    case 6:case 7:case 8:return level<31?105:level<55?210:420;
    case 9:case 10:case 11:return level<55?230:460;
    case 15:case 17:return level<37?110:level<61?220:440;
    case 16:return level<37?115:level<61?230:260;
    case 18:case 20:return level<61?230:460;
    case 19:return level<61?240:480;
    case 22:return 500;
    case 24:case 25:return level<42?220:level<75?440:880;
    case 26:case 27:return level<75?460:920;
    default:return 960;
    }
}

// 1.13c D2Game+0x532C0
uint32_t d2game_532C0(Env& e,void* game,int16_t id,uint32_t experience) {
    int32_t level=1;
    // x86's products wrap at 32 bits; this is the saved hireling's inferred level.
    for(int32_t next=2;next<=99;++next) {
        const uint32_t threshold=uint32_t(d2game_52A20(id,next))*uint32_t(next)*uint32_t(next+1)*3u;
        if(threshold>experience) break;
        ++level;
    }
    void* row=reinterpret_cast<void*>(e.call(11156,{r32(game,0x70),uint16_t(id),Word(level)}));
    if(!row)return experience;
    const uint32_t minimum=uint32_t(e.call(10448,{Word(level),r32(row,0x20)}));
    return minimum>experience?minimum:experience;
}

// Exact packed 72-byte saved-item record. Unaligned fields are accessed with memcpy.
struct Saved { uint8_t bytes[72]{}; };
static_assert(sizeof(Saved)==72);
struct Bits {
    const uint8_t* data; uint32_t position=0;
    uint32_t read(unsigned n) { uint32_t v=0; for(unsigned i=0;i<n;++i,++position) v|=uint32_t((data[position/8]>>(position%8))&1)<<i; return v; }
    int32_t signedRead(unsigned n) { uint32_t v=read(n); if(n<32&&(v&(1u<<(n-1)))) v|=~((1u<<n)-1); int32_t s;std::memcpy(&s,&v,4);return s; }
};
// 1.13c D2Game+0x52BD0
uint32_t d2game_52BD0(Env& e,const uint8_t* data,Saved& out) {
    out={}; Bits b{data}; auto* s=out.bytes;
    const uint32_t flags=b.read(32); w32(s,0x24,flags);
    const bool modern=(flags&0x100000)!=0,compact=(flags&0x200000)!=0,ear=(flags&0x10000)!=0;
    if(modern) w16(s,0x42,uint8_t(b.read(10)));
    if(modern&&compact) {
        s[0]=uint8_t(b.read(5)); w32(s,2,1); s[10]=uint8_t(b.read(3));
        w32(s,24,uint8_t(b.read(5))); w32(s,28,uint8_t(b.read(3))); s[48]=uint8_t(b.read(8));
        if(ear) { w32(s,68,0x20726165); w32(s,32,b.read(3));s[49]=uint8_t(b.read(8));for(unsigned i=0;i<16;++i)s[50+i]=uint8_t(b.read(7)); }
        else w32(s,68,b.read(32));
    } else if(!modern&&ear) {
        s[0]=uint8_t(b.read(5));w32(s,2,1);b.signedRead(10);
        w32(s,6,uint32_t(e.call(10814,{0x20726165})));
        s[10]=uint8_t(b.read(3));w32(s,24,uint8_t(b.read(5)));w32(s,28,uint8_t(b.read(3)));
        w32(s,32,uint32_t(b.signedRead(10)));s[48]=uint8_t(b.read(8));s[49]=uint8_t(b.read(8));
        for(unsigned i=0;i<16;++i) { s[50+i]=uint8_t(b.read(7));if(!s[50+i])break; } w32(s,68,0x20726165);
        return 25;
    } else {
        if(modern) w32(s,68,b.read(32));
        s[0]=uint8_t(b.read(5));s[1]=uint8_t(b.read(3));w32(s,2,uint8_t(b.read(12)));
        int32_t oldId=-1;if(!modern)oldId=b.signedRead(10);
        s[10]=uint8_t(b.read(3));s[11]=uint8_t(b.read(4));w32(s,12,uint32_t(b.signedRead(20)));
        w32(s,16,uint8_t(b.read(8)));w32(s,20,uint8_t(b.read(8)));w32(s,24,uint8_t(b.read(5)));w32(s,28,uint8_t(b.read(3)));
        w32(s,32,uint32_t(b.signedRead(10)));w32(s,40,b.read(32));w32(s,44,b.read(32));s[48]=uint8_t(b.read(8));
        if(!modern) {
            void* row=nullptr; uint32_t id=0xffffffff;
            if(oldId>=0) { id=uint32_t(e.call(10442,{uint32_t(oldId)}));row=reinterpret_cast<void*>(e.call(10695,{id})); }
            w32(s,6,row?id:0xffffffff);w32(s,68,row?r32(row,0x80):0);return 25;
        }
    }
    w32(s,6,uint32_t(e.call(10814,{r32(s,68)})));
    return (b.position+7)/8;
}

// 1.13c D2Game+0x54D90
int d2game_54D90(Env& e,const uint8_t* section,void* game,void* player,uint32_t version,int corpse,int32_t* consumed) {
    (void)version;
    if(r16(section,0)!=0x4d4a)return 14;
    uint32_t offset=4;
    // The retail duplicate table count starts at zero and its only increment is
    // inside the count>0 loop (confirmed in disassembly). Do not invent insertion:
    // this build never enters its duplicate-skip path.
    for(unsigned n=0;n<r16(section,2);++n) {
        Saved item;
        auto read=[&]() { if(r16(section,offset)!=0x4d4a)return false;offset+=2;offset+=d2game_52BD0(e,section+offset,item);return true; };
        if(!read())return 14;
        void* parent=nullptr;int32_t sockets=0;
        const uint32_t callee=corpse?0x54a20:0x54bc0;
        int result=int(e.call(callee,{a(game),a(player),a(&item),a(&parent),a(&sockets)}));
        if(corpse&&result)return result;
        if(!corpse&&result)continue;
        for(int32_t i=0;i<sockets;++i) { if(!read())return 14;int32_t unused=0;result=int(e.call(callee,{a(game),a(player),a(&item),a(&parent),a(&unused)}));if(result)return result; }
    }
    *consumed=int32_t(offset);return 0;
}

// 1.13c D2Game+0xD5740
void d2game_D5740(Env& e,void* unit,void* game,void* item) {
    void* inventory=e.ptr(unit,0x60);
    e.call(10977,{a(item),0x100,1});e.call(10694,{a(e.ptr(unit,0x60)),a(item)});
    if(e.call(10175,{a(item)})==1)e.call(0xd46b0,{a(game),a(item),a(unit),1,1});
    e.call(0xd51a0,{a(game),a(unit),0});e.call(10050,{a(inventory),a(item)});e.call(10201,{a(unit),1});
    e.call(10887,{a(item),72,0,0});
    void* client=nullptr;if(r32(unit,0)==0) {void* data=reinterpret_cast<void*>(e.call(10920,{a(unit)}));client=e.ptr(data,0x9c);}
    e.call(0x8b010,{a(client),a(item),1,72,0,0});e.call(10600,{a(unit),a(item),1,1});
    w16(unit,0x6e,9);e.setptr(unit,0x70,unit);e.call(10158,{a(unit)});w32(unit,0xc4,r32(unit,0xc4)|0x400);
}

// 1.13c D2Game+0xD57F0
int d2game_D57F0(Env& e,void* game,void* player,uint32_t id,uint32_t bodyLocation) {
    if(!game)fatal(e,0x14f7);
    if(!player)fatal(e,0x14f8);
    e.call(0xd29f0,{a(player)});
    void* item=e.ptr(game,0x1720+(id&0x7f)*4);
    while(item&&r32(item,12)!=id)item=e.ptr(item,0xe4);
    if(!item||r32(item,0)!=4||r32(item,16)!=0||!e.call(10244,{a(item),a(player),0,0,0,0}))return 0;
    void* inv=e.ptr(player,0x60);void* removed=reinterpret_cast<void*>(e.call(10646,{a(inv),a(item)}));
    if(!removed)fatal(e,0x1513);
    if(removed!=item)fatal(e,0x1514);
    const uint32_t layout=uint32_t(e.call(10584,{a(player),0,r32(game,0x70)}));
    const uint32_t body=bodyLocation&255;
    auto y=e.call(0x1130,{a(item)}),x=e.call(0x1100,{a(item)});
    e.call(10869,{0x1516,a(inv),x,y,layout,0,0});e.call(10012,{a(item),255});
    if(!e.call(11041,{a(inv),a(item),body}))fatal(e,0x1538);
    if(!e.call(10405,{a(inv),a(item),3})) {e.call(10608,{a(inv),body});fatal(e,0x1535);}
    e.call(10058,{a(item),bodyLocation});e.call(10138,{a(inv),a(item)});e.call(10466,{a(inv),0});
    if(e.call(10744,{a(item),13}))fatal(e,0x151e);
    e.call(0xd47c0,{a(game),a(item),a(player),0});w32(item,0xc4,r32(item,0xc4)&~2u);
    e.call(11090,{a(item),1});e.call(10012,{a(item),255});e.call(10892,{a(item),0x8000,1});
    if(e.call(0xd1bc0,{a(item)}))e.call(10977,{a(item),1,1});
    if(e.call(10202,{a(item),0x4000,0x152b}))e.call(10977,{a(item),0x4000,0});
    e.call(10050,{a(inv),a(item)});e.call(0xd28d0,{a(player)});e.call(10201,{a(player),1});e.call(0xd51a0,{a(game),a(player),0});return 1;
}

// 1.13c D2Game+0x550A0
int d2game_550A0(Env& e,void* game,void* player,const uint8_t* data,uint32_t version,int32_t remaining,int32_t* consumed) {
    *consumed=0;data+=12;remaining-=12;
    if(!player||r32(player,4)>=7)return 8;
    void* body=reinterpret_cast<void*>(e.call(11153,{r32(game,0x1c),0}));if(!body)return 8;
    w32(body,0,0);w32(body,4,r32(player,4));w32(body,8,r32(game,0x1c));e.setptr(body,0x80,game);
    uint32_t flags=r32(body,0xc8);flags=r32(game,0x70)?flags|0x2000000:flags&~0x2000000u;w32(body,0xc8,flags|0x4000000);
    auto level=e.call(10826,{0});const uint8_t act=uint8_t(e.call(10459,{level}));static_cast<uint8_t*>(body)[0x18]=act;
    e.setptr(body,0x1c,e.ptr(game,0xbc+uint32_t(act)*4));w32(body,0x74,0);w32(body,0xc4,r32(body,0xc4)|0x10);w32(body,0xd0,11);
    uint32_t guid=r32(game,0x90)+1;if(guid==0xffffffff)guid=1;w32(game,0x90,guid);
    e.call(0x38620,{a(game),a(body),guid});e.call(0xe0f40,{0,a(body),0,0,a(game),1});
    if(r32(body,0x10)==17||r32(body,0x10)==0) {e.call(10775,{a(body),1});e.call(10671,{a(body),5});e.call(11056,{a(e.ptr(body,0x2c)),0x8000});}
    w32(body,0xc4,r32(body,0xc4)&~4u);
    void* client=nullptr;
    if(r32(player,0)==0) {void* pd=reinterpret_cast<void*>(e.call(10920,{a(player)}));client=e.ptr(pd,0x9c);}
    if(r32(body,0)!=0)fatal(e,0x4bc);
    void* bd=reinterpret_cast<void*>(e.call(10920,{a(body)}));e.setptr(bd,0x9c,client);if(!client)fatal(e,0x9d9);
    e.call(10140,{a(body),a(static_cast<uint8_t*>(client)+13)});e.call(0xe0110,{a(body),a(static_cast<uint8_t*>(game)+0xd0)});
    e.call(10945,{a(body),7,1});w32(body,0xc4,r32(body,0xc4)&~0x10u);
    e.call(10268,{a(e.ptr(body,0x60)),r32(player,12)});
    int32_t read=0;int result;
    if(version==71)result=d2game_54D90(e,data,game,body,version,1,&read);
    else if(version>=72&&version<=96)result=int(e.call(0x54810,{a(game),a(body),a(data),version,Word(remaining),1,a(&read)}));
    else return 1;
    if(result)return result;
    e.call(10990,{a(e.ptr(player,0x60)),r32(body,12),0,1});
    uint8_t packet[10]={0x8e,1};w32(packet,2,r32(player,12));w32(packet,6,r32(body,12));
    e.call(0xdfc10,{a(game),0x8c530,a(packet)});*consumed=read+12;return 0;
}

// The CRT division workaround uses x87 binary80, including caller control/status.
// Arithmetic primitives are supplied by the host's x87 emulator: no conversion to
// C++ long double (binary128 on wasm) or loss of precision/control-word state.
struct F80 { uint64_t significand;uint16_t signExponent; };
struct X87 {
    uint16_t control;
    F80 (*multiplyFloat32)(F80,uint32_t,uint16_t);
    F80 (*divide)(F80 numerator,F80 denominator,uint16_t control);
};
// 1.13c D2Game+0xF4940
F80 d2game_F4940(X87& x,F80 denominator,F80 numerator) {
    F80 classifier=denominator;
    const uint16_t temporary=uint16_t((x.control|0x033f)&0xf3ff);
    for(;;) {
        const uint32_t hi=uint32_t(classifier.significand>>32);
        const uint32_t exponent=classifier.signExponent&0x7fff;
        if(hi&0x80000000u) {
            const uint32_t probe=(hi*2u)^0x0e000000u;
            if((probe&0x0e000000u)!=0)return x.divide(numerator,denominator,x.control);
            const unsigned index=probe>>28;
            // Only 1,4,7,10,13 select the Pentium FDIV correction.
            const bool affected=index==1||index==4||index==7||index==10||index==13;
            if(!affected||exponent==0||exponent==0x7fff)return x.divide(numerator,denominator,x.control);
            const uint32_t scale=(numerator.signExponent&0x7fff)==1?0x3f880000u:0x3f700000u;
            denominator=x.multiplyFloat32(denominator,scale,temporary);
            numerator=x.multiplyFloat32(numerator,scale,temporary);
            return x.divide(numerator,denominator,x.control);
        }
        if(classifier.significand==0||exponent!=0)return x.divide(numerator,denominator,x.control);
        const uint32_t ne=numerator.signExponent&0x7fff;
        const bool integerBit=(numerator.significand>>63)!=0;
        if((ne==0&&integerBit)||(ne!=0&&(ne==0x7fff||!integerBit)))return x.divide(numerator,denominator,x.control);
        // Retail normalises only the saved classifier, preserving both real operands.
        classifier=x.multiplyFloat32(denominator,0x5f000000u,temporary);
    }
}
// 1.13c D2Game+0xF4EF6
F80 d2game_F4EF6(X87& x,F80 st0,F80 st1) { return d2game_F4940(x,st0,st1); }

}
