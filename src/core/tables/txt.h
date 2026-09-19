// Tab-separated .txt table as retail reads it (D2Common DATATBLS_*): first line is the header,
// every further line a row, cells split on '\t', lines end "\r\n". Row 0 is the first data row.
// A row that is entirely "Expansion" (the classic/expansion separator) is kept as a row, because
// retail keeps it and row INDICES are what the binary tables and the game code use.
//
// This is the generic reader; per-table typed views (skills, levels, treasureclassex, ...) land
// beside it as their parity vectors arrive. Keep this dumb: no trimming, no case folding --
// the compiled .bin and every index derived from it depend on the text exactly as shipped.
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace d2 {

struct TxtTable {
    std::vector<std::string> columns;               // header cells
    std::vector<std::vector<std::string>> rows;     // rows[r][c]; short rows are padded with ""
    int column(const char* name) const;             // -1 if absent (exact, case-sensitive like retail)
    const std::string& cell(size_t r, int c) const { static const std::string e; return c < 0 ? e : rows[r][(size_t)c]; }
    // Numeric cells: empty -> 0, like retail's atoi on a missing value.
    int32_t num(size_t r, int c) const;
};

// Parses in place; never fails on shape (a malformed table is a parity diff, found by vectors).
TxtTable parseTxt(const uint8_t* data, size_t len);

} // namespace d2
