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
    void (*aiTarget)(P game,P unit,P t);                  // 8D230
    void (*clearFlag)(P);                                 // 8E6F0
    void (*queue)(P,P,I,I,I,I,I);                         // 8EA80
    void (*be10)(P);                                      // 7BE10
    I (*lifeOf)(P);                                       // 21180
    void (*dir)(P,I,I); I (*dirOf)(P,I,I); void (*setDir)(P,I); // 10192/10706 (GUESS)
    void (*setMode)(P,I);                                 // 10331 pre-call, GUESS
    void (*fn21320)(P); void (*f11d90)(P,P,P,I,I);        // 21320 / F1D90
    P (*mon)(P);
};
static U roll(P u) {
    uint64_t v=uint64_t(r(u,0x20))*0x6ac690c5u+r(u,0x24);
    w(u,0x20,U(v)); w(u,0x24,U(v>>32)); return U(v);
}
static I prm(P g,P m,U o) { return h(m,o+2*b(g,0x6d)); }
static void flee(Env& e,P g,P u) {            // shared tail: flag 0x20000, reset, 75E10
    if (u) w(u,0xc4,r(u,0xc4)|0x20000);
    e.reset(u,0); e.change(g,u);
}

// 1.13c D2Game+0x7C340
void sub_7C340(Env& e,P g,P u,P t) {
    P m=e.ptr(t,0x1c),ai=e.ptr(t,0);
    if (s(t,0x14)<prm(g,m,0x56)||prm(g,m,0x5c)<s(t,0x14)) { e.idle(g,u,0x28); return; }
    I dx=e.coord(u,0)-e.coord(e.ptr(t,8),0); /* GUESS: unaff_ESI-unaff_EBX */
    if (dx<0) dx=-dx;
    if (dx<3 && s(ai,0x14)<s(g,0xa8)) {
        w(ai,0x14,U(prm(g,m,0x62)+s(g,0xa8)));
        if (e.ok7B510(u)!=1) { e.modeTarget(g,u,e.ptr(t,8)); return; }
        if (h(m,0x170)>=0) { w(ai,0x1c,U(prm(g,m,0x68)+s(g,0xa8))); e.skill(g,u); return; }
    }
    e.idle(g,u,0x1e);
}
// 1.13c D2Game+0x7C460 (same body as 7C340; only the unaff registers differ: EDI/EBP)
void sub_7C460(Env& e,P g,P u,P t) { sub_7C340(e,g,u,t); }

// 1.13c D2Game+0x7C580
void sub_7C580(Env& e,P g,P u,P t) {
    P ai=e.ptr(t,0),m=e.ptr(t,0x1c);
    if (r(t,8)!=0 && s(t,0x14)<=prm(g,m,0x56) && s(ai,0x14)<prm(g,m,0x5c)) {
        if (!r(ai,0x18)) { e.modeTarget(g,u,e.ptr(t,8)); w(ai,0x14,r(ai,0x14)+1); w(ai,0x18,1); return; }
        w(ai,0x18,0); e.idle(g,u,prm(g,m,0x62)); return;
    }
    flee(e,g,u);
}

// 1.13c D2Game+0x7C6E0 (EAX = other unit, ECX = param_1; 8-direction scan, GUESS arg wiring)
I sub_7C6E0(Env& e,P g,P self,P other) {
    static const uint8_t order[8]={6,4,2,2,2,0,6,6};
    I ox=e.coord(other,0),oy=e.coord(other,1);
    U d=e.dirOf(self,ox,oy)&7;
    for (int i=0;i<8;i++) {
        I dx=0,dy=0; (void)order[d&7]; /* 10740(order[d], &dx,&dy) */
        I rows=e.tableRows();
        if (h(self,0x170)>=0) {   // GUESS: retail tests (iStack_8+0x170), iStack_8 unset
            e.setMode(self,0);
            if (e.path(e.coord(self,1)+dy*3,((0x12a<rows)-1&0xfffffed5)+0x12a,e.coord(self,0)+dx*3,0,self)) {
                e.skill(g,self); return 1;
            }
        }
        d=(d+1)&7;
    }
    return 0;
}

// 1.13c D2Game+0x7C910
void sub_7C910(Env& e,P g,P u,P t) {
    P ai=e.ptr(t,0),tgt=e.ptr(t,8);
    I tm=s(ai,0x14);
    if (tm>0) { e.idle(g,u,tm); w(ai,0x14,0); return; }
    I tx=e.coord(tgt,0),ty=e.coord(tgt,1),ux=e.coord(u,0),uy=e.coord(u,1);
    I ax=tx-ux<0?ux-tx:tx-ux, ay=ty-uy<0?uy-ty:ty-uy;
    e.dir(u,ax<ay?ux:tx,ax<ay?uy:ty);               // 10192 / 10706 (table lookups elided, GUESS)
    if ((ax>5&&ay>5)) { e.idle(g,u,prm(g,e.ptr(t,0x1c)?e.ptr(t,0x1c):tgt,0x68)); return; } // GUESS m
    P m=tgt; // GUESS: retail uses unaffected piVar12 (= t[2] unit's stat record)
    if (h(m,0x170)>=0 && ux<prm(u,m,0x56) && e.lifeOf(u)<prm(u,m,0x5c)) {
        e.skill(g,u); w(ai,0x14,U(prm(u,m,0x62))); return;
    }
    e.idle(g,u,prm(u,m,0x68));
}

