// 1.13c D2CMP+0x11520 (#10065, CloseCelFile): release every frame of a version-6 cel file.
// File layout read: +0x00 version (must be 6, fatal 0x5F1), +0x10 directions, +0x14 frames per direction,
// +0x18.. an inline array of directions*framesPerDirection frame pointers. Each frame holds a resource
// pointer at +0x18. The first frame's resource is saved on entry and must be non-null (fatal 0x5F5; a null
// first frame pointer faults). Then for every slot whose frame and resource are non-null: the release
// callback global (D2CMP+0x77EF8, re-read after each call) is called with ECX = resource when set, and the
// slot's frame resource is cleared (the slot is re-read after the call). Finally the saved resource goes to
// the Fog pool free (#10043, ECX = resource, EDX = the file string, line 0x61D, flag 0). Returns 1; a null
// file returns 0.
#include <cstdint>
#include <cstring>

struct D2Cmp11520Env {
	uint32_t (*releaseCallback)();               // the global at D2CMP+0x77EF8 (0 = none)
	void (*callRelease)(uint32_t resource);      // call it with ECX = resource
	void (*poolFree)(uint32_t resource, uint32_t line, uint32_t flag);
	void (*fatal)(uint32_t line);                // does not return in retail (_exit(-1))
};

static uint32_t celRd(uint32_t base, uint32_t off)
{
	uint32_t v;
	std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(base) + off), 4);
	return v;
}
static void celWr(uint32_t base, uint32_t off, uint32_t v)
{
	std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(base) + off), &v, 4);
}

// 1.13c D2CMP+0x11520
extern "C" uint32_t d2cmp_11520_run(uint32_t file, const D2Cmp11520Env* env)
{
	if (file == 0)
		return 0;
	if (celRd(file, 0) != 6)
	{
		env->fatal(0x5F1);
		return 0;
	}
	const uint32_t saved = celRd(celRd(file, 0x18), 0x18);
	if (saved == 0)
	{
		env->fatal(0x5F5);
		return 0;
	}
	uint32_t slot = file + 0x18;
	for (uint32_t i = 0; i < celRd(file, 0x14) * celRd(file, 0x10); ++i, slot += 4)
	{
		const uint32_t frame = celRd(slot, 0);
		if (frame != 0 && celRd(frame, 0x18) != 0)
		{
			if (env->releaseCallback() != 0)
				env->callRelease(celRd(frame, 0x18));
			celWr(celRd(slot, 0), 0x18, 0);
		}
	}
	env->poolFree(saved, 0x61D, 0);
	return 1;
}
