// The name a monster's plate shows and the line under it, as the 1.13c client composes them from the
// monster assignment's fields: a super unique's own name, a champion's kind, a unique's rolled name, a
// hireling's name from the pet list, and the type and enchantment line of a unique or its minions.
#pragma once
#include <cstdint>
#include <string>

struct D2UnitStrc;

namespace d2::realm {

struct MonsterNameInput {
    D2UnitStrc* unit = nullptr;          // the client unit (class and type tests)
    uint32_t classId = 0;
    uint32_t typeFlags = 0;              // 0x02 super unique, 0x04 champion, 0x08 unique, 0x10 minion
    uint32_t nameSeed = 0;
    int32_t superUnique = -1;
    const uint8_t* umods = nullptr;      // the assignment's enchantment bytes, at most nine
    uint32_t umodCount = 0;
    uint32_t hirelingNameId = 0;         // nonzero for a listed hireling
};

std::u16string clientMonsterName(const MonsterNameInput& in);
// Empty when the monster is neither a unique nor a minion: the caller keeps the class's own type line.
std::u16string clientMonsterEnchantLine(const MonsterNameInput& in);
std::string clientUtf8(const std::u16string& text);

}  // namespace d2::realm
