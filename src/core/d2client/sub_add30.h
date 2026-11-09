#pragma once
#include <cstdint>
namespace d2client_add30 {
struct Env {
 uint32_t (*find)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t stk0); // D2Client+0xA4E20: eax=id&0x7f, ecx=id, edx=bucket, stack idx
 void (*f720B0)(uint32_t eax, uint32_t ecx);                                // D2Client+0x720B0: eax=p[6], ecx=found
 void (*f73630)(uint32_t eax, uint32_t stk0);                               // D2Client+0x73630: eax=p[6], stack found
};
void sub_add30(const Env& e, const uint8_t* p);
}
