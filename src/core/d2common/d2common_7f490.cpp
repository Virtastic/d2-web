// 1.13c D2Common+0x7F490: EAX = a, ECX = obj, EDX = b, EDI = d, plain ret; returns 1 (overlap) or 0. 0x3BA60(obj, &r) fills a rect r = {r0, r1, r2, r3};
// the tests are signed: b + d < r0 chooses the left branch (0 when b - d > r0 + r2), then a + d >= r1 gives 1, else a - d <= r1 + r3 gives 1 (right branch: no left test).
#include <cstdint>
typedef void (*D2C7F490Rect)(uint32_t obj, int32_t* r);
extern "C" uint32_t d2c_7f490_port(int32_t a, uint32_t obj, int32_t b, int32_t d, D2C7F490Rect rect)
{
	int32_t r[4];
	rect(obj, r);
	if (b + d < r[0] && b - d > r[0] + r[2]) return 0;
	if (a + d >= r[1]) return 1;
	return (a - d <= r[1] + r[3]) ? 1u : 0u;
}
