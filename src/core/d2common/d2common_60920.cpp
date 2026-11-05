// 1.13c D2Common+0x60920 (stdcall, one argument: the archive; ret 4).
// Builds the nine-entry field table of experience.txt on the stack (eight dword columns at offsets 0..0x1C, then the
// "end" terminator), compiles it through 0x5EF40 (archive, "experience", fields, no record-count out, record size 0x20)
// and stores the result in the experience-table slot [+0xA0B50].
#include <cstdint>

struct D2Common60920Field { const char* szName; int32_t nType, nLength, nOffset; const void* pLink; };
struct D2Common60920Env {
	uint32_t (*compile)(uint32_t hArchive, const char* szName, const D2Common60920Field* pFields, uint32_t pCount, uint32_t nSize);
	void (*store)(uint32_t pTable);
};

extern "C" void D2Common_60920_LoadExperienceTxt(uint32_t hArchive, const D2Common60920Env* e) {   // 1.13c D2Common+0x60920
	const D2Common60920Field fields[9] = {
		{"Amazon", 2, 0, 0x00, nullptr},    {"Sorceress", 2, 0, 0x04, nullptr}, {"Necromancer", 2, 0, 0x08, nullptr},
		{"Paladin", 2, 0, 0x0C, nullptr},   {"Barbarian", 2, 0, 0x10, nullptr}, {"Druid", 2, 0, 0x14, nullptr},
		{"Assassin", 2, 0, 0x18, nullptr},  {"ExpRatio", 2, 0, 0x1C, nullptr},  {"end", 0, 0, 0, nullptr},
	};
	e->store(e->compile(hArchive, "experience", fields, 0, 0x20));
}
