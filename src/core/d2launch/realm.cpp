// 1.13c D2Launch+0x9F40 -- IsBetaRealm(const Unicode* name) -> 1/0
#include <cstdint>

namespace {
// The seven ASCII names retail converts from .rdata (RVA 0x1EB30..0x1EB7F) into its .bss wide
// slots 0x6FA65F08..0x6FA66B08 and compares the caller's wide name against, case-sensitively
// (D2Lang Unicode::strcmp). Exact match, NUL-terminated UTF-16LE.
const char16_t* const kRealmNames[] = {
    u"Testv104", u"FatRealm", u"BetaWest", u"BetaEurope",
    u"BetaUSWest", u"BetaUSEast", u"BetaAsia",
};
}  // namespace

// 1.13c D2Launch+0x9F40
extern "C" int d2launch_sub_9f40(const char16_t* name) {
    for (const char16_t* n : kRealmNames) {
        const char16_t* a = name;
        while (*a != 0 && *a == *n) { ++a; ++n; }
        if (*a == *n) return 1;  // both NUL: equal
    }
    return 0;
}
