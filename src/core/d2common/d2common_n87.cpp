// n87 batch: D2Common STALLED rows written from the 1.13c decompile. Guest structs are 32-bit images
// (uint32_t words, guest pointers as uint32_t); the env resolves guest addresses and holds every callee.
#include <cstdint>

struct N87Env
{
	void* (*at)(uint32_t guest);					// guest address -> host pointer
	uint32_t (*alloc)(const char* file, int line, int zero);	// Ordinal_10045
	void (*fatal)(uint32_t line);					// Ordinal_10265/10024 + _exit(-1)
	void (*CallbackPlaceholder)(void);
	// callees
	void (*f7C270)(void);							// 0x7C270
	void (*f7C300)(void* a, int b, int c);					// 0x7C300
	void (*f1B0D)(void);
	uint32_t (*f208E0)(uint32_t a, uint32_t b, int c, uint32_t* pOutB, uint32_t* pOutA);
	uint32_t (*f218A0)(uint32_t a, uint32_t b, uint32_t c, int d);
	void (*f21B40)(uint32_t p);
	void (*f28E80)(uint32_t a, int b, uint32_t* c, int d, int e);
	void (*f2E2C0)(void);
	void (*f61BF0)(void); void (*f5CD10)(uint32_t u); void (*f93580)(void);
	void (*f7D420)(uint32_t u); void (*f7DC90)(void);
	uint32_t (*f80F80)(uint32_t unit); uint32_t (*f85360)(uint32_t unit, uint32_t v);
	void (*f84F80)(uint32_t path, int v); uint32_t (*f85100)(void);
	void (*f859D0)(uint32_t unit); void (*f66800)(uint32_t unit, uint32_t v);
	void (*f87570)(uint32_t v); void (*f876F0)(int v); void (*f871C0)(uint32_t v, uint32_t p);
	uint32_t (*f882B0)(void); uint32_t (*f88640)(void); void (*f881A0)(uint32_t a, uint32_t p);
	void (*f89F70)(int delta, uint32_t arg); void (*f88550)(void);
	uint16_t (*f9C570)(int y, int x, uint16_t f); uint16_t (*f9C5C0)(int x, int y, uint16_t f);
	uint32_t (*f9B3F0)(uint32_t a, int* box, uint32_t f);
	uint32_t sgptDataTables; const int32_t* levelBandTbl;		// 0x6FDE4668 (6 ints)
};

static uint32_t* W(const N87Env* e, uint32_t g) { return (uint32_t*)e->at(g); }

// 1.13c D2COMMON+0x1A0D0
extern "C" uint32_t D2Common_1A0D0(const N87Env* e, uint32_t room, uint32_t other, uint32_t* pA, uint32_t* pB)
{
	uint32_t n = W(e, other)[0x4c / 4];
	for (;; n = W(e, n)[1])
	{
		if (!n) return 0;
		if (W(e, W(e, n)[4])[0] == room) break;
	}
	uint32_t r = W(e, n)[0];
	for (uint32_t m = W(e, r)[0x4c / 4]; m; m = W(e, m)[1])
		if (W(e, m)[0] == other)
		{
			*pA = W(e, W(e, m)[4])[0];
			*pB = W(e, m)[4];
			if (!W(e, r)[0x30 / 4]) e->f7C270();
			return W(e, r)[0x30 / 4];
		}
	return 0;
}

// 1.13c D2COMMON+0x1E880
extern "C" uint32_t D2Common_1E880(const N87Env* e, uint32_t v, uint32_t pInv)
{
	if (v && pInv && W(e, pInv)[0] == 4 && W(e, pInv)[5])
		return v == W(e, W(e, pInv)[5])[0x5c / 4] ? pInv : 0;
	return 0;
}

// 1.13c D2COMMON+0x21C50
extern "C" void D2Common_21C50(const N87Env* e, uint32_t arg)
{
	static const char* f = "..\\Source\\D2Common\\INVENTORY\\Inventory.cpp";
	for (int pass = 0; pass < 2; ++pass)
	{
		uint32_t g = e->alloc(f, 0x22e, pass);
		if (g)
		{
			uint32_t* p = W(e, g);
			for (int i = 0; i < 16; ++i) p[i] = 0;
			p[0] = 0x1020304; p[1] = arg; p[2] = 0;
		}
		if (pass == 1) { e->f21B40(0); e->f21B40(g); }
	}
}

// 1.13c D2Common+0x21E10
extern "C" uint32_t D2Common_21E10(const N87Env* e, uint32_t a, uint32_t b, int c, uint32_t d, uint32_t flags)
{
	if (!a || !b || W(e, b)[0] != 4) return 0;
	uint32_t oa = b, ob = a;
	if (!e->f208E0(a, b, c, &oa, &ob)) return 0;
	return e->f218A0(oa, ob, (flags & 0xff) + 2, c);
}

// 1.13c D2Common+0x29D60
extern "C" void D2Common_29D60(const N87Env* e, uint32_t, uint32_t a, int b, uint32_t* c, int d, int f)
{
	e->f28E80(a, b, c, d, f);
}

// 1.13c D2Common+0x2E360
extern "C" void D2Common_2E360(const N87Env* e, uint32_t unit)
{
	uint32_t* u = W(e, unit);
	u[0x71] = W(e, u[0x6d])[0x470 / 4] + u[0x74];
	u[0x72] = 0x29a;
	if (u[0] == 1) e->f61BF0();
	else if (u[0] == 2) { e->f5CD10(unit); e->f7D420(unit); e->f7DC90(); return; }
	else if (u[0] == 3) { e->f93580(); e->f7D420(unit); e->f7DC90(); return; }
	e->f7D420(unit); e->f7DC90();
}

