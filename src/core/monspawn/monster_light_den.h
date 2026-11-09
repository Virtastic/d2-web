#pragma once

#include <cstdint>

namespace d2::monspawn {

// D2Client 1.13c+0x4C150, the monster light setter: on Den of Evil (level 8), with the Den quest byte
// (g_QuestAvail[1]) set, a monster whose unit flags lack bit 21 and whose MonStats nAlign is neither 1
// nor 2 takes radius 3. Every other monster takes max(item light, MonStats2 nLight), and the light
// allocator clamps that to 18; zero means no light.
constexpr int32_t kDenOfEvilLightRadius = 3;
constexpr int32_t kMonsterLightRadiusMax = 18;

constexpr bool denOfEvilLightOverride(int32_t levelId, bool denQuestAvail, uint32_t unitFlags, uint8_t nAlign) {
    return levelId == 8 && denQuestAvail && (unitFlags & 0x200000u) == 0 && nAlign != 1 && nAlign != 2;
}

constexpr int32_t monsterLightRadius(bool denOverride, int32_t nLight, int32_t itemLight) {
    if (denOverride) return kDenOfEvilLightRadius;
    const int32_t light = nLight > itemLight ? nLight : itemLight;
    return light > 0 ? (light > kMonsterLightRadiusMax ? kMonsterLightRadiusMax : light) : 0;
}

}  // namespace d2::monspawn
