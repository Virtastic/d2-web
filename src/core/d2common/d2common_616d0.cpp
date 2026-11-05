// 1.13c D2Common+0x616D0 (EDI = the archive, plain ret).
// Sixteen code tables, each a two-entry field table on the stack: {column, type, 0, 0, &linker slot} then {"end"}.
// For each: a linker is created (Fog 10211 through thunk 0x924C with DataTbls.cpp and a line), stored in its slot, and
// the table is compiled through 0x5EF40 (archive, file, fields, record-count out, record size); the result goes to its
// table slot. compcode always loads; the other fifteen only when text compilation ([+0xA145C]) is on. A count out of
// 0 means the caller's shared stack scratch word.
#include <cstdint>

struct D2Common616D0Field { const char* szName; int32_t nType, nLength, nOffset; uint32_t nLinkRva; };
struct D2Common616D0Env {
	uint32_t (*rd)(uint32_t nRva);
	void (*wr)(uint32_t nRva, uint32_t nValue);
	uint32_t (*link)(uint32_t nLine);
	uint32_t (*compile)(uint32_t hArchive, const char* szFile, const D2Common616D0Field* pFields, uint32_t nCountRva, uint32_t nSize);
};

namespace {
struct Load { uint32_t nLine; const char* szFile; const char* szColumn; int32_t nType; uint32_t nLinkRva, nTableRva, nCountRva, nSize; };
const Load kLoads[16] = {
	{0x073, "compcode", "code", 10, 0x9FF58, 0x9FF54, 0x9FF5C, 4},
	{0x084, "playerclass", "code", 10, 0x9FEDC, 0x9FED8, 0, 4},
	{0x090, "bodylocs", "code", 10, 0x9FEE4, 0x9FEE0, 0, 4},
	{0x09C, "storepage", "code", 10, 0x9FEEC, 0x9FEE8, 0, 4},
	{0x0A8, "elemtypes", "code", 10, 0x9FEF4, 0x9FEF0, 0, 4},
	{0x0B8, "hitclass", "code", 10, 0x9FEFC, 0x9FEF8, 0, 4},
	{0x0C4, "colors", "code", 10, 0xA088C, 0xA0888, 0, 4},
	{0x0D0, "hiredesc", "code", 10, 0x9FF90, 0x9FF8C, 0, 4},
	{0x0DC, "monmode", "code", 10, 0x9FF04, 0x9FF00, 0, 4},
	{0x0E8, "plrmode", "code", 10, 0x9FF0C, 0x9FF08, 0, 4},
	{0x0F4, "monai", "AI", 0x11, 0x9FF64, 0x9FF60, 0x9FF68, 2},
	{0x100, "monplace", "code", 0x11, 0xA0978, 0xA0974, 0xA097C, 2},
	{0x10C, "skillcalc", "code", 10, 0x9FF14, 0x9FF10, 0, 4},
	{0x118, "misscalc", "code", 10, 0x9FF34, 0x9FF30, 0, 4},
	{0x123, "skills", "skill", 0x11, 0x9FF48, 0x9FF44, 0, 2},
	{0x12F, "events", "event", 0x11, 0x9FF50, 0x9FF4C, 0, 2},
};
void LoadOne(uint32_t hArchive, const D2Common616D0Env* e, const Load& l) {
	const D2Common616D0Field fields[2] = { {l.szColumn, l.nType, 0, 0, l.nLinkRva}, {"end", 0, 0, 0, 0} };
	e->wr(l.nLinkRva, e->link(l.nLine));
	e->wr(l.nTableRva, e->compile(hArchive, l.szFile, fields, l.nCountRva, l.nSize));
}
}

extern "C" void D2Common_616D0_LoadCodeTables(uint32_t hArchive, const D2Common616D0Env* e) {   // 1.13c D2Common+0x616D0
	LoadOne(hArchive, e, kLoads[0]);
	if (!e->rd(0xA145C)) return;
	for (int i = 1; i < 16; ++i) LoadOne(hArchive, e, kLoads[i]);
}
