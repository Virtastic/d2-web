// Prior art used: D2MOO AiGeneral/AiThink movement helpers (D2GAME_AI_*), unit path accessors.
// Retail 1.13c cached decompiles. Hidden register args (EAX/EBX/ESI/EDI) are explicit params
// below. Pointer fields go through env seams keyed by RETAIL offsets; no pointer in a dword.
#include <cstdint>
#include <cstring>
#include "d2game_batch_f17e0.h"
namespace d2game_batchf17e0 {
using U = uint32_t; using I = int32_t; using P = void*;
static I s(P p,U o) { I v; std::memcpy(&v,(uint8_t*)p+o,4); return v; }
static void w(P p,U o,I v) { std::memcpy((uint8_t*)p+o,&v,4); }
static void wb(P p,U o,uint8_t v) { ((uint8_t*)p)[o]=v; }
static I xOf(Env& e,P u) {
    I t=s(u,0); P p=e.ptr(u,0x2c);
    if (t==2||(t>3&&t<6)) return s(p,0xc);
    return p?e.pathX(p):0;
}
static I yOf(Env& e,P u) {
    I t=s(u,0); P p=e.ptr(u,0x2c);
    if (t==2||(t>3&&t<6)) return s(p,0x10);
    return p?e.pathY(p):0;
}
static P aiData(Env& e,P u) { return (s(u,0)==1&&s(u,0x14))?e.ptr(e.ptr(u,0x14),0x2c):nullptr; }
// 1.13c D2Game+0xF17E0
I sub_F17E0(Env& e,P unit,P game,P a,P b) {
    if (!unit||!game) return 0;
    P ai=e.ptr(unit,0x80);
    if (ai && e.stateTest(unit,0x36)==0 && e.f114f0(unit,ai,a,b,nullptr,0)) { e.f7a4c0(a,I(uintptr_t(b))); return 1; }
    return 0;
}
// 1.13c D2Game+0xF1A20
void sub_F1A20(Env& e,P unit,I a,I b,uint8_t c) {
    if (!(b>-0x7f&&b<0x7f)) { e.fatal(0x110); return; }
    if (a==1) a=7;
    P d=aiData(e,unit); // retail derefs null d when unit invalid; same here
    if (a) w(d,0x18,a);
    if (b) w(d,0x1c,b);
    if (c) wb(d,0x20,c>0x4d?0x4d:c);
}
// 1.13c D2Game+0xF1C30
I sub_F1C30(Env& e,P a,P b,P info) {
    // b is EDI; ordinal 10522 called with unit regs in retail
    (void)a;(void)b;(void)info;
    return 0; // ponytail: body is a register-only prologue (10867/10750 results discarded); behavior via change
}
// 1.13c D2Game+0xF29B0
void sub_F29B0(Env& e,P game,P u,uint8_t al,I(*rand)(P seed)) {
    I x=xOf(e,u), y=yOf(e,u);
    uint64_t* seed=(uint64_t*)((uint8_t*)u+0x20);
    I dx=al, dy=al;
    *seed=uint64_t(uint32_t(*seed))*0x6ac690c5u+uint32_t(*seed>>32);
    // seed step uses dword at +0x24 as carry, matching retail
    dx=rand((P)seed); 
    uint64_t v=uint64_t(uint32_t(*seed))*0x6ac690c5u+s(u,0x24); *seed=v; if(v&1) dx=-dx;
    v=(v&0xffffffffu)*0x6ac690c5u+(v>>32); *seed=v; if(v&1) dy=-dy;
    e.walk(game,u,nullptr,2,dx+x,I(int8_t(dy))+y,1,0);
}
// 1.13c D2Game+0xF2C10
void sub_F2C10(Env& e,P game,P a,P b,uint8_t d) {
    // a=EAX (reference), b=EBX (mover)
    U bx=U(xOf(e,b)),by=U(yOf(e,b)),ax=U(xOf(e,a)),ay=U(yOf(e,a));
    I sx=bx<ax?-1:(ax<bx?1:0);
    int8_t sy=by<ay?-1:(ay<by?1:0);
    int8_t oy=int8_t(d*sy);
    if (d>5) { P ai=aiData(e,b); if (d) wb(ai,0x20,d>0x4d?0x4d:d); }
    e.walk(game,b,nullptr,0xf,I(bx+U(d)*U(sx)),I(int8_t(by)+oy),1,0);
}
// 1.13c D2Game+0xF2F00
void sub_F2F00(Env& e,P target,P unit,P game,I flag) { e.walk(game,unit,target,0xf,0,0,(flag&0xff)?flag:((flag&~0xff)|1),0); }
// 1.13c D2Game+0xF2F30
void sub_F2F30(Env& e,P target,P unit,P game,I flag) { e.walk(game,unit,target,2,0,0,(flag&0xff)?flag:((flag&~0xff)|1),0); }
// 1.13c D2Game+0xF3000
void sub_F3000(Env& e,P target,P unit,P game,I wv) {
    w(aiData(e,unit),0x18,0xd);
    e.walk(game,unit,target,2,0,0,1,wv);
}
// 1.13c D2Game+0xF3220
// ECX = unit, EDX = a, EAX = x0, stack (game, b) (ret 8). B1C90 gets EAX = game and (unit, a, b, x0); the
// 1560 class test takes EAX = unit+4, EDX = 2, and F1B10 gets AL = 5 over that test's nonzero result.
P sub_F3220(Env& e,P unit,P a,P game,P b,I x0) {
    P r=e.f1c90(game,unit,a,b,x0);
    if (r) return r;
    if (unit && s(unit,0)==1 && e.ptr(unit,0x14)) {
        I m=s(e.ptr(unit,0x14),0x54);
        if (m==3||m==0x13) { U t=e.f1560(s(unit,4),2); if (t) { e.f1b10((t&~0xFFu)|5,game,unit); return nullptr; } }
    }
    I d=e.r10123(unit);
    I y=yOf(e,unit),x=xOf(e,unit);
    I q=e.r10331(unit,x,y,d,0x40);
    if (e.r10851(q)) { U t=e.f1560(s(unit,4),2); if (t) { e.f1b10((t&~0xFFu)|5,game,unit); return nullptr; } }
    I m=s(b,0);
    if (m>0x22) e.idle(game,unit,0x19);
    else if (m<0x19) e.idle(game,unit,10);
    else e.idle(game,unit,m-10);
    return nullptr;
}
// 1.13c D2Game+0xF41C0
// EDI = unit, EBX = passed on, EAX = passed on in EAX/ESI; returns F0640's result (flag 1).
I sub_F41C0(Env& e,P unit,P ebx,I eax) {
    if (!unit || s(unit,0)!=0) { e.fatal(0x24); return 0; }
    e.f102f0(unit,eax,eax,ebx);
    return e.f10640(unit,8,eax,ebx,1);
}
// 1.13c D2Game+0xF4210
// As F41C0 with flag 0; returns 1.
I sub_F4210(Env& e,P unit,P ebx,I eax) {
    if (!unit || s(unit,0)!=0) { e.fatal(0x15); return 0; }
    e.f102f0(unit,eax,eax,ebx);
    e.f10640(unit,8,eax,ebx,0);
    return 1;
}
}
