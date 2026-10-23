// D2Net CRT process shutdown (VS2003 _doexit family). Side effects are injected so the control flow is a pure function.
#include <cstdint>
struct D2NetExit {
    int exiting;                 // DAT_6fbfade4
    int exitFlag;                // DAT_6fbfade0
    uint8_t quick;               // DAT_6fbfaddc
    void (*lock8)();  void (*unlock8)();
    void (*runAtexit)();         // the DAT_6fbfb7b0..b7ac function-pointer walk (reverse)
    void (*termPre)();  void (*termPost)();  // initterm(6fbfa020), initterm(6fbfa028)
    void (*flush)();             // FUN_6fbf14b0
    void (*terminate)(unsigned); // TerminateProcess(self, code)
    void (*exitProcess)(unsigned); // 1.13c D2Net+0x133D target
};
// 1.13c D2Net+0x133D: ___crtExitProcess(code) -> ExitProcess(code); no return.
extern "C" void d2net_crt_exit_process_133d(unsigned code, void (*exitProcess)(unsigned)) { exitProcess(code); }
// 1.13c D2Net+0x1401: _doexit(code, quick, retain)
extern "C" void d2net_doexit_1401(D2NetExit* s, unsigned code, int quick, int retain) {
    s->lock8();
    if (s->exiting == 1) s->terminate(code);
    s->exitFlag = 1;
    s->quick = (uint8_t)retain;
    if (quick == 0) { s->runAtexit(); s->termPre(); }
    s->termPost();
    s->flush();
    if (retain) return;
    s->exiting = 1;
    d2net_crt_exit_process_133d(code, s->exitProcess);
}
// 1.13c D2Net+0x14C4: _exit(code) = _doexit(code, 0, 0)
extern "C" void d2net_exit_14c4(D2NetExit* s, int code) { d2net_doexit_1401(s, (unsigned)code, 0, 0); }
