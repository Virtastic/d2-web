#include "client_monster_name.h"

#include <D2DataTbls.h>
#include <D2Lang.h>
#include <D2Monsters.h>
#include <D2Seed.h>
#include <DataTbls/MonsterTbls.h>
#include <Units/Units.h>

#include "../d2client/client_string_algorithms.h"

const Unicode* DATATBLS_RollRandomUniquePrefixStringSeam(D2UnitStrc* pUnit, const Unicode* (*pfnLookup)(short));
const Unicode* DATATBLS_RollRandomUniqueSuffixStringSeam(D2UnitStrc* pUnit, const Unicode* (*pfnLookup)(short));
const Unicode* DATATBLS_RollRandomUniqueAppellationStringSeam(D2UnitStrc* pUnit, const Unicode* (*pfnLookup)(short));

namespace d2::realm {
namespace {
constexpr uint32_t kSuperUnique = 0x02, kChampion = 0x04, kUnique = 0x08, kMinion = 0x10;

const char16_t* wide(const Unicode* text) { return text ? reinterpret_cast<const char16_t*>(text) : u""; }
const char16_t* str(uint32_t index) { return wide(D2LANG_GetStringFromTblIndex(static_cast<short>(index))); }
const Unicode* lookup(short index) { return D2LANG_GetStringFromTblIndex(index); }

std::u16string format(uint32_t formatIndex, const char16_t* a, const char16_t* b, const char16_t* c = nullptr) {
    char16_t out[2000] = {};
    const char16_t* args[3] = {a, b, c};
    d2::client::formatSelectors(out, str(formatIndex), args, c ? 3 : 2);
    return out;
}

bool nameUmod(uint32_t umod) { return umod == 1 || umod == 12 || umod == 16 || (umod >= 36 && umod <= 39); }

// One run of the name step for one enchantment. False when it leaves the name as it was.
bool nameStep(const MonsterNameInput& in, uint32_t umod, std::u16string& name) {
    if (in.typeFlags & kSuperUnique) {
        const D2SuperUniquesTxt* record = DATATBLS_GetSuperUniquesTxtRecord(in.superUnique);
        if (!record) return false;
        name = str(record->wNameStr ? record->wNameStr : 0x1506);
        return true;
    }
    if (in.typeFlags & kChampion) {
        static const uint32_t kinds[4][2] = {{0x10, 0xc94}, {0x24, 0x2b4c}, {0x25, 0x2b4d}, {0x26, 0x2b4e}};
        uint32_t kind = 0x2b4f;
        for (const auto& entry : kinds) if (entry[0] == umod) { kind = entry[1]; break; }
        const D2MonStatsTxt* row = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(in.classId));
        const std::u16string kindText = str(kind), base = str(row ? row->wNameStr : 0x1506);
        name = format(0x2b40, kindText.c_str(), base.c_str());
        return true;
    }
    D2UnitStrc roller{};
    SEED_InitLowSeed(&roller.pSeed, static_cast<int>(in.nameSeed));
    uint32_t fixed = 0;
    if (umod == 0x0c) fixed = 0xc27;
    else switch (in.classId) {
    case 0x9c: fixed = 0xbcd; break;
    case 0xd3: case 0x2c4: fixed = 0xbee; break;
    case 0xf2: case 0x2c0: fixed = 0xbf6; break;
    case 0xf3: case 0x14d: case 0x2c1: fixed = 0xbf4; break;
    case 0x100: case 0x2c2: fixed = 0x3f6; break;
    case 0x220: case 0x2c5: fixed = 0xbf5; break;
    case 0x2c3: fixed = 0x2b92; break;
    default: break;
    }
    if (fixed) { name = str(fixed); return true; }
    {
        const std::u16string suffix = wide(DATATBLS_RollRandomUniqueSuffixStringSeam(&roller, lookup));
        const std::u16string prefix = wide(DATATBLS_RollRandomUniquePrefixStringSeam(&roller, lookup));
        name = format(0x6b9, prefix.c_str(), suffix.c_str());
    }
    if (SEED_RollLimitedRandomNumber(&roller.pSeed, 100) < 0x32) {
        const std::u16string appellation = wide(DATATBLS_RollRandomUniqueAppellationStringSeam(&roller, lookup));
        const std::u16string suffix = wide(DATATBLS_RollRandomUniqueSuffixStringSeam(&roller, lookup));
        const std::u16string prefix = wide(DATATBLS_RollRandomUniquePrefixStringSeam(&roller, lookup));
        name = format(0x6ba, prefix.c_str(), suffix.c_str(), appellation.c_str());
    }
    return true;
}

uint32_t enchantString(uint32_t umod) {
    switch (umod) {
    case 5: return 3205; case 6: return 3206; case 7: return 3207; case 8: return 3208; case 9: return 3209;
    case 17: return 3211; case 18: return 3210; case 24: return 3217; case 25: return 3212; case 26: return 3214;
    case 27: return 3213; case 28: return 3215; case 29: return 3216; case 30: return 3218;
    default: return 0;
    }
}
}  // namespace

