// D2Client 0x666D0: draw one unit through 0x6CC00. Callees are the Env virtuals, named by RVA.
#pragma once
#include <cstdint>
namespace d2client_666d0 {
struct Globals { int32_t cc418, c9960, c995c; };  // 0x11C418, 0x119960, 0x11995C
struct Env {
    virtual ~Env() = default;
    virtual int32_t  c1fc(const uint8_t* unit, int32_t idx) = 0;                 // stdcall(unit, stat/flag idx)
    virtual int32_t  d07e() = 0;                                                  // D2gfx 10013: renderer mode (0 = software path)
    virtual uint32_t c1ae(const uint8_t* unit) = 0;                               // unit x
    virtual uint32_t c232(const uint8_t* unit) = 0;                               // unit y
    virtual int32_t  d09c(uint32_t p, uint32_t q) = 0;
    virtual int32_t  f66620(const uint8_t* unit) = 0;                             // ESI = unit
    virtual int32_t  c226(uint32_t id) = 0;
    virtual const uint8_t* c65e(const uint8_t* unit) = 0;
    virtual void     d132(uint32_t p, uint32_t q, int32_t k, int32_t* ox, int32_t* oy, int32_t flag) = 0;
    virtual int32_t  c1d2(const uint8_t* unit) = 0;
    virtual int32_t  c1f6(const uint8_t* unit) = 0;
    virtual int32_t  cc00(const uint8_t* unit, uint32_t arg, int32_t x, int32_t y, int32_t e, int32_t one) = 0; // ECX = unit, EDX = arg
    virtual void     f728b0(const uint8_t* unit) = 0;                             // EAX = unit
    virtual void     f72830(const uint8_t* unit) = 0;
    virtual int32_t  f4c110(const uint8_t* unit) = 0;                             // ECX = unit
    virtual const uint8_t* f1580(int32_t id) = 0;                                 // EAX = id; record with s16 at +0xA2 +0xA4 +0xA6
    virtual void     dd138() = 0;
};
// unit is writable (+0xC4 |= 0x10000000)
void draw_unit(Env& env, const Globals& g, uint8_t* unit, int32_t skip, uint32_t arg);
}
