// src/core/d2common/d2common_batch_2f5c0.cpp -- four 1.13c D2Common.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c). Image base 0x6FD50000.
//
// Retail records are opaque here and are read by their retail byte offsets (rd8/rd16/rd32), with
// retail addresses travelling as 32-bit integers in the style of the other d2common batches.
// Callees src/ does not implement are function-pointer seams on D2CommonBatch2F5C0Env, handed over
// after the retail arguments; a missing seam contributes zero. The paths that end in _exit(-1) are
// modelled as plain returns, so the exported body never faults.
//
// The eight further rows of this batch whose 1.13c bodies are the vendored functions are marked in
// place where those bodies live (Units.cpp, D2Inventory.cpp, D2Skills.cpp, Items.cpp, the DataTbls
// sources).
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace
{
	inline const uint8_t* P(uint32_t a) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }

	inline uint8_t  rd8 (const void* p, uint32_t o) { uint8_t  v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 1); return v; }
	inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 2); return v; }
	inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }

	inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8(P(a), o); }
	inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16(P(a), o); }
	inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32(P(a), o); }
}

// Host seams for the callees the cached decompiles name but this file does not implement.
struct D2CommonBatch2F5C0Env {
	// 0x81FB0(record, sub, out24): the inventory grid layout the record asks for.
	int32_t  (*sub81FB0)(int32_t nRec, int32_t nSub, uint8_t* pOut24);
	// 0x1E1B0(local24): the grid descriptor (width +8, height +9, cells +0xC) or zero.
	uint32_t (*sub1E1B0)(const uint8_t* pInfo);
	// 0x1050(cell): the cell's first word.
	uint32_t (*sub1050)(uint32_t pCell);
	// 0x1080(cell): the cell's second word.
	uint32_t (*sub1080)(uint32_t pCell);
};

// 1.13c D2Common+0x3C1B0 (#10767)
// Walks the node chain rooted at record+0x10 and answers the first node whose rectangle contains
// the point: x (argument 2) inside [node+0x5C, node+0x5C+node+0x64) and y (argument 3) inside
// [node+0x60, node+0x60+node+0x68). The next node is node+0x7C; an exhausted chain answers null.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3c1b0(uint32_t pRecord, int32_t nX, int32_t nY)
{
	uint32_t pNode = pRecord != 0 ? rd32(pRecord, 0x10) : 0;
	while (pNode != 0) {
		const int32_t nLeft = (int32_t)rd32(pNode, 0x5C);
		const int32_t nTop = (int32_t)rd32(pNode, 0x60);
		if (nLeft <= nX && nX < nLeft + (int32_t)rd32(pNode, 0x64) &&
		    nTop <= nY && nY < nTop + (int32_t)rd32(pNode, 0x68)) {
			return pNode;
		}
		pNode = rd32(pNode, 0x7C);
	}
	return 0;
}

// 1.13c D2Common+0x3D120 (#10778)
// Counts the qualifying nodes of the list at record+8 -- a node qualifies when its second dword
// (node+4) is zero -- and returns the first word of the node at index nIndex. A node's successor
// is at node+8. No such node, or a short list, answers 0xFFFF.
extern "C"
EMSCRIPTEN_KEEPALIVE uint16_t d2_fn_d2common_3d120(uint32_t pRecord, int32_t nIndex)
{
	uint32_t pNode = pRecord != 0 ? rd32(pRecord, 8) : 0;
	int32_t nSeen = 0;
	while (pNode != 0) {
		if (rd32(pNode, 4) == 0) {
			if (nSeen == nIndex) {
				return rd16(pNode, 0);
			}
			++nSeen;
		}
		pNode = rd32(pNode, 8);
	}
	return 0xFFFF;
}

// 1.13c D2Common+0x3B1A0 (#10779)
// A record field at record+0x10 points at a second structure; this answers its +0x58 pointer plus
// 0x1E0. A null record is the 1.13c assert at line 0x5B9 (fatal); it answers null here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3b1a0(uint32_t pRecord)
{
	if (pRecord == 0) {
		return 0;
	}
	return rd32(rd32(pRecord, 0x10), 0x58) + 0x1E0;
}

// 1.13c D2Common+0x1EF90 (#10801)
// An inventory (magic 0x01020304 at +0) answers the cell pointer at (x, y) of the grid layout that
// 0x81FB0 builds for record nRec: x must be in [0, width] and y in [0, height] (the counts are the
// bytes at +8 and +9 of the grid descriptor), and the cell is at cells(+0xC)[width * y + x]. A
// non-null cell has its two words copied to the two out-pointers
// through 0x1050 and 0x1080. Every other path answers null.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_1ef90(uint32_t pInventory, int32_t nX, int32_t nY,
                                                   uint32_t* pOut1, uint32_t* pOut2, int32_t nRec,
                                                   const D2CommonBatch2F5C0Env* env)
{
	if (pInventory == 0 || rd32(pInventory, 0) != 0x01020304) {
		return 0;
	}

	uint8_t info[24];
	std::memset(info, 0, sizeof(info));
	if (env->sub81FB0 == nullptr) {
		return 0;
	}
	env->sub81FB0(nRec, 0, info);

	const uint32_t pGrid = env->sub1E1B0 != nullptr ? env->sub1E1B0(info) : 0;
	if (pGrid == 0) {
		return 0;
	}

	const int32_t nWidth = (int32_t)rd8(pGrid, 8);
	const int32_t nHeight = (int32_t)rd8(pGrid, 9);
	if (nX < 0 || nX > nWidth || nY < 0 || nY > nHeight) {
		return 0;
	}

	const uint32_t pCell = rd32(rd32(pGrid, 0xC) + (uint32_t)(nWidth * nY + nX) * 4, 0);
	if (pCell == 0) {
		return 0;
	}

	if (pOut1 != nullptr && env->sub1050 != nullptr) {
		*pOut1 = env->sub1050(pCell);
	}
	if (pOut2 != nullptr && env->sub1080 != nullptr) {
		*pOut2 = env->sub1080(pCell);
	}
	return pCell;
}
