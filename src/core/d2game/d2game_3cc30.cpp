// 1.13c D2Game+0x3CC30: MISSMODE_FistOfTheHeavensDelay_AuraCallback, the register-convention rule over seams.
#include "d2game_auracb.h"
int32_t D2Game_3CC30(const void* aura, const void* tgt, const D2GameAuraCbEnv* e) { return auracb::Run(aura, tgt, true, e); }
