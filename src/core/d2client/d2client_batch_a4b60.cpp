// src/core/d2client/d2client_batch_a4b60.cpp -- 1.13c D2Client.dll rows, reimplemented from the Ghidra decompile.
// and the unrecovered callees 0xA4A90/0xA4DA0 are host seams in D2ClientA4b60Env. Memory is read
// through rd32/rd_ptr so pointers stay uintptr_t. Several rows take args in registers (retail
// __usercall); the signatures name them (EAX, EBX, ESI, EDI).
#include <cstdint>
#include <emscripten.h>

#include "d2client_a4b60_env.h"

static bool unit_is_a(uint32_t t) { return t == 2 || (t > 3 && t < 6); }

// unit x / y as the retail inline pair: types 2,4,5 read the pointed record (+0xc / +0x10).
static int32_t unit_x(uintptr_t u, const D2ClientA4b60Env* e) {
    uintptr_t p = e->rd_ptr(u + 0x2c);
    if (unit_is_a(e->rd32(u))) return (int32_t)e->rd32(p + 0xc);
    return p ? e->o10867(p) : 0;
}
static int32_t unit_y(uintptr_t u, const D2ClientA4b60Env* e) {
    uintptr_t p = e->rd_ptr(u + 0x2c);
    if (unit_is_a(e->rd32(u))) return (int32_t)e->rd32(p + 0x10);
    return p ? e->o10750(p) : 0;
}

// 1.13c D2Client+0xA4B60 (caller 0x4DC70): EAX = obj; null asserts (line 0xA34); obj+0x58 = 3.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a4b60(uintptr_t eax, const D2ClientA4b60Env* e) {
    if (!eax) { e->fatal(0xa34); return; }   // retail: Fog assert then _exit(-1), no return
    e->wr32(eax + 0x58, 3);
}

// 1.13c D2Client+0xA4B90 (caller 0x6CC00): EAX = obj; null asserts (0xA19); if +0x58==0 false;
// else decrement it and return (thunk 10494() == 0).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a4b90(uintptr_t eax, const D2ClientA4b60Env* e) {
    if (!eax) e->fatal(0xa19);
    uint32_t v = e->rd32(eax + 0x58);
    if (v == 0) return 0;
    e->wr32(eax + 0x58, v - 1);
    return e->o10494() == 0;
}

// 1.13c D2Client+0xA4C50 (caller 0xA54A0): ESI = obj. 10511()==0 -> true; r=10858(): if non-null and
// r+0x10 != 0x12 -> true; if byte r+0x16 not in {0,0xff} -> byte <= (int)obj+0x38 >> 8; else false.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a4c50(uintptr_t esi, const D2ClientA4b60Env* e) {
    if (e->o10511() == 0) return 1;
    uintptr_t r = e->o10858();
    if (r) {
        if ((e->rd32(r + 0x10) & 0xff) != 0x12) return 1;
        uint32_t b = e->rd32(r + 0x16) & 0xff;
        if (b != 0 && b != 0xff) return (int32_t)b <= ((int32_t)e->rd32(esi + 0x38) >> 8);
    }
    return 0;
}

// 1.13c D2Client+0xA52A0 (caller 0xA5C50): walk both 0x80-slot chains (next at +0xE4); 1 on first
// 0xA4A90 hit.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a52a0(uintptr_t param_1, const D2ClientA4b60Env* e) {
    for (int t = 0; t < 2; t++)
        for (int i = 0; i < 0x80; i++)
            for (uintptr_t u = e->heads[t][i]; u; u = e->rd_ptr(u + 0xe4))
                if (e->a4a90(u, param_1)) return 1;
    return 0;
}

// 1.13c D2Client+0xA5310 (caller 0xA5D90): EAX = obj (may be null -> -1), arg. r=10093(arg, obj+4);
// r ? 10014(r)+0x1be : 0.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_a5310(uintptr_t eax, uint32_t arg, const D2ClientA4b60Env* e) {
    uint32_t k = eax ? e->rd32(eax + 4) : 0xffffffffu;
    uintptr_t r = e->o10093(arg, k);
    return r ? e->o10014(r) + 0x1be : 0;
}

// 1.13c D2Client+0xA5360 (caller 0x47970): EAX = obj; null asserts (0x977); 0xA4DA0(table 0x10AA08).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a5360(uintptr_t eax, const D2ClientA4b60Env* e) {
    if (!eax) e->fatal(0x977);
    e->a4da0(0x10AA08);
}

