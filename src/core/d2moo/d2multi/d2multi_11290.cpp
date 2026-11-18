// 1.13c D2Multi+0x11290: EAX, ECX, EDI inputs. Calls 0x10D10(edi, ecx, eax) (stdcall, returns a row); when DAT_6fa0a090 is set, formats
// "%s has joined the channel." with the row's name (row+0x36, or EDI when that is empty) and shows it with 0xFC90(text, 5).
#include "d2multi_env.h"
#include <cstring>

void d2multi_11290(const D2mEnv& e, uint32_t eax, uint32_t ecx, uint32_t edi, uint32_t dat0a090) {
    uint32_t row = d2m_call(e, 0x10d10, 0, 0, 0, 0, 0, edi, ecx, eax);
    if (!dat0a090) return;
    char buf[500];
    uint32_t name = strlen((const char*)(uintptr_t)(row + 0x36)) ? row + 0x36 : edi;
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(buf), D2M_MOD(0x17758), name);
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, d2m_ptr(buf), 5);
}
