// src/core/gfx/fog_batch_1000.cpp -- 12 UNMAPPED 1.13c Fog.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane, RE-DONE; no spec, no vector).
// Retail base 0x6F500000; every RVA below is retail's.
//
// The twelve are one object model: Fog's intrusive doubly-linked list (link = two 32-bit words,
// [0] = pNext, [4] = pPrev-as-tag) plus the destructors of two heap objects. The tag is a raw
// previous-link address when >= 0, or the bitwise complement of the address of the slot that points
// at the node when < 0; the >= 0 arithmetic `node + (tag - next->tag)` collapses to the previous
// link address on a consistent list. The same 54-byte unlink body was already recovered as
// Storm+0x4C20 (src/core/fn/storm_leaves_b23.cpp) -- same template.
// 0x1120 initialises the list root: +0 a word the unlink/insert arithmetic uses as a base (0 for
// every list this batch builds), +4/+8 the sentinel link whose pNext points at itself and whose tag
// is ~&sentinel.
//
// Prior art used: reference/D2MOO/source/Fog/definitions/Fog.1.10f.def and Storm's exports header
// (reference/D2MOO/source/Storm/include/Storm.h: #401 SMemAlloc, #403 SMemFree) name Fog's two
// imports; the 1.13c .data strings at RVA 0x2FF84/0x2FFE8 are ".\?AUMessage@LogManager@@" and
// ".PAX", which identify 0x1290 as LogManager::AUMessage::operator new (0x1000 its operator delete)
// and 0x13B0 as a "PAX" object's destructor. the comments records the link encoding.
//
// Host seams (retail calls out of Fog, or rows not ported): Storm #401/#403 (our engine has no
// Storm allocator -- weak so the proof instance can link a strong one) and Fog+0x1520 (the
// unmapped block free). The r[] ABI is the fn one: args 0..3 in r[0..3], result in r[4].
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <emscripten.h>
#include <Windows.h>   // DeleteCriticalSection: a kernel object, a no-op under wasm

// --- host seams ----------------------------------------------------------------------------------
// Storm #401 SMemAlloc(amount, logfilename, logline, flags).
extern "C" __attribute__((weak)) void* d2_fog_smem_alloc(int32_t amount, const char* logname,
                                                          int32_t logline, uint32_t flags) {
    (void)logname; (void)logline; (void)flags;
    return std::calloc(1, (size_t)(uint32_t)amount);
}
// Storm #403 SMemFree(location, logfilename, logline, flags).
extern "C" __attribute__((weak)) int32_t d2_fog_smem_free(void* location, const char* logname,
                                                           int32_t logline, uint32_t flags) {
    (void)logname; (void)logline; (void)flags;
    std::free(location);
    return 1;
}
// Fog+0x1520: the "PAX" heap's block release (a separate 270-byte row, not ported). It walks a
// handle table; the seam keeps only its observable entry/exit so 0x1390 stays callable.
extern "C" __attribute__((weak)) void d2_fog_sub_1520(uint32_t block) { (void)block; }

namespace
{
	struct FogLink
	{
		uint32_t pNext;   // +0: wasm/retail 32-bit address, 0 = unlinked
		uint32_t nPrev;   // +4: previous link address, or ~(slot address)
	};
	static_assert(offsetof(FogLink, nPrev) == 4, "FogLink nPrev at +4");
	static_assert(sizeof(FogLink) == 8, "FogLink is two 32-bit words");

	struct FogList
	{
		uint32_t nBase;   // +0: base the >=0 tag arithmetic is relative to; 0 for these lists
		FogLink  link;    // +4/+8: sentinel link
	};
	static_assert(offsetof(FogList, link) == 4, "FogList sentinel at +4");
	static_assert(sizeof(FogList) == 12, "FogList is three 32-bit words");

	// Retail PTR_LAB_6ff78388 (RVA 0x28388), the vtable 0x1390 stores. A load-base-relative
	// address, so the port stores its own symbol; the proof compares it minus the load base.
	uint32_t g_fog_vtable_28388;

	// 1.13c Fog+0x1310 (identical body to Storm+0x4C20): unlink one link pair.
	void fog_link_unlink(FogLink* node)
	{
		const uint32_t next = node->pNext;
		if (next == 0)
			return;
		const uint32_t tag = node->nPrev;
		FogLink* slot;
		if ((int32_t)tag < 0)
			slot = (FogLink*)(uintptr_t)~tag;
		else
			slot = (FogLink*)((uint8_t*)node + (uint32_t)(tag - ((FogLink*)(uintptr_t)next)->nPrev));
		slot->pNext = next;
		((FogLink*)(uintptr_t)next)->nPrev = tag;
		node->pNext = 0;
		node->nPrev = 0;
	}

