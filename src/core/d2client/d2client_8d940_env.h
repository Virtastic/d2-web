// Seam for the 0x8D940..0x8E770 tooltip builders (d2client_batch_8d940.cpp); the FN ports include it.
#pragma once
#include <cstdint>
namespace d2client_batch_8d940 {
typedef uint16_t wc;
struct Env {
 virtual ~Env()=default;
 virtual const wc* str(int32_t slot)=0;                 // Ordinal_10003: localized string / colour code, call order per function
 virtual void fmt(char* out,int32_t n,int32_t v)=0;     // Ordinal_578 (_snprintf with DAT_6fb834c4 / 6fb835a0 "%d")
 virtual int32_t q(int32_t what,int32_t a,int32_t b)=0; // generic D2Common/D2Client query (what = ordinal)
 virtual int32_t g(int32_t what)=0;                     // D2Client global read (what = RVA of the global)
 virtual void gset(int32_t what,int32_t v)=0;
 virtual void draw(int32_t a,int32_t b,int32_t c,int32_t d,int32_t e,int32_t f)=0; // Ordinal_10041/10150/10184
 virtual bool personalize(wc* out,const wc* in,int32_t n)=0; // Unicode::Personalize(.., lang)
 // Ordinal_10041 as retail calls it: the cel block (n dwords on the caller's stack) and the five stack words after it
 virtual void cel(const uint32_t*,int32_t,int32_t,int32_t,int32_t,int32_t,int32_t){ __builtin_trap(); }
 // a register-passing import (ECX, EDX) with three stack words; answers its EAX
 virtual int32_t qr(int32_t,uint32_t,uint32_t,int32_t,int32_t,int32_t){ __builtin_trap(); }
};
// FN-probe base: every seam traps unless the port under test overrides it.
struct TrapEnv : Env {
 const wc* str(int32_t) override { __builtin_trap(); }
 void fmt(char*,int32_t,int32_t) override { __builtin_trap(); }
 int32_t q(int32_t,int32_t,int32_t) override { __builtin_trap(); }
 int32_t g(int32_t) override { __builtin_trap(); }
 void gset(int32_t,int32_t) override { __builtin_trap(); }
 void draw(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t) override { __builtin_trap(); }
 bool personalize(wc*,const wc*,int32_t) override { __builtin_trap(); }
};
struct Imp8d940 { int32_t(*o10630)(int32_t,int32_t); int32_t(*o10306)(int32_t,int32_t,int32_t);
 int32_t(*o10567)(int32_t,int32_t,int32_t,int32_t); int32_t(*o10297)(int32_t,int32_t,int32_t,int32_t); };
void tooltipRequirement(Env& e,const Imp8d940& f,int32_t player,int32_t unit,const uint8_t* data,wc* out);
void tooltipField10a(Env& e,const uint8_t* data,int32_t add,wc* out);
void tooltipNumber(Env& e,int32_t v,wc* out);
void tooltipStat46(Env& e,int32_t unit,const uint8_t* rec,wc* out);
}