std::u16string clientMonsterName(const MonsterNameInput& in) {
    const D2MonStatsTxt* row = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(in.classId));
    std::u16string name = row ? str(row->wNameStr) : u"";
    if (in.hirelingNameId) return str(in.hirelingNameId);
    // The enchantments run only when the first byte is set, and the name step only for a unique: once for the
    // fixed first enchantment, then once for every name-bearing byte of the list. The last run stands.
    if (!in.umods || !in.umodCount || !in.umods[0] || !(in.typeFlags & kUnique)) return name;
    nameStep(in, 1, name);
    for (uint32_t i = 0; i < in.umodCount && i < 9; ++i)
        if (nameUmod(in.umods[i])) nameStep(in, in.umods[i], name);
    return name;
}

std::u16string clientMonsterEnchantLine(const MonsterNameInput& in) {
    if (!(in.typeFlags & (kUnique | kMinion))) return {};
    const uint32_t type = MONSTERS_IsDemon(in.unit) ? 0x275e : MONSTERS_IsUndead(in.unit) ? 0x275d : 0;
    std::u16string line;
    if (in.typeFlags & kMinion) {
        if (type) { line += str(type); line += str(0xf9b); }
        line += str(0xc95);
        return line;
    }
    uint32_t count = 0;
    if (type) { line += str(type); count = 1; }
    if (in.classId >= 0x2c0 && in.classId <= 0x2c5) return line;
    for (uint32_t i = 0; i < in.umodCount && i < 9; ++i) {
        const uint32_t id = enchantString(in.umods[i]);
        if (!id) continue;
        if (count) line += u' ';
        line += str(id);
        ++count;
    }
    return line;
}

std::string clientUtf8(const std::u16string& text) {
    std::string out;
    for (size_t i = 0; i < text.size(); ++i) {
        uint32_t c = text[i];
        if (c >= 0xd800 && c < 0xdc00 && i + 1 < text.size() && text[i + 1] >= 0xdc00 && text[i + 1] < 0xe000)
            c = 0x10000 + ((c - 0xd800) << 10) + (text[++i] - 0xdc00);
        if (c < 0x80) out += static_cast<char>(c);
        else if (c < 0x800) { out += static_cast<char>(0xc0 | (c >> 6)); out += static_cast<char>(0x80 | (c & 0x3f)); }
        else if (c < 0x10000) { out += static_cast<char>(0xe0 | (c >> 12)); out += static_cast<char>(0x80 | ((c >> 6) & 0x3f)); out += static_cast<char>(0x80 | (c & 0x3f)); }
        else { out += static_cast<char>(0xf0 | (c >> 18)); out += static_cast<char>(0x80 | ((c >> 12) & 0x3f)); out += static_cast<char>(0x80 | ((c >> 6) & 0x3f)); out += static_cast<char>(0x80 | (c & 0x3f)); }
    }
    return out;
}

}  // namespace d2::realm
