// 1.13c D2Multi+0x111A0 (191 B): entry of the chat callback table.
// Shows the joined-channel line (format 0x17720 with the channel name, the second stack word, through sprintf 0x1000;
// 0xFC90 with colour 5), copies the channel name into the module buffer at 0x38D48 and from there into the session
// record (the module dword 0x39F14) at +0x35D, and at +0x37D too when that field is empty. The eleven control handles
// at 0x38E50 go to the thunk 0x7C14 with EDX = 1, the handle at 0x39F80 to the thunk 0x7C20 with EDX = 0, then 0x10AA0
// runs. Returns 1.
#include "d2multi_env.h"
#include <cstring>

// current: the module buffer at 0x38D48; session: the record the dword at 0x39F14 points at; handles: the eleven dwords
// at 0x38E50; last: the dword at 0x39F80.
uint32_t d2multi_111a0(const D2mEnv& e, uint32_t a, uint32_t channel, char* current, char* session, const uint32_t* handles,
                       uint32_t last) {
    (void)a;
    char line[500];
    d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x17720), channel);
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, d2m_ptr(line), 5);
    strcpy(current, (const char*)(uintptr_t)channel);
    strcpy(session + 0x35d, current);
    if (session[0x37d] == 0) strcpy(session + 0x37d, current);
    for (int i = 0; i < 11; i++) d2m_call(e, 0x7c14, 0, handles[i], 1);
    d2m_call(e, 0x7c20, 0, last, 0);
    d2m_call(e, 0x10aa0);
    return 1;
}