// 1.13c D2Client+0xA5540 (caller 0x4B8C8): ESI = unit. If the current object (0x11BBFC) is kind 1
// or 5: its +0x2c must be non-null (0x802); call 10739(that, x, y, 0) with unit x/y.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a5540(uintptr_t esi, const D2ClientA4b60Env* e) {
    uintptr_t c = e->cur;
    if (!c) return;
    uint32_t k = e->rd32(c + 0x10);
    if (k != 1 && k != 5) return;
    uintptr_t a = e->rd_ptr(c + 0x2c);
    if (!a) e->fatal(0x802);
    int32_t y = unit_y(esi, e), x = unit_x(esi, e);
    e->o10739(a, x, y, 0);
}

// 1.13c D2Client+0xA5600 (caller 0x4B8C8): EAX = obj with +0x2c (null asserts 0x7f3); 11090(); then
// 10739(obj+0x2c, x, y, 0) with x/y of the current object (0x11BBFC).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a5600(uintptr_t eax, const D2ClientA4b60Env* e) {
    uintptr_t a = e->rd_ptr(eax + 0x2c);
    if (!a) e->fatal(0x7f3);
    e->o11090();
    int32_t y = unit_y(e->cur, e), x = unit_x(e->cur, e);
    e->o10739(a, x, y, 0);
}

// 1.13c D2Client+0xA56C0 (caller 0xA6070): EAX = index into the D2Common table (count +0xba0, base
// +0xb98, stride 0x23c). byte rec+0x15 in 1..nclass-1 -> classify[byte]() != 0.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a56c0(int32_t eax, const D2ClientA4b60Env* e) {
    uintptr_t dt = e->rd_ptr(e->data_tables);
    if (eax < 0 || eax >= (int32_t)e->rd32(dt + 0xba0)) return 0;
    uintptr_t rec = (uintptr_t)eax * 0x23c + e->rd_ptr(dt + 0xb98);
    if (!rec) return 0;
    uint32_t b = e->rd32(rec + 0x15) & 0xff;
    if (b == 0 || b >= e->nclass) return 0;
    return e->classify[b]() != 0;
}

// 1.13c D2Client+0xA5720 (caller 0xA68E0): EAX = unit, stack param_1 = py, EBX = px, ESI = y bias,
// EDI = rect struct (+0x1a0..+0x1ac). Hit test of (px,py) against the unit's transformed box.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a5720(uintptr_t eax, int32_t param_1, int32_t ebx, int32_t esi, uintptr_t edi,
                         const D2ClientA4b60Env* e) {
    int32_t x = unit_x(eax, e), y = unit_y(eax, e);
    e->o11087(&x, &y);
    int32_t l = (int32_t)e->rd32(edi + 0x1a0) + y;
    int32_t t = (int32_t)e->rd32(edi + 0x1a4) + esi;
    return l <= ebx && t <= param_1 && ebx <= (int32_t)e->rd32(edi + 0x1a8) + l &&
           param_1 <= (int32_t)e->rd32(edi + 0x1ac) + t;
}

// 1.13c D2Client+0xA57E0 (caller 0x51470): EAX = unit (kind must be 5, else assert), stack param_1,
// EBX, ESI. r=10633(10331()); r ? box test against r+4..+0x10 : 0. The decompile's second axis uses
// ESI as the base; the 11087 outputs land in dead stack slots, so the pre-transform y is compared.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a57e0(uintptr_t eax, int32_t param_1, int32_t ebx, int32_t esi,
                         const D2ClientA4b60Env* e) {
    if (e->rd32(eax) != 5) e->fatal(0);
    uintptr_t r = e->o10633(e->o10331(eax));
    if (!r) return 0;
    int32_t y = unit_y(eax, e), x = unit_x(eax, e);
    int32_t ty = y, tx = x;
    e->o11087(&tx, &ty);
    int32_t l = (int32_t)e->rd32(r + 4) + y;
    int32_t t = (int32_t)e->rd32(r + 8) + esi;
    return l <= ebx && t <= param_1 && ebx <= (int32_t)e->rd32(r + 0xc) + l &&
           param_1 <= (int32_t)e->rd32(r + 0x10) + t;
}

// 1.13c D2Client+0xA59E0 (caller 0x72610): ESI = unit, BL = old, stack param_1 = new. If unit kind
// is 1 and old != new and 10331(unit) non-null: new==2 -> 10345(r); else (old==2 -> 10184(r)); then
// 10704().
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a59e0(uintptr_t esi, uint8_t bl, uint8_t param_1, const D2ClientA4b60Env* e) {
    if (!esi || e->rd32(esi) != 1 || bl == param_1) return;
    uintptr_t r = e->o10331(esi);
    if (!r) return;
    if (param_1 == 2) e->o10345(r);
    else if (bl == 2) e->o10184(r);
    e->o10704();
}
