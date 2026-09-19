// Load-time fixups retail applies to a .bin table after reading it into memory
// (D2Common DATATBLS_Load*Txt, 1.13c). Each function takes the raw record array and mutates
// it in place to what sgptDataTables->pXxx holds; the comments list what each one does
// and where it came from. Record layouts: D2MOO DataTbls headers, offsets verified by the
// s02 diff against the captured memory.
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include "../lang/strtable.h"
#include "../../assets/mpqvfs.h"

namespace d2 {

// Applies the fixup for `table` (lower-case name as in the .bin) if one is implemented.
// Returns false when no fixup exists for that table (the .bin is then expected to be final).
bool applyTableFixups(const std::string& table, uint8_t* records, size_t count, size_t recSize, const LangTables& lang, const MpqVfs& vfs);

} // namespace d2
