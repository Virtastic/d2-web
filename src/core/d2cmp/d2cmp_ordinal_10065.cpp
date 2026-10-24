// 1.13c D2CMP+0x11520 ordinal 10065 CloseCelFile: real extern "C" entry for importers (D2Client+0xC18A thunk).
#include <cstdint>
struct D2CMPCelFileOwner; struct D2CMPCelCloseOps { void (**releaseFrame)(void*); void (*releaseFile)(void*, uint32_t, uint32_t); };
extern "C" uint32_t d2cmp_11520(D2CMPCelFileOwner* file, const D2CMPCelCloseOps& ops);
extern "C" void Fog_Ordinal_10043(int32_t, int32_t);
// ponytail: DAT_6fe87ef8 (frame-release hook) is null in this build; wire when the D2CMP init row owns it.
static void (*g_releaseFrame)(void*) = nullptr;
static void releaseFile(void*, uint32_t line, uint32_t flags) { Fog_Ordinal_10043((int32_t)line, (int32_t)flags); }
extern "C" uint32_t D2CMP_Ordinal_10065(void* file) {
	const D2CMPCelCloseOps ops{&g_releaseFrame, releaseFile};
	return d2cmp_11520((D2CMPCelFileOwner*)file, ops);
}
