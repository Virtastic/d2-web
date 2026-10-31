// Declarations for d2client_batch_30340.cpp (the skill-description builders), shared with the FN probes in src/core/fn.
#pragma once
#include <cstdint>
#include <cstring>
namespace d2client_batch_30340 {
using U=uint32_t; using I=int32_t; using P=void*; using C16=char16_t;
struct Ops {
 I (*stat)(P,I,I);                      // Ordinal_10973(unit,stat,layer)
 I (*dmgMin)(P,I,I,I); I (*dmgMax)(P,I,I,I); // Ordinal_10567 / 10297 (unit,skill,lvl,flag)
 I (*skMin)(P,I,I,I);  I (*skMax)(P,I,I,I);  // Ordinal_10121 / 11091
 I (*bonusTotal)();                     // D2Client+0x2FB50
 I (*rec2CD0)(I,I); I (*rec15E0)(I,I);  // eax=skill, edx=level
 void (*calc10323)(P,I*,I*,I*);
 void (*bonus2F980)(P,I*,I*,I*);
 void (*emit)(I eax,I ecx,I s0,I s1,I s2,I s3); // D2Client+0x2F0A0
 void (*fade190)(P rec,P unit,I*,I*,I*);        // D2Client+0x2E190
 I (*o10037)(P,I);
 I (*o10854)(P,P*); const uint8_t* (*o10695)(I);
 P (*o10708)(P,I,I); I (*hasSkill)(P,I); /*10494*/ I (*o10306)(P,P,I);
 int16_t (*o10213)(P); int16_t (*o10120)(P);
 const uint8_t* (*o17F0)(I);            // skill-desc record, +0x18/+0x1c/+0x20.. fields
 I (*o2C830)(uint8_t); I (*o10601)(I,const char*,I,I); I (*o10435)(P,I,I);
 P (*o10061)(P); I (*o10711)(P); I (*o10804)(P,P,I,I);
 void (*o2D6E0)(uint8_t,I*,I*);
 void (*o2D500)(P eax,I,I,I,I,I,I,I,I,I,I);     // ten stack args, first-listed = first param
 void (*o2E140)(I eax,I ecx,C16* buf,I s);
 void (*o2D3C0)(C16*,C16*,I,I,I,I,I);
 I (*o10747)(P); void (*o2C4F0)(P sk,I*,I*,I,I);
 I (*o10263)(P); I (*o10975)(P);
 I (*o10051)(P,P); I (*o10597)(P,P,I); I (*o10910)(P,I,I);
 I (*o10744)(P,I); I (*o11006)(P); I (*o11160)(P,I);
 void (*o10164)(P,P,I); P (*o11139)(P,I); I (*o10417)(P); I (*o10786)(P,I,I,I);
 I (*o2D810)(P eax,I edi);
 I (*o2F7D0)(I ebx,I,I,I,I); I (*o2F870)(I ebx,I,I,I,I,I); I (*o2DCA0)(I ebx,I,I,I);
 P (*owner)(P unit);                    // unit+0x60
};
struct Tables { I colors[4]; I rows[4][3]; uint8_t mask0c; bool modeFlag9854; };

void sub_30340(const Ops& o,P unit,I flag,const int16_t* sk,I lvl,I a2,I a3,I a4);
void sub_304A0(const Ops& o,const Tables& t,P unit,I flag,const int16_t* sk,I a1,I a2,I a3,I a4);
void sub_305D0(const Ops& o,P unit,I flag,const int16_t* sk,I lvl,I a2,I a3,I a4);
void sub_307B0(const Ops& o,P unit,I flag,const uint8_t* sk,I lvl,I a2,I a3,I a4);
void sub_308C0(const Ops& o,P unit,I flag,const uint8_t* rec,I a1,I a2,I a3,I a4);
void sub_30B40(const Ops& o,const uint8_t* rec,P self,P a0,I a1,I a2,I a3,I a4,I a5,I a6,I a7,I a8,I flag);
void sub_30DB0(const Ops& o,P unit,I flag,I a0,I* a1,I* a2,I a3,I a4,uint32_t cb);
void sub_30E30(const Ops& o,const Tables& t,P unit,I* mn,I* mx,I* mode,P flag);
I sub_314D0(const Ops& o,const uint8_t* self,P extra,I a0,I a1,I a2,I a3);
}
