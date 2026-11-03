// 1.13c D2Game+0x3CD00: MISSMODE_LightningFury_AuraCallback, the register-convention rule over seams.
#include "d2game_auracb.h"
int32_t D2Game_3CD00(const void* aura, const void* tgt, const D2GameAuraCbEnv* e) { return auracb::Run(aura, tgt, false, e); }
