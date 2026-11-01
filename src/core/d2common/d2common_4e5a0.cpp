// 1.13c D2Common+0x4E5A0: skill flag test. Returns the row's flag byte at +6 masked by gdwBitMasks[1] (1<<1).
#include <cstdint>
#include "D2DataTbls.h"

extern "C" uint32_t D2Common_4E5A0_GetSkillFlag(int skillId)
{
    const auto* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) {
        return 0;
    }
    return static_cast<uint32_t>(reinterpret_cast<const uint8_t*>(row)[6]) & (1u << 1);
}
