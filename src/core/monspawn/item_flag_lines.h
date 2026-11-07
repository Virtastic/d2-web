#pragma once

#include <cstdint>
#include <string>

namespace d2::item_text {

inline constexpr uint32_t kSocketedItemFlag = 0x00000800u;
inline constexpr uint32_t kEtherealItemFlag = 0x00400000u;
inline constexpr int kEtherealStringId = 22745;   // "Ethereal (Cannot be Repaired)"
inline constexpr int kSocketedStringId = 3453;    // "Socketed"

// The flag-driven line retail's tooltip builder (D2Client+0x8E180) fills. The tooltip text is built bottom
// line first (the name and Cost go on last, shop tooltips draw Cost on top), so this line is the BOTTOM
// line of the tooltip, under the property lines. Formerly this comment placed it right after the name:
// before the Defense / Damage / Durability / requirement lines: the Ethereal line when the ethereal flag
// is set, then "Socketed (N)" when the socketed flag is set, each in D2Win colour 3 (blue). Lines are
// '\n'-separated with the engine's "\x01<digit>" colour prefix; empty when neither flag is set.
// `ethereal` / `socketed` are the two table strings, `space` the table's space string (id 3995).
inline std::string flagLines(uint32_t itemFlags, int sockets, const std::string& ethereal,
                             const std::string& socketed, const std::string& space) {
    std::string out;
    if (itemFlags & kEtherealItemFlag) out += "\x01" "3" + ethereal;
    if (itemFlags & kSocketedItemFlag) {
        if (!out.empty()) out += ", ";   // D2Client+0x8E180: one line, "Ethereal ..., Socketed (n)"
        else out += "\x01" "3";
        out += socketed + space + "(" + std::to_string(sockets) + ")";
    }
    return out;
}

}  // namespace d2::item_text
