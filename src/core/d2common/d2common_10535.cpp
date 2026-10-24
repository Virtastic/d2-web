// 1.13c D2Common+0x71A40 (ordinal 10535): return the items.txt table header {count, base, ...}.
#include "D2DataTbls.h"

extern "C" D2ItemDataTbl* D2Common_10535_GetItemDataTables()
{
	return &sgptDataTables->pItemDataTables;
}
