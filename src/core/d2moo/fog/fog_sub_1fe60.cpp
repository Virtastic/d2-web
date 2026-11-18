// 1.13c Fog+0x1FE60 -- QServerNT.cpp packet enqueue: copy `size` bytes into a fresh node and append it
// to the singly linked list headed at owner+0x74. Alloc (Fog+0x1B560) and fatal (Fog+0x10F30) are seams.
#include <cstdint>
#include <cstring>

struct QServerPkt {
	uint32_t size;    // retail +0
	QServerPkt* next; // retail +4 (pointer-width here; data follows the header, retail +8)
};

struct QServerOwner {
	uint8_t pad[0x74];
	QServerPkt* head;
};

struct FogQServerApi {
	QServerPkt* (*alloc)(void* zero);                                           // Fog+0x1B560
	void (*fatal)(int level, const char* file, uint32_t line, const char* fmt); // Fog+0x10F30
};

// 1.13c Fog+0x1FE60
void fog_sub_1fe60(const FogQServerApi& api, QServerOwner* owner, const void* data, uint32_t size) {
	QServerPkt* n = api.alloc(nullptr);
	if (!n) api.fatal(2, "..\\Source\\Fog\\Src\\QServer\\QServerNT.cpp", 0x439, "");
	memcpy(n + 1, data, size);
	n->size = size;
	n->next = nullptr;
	QServerPkt** p = &owner->head;
	while (*p) p = &(*p)->next;
	*p = n;
}
