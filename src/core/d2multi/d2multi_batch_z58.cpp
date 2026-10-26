// src/core/d2multi/d2multi_batch_z58.cpp -- D2Multi.dll CRT doexit (batch z58).
// Other z58 rows (0x1274 0x1333 0x1363 0x136C 0x1375 0x138D 0x14A6 0x14BA 0x14CB 0x14DC 0x14EB) already carry markers.
#include <cstdint>

struct D2MultiDoexitSeam {
    uint32_t* state_exiting;   // DAT_6f9ebac4
    uint32_t* state_doexit;    // DAT_6f9ebac0
    uint8_t*  state_retcaller; // DAT_6f9ebabc
    uint32_t* onexit_begin;    // DAT_6fa0b4b0
    uint32_t** onexit_end;     // DAT_6fa0b4ac (cursor)
    void (*lock8)();           // __lock(8)
    void (*initterm_a)();      // __initterm(&DAT_6f9e9028)
    void (*initterm_b)();      // __initterm(&DAT_6f9e9030)
    void (*unlock_tail)();     // FUN_6f9d14a6
    void (*terminate)(uint32_t); // TerminateProcess(GetCurrentProcess(), code)
    void (*exit_process)(uint32_t); // ___crtExitProcess
    void (*call32)(uint32_t);  // call a registered onexit entry (32-bit code ptr)
};

// 1.13c D2MULTI+0x13F7  _doexit(code, quick, retcaller)
void d2multi_z58_doexit(const D2MultiDoexitSeam& s, uint32_t code, int quick, int retcaller) {
    s.lock8();
    if (*s.state_exiting == 1) s.terminate(code);
    *s.state_doexit = 1;
    *s.state_retcaller = (uint8_t)retcaller;
    if (quick == 0) {
        if (s.onexit_begin) {
            while (*s.onexit_end -= 1, s.onexit_begin <= *s.onexit_end)
                if (**s.onexit_end) s.call32(**s.onexit_end);
        }
        s.initterm_a();
    }
    s.initterm_b();
    s.unlock_tail();
    if (retcaller) return;
    *s.state_exiting = 1;
    s.exit_process(code);
}