// 1.13c D2Game+0x7CE40
void sub_7CE40(Env& e,P g,P u,P t) {
    P ai=e.ptr(t,0);
    if (s(ai,0)==10||s(ai,0)==11) {
        e.aiTarget(g,u,u);
        if (u) for (P n=e.ptr(u,0xdc); n; n=e.ptr(n,0x24)) if (b(n,0)==2) e.clearFlag(n);
        e.queue(g,u,2,s(g,0xa8)+1,0,0,0); return;
    }
    if (h(e.ptr(t,0x1c),0x170)>=0 && 1<s(ai,0x1c)) { e.skill(g,u); w(ai,0x1c,1); e.be10(g); return; }
    e.aiTarget(g,u,u); e.idle(g,u,1);
}

// 1.13c D2Game+0x7CF10 (param_4 = nearest-search accumulator)
I sub_7CF10(Env& e,P a,P cand,P other,U* acc) {
    if (a==other||e.isFriend(a,other)!=0||!other||!((r(other,0xc4)>>2)&1)||!e.check(a,other)) return 0;
    I d=e.dist(a,other);
    if (d>0x400) return 0;
    acc[7]++;
    if (d<0xe2) acc[2]++;
    if (d<I(acc[1])) { acc[0]=U(uintptr_t(other)); acc[1]=U(d); }
    I hp=e.stat(other,6,0)>>8;
    if (hp<I(acc[4])) { acc[3]=U(uintptr_t(other)); acc[4]=U(hp); }
    if (d<I(acc[6]) && a && s(a,0)==0) { acc[5]=U(uintptr_t(other)); acc[6]=U(d); }
    (void)cand; return 0;
}

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
        e.setMode(u,0);
        if (e.path(e.coord(u,1),((skillBase<rows)-1&mask)+skillBase,e.coord(u,0),0,u)) {
            if (!stamp) { w(ai,0x14,r(g,0xa8)); }
            w(ai,0x18,r(ai,0x18)+1); e.skill(g,u); return;
        }
    }
    e.idle(g,u,int(roll(u)%10)+0x14); (void)cap;
}
// 1.13c D2Game+0x7CFD0
void sub_7CFD0(Env& e,P g,P u,P t) { ranged(e,g,u,t,0x19,0x19,0,0xe4,I(0xffffff1b),false,false); }
// 1.13c D2Game+0x7D170
void sub_7D170(Env& e,P g,P u,P t) { ranged(e,g,u,t,0x14,0x19,0,0xce,I(0xffffff31),true,true); }

// 1.13c D2Game+0x7D7C0
void sub_7D7C0(Env& e,P g,P u,P t) {
    P tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    if (!r(t,0x18)) {
        if (roll(u)%100<U(prm(g,m,0x56))) { e.f11d90(tgt,g,u,2,0); return; }
        e.idle(g,u,prm(g,m,0x5c)); return;
    }
    if (roll(u)%100<U(prm(g,m,0x62))) { e.reset(u,0); if (u) e.fn21320(u); e.change(g,u); return; }
    e.idle(g,u,prm(g,m,0x5c));
}
// 1.13c D2Game+0x7DF00
void sub_7DF00(Env& e,P g,P u,P t) {
    P tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    if (!r(t,0x18)) {
        if (roll(u)%100<U(prm(g,m,0x62))) {
            if (u && s(u,0)==1 && r(u,0x14)) w(e.ptr(e.ptr(u,0x14),0x2c),0x18,0xd);
            e.walk(g,u,tgt,2,0,0,1); return;
        }
        e.idle(g,u,prm(g,m,0x68)); return;
    }
    if (roll(u)%100<U(prm(g,m,0x56))) { e.reset(u,0); if (u) e.fn21320(u); e.change(g,u); return; }
    e.idle(g,u,prm(g,m,0x5c));
}
// 1.13c D2Game+0x7E4D0
void sub_7E4D0(Env& e,P g,P u,P t) {
    P ai=e.ptr(t,0),tgt=e.ptr(t,8),m=e.ptr(t,0x1c);
    if (!r(ai,0x14)) { w(ai,0x14,1); e.idle(g,u,5); return; }
    if (r(t,0x18)) {
        if (roll(u)%100<U(prm(g,m,0x56))) { e.reset(u,0); if (u) e.fn21320(u); e.change(g,u); return; }
        e.idle(g,u,prm(g,m,0x5c)); return;
    }
    if (roll(u)%100<U(prm(g,m,0x62))) { e.walk(g,u,tgt,2,0,0,1); return; }
    e.idle(g,u,10);
}
}
