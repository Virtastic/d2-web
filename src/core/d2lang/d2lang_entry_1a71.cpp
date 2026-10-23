// 1.13c D2Lang+0x01A71: CRT DllMain entry (reason 0 detach, 1 attach, 2 thread attach, 3 thread detach).
#include <cstdint>

typedef int (*d2lang_dllmain_fn)(uint32_t, int, uint32_t);
typedef int (*d2lang_crt_init_fn)(uint32_t, int);
typedef int (*d2lang_a8c0_fn)();

// Seams: DAT_6fc105c8 (attach refcount), DAT_6fc11e24 (user DllMain hook), __CRT_INIT@12 (0x18F0), FUN 0xA8C0.
extern "C" {
int32_t d2lang_1a71_attached = 0;
d2lang_dllmain_fn d2lang_1a71_hook = nullptr;
d2lang_crt_init_fn d2lang_1a71_crt_init = nullptr;
d2lang_a8c0_fn d2lang_1a71_a8c0 = nullptr;

int d2lang_entry_1a71(uint32_t hinst, int reason, uint32_t reserved)
{
	int ret = 1;
	if (reason == 0 && d2lang_1a71_attached == 0) return 0;
	if (reason == 1 || reason == 2) {
		if (d2lang_1a71_hook) ret = d2lang_1a71_hook(hinst, reason, reserved);
		if (ret == 0 || d2lang_1a71_crt_init(hinst, reason) == 0) return 0;
	}
	ret = d2lang_1a71_a8c0();
	if (reason == 1 && ret == 0) d2lang_1a71_crt_init(hinst, 0);
	if (reason == 0 || reason == 3) {
		if (d2lang_1a71_crt_init(hinst, reason) == 0) ret = 0;
		if (ret != 0 && d2lang_1a71_hook) ret = d2lang_1a71_hook(hinst, reason, reserved);
	}
	return ret;
}
}
