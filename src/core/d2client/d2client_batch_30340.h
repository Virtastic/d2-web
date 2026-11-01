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
 void (*fade190b)(P rec,I ebx,P unit,I*,I*,I*); // 2E190 with ebx=a1 (30B40)
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
 I (*o10747)(P); void (*o2C4F0)(P sk,P unit,I*,I*,I,I); // eax=sk, esi=unit
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
struct Ops30E30 { I (*stat)(P,I,I); const uint8_t* (*unitRec)(P); void (*f2980)(P,I*,I*,I*); I (*o10263)(P); I (*o10975)(P); };
void sub_30E30(const Ops30E30& o,const Tables& t,P unit,I* mn,I* mx,I* mode,P flag);
struct Ops31220 {
 const uint8_t* (*rec)(P flag);          // D2Client+0xC640
 P (*owner)(P unit);                      // unit+0x60
 P (*skillOf)(P own);                     // D2Client+0xCA0C
 void (*c1ba)(I);                         // result unused
 I (*skStat)(P unit,P sk,I id);           // D2Client+0xCEDA
 I (*mode)(P unit,P own);                 // D2Client+0xC89E
 I (*stat)(P unit,I id);                  // D2Client+0xC1C6 (layer 0)
 I (*o10804)(P unit,P sk,P flag,I one);   // D2Client+0xC922
 I (*bonusTotal)();                       // D2Client+0x2FB50
 int16_t (*pct1)(P sk); int16_t (*pct2)(P sk); // D2Client+0xC940 / 0xC8CE
 I (*skval)(P sk,I id,I z);               // D2Client+0xC454
 I (*o10263)(P); I (*o10975)(P);
};
I sub_31220(const Ops31220& o,const Tables& t,I base,P unit,I* mn,I* mx,I* mode,I a3,P flag,I scale,P sk,I useSkill);
struct Ops32BF0 {
 I (*o2C830)(uint8_t); I (*dmgMin)(P,I,I,I); I (*dmgMax)(P,I,I,I); I (*bonusTotal)();
 void (*calc10323)(P,I*,I*,I*); I (*o10786)(P,I,I,I);
 P (*owner)(P); P (*skillOf)(P); I (*o10417)(P); P (*o11139)(P,I);
 I (*o10744)(P,I); I (*o11006)(P); I (*o11160)(P,I); void (*o10164)(P,P,I);
 void (*o30E30)(P unit,I* mn,I* mx,I* mode,I flag);
 void (*emit)(I eax,I ecx,I s0,I s1,I s2,I s3);
};
void sub_32BF0(const Ops32BF0& o,P unit,I flag,const uint8_t* sk,I lvl,I a2,I a3,I a4);
I sub_314D0(const Ops& o,const uint8_t* self,P extra,I a0,I a1,I a2,I a3);
}
