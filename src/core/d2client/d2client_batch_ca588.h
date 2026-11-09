#pragma once
// Env seams of the D2Client 0xCA588 CRT batch (shared with the Fog TranslatorGuardHandler probe).
#include <cstdint>
namespace d2client_batch_ca588 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*wr32)(P, uint32_t);
 P fptrap;                     // PTR___fptrap_6fb8b8e8 .. +0x14 (6 slots)
 P cfltcvt, lab_b201, fassign, f_b1c5, lab_b24c;   // slot values
 uint32_t (*ms_p5_mp_test_fdiv)();   // CB5D2
 void (*setdefaultprecision)();      // CB624
 P datFdiv;                    // DAT_6fbd3710
 P datB780;                    // DAT_6fb8b780
 P excList;                    // ExceptionList (fs:[0]) cell
 void (*rtlUnwind)(P, P, P, P);
 P unwindRet;                  // 0x6fb7ae4a
 P translatorGuardHandler;
 uint32_t (*internalCxxFrameHandler)(P, P, P, P, P, int32_t, P, uint32_t);   // CBE55
 P (*getptd)();                // 72F7; _tiddata
 void (*callNLG)(P ptd, uint32_t code, P args);   // ptd->_NLG_dwCode
 void (*inconsistency)();      // CBF2C
 void (*jump)(P);              // indirect continuation call
 P (*getLocal8)();             // &local_8 slot for TranslatorGuardHandler
};
}
