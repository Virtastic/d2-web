// 1.13c D2Common+0x4D140: spiral search for a free spot near (*pPos) in a level (FieldTbls.cpp).
// Callees go through an env of function pointers (no direct engine/Win32 deps). FUN_6fd51330 takes
// level in EAX, x in EBX, y in EDI: modelled as getRoom(level,x,y), level = param_1 (otherwise unused).
#include <cstdint>

struct D2Common4D140Env
{
	void* (*getRoom)(void* pLevel, int x, int y);			// 0x1330
	uint16_t (*checkSpot)(void* pRoom, int x, int y, uint32_t a, uint32_t b);	// 0x4CE00
	void* (*allocUnit)(void);						// 0x7E410
	void (*initUnit)(void* p, uint32_t a, uint32_t b);		// 0x7E390
	int (*testUnit)(void* p, void* pRoom, int x, int y, uint32_t c);	// 0x7E6C0
	void (*freeUnit)(void* p);					// 0x7E3D0
	void (*fatal)(uint32_t line);					// Ordinal_10265/10024 + _exit
	void (*assertFree)(void);					// Ordinal_10046 ("FieldTbls.cpp", 0x94, 0)
};

extern "C" int* D2Common_4D140_FindFreeSpot(const D2Common4D140Env* e, void* pLevel, int* pPos, uint32_t* pUnit,
	uint32_t p4, uint32_t p5, uint32_t p6, int p7, int p8, int p9)
{
	if (!pPos) e->fatal(0x726);
	const int y0 = pPos[1], x0 = pPos[0];
	void* pU = 0;
	if (pUnit)
	{
		pU = e->allocUnit();
		if (!pU) e->fatal(0x730);
		e->initUnit(pU, pUnit[0], pUnit[1]);
	}
	auto ok = [&](void* r, int x, int y) {
		return r && e->checkSpot(r, x, y, p4, p5) == 0 && (!pUnit || e->testUnit(pU, r, x, y, p6) == 0);
	};
	void* r = e->getRoom(pLevel, x0, y0);
	if (ok(r, x0, y0))
	{
		if (pU) e->freeUnit(pU);
		return (int*)r;
	}
	int best = -1;
	if (1 < p8)
	{
		int hiY = y0 + 1, loY = y0 - 1, d40 = -2, d44 = 2, s48 = 1, hiX = x0, loX = x0;
		do
		{
			best = -1;
			if (loY <= hiY)
			{
				int xs = loX - 1, a38 = -x0 - 1 + loX, a34 = s48, y = loY;
				do
				{
					if (xs <= hiX + 1)
					{
						int d = xs - x0, a3c = x0 - xs, x = xs;
						do
						{
							void* rr = e->getRoom(pLevel, x, y);
							if (ok(rr, x, y))
							{
								int i7 = d < 0 ? a3c : d;
								int i8 = a38 < 0 ? a34 : a38;
								if (best == -1 || i8 + i7 < best) { pPos[0] = x; pPos[1] = y; best = i8 + i7; }
							}
							x += d44; d += d44; a3c += d40;
						} while (x <= hiX + 1);
					}
					a38 += p9; a34 -= p9; y += p9;
				} while (y <= hiY);
			}
			if (loX <= hiX)
			{
				int a38 = -x0 + loX, a3c = s48 - 1, x = loX;
				do
				{
					if (loY <= hiY)
					{
						int d = loY - y0, a34 = y0 - loY, y = loY;
						do
						{
							void* rr = e->getRoom(pLevel, x, y);
							if (ok(rr, x, y))
							{
								int i12 = a38 < 0 ? a3c : a38;
								int i7 = d < 0 ? a34 : d;
								if (best == -1 || i7 + i12 < best) { pPos[0] = x; pPos[1] = y; best = i7 + i12; }
							}
							y += d44; d += d44; a34 += d40;
						} while (y <= hiY);
					}
					a38 += p9; x += p9; a3c -= p9;
				} while (x <= hiX);
			}
			if (best != -1) break;
			s48 += p9; hiY += p9; d44 += p9 * 2; d40 += p9 * -2; hiX += p9; loX -= p9; loY -= p9;
		} while ((1 - x0) + hiX < p8);
	}
	if (pU) e->assertFree();
	if (best == -1 && p7 == 0) return 0;
	return (int*)e->getRoom(pLevel, pPos[0], pPos[1]);
}
