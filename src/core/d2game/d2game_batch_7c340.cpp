// Batch 7C340..7E4D0: AI think handlers. Pointer fields go through Env seams keyed by retail
// offsets; scalar offsets are x86 byte offsets. GUESS marks args the decompile left in
// unaffected registers (unaff_*) or whose callee meaning is unverified.
#include <cstdint>
#include <cstring>
namespace d2game_batch7c340 {
using U = uint32_t; using I = int32_t; using P = void*;
static U r(P p,U o) { U v; std::memcpy(&v,(uint8_t*)p+o,4); return v; }
static I s(P p,U o) { return I(r(p,o)); }
static I h(P p,U o) { int16_t v; std::memcpy(&v,(uint8_t*)p+o,2); return v; }
static uint8_t b(P p,U o) { return ((uint8_t*)p)[o]; }
static void w(P p,U o,U v) { std::memcpy((uint8_t*)p+o,&v,4); }
struct Env {
    P (*ptr)(P,U);
    void (*idle)(P game,P unit,I frames);                 // F22C0, unit implicit ESI
    void (*modeTarget)(P game,P unit,P target);           // F2220
    void (*skill)(P game,P unit);                         // F2380 (args implicit, GUESS)
    void (*castAt)(P unit,I skill,P target,I x,I y,I lvl,P game); // F2380: ECX=unit, stack (skill,target,x,y), AL=level, EBX=game
    void (*idle20C0)(P game,P unit,I frames);             // F20C0 (AITACTICS_Idle): EAX=game, EBX=unit, stack frames
    void (*sendPkt)(P game,const U* pkt);                 // 75E10(game, pkt, 1): pkt is 8 dwords
    I (*dir10266)(I d);                                   // D2Common 10266
    void (*step10740)(I dir,I* dx,I* dy);                 // D2Common 10740
    void (*walk6)(P,P,P,I,I,I,I,I);                       // F2540 with its sixth stack arg
    U bit4;                                               // gdwBitMasks[4]
    void (*walk)(P,P,P,I,I,I,I);                          // F2540
    void (*reset)(P,I);                                   // D2Common 11103
    void (*change)(P game,P unit);                        // 75E10, mode block built by callee (GUESS)
    I (*coord)(P unit,I axis);                            // 10141/10867/10750 (GUESS axis)
    I (*ok7B510)(P);                                      // 7B510
    I (*path)(I x,I y,I dst,I flag,P unit);               // F3D20
    I (*tableRows)();                                     // sgptDataTables+0xA80
    P (*statlist)(P,U); I (*life)(P);                     // 1180 / 1900 area
    I (*dist)(P a,P b);                                   // 21900
    I (*isFriend)(P,P);                                   // DFA90
    U (*check)(P,P);                                      // E0A60
    I (*stat)(P,I,I);                                     // 10973
    void (*aiTarget)(P game,P unit,P ai,I kind);          // 9D230: ECX=game, EDX=unit, stack (ai, kind)
    void (*clearFlag)(P node,P game);                     // 8E6F0: EAX=node, EDI=game
    void (*ord11129)(P unit,P target);                    // D2Common 11129
    void (*queue)(P,P,I,I,I,I,I);                         // 8EA80
    void (*be10)(P);                                      // 7BE10
    I (*lifeOf)(P);                                       // 21180
    void (*dir)(P,I,I); I (*dirOf)(P,I,I); void (*setDir)(P,I); // 10192/10706 (GUESS)
    void (*setMode)(P,I);                                 // 10331 pre-call, GUESS
    P (*fn21320)(P); void (*f11d90)(P,P,P,I,I);        // 21320 / F1D90
    P (*mon)(P);
    I (*posQ)(P rec);                                     // A454(rec): first position word of the unit's +0x2C record
    I (*posR)(P rec);                                     // A3B8(rec): second position word
    P (*room)(P unit,I a,I b);                            // A478(unit, a, b): the room the cell is in
    I (*sub3D20)(P room,I y,I id);                        // F3D20: EAX = room, ECX = y, EDX = id, no stack words
};
static U roll(P u) {
    uint64_t v=uint64_t(r(u,0x20))*0x6ac690c5u+r(u,0x24);
    w(u,0x20,U(v)); w(u,0x24,U(v>>32)); return U(v);
}
static I prm(P g,P m,U o) { return h(m,o+2*b(g,0x6d)); }
static void flee(Env& e,P g,P u) {            // shared tail: flag 0x20000, reset, 75E10 with {0, unit, 0, 0, 0, 0x6400, 0, 0}
    if (u) w(u,0xc4,r(u,0xc4)|0x20000);
    e.reset(u,0);
    U pkt[8]={0,U(uintptr_t(u)),0,0,0,0x6400,0,0};
    e.sendPkt(g,pkt);
}

// 75E10 packet {4, unit, target, 0, 0, flags(byte1 of dword 5), 0, 0}: flags 0x65 when the client record has gdwBitMasks[4].
static void sendChange(Env& e,P g,P u,P target) {
    U pkt[8]={4,U(uintptr_t(u)),U(uintptr_t(target)),0,0,0,0,0};
    uint8_t v=100;
    if (u) { P c=e.fn21320(u); if (c && (b(c,0x104)&e.bit4)) v=0x65; }
    pkt[5]=U(v)<<8;
    e.sendPkt(g,pkt);
}

// 1.13c D2Game+0x7C340 and 0x7C460: one body. ECX = game, EDX = unit (ESI), stack word 0 = the AI record t (ret 4).
// Retail calls 0xA544(unit, &pt) for the unit and then the target (t+8) into two {x, y} locals and compares ONE axis of the pair:
// 7C340 the y words (+4), 7C460 the x words (+0), so `axis` is the only difference between the two functions (disassembly 0x7B38A/0x7B4B6).
// The skill start is 0xF2380 (ECX = unit, stack skill, target, 0, 0, AL = level byte, EBX = game); the idle calls take EAX = game and ESI = unit.
static void aiApproach(Env& e,P g,P u,P t,I axis) {
    P m=e.ptr(t,0x1c),ai=e.ptr(t,0),tgt=e.ptr(t,8);
    if (s(t,0x14)<prm(g,m,0x56)||prm(g,m,0x5c)<s(t,0x14)) { e.idle(g,u,0x28); return; }
    I a=e.coord(u,axis);
    I c=e.coord(tgt,axis);
    I dx=a-c;
    if (dx<0) dx=-dx;
    if (dx<3 && s(ai,0x14)<s(g,0xa8)) {
        w(ai,0x14,U(prm(g,m,0x62)+s(g,0xa8)));
        if (e.ok7B510(u)!=1) { e.modeTarget(g,u,tgt); return; }
        if (h(m,0x170)>=0) {
            w(ai,0x1c,U(prm(g,m,0x68)+s(g,0xa8)));
            e.castAt(u,h(m,0x170),tgt,0,0,b(m,0x180),g); return;
        }
    }
    e.idle(g,u,0x1e);
}
inline void sub_7C340(Env& e,P g,P u,P t) { aiApproach(e,g,u,t,1); }
inline void sub_7C460(Env& e,P g,P u,P t) { aiApproach(e,g,u,t,0); }

// 1.13c D2Game+0x7C6E0 (EAX = other unit, ECX = self, stack: game, rec = monster stat row; ret 8). Disassembly 0x7B6E0..0x7B8BB.
// A unit's two position words come from its +0x2C record: types 2, 4, 5 read +0xC and +0x10 directly, every other type asks A454 / A3B8 on the
// record (0 when it is null). The direction from self toward other (A7A8, then AD1E) seeds a byte that is stepped through order[] eight times;
// each step offsets self's position by three AC88 steps, and when the monster row's skill (+0x170) is valid and F3D20 accepts the cell the
// skill is started through F2380 (ECX = self, stack skill, 0, x, y, AL = level byte, EBX = game).
static I posQ(Env& e,P u) { U t=r(u,0); P p=e.ptr(u,0x2c); if (t==2||(t>3&&t<=5)) return s(p,0xc); return p?e.posQ(p):0; }
static I posR(Env& e,P u) { U t=r(u,0); P p=e.ptr(u,0x2c); if (t==2||(t>3&&t<=5)) return s(p,0x10); return p?e.posR(p):0; }
inline I sub_7C6E0(Env& e,P g,P self,P other,P rec) {
    static const uint8_t order[8]={6,4,2,2,2,0,6,6};
    I oR=posR(e,other);
    I oQ=posQ(e,other);
    uint8_t d=uint8_t(e.dir10266(e.dirOf(self,oQ,oR)));
    for (int i=0;i<8;i++) {
        d&=7;
        I dx=0,dy=0; e.step10740(order[d],&dx,&dy);
        I x=posQ(e,self)+dx*3;
        I y=posR(e,self)+dy*3;
        I rows=e.tableRows();
        I id=((0x12a<rows)-1&0xfffffed5)+0x12a;
        if (h(rec,0x170)>=0) {
            P rm=e.room(self,x,0);
            if (e.sub3D20(rm,y,id)) {
                e.castAt(self,h(rec,0x170),nullptr,x,y,b(rec,0x180),g); return 1;
            }
        }
        d++;
    }
    return 0;
}

// 1.13c D2Game+0x7C910
inline void sub_7C910(Env& e,P g,P u,P t) {
    static const uint8_t idx[64]={0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0};
    static const uint8_t val[4]={0x1f,0x31,0x00,0x11};
    P ai=e.ptr(t,0),tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    I dist=s(t,0x14);
    I tm=s(ai,0x14);
    if (tm>0) { e.idle(g,u,tm); w(ai,0x14,0); return; }
    e.ord11129(u,tgt);
    I tx=e.coord(tgt,0),ty=e.coord(tgt,1),ux=e.coord(u,0),uy=e.coord(u,1);
    I dx=tx-ux, dy=ty-uy;
    I adx=dx<0?-dx:dx, ady=dy<0?-dy:dy;
    U dir=U(e.dirOf(u,adx<ady?ux:tx,adx<ady?ty:uy))&0xff;
    e.setDir(e.ptr(u,0x2c),val[idx[dir&63]]);       // table index past 63 is outside the direction range
    if (!(adx<6) && !(ady<6)) { e.idle(g,u,prm(g,m,0x68)); return; }
    if (h(m,0x170)>=0 && dist<prm(g,m,0x56) && I(roll(u)%100)<prm(g,m,0x5c)) {
        e.castAt(u,h(m,0x170),tgt,0,0,b(m,0x180),g); w(ai,0x14,U(prm(g,m,0x62))); return;
    }
    e.idle(g,u,prm(g,m,0x68));
}

// 1.13c D2Game+0x7CE40
inline void sub_7CE40(Env& e,P g,P u,P t) {
    P ai=e.ptr(t,0);
    I kind=s(ai,0);
    if (kind==10||kind==11) {
        e.aiTarget(g,u,ai,kind);
        if (u) for (P n=e.ptr(u,0xdc); n; n=e.ptr(n,0x24)) if (b(n,0)==2) e.clearFlag(n,g);
        e.queue(g,u,2,s(g,0xa8)+1,0,0,0); return;
    }
    P m=e.ptr(t,0x1c);
    if (h(m,0x170)>=0 && 2<=s(ai,0x1c)) { e.castAt(u,h(m,0x170),e.ptr(t,8),0,0,b(m,0x180),g); w(ai,0x1c,1); e.be10(g); return; }
    e.aiTarget(g,u,ai,kind); e.idle20C0(g,u,1);
}

// 1.13c D2Game+0x7CF10 moved to d2game_7cf10.cpp (retail register and stack arguments).

static void ranged(Env& e,P g,P u,P t,I maxDist,I idleDist,I cap,I skillBase,I mask,bool le,bool stamp) {
    P ai=e.ptr(t,0),m=e.ptr(t,0x1c);
    if (maxDist< s(t,0x14)) { e.idle(g,u,idleDist); return; }
    I cnt=s(ai,0x18);
    if (le?(prm(g,m,0x62)<=cnt):(prm(g,m,0x62)<cnt)) {
        flee(e,g,u); return;
    }
    I dt=s(g,0xa8)-s(ai,0x14); if (dt<0) dt=-dt;
    if (h(m,0x170)>=0 && prm(g,m,0x56)<=dt) {
        if (stamp) w(ai,0x14,r(g,0xa8));
        I rows=e.tableRows();
        I py=e.coord(u,1), px=e.coord(u,0);          // retail asks 0x1130 (y) then 0x1100 (x)
        if (e.path(py,((skillBase<rows)-1&mask)+skillBase,px,0,u)) {
            if (!stamp) { w(ai,0x14,r(g,0xa8)); }
            w(ai,0x18,r(ai,0x18)+1); e.castAt(u,h(m,0x170),e.ptr(t,8),0,0,b(m,0x180),g); return;
        }
    }
    e.idle(g,u,int(roll(u)%10)+0x14); (void)cap;
}
// 1.13c D2Game+0x7CFD0
inline void sub_7CFD0(Env& e,P g,P u,P t) { ranged(e,g,u,t,0x19,0x19,0,0xe4,I(0xffffff1b),false,false); }
// 1.13c D2Game+0x7D170
inline void sub_7D170(Env& e,P g,P u,P t) { ranged(e,g,u,t,0x14,0x19,0,0xce,I(0xffffff31),true,true); }

// 1.13c D2Game+0x7D7C0
inline void sub_7D7C0(Env& e,P g,P u,P t) {
    P tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    if (!r(t,0x18)) {
        if (I(roll(u)%100)<prm(g,m,0x56)) { e.f11d90(tgt,g,u,2,0); return; }
        e.idle(g,u,prm(g,m,0x5c)); return;
    }
    if (I(roll(u)%100)<prm(g,m,0x62)) { e.reset(u,0); sendChange(e,g,u,tgt); return; }
    e.idle(g,u,prm(g,m,0x5c));
}
// 1.13c D2Game+0x7DF00
inline void sub_7DF00(Env& e,P g,P u,P t) {
    P tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    if (!r(t,0x18)) {
        if (I(roll(u)%100)<prm(g,m,0x62)) {
            if (u && s(u,0)==1 && r(u,0x14)) w(e.ptr(e.ptr(u,0x14),0x2c),0x18,0xd);
            e.walk6(g,u,tgt,2,0,0,1,7); return;
        }
        e.idle(g,u,prm(g,m,0x68)); return;
    }
    if (I(roll(u)%100)<prm(g,m,0x56)) { e.reset(u,0); sendChange(e,g,u,tgt); return; }
    e.idle(g,u,prm(g,m,0x5c));
}
// 1.13c D2Game+0x7E4D0
inline void sub_7E4D0(Env& e,P g,P u,P t) {
    P ai=e.ptr(t,0),tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    if (!r(ai,0x14)) { w(ai,0x14,1); e.idle(g,u,5); return; }
    if (r(t,0x18)) {
        if (I(roll(u)%100)<prm(g,m,0x56)) { e.reset(u,0); sendChange(e,g,u,tgt); return; }
        e.idle(g,u,prm(g,m,0x5c)); return;
    }
    if (I(roll(u)%100)<prm(g,m,0x62)) { e.walk6(g,u,tgt,2,0,0,1,7); return; }
    e.idle(g,u,10);
}
}
