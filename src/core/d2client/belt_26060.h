// Belt (D2Client 0x26060..0x271C0) shared types: the host seams the ports call, probes supply them.
#pragma once
#include <cstdint>
namespace d2client_belt {
using U=uint32_t; using I=int32_t; using P=void*;
struct Draw { U words[13]{}; P cell=nullptr; U tail[4]{}; };
struct Effect { U words[23]{}; P unit=nullptr; };
struct State {
 I selected=-1,x=-1,y=-1,active=0,mode=0,belt=0,width=0,height=0,mx=0,my=0,expanded=0,enabled=0;
 P current=nullptr,previous=nullptr,inventory=nullptr,player=nullptr,quest=nullptr;
 uint8_t colors[4]{}; P cells[30]{};
};
struct Calls {
 P (*resolve)(U);
 I (*pathX)(P); I (*pathY)(P); I (*stat)(P,I,I); void (*effect)(Effect*);
 I (*questFlag)(P,I,I); I (*distance)(P,P); void (*stopPath)(P); I (*canAct)(P); // 29F30 reads the player in ESI.
 void (*action)(I,P,const U*,I); void (*setMode)(P,I); U (*finish)(P,I);
 void (*beltInfo)(I,P,uint8_t*); void (*rect)(I,P,I*,I);
 I (*special)(P); P (*item)(P,I); I (*itemIndex)(P);
 U (*color)(I,I,I); const char16_t* (*text)(U); void (*tooltip)(const char16_t*,I,I,I,I);
 void (*fill)(I,I,I,I,U,I); void (*draw)(Draw*,I,I,U,I,I);
 void (*unload)(P); void (*release)(P,I,I); P (*load)(const char*);
};
void sub_26060(P source,P unit,Calls& c);
U sub_261F0(State& s,Calls& c,P actor,U eax);
I sub_26460(State& s,Calls& c,I x,I y);
void sub_265E0(State& s,Calls& c);
void sub_266C0(State& s,Calls& c,P unit,I x,I y);
}
