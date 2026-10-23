// 1.13c D2CMP+0x11730 (#10060): return the dword at +0x4 of the object; null is fatal in retail
#include <cstdint>
#include <cstddef>
struct CmpObj_11730 { uint8_t pad[0x4]; uint32_t dwValue; };
static_assert(offsetof(CmpObj_11730, dwValue) == 0x4, "D2CMP obj +0x4");
uint32_t __stdcall d2cmp_sub_11730(const CmpObj_11730* p) { return p->dwValue; }
