// 1.13c Fog+0x12120 (#10212): free an Excel string linker.
// The linker (0x10 bytes) holds the code table at +0x08 and the string-tree root at +0x0C. The root's
// two children (+0x24, +0x28) are freed through the subtree free at +0x120A0 (each only if non-null),
// then the root, the code table (if non-null) and the linker itself go back to the pool through
// +0x1B0C0, which takes the address of a slot holding the block. A null linker returns at once.
// The three pool frees carry the source file and lines 0x346 / 0x35D / 0x360.
#include <cstdint>
#include <cstring>

struct Fog12120Env {
	void (*treeFree)(uint32_t node);                                  // Fog+0x120A0, ECX = node
	void (*poolFree)(uint32_t* slot, uint32_t line);                  // Fog+0x1B0C0, ECX = &slot, file = Excel.cpp
};

static uint32_t fogRd(uint32_t base, uint32_t off)
{
	uint32_t v;
	std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(base) + off), 4);
	return v;
}

// 1.13c Fog+0x12120
extern "C" void fog_12120_run(uint32_t linker, const Fog12120Env* env)
{
	if (linker == 0)
		return;
	uint32_t root = fogRd(linker, 0xC);
	if (root != 0)
	{
		uint32_t left = fogRd(root, 0x24);
		if (left != 0)
			env->treeFree(left);
		uint32_t right = fogRd(root, 0x28);
		if (right != 0)
			env->treeFree(right);
		env->poolFree(&root, 0x346);
	}
	uint32_t table = fogRd(linker, 0x8);
	if (table != 0)
		env->poolFree(&table, 0x35D);
	env->poolFree(&linker, 0x360);
}
