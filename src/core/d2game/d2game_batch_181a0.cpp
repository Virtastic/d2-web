// Prior art used: D2MOO AiThink (SpecialState14, SuicideMinion, SuccubusWitch,
// Succubus, DesertTurret), AiGeneral minion/command helpers, MonsterUnique cold/poison
// and damage modifiers. Retail 1.13c cached decompiles cross-checked with x86 disassembly.
// Pointer fields use native-pointer access seams keyed by RETAIL offsets: no pointer
// is ever stored in a retail dword. All scalar offsets below remain x86 byte offsets.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
namespace d2game_batch181a0 {
using U = uint32_t; using I = int32_t; using P = void*;
static U r(P p,U o) { U v; std::memcpy(&v,(uint8_t*)p+o,4); return v; }
static I s(P p,U o) { return I(r(p,o)); }
static I h(P p,U o) { int16_t v; std::memcpy(&v,(uint8_t*)p+o,2); return v; }
static uint8_t b(P p,U o) { return ((uint8_t*)p)[o]; }
static void w(P p,U o,U v) { std::memcpy((uint8_t*)p+o,&v,4); }
static void wb(P p,U o,uint8_t v) { ((uint8_t*)p)[o]=v; }
static I add(I a,I c) { return I(U(a)+U(c)); }
static I mul(I a,I c) { return I(U(a)*U(c)); }
struct Mode { I mode; P unit; P target; I x=0,y=0; U zero[3]={}; uint8_t rate=100; };
struct Env {
    P (*ptr)(P,U); void (*putptr)(P,U,P); // retail pointer field -> native object
    P tables;
    void (*idle)(P game,P unit,I frames); // F22C0, unit is implicit ESI
    I (*walk)(P,P,P,I,I,I,I,I); // F2540(game,unit,target,mode,x,y,flags,zero)
    void (*reset)(P,I); // D2Common 11103
    P (*monstats2)(U); // 1320
    void (*change)(P,const Mode&,I); // 75E10
    P (*statlist)(P,U); I (*life)(P); I (*maxLife)(P); I (*maxMana)(P);
    void (*skill)(P,P,I,I,P,I,I); // F2380 game/unit/mode/skill/target/x/y
    P (*target)(P,P,I*,I); // B1BC0
    I (*escape)(P,P,P,uint8_t,I); // F2D80
    void (*modeTarget)(P,P,I,P); // F2220
    I (*coordX)(P); I (*coordY)(P); uint8_t (*direction)(P,I,I);
    uint8_t (*direction8)(uint8_t); void (*setTarget)(P,P);
    I (*skillCan)(P target,P game,I skill,I x,I y); void (*pathTarget)(P,I,I);
    void (*allocMinion)(P,P,P); // 20CF0
    P (*command)(P,I,I); void (*copyCommand)(P,P,const I*); // 20E00,21290
    I (*stat)(P,I,I); void (*addStat)(P,I,I,I); // 10973,10551
    I (*constant)(I); void (*resistant)(P,I,I); // 21FB0,21BE0
    P (*room)(P); I (*town)(P); void (*roomFlag)(P,I);
    void (*remove)(I,P,P,I,I); // EDDB0
    void (*state)(P,I,I); P (*difficulty)(uint8_t); I (*baseId)(P);
};
static U roll(P u) {
    uint64_t v=uint64_t(r(u,0x20))*0x6ac690c5u+r(u,0x24);
    w(u,0x20,U(v)); w(u,0x24,U(v>>32)); return U(v);
}
static I pct(P u) { return I(roll(u)%100); }
static I param(P g,P m,U offset) { return h(m,offset+2*b(g,0x6d)); }
static void mode(Env& e,P g,P u,P t,I m) {
    e.reset(u,0); Mode info{m,u,t};
    if (u) { P row=e.monstats2(r(u,4)); if(row && (b(row,0x104)&(1u<<m))) info.rate=101; }
    e.change(g,info,1);
}
static void skill(Env& e,P g,P u,P m,U index,P t,I x=0,I y=0) {
    e.skill(g,u,b(m,0x180+index),h(m,0x170+2*index),t,x,y);
}
// 1.13c D2Game+0x181A0
void sub_181A0(Env& e,P g,P u,P tick) {
    P t=e.ptr(tick,8);
    if (!r(tick,0x18)) { if(pct(u)<89) { e.walk(g,u,t,2,0,0,1,0); return; } }
    else if(pct(u)<95) {
        roll(u); // Retail XOR EAX,EAX makes the alternate mode-4 block unreachable.
        mode(e,g,u,t,5); return;
    }
    e.idle(g,u,10);
}
// 1.13c D2Game+0x18360
void sub_18360(Env& e,P g,P u,P tick) {
    P ai=e.ptr(tick,0),m=e.ptr(tick,0x1c),t=e.ptr(tick,8);
    I timer=s(ai,0x14);
    if(timer) {
        if(timer<s(g,0xa8)) { e.reset(u,0); Mode info{0,u,t}; e.change(g,info,1); return; }
    } else if(!r(tick,0x18)) {
        if(pct(u)<param(g,m,0x62)) { e.walk(g,u,t,2,0,0,1,0); return; }
    } else w(ai,0x14,U(add(s(g,0xa8),param(g,m,0x6e))));
    e.idle(g,u,param(g,m,0x5c));
}
static bool curse(Env& e,P g,P u,P m,P t,I distance,bool requireTarget) {
    if((requireTarget&&!t)||e.statlist(t,32)||distance>=param(g,m,0x68)||pct(u)>=param(g,m,0x62)) return false;
    if(h(m,0x170)>0 && e.life(t)>=param(g,m,0x7a)) { skill(e,g,u,m,0,t); return true; }
    if(h(m,0x172)>0 && e.life(u)<=param(g,m,0x80)) { skill(e,g,u,m,1,t); return true; }
    if(h(m,0x174)>0) {
        I mana=e.maxMana(t),life=e.maxLife(t);
        if(mana<life || !t || r(t,0)!=0) { skill(e,g,u,m,2,t); return true; }
    }
    if(h(m,0x176)>0 && t && r(t,0)==0) { skill(e,g,u,m,3,t); return true; }
    return false;
}
// 1.13c D2Game+0x1A9F0
void sub_1A9F0(Env& e,P g,P u,P tick) {
    P m=e.ptr(tick,0x1c),t=e.ptr(tick,8); I distance=s(tick,0x14);
    if(curse(e,g,u,m,t,distance,true)) return;
    if(r(tick,0x18)) {
        if(pct(u)<param(g,m,0x62) && e.escape(g,u,t,b(m,0x68+2*b(g,0x6d)),1)) return;
        if(pct(u)<param(g,m,0x56)) mode(e,g,u,t,4);
        else e.idle(g,u,param(g,m,0x74));
        return;
    }
    if(h(m,0x178)>=0 && param(g,m,0x80)>0 && pct(u)<param(g,m,0x6e)) {
        P other=e.target(g,u,&distance,0);
        if(other && pct(u)<param(g,m,0x80)) { skill(e,g,u,m,4,other); return; }
    }
    if(distance<param(g,m,0x68) && pct(u)<param(g,m,0x62) && e.escape(g,u,t,b(m,0x68+2*b(g,0x6d)),1)) return;
    if(h(m,0x178)<0) {
        P other=e.target(g,u,&distance,0);
        if(other && pct(u)<param(g,m,0x6e)) { e.modeTarget(g,u,9,other); return; }
    }
    if(pct(u)<param(g,m,0x5c)) { e.walk(g,u,t,2,0,0,1,0); return; }
    if(pct(u)<50) {
        I which=(roll(u)&255)>127?6:5;
        P data=(u && r(u,0)==1)?e.ptr(u,0x14):nullptr;
        P control=data?e.ptr(data,0x2c):nullptr;
        w(control,0x18,U(which)); wb(control,0x20,6);
        if(e.walk(g,u,t,2,0,0,1,4)) return;
    }
    e.idle(g,u,param(g,m,0x74));
}
// 1.13c D2Game+0x1AF80
void sub_1AF80(Env& e,P g,P u,P tick) {
    P m=e.ptr(tick,0x1c),t=e.ptr(tick,8); I distance=s(tick,0x14);
    if(curse(e,g,u,m,t,distance,false)) return;
    if(r(tick,0x18)) {
        if(pct(u)<param(g,m,0x56)) mode(e,g,u,t,4);
        else e.idle(g,u,param(g,m,0x6e));
        return;
    }
    if(h(m,0x178)>0 && param(g,m,0x80)>0) {
        P other=e.target(g,u,&distance,0);
        if(other && pct(u)<param(g,m,0x80)) { skill(e,g,u,m,4,other); return; }
    }
    if(pct(u)<param(g,m,0x5c)) e.walk(g,u,t,2,0,0,1,0);
    else e.idle(g,u,param(g,m,0x74));
}
// 1.13c D2Game+0x1C1E0
void sub_1C1E0(Env& e,P g,P u,P tick) {
    static constexpr I indices[64]={0,0,1,2,5,6,7,0,1,1,1,2,3,6,7,0,1,2,2,2,3,4,7,0,1,2,3,3,3,4,5,6,1,2,3,4,4,4,5,6,7,2,3,4,5,5,5,6,7,0,3,4,5,6,6,6,7,0,1,4,5,6,7,7};
    static constexpr I offsets[8][2]={{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0}};
    P ai=e.ptr(tick,0),m=e.ptr(tick,0x1c),t=e.ptr(tick,8);
    if(h(m,0x170)>=0&&!r(ai,0x14)) {
        skill(e,g,u,m,0,nullptr); w(ai,0x1c,0); w(ai,0x14,r(g,0xa8)); return;
    }
    if(s(ai,0x14)<=s(g,0xa8)) {
        if(param(g,m,0x68)<s(tick,0x14)) {
            if(s(ai,0x18)>0) w(ai,0x18,r(ai,0x18)-1);
            e.idle(g,u,15); return;
        }
        I tx=e.coordX(t),ty=e.coordY(t),ux=e.coordX(u),uy=e.coordY(u);
        I direction=e.direction8(e.direction(u,tx,ty))&7;
        I index=indices[s(ai,0x1c)%8+direction*8]; w(ai,0x1c,U(index));
        I spread=param(g,m,0x6e),x=add(ux,mul(offsets[index][0],spread)),y=add(uy,mul(offsets[index][1],spread));
        e.setTarget(u,t);
        if(h(m,0x170)>=0 && e.skillCan(t,g,h(m,0x170),x,y)) {
            // Retail fetches target Y before X for its second F3E30 call.
            ty=e.coordY(t); tx=e.coordX(t);
            if(e.skillCan(t,g,h(m,0x170),tx,ty)) {
                skill(e,g,u,m,0,nullptr,x,y); e.pathTarget(e.ptr(u,0x2c),x,y);
                I shots=add(s(ai,0x18),1); w(ai,0x18,U(shots));
                if(param(g,m,0x5c)<shots) { w(ai,0x14,U(add(s(g,0xa8),param(g,m,0x62)))); w(ai,0x18,0); }
                else w(ai,0x14,U(add(s(g,0xa8),param(g,m,0x56))));
                return;
            }
        }
        if(s(ai,0x18)>0) w(ai,0x18,r(ai,0x18)-1);
    }
    e.idle(g,u,10);
}
static P lookup(Env& e,P g,U guid) {
    if(!g) std::_Exit(-1); // retail Fog assertion then exit(-1)
    for(P u=e.ptr(g,0x1320+(guid&127)*4);u;u=e.ptr(u,0xe4)) if(r(u,0xc)==guid) return u;
    return nullptr;
}
static P ownerControl(Env& e,P u) {
    P data=e.ptr(u,0x14); return data?e.ptr(data,0x28):nullptr;
}
static void owner(Env& e,P g,P u,U guid,bool flags) {
    if(u && r(u,0)==1) {
        P ai=ownerControl(e,u); if(flags) wb(ai,8,b(ai,8)|3);
        w(ai,0x2c,guid); w(ai,0x30,1); e.putptr(ai,0x28,g);
    }
}
// 1.13c D2Game+0x21040
void sub_21040(Env& e,P ai) {
    P g=e.ptr(ai,0x28);
    for(P list=e.ptr(ai,0x34);list;list=e.ptr(list,4)) owner(e,g,lookup(e,g,r(list,0)),~U(0),false);
}
// 1.13c D2Game+0x210D0
void sub_210D0(Env& e,P ai,P unit) {
    P first=e.ptr(ai,0x34); if(!first) return;
    P g=e.ptr(ai,0x28);
    for(P list=first;list;list=e.ptr(list,4)) {
        P minion=lookup(e,g,r(list,0)); if(!minion) continue;
        U guid=r(list,0); owner(e,g,minion,guid,true); owner(e,g,unit,guid,false);
        e.allocMinion(g,minion,unit);
        for(P rest=first;rest;rest=e.ptr(rest,4)) {
            P other=lookup(e,g,r(rest,0));
            if(other && other!=minion) { owner(e,g,other,guid,false); e.allocMinion(g,minion,other); }
        }
        return;
    }
}
// 1.13c D2Game+0x217D0
P sub_217D0(Env& e,P game,P unit,I command,I set) {
    if(!e.command(unit,command,set)) {
        I params[5]={command,0,0,0,0}; e.copyCommand(game,unit,params);
    }
    return e.command(unit,command,set);
}
static void enchant(Env& e,P u,I modifier,I unique,bool poison) {
    if(!u||r(u,0)!=1) return;
    P g=e.ptr(u,0x80); if(!g) return;
    U diff=std::min<U>(b(g,0x6d),2);
    I gameType=(b(g,0x6a)||r(g,0x74))?1:0;
    I level=std::min(std::max(e.stat(u,12,0),1),add(s(e.tables,0xb74),-1));
    if(level<0||level>=s(e.tables,0xb74)) return;
    P table=e.ptr(e.tables,0xb70);
    P row=(uint8_t*)table+U(level)*0x78;
    I damage=s(row,diff*4+(gameType+6)*12);
    // Multiplication is x86 low-dword IMUL, THEN signed division by 100.
    I minimum=mul(e.constant(I(diff)+(unique?28:16)),damage)/100;
    I maximum=mul(e.constant(I(diff)+(unique?31:19)),damage)/100;
    I stat=poison?57:54;
    e.addStat(u,stat,minimum,0); e.addStat(u,stat+1,maximum,0);
    I duration=poison?mul(add(mul(level,5),150),2):add(mul(level,5),100);
    e.addStat(u,stat+2,duration,0); e.resistant(u,modifier,unique);
}
// 1.13c D2Game+0x22320
void sub_22320(Env& e,P u,I modifier,I unique) { enchant(e,u,modifier,unique,true); }
// 1.13c D2Game+0x22490
void sub_22490(Env& e,P u,I modifier,I unique) { enchant(e,u,modifier,unique,false); }
// 1.13c D2Game+0x22B10
void sub_22B10(Env& e,P g,P u,I modifier,I unique) {
    (void)modifier; (void)unique;
    if(!u||!r(u,0x10)) {
        P room=e.room(u);
        if(e.town(room)) { e.roomFlag(room,0x227); e.remove(1,u,g,1,0); }
        e.state(u,0x68,1);
    }
}
// 1.13c D2Game+0x22BC0
void sub_22BC0(Env& e,P u,I unique) {
    if(!u||r(u,0)!=1) return;
    P game=e.ptr(u,0x80); if(!game) return;
    I count=s(e.tables,0xc54); P rows=e.ptr(e.tables,0xc50);
    I first=unique?15:14,second=unique?13:12;
    I armor=0,tohit=0;
    if(count>first) armor=s((uint8_t*)rows+U(first)*0x20,0x1c);
    if(count>second) tohit=s((uint8_t*)rows+U(second)*0x20,0x1c);
    P difficulty=e.difficulty(b(game,0x6d));
    if(difficulty) {
        armor=mul(s(difficulty,0x34),armor)/100;
        tohit=mul(s(difficulty,0x34),tohit)/100;
        I level=e.baseId(u);
        if(level>=0&&level<s(e.tables,0xa80)&&level==0x76) armor/=2;
    }
    e.addStat(u,0x19,armor,0); e.addStat(u,0x77,tohit,0);
}
} // namespace d2game_batch181a0
