// The one VFS + string-table set the retail-gated checks share (owned by tables_check.cpp).
#pragma once
#include "../lang/strtable.h"
#include "../../assets/mpqvfs.h"
namespace d2 {
MpqVfs& sharedVfs();
const LangTables& sharedLang();   // loads the three .tbl files on first use
}
