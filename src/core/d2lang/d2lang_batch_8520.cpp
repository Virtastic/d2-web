// 1.13c D2Lang+0x8520 / +0x9FB0
#include <cstdint>
#include <cstring>
#include <cstdlib>

// 1.13c D2Lang+0x8520: widen up to 20 bytes of set, true when the wide string's last unit is in that set.
extern "C" bool d2lang_8520(const char* set, const uint16_t* wide)
{
	uint16_t buf[21] = {0};
	int n = 0;
	while (n < 20 && set[n]) { buf[n] = (uint8_t)set[n]; n++; }
	int len = 0;
	if (wide) while (wide[len]) len++;
	for (int i = 0; i < n; i++)
		if (wide[len - 1] == buf[i]) return true;
	return false;
}

// 1.13c D2Lang+0x9FB0: missing-string cache; node = {wide*, key copy, next}. Seams do the Fog alloc and widening.
struct d2lang_9fb0_node { void* wide; char* key; d2lang_9fb0_node* next; };
extern "C" {
d2lang_9fb0_node* d2lang_9fb0_head = nullptr;
const char* d2lang_9fb0_suffix = "";                      // DAT 0x6FC10568 text
void* (*d2lang_9fb0_alloc)(int size) = nullptr;           // Fog alloc (ordinal 10042)
void* (*d2lang_9fb0_widen)(const char* s) = nullptr;      // alloc 0x273 + unicodeWidth/toUnicode
void* d2lang_9fb0(const char* key)
{
	for (d2lang_9fb0_node* n = d2lang_9fb0_head; n; n = n->next)
		if (!strcmp(key, n->key)) return n->wide;
	d2lang_9fb0_node* n = (d2lang_9fb0_node*)d2lang_9fb0_alloc(sizeof *n);
	n->next = d2lang_9fb0_head;
	d2lang_9fb0_head = n;
	n->key = (char*)d2lang_9fb0_alloc((int)strlen(key) + 1);
	strcpy(n->key, key);
	char* buf = (char*)d2lang_9fb0_alloc((int)(strlen(key) + strlen(d2lang_9fb0_suffix) + 1));
	strcpy(buf, key);
	strcat(buf, d2lang_9fb0_suffix);
	n->wide = d2lang_9fb0_widen(buf);
	return n->wide;
}
}