	// 1.13c Fog+0x11F0: insert `node` before `pos` (mode 1) or after it (mode != 1); NULL means
	// the sentinel. `node` is unlinked first.
	void fog_link_insert(FogList* root, FogLink* pos, FogLink* node, int32_t mode)
	{
		FogLink* n = node ? node : &root->link;
		FogLink* p = pos ? pos : &root->link;
		fog_link_unlink(n);
		if (mode != 1)
		{
			FogLink* next = (FogLink*)(uintptr_t)p->pNext;
			n->pNext = p->pNext;
			n->nPrev = next->nPrev;
			next->nPrev = (uint32_t)(uintptr_t)node;   // retail stores param_1, not the sentinel
			p->pNext = (uint32_t)(uintptr_t)n;
			return;
		}
		n->pNext = (uint32_t)(uintptr_t)p;
		n->nPrev = p->nPrev;
		const uint32_t tag = p->nPrev;
		uint32_t base = root->nBase;
		if ((int32_t)tag < 0)
		{
			((FogLink*)(uintptr_t)~tag)->pNext = (uint32_t)(uintptr_t)n;
			p->nPrev = (uint32_t)(uintptr_t)node;
			return;
		}
		if ((int32_t)base < 0)
			base = (uint32_t)((uint8_t*)p - (uint8_t*)(uintptr_t)((FogLink*)(uintptr_t)p->pNext)->nPrev);
		((FogLink*)(uintptr_t)(tag + base))->pNext = (uint32_t)(uintptr_t)n;
		p->nPrev = (uint32_t)(uintptr_t)node;
	}

	// 1.13c Fog+0x1350: drain from the tail while sentinel.prev (> 0) is a node address.
	void fog_list_drain(FogList* root)
	{
		for (;;)
		{
			const uint32_t node = root->link.nPrev;
			if ((int32_t)node <= 0)
				return;
			fog_link_unlink((FogLink*)(uintptr_t)node);
		}
	}
} // namespace

// 1.13c Fog+0x1000: LogManager::AUMessage::operator delete(p).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1000(uint32_t* r)
{
	if (r[0] != 0)
		d2_fog_smem_free((void*)(uintptr_t)r[0], "delete", -1, 0);
}

// 1.13c Fog+0x1030: DeleteCriticalSection(EAX) -- the register argument is r[0] here.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1030(uint32_t* r)
{
	DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)r[0]);
}

// 1.13c Fog+0x1040: SEH-framed call of 0x1080; the frame setup has no observable side effect.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1040(uint32_t* r)
{
	fog_link_unlink((FogLink*)(uintptr_t)r[0]);
	fog_link_unlink((FogLink*)(uintptr_t)r[0]);   // 0x1080's second pass; re-reads the cleared node
}

// 1.13c Fog+0x1080: unlink the pair at the argument. The decompile renders the body twice; the
// second pass re-reads a node the first pass cleared and is a no-op on every input.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1080(uint32_t* r)
{
	fog_link_unlink((FogLink*)(uintptr_t)r[0]);
	fog_link_unlink((FogLink*)(uintptr_t)r[0]);
}

// 1.13c Fog+0x1120: list-root constructor.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1120(uint32_t* r)
{
	FogList* root = (FogList*)(uintptr_t)r[0];
	root->link.pNext = 0;
	root->link.nPrev = 0;
	root->link.pNext = (uint32_t)(uintptr_t)&root->link;
	root->nBase = 0;
	root->link.nPrev = ~(uint32_t)(uintptr_t)&root->link;
}

// 1.13c Fog+0x1170: list-root destructor -- drain, then unlink the sentinel.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1170(uint32_t* r)
{
	FogList* root = (FogList*)(uintptr_t)r[0];
	fog_list_drain(root);
	fog_link_unlink(&root->link);
}

// 1.13c Fog+0x11F0: insert(r[2]) at r[1] (NULL = sentinel) in the root at r[0], mode r[3].
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_11f0(uint32_t* r)
{
	fog_link_insert((FogList*)(uintptr_t)r[0], (FogLink*)(uintptr_t)r[1],
	                (FogLink*)(uintptr_t)r[2], (int32_t)r[3]);
}

// 1.13c Fog+0x1290: LogManager::AUMessage::operator new(size, flags); mode r[3] non-zero inserts
// the 0x24-byte header into the root at r[0]. Returns the block (r[4]).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1290(uint32_t* r)
{
	FogLink* node = (FogLink*)(uintptr_t)d2_fog_smem_alloc(
		(int32_t)(r[1] + 0x24), ".?AUMessage@LogManager@@", -2, r[2] | 8);
	if (node != 0)
	{
		node->pNext = 0;
		node->nPrev = 0;
		if ((int32_t)r[3] != 0)
			fog_link_insert((FogList*)(uintptr_t)r[0], 0, node, (int32_t)r[3]);
	}
	r[4] = (uint32_t)(uintptr_t)node;
}

// 1.13c Fog+0x1310: unlink the pair in EAX (r[0] here).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1310(uint32_t* r)
{
	fog_link_unlink((FogLink*)(uintptr_t)r[0]);
}

// 1.13c Fog+0x1350: drain the root in EBX (r[0] here).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1350(uint32_t* r)
{
	fog_list_drain((FogList*)(uintptr_t)r[0]);
}

// 1.13c Fog+0x1390: destructor -- store the vtable at +0, then release the block at +4.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_1390(uint32_t* r)
{
	uint32_t* obj = (uint32_t*)(uintptr_t)r[0];
	obj[0] = (uint32_t)(uintptr_t)&g_fog_vtable_28388;
	if (obj[1] != 0)
		d2_fog_sub_1520(obj[1]);
}

// 1.13c Fog+0x13B0: "PAX" object destructor -- free the block at +8.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_fog_13b0(uint32_t* r)
{
	uint32_t* obj = (uint32_t*)(uintptr_t)r[0];
	if (obj[2] != 0)
		d2_fog_smem_free((void*)(uintptr_t)obj[2], ".PAX", -2, 0);
}