// 1.13c D2COMMON+0x36030
extern "C" void D2Common_36030(const N87Env* e, int y, uint32_t unit, int force, int x, uint32_t flag7)
{
	uint32_t g = e->alloc("..\\Source\\D2Common\\PATH\\Path.cpp", 0x453, 0);
	uint32_t* p = W(e, g);
	uint32_t* u = W(e, unit);
	for (int i = 0; i < 0x80; ++i) p[i] = 0;
	u[0xb] = g; p[0xc] = unit;
	p[0x11] = e->f80F80(unit);
	p[0x12] = e->f85360(unit, p[0x11]);
	p[0] = (uint32_t)x * 0x10000 + 0x8000;
	*(int16_t*)&p[0x76] = (int16_t)x;
	p[1] = (uint32_t)y * 0x10000 + 0x8000;
	p[0x1f] = 0x800; p[7] = flag7;
	((uint8_t*)p)[0x93] = 0; p[0x75] = 1;
	*(int16_t*)((uint8_t*)p + 0x1da) = (int16_t)y;
	if (u[0] == 0)
	{
		p[0x13] = 0x80; p[0x14] = 0x1c09;
		e->f84F80(g, 7);
		((uint8_t*)p)[0x91] = 0x49; ((uint8_t*)p)[0x92] = 0x46;
	}
	else if (u[0] == 1)
	{
		p[0x13] = 0x100;
		p[0xd] &= 0xfff800ff; p[0xf] = 2; p[0x26] = 0;
		if (p[0x10] == 0xb) e->fatal(0x871);
		if (p[0x10] == 8) e->fatal(0x872);
		int cls = (int)u[1];
		const uint32_t* dt = W(e, e->sgptDataTables);
		bool hit = false;
		if (cls >= 0)
		{
			if (cls < (int)dt[0xa80 / 4] && dt[0xa78 / 4] + cls * 0x1a8u)
				cls = *(int16_t*)((uint8_t*)e->at(dt[0xa78 / 4] + cls * 0x1a8u) + 2);
			hit = cls >= 0 && cls < (int)dt[0xa80 / 4] && cls == 0x26;
		}
		if (hit) { W(e, u[0xb])[0x48 / 4] = 5; p[0x14] = 0x804; }
		else p[0x14] = e->f85100();
		((uint8_t*)p)[0x91] = 0xe;
	}
	else if (u[0] == 3)
	{
		p[0x13] = 0; p[0x14] = 0;
		e->f84F80(g, 4);
	}
	if (flag7) { e->f859D0(unit); e->f66800(unit, p[7]); }
	p[2] = (uint32_t)((int32_t)((p[0] >> 11) - (p[1] >> 11)) >> 1);
	p[3] = (uint32_t)((int32_t)((p[1] >> 11) + (p[0] >> 11)) >> 2);
	if (force) p[0xd] |= 0x10;
}

// 1.13c D2Common+0x37AF0
extern "C" bool D2Common_37AF0(const N87Env* e, uint32_t a, uint32_t b)
{
	if (!a) e->fatal(0x547);
	uint32_t g = W(e, a)[1];
	uint32_t* p = W(e, g);
	int32_t old = (int32_t)p[1];
	int32_t lvl = 0;
	if (b) lvl = (int32_t)W(e, W(e, W(e, b)[4])[0x58 / 4])[0x1d0 / 4];
	uint32_t band = 0;
	for (int i = 1; i < 6; ++i)
		if (lvl < e->levelBandTbl[i]) { band = (uint8_t)(i - 1); break; }
	e->f87570(band); e->f876F0(lvl); e->f871C0(band, g);
	if (lvl == 0x78) { ((uint8_t*)p)[0x18] = 0xf5; ((uint8_t*)p)[0x19] = 0xf0; ((uint8_t*)p)[0x1a] = 0xff; }
	return old != (int32_t)p[1];
}

// 1.13c D2COMMON+0x3A1D0
extern "C" uint32_t D2Common_3A1D0(const N87Env* e, uint32_t o, uint32_t, int v, uint32_t, uint32_t arg)
{
	if (!o) return 0;
	uint32_t* p = W(e, o);
	int idx = (int)e->f882B0();
	uint32_t ent = 0;
	if (idx >= 0) ent = p[9] + idx * 8;
	if (!ent)
	{
		if (!v) return 0;
		ent = e->f88640();
	}
	int32_t old = (int32_t)W(e, ent)[1];
	if (v - old == 0) return 0;
	if (!v) e->f881A0(p[0], ent); else W(e, ent)[1] = v;
	e->f89F70(v - old, arg);
	if ((int32_t)p[4] < 0 && p[2] == 0) e->f88550();
	return 1;
}

// 1.13c D2COMMON+0x3C800
extern "C" void D2Common_3C800(const N87Env* e) { e->f2E2C0(); }

// 1.13c D2COMMON+0x3CCA0
extern "C" void D2Common_3CCA0(const N87Env* e, int a, void* b, int c, int d)
{
	if (!a) e->fatal(0x2d7);
	e->f7C300(b, c, d);
}

// 1.13c D2COMMON+0x4CF40
extern "C" uint32_t D2Common_4CF40(const N87Env* e, uint32_t a, int x, int y, uint32_t mode, uint32_t flags)
{
	switch (mode)
	{
	case 0: return e->f9C570(y, x, (uint16_t)flags);
	case 1: case 3: case 5: return e->f9C5C0(x, y, (uint16_t)flags);
	case 2: case 4: { int box[4] = { x - 1, y - 1, x + 1, y + 1 }; return e->f9B3F0(a, box, flags); }
	}
	return 0xffff | ((mode >> 16) << 16);
}
