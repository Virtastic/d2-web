// 1.13c D2Win+0x12410 (size 739): word-wrap a UTF-16 string to a pixel width; returns a singly linked list of lines.
#include <cstdint>
#include <cstring>
struct D2Win12410Env {
  void* (*alloc)(uint32_t line, uint32_t);                    // Fog #10042
  int32_t (*width)(const uint16_t* s, int32_t n);             // D2Win+0x120C0
  int32_t (*isLineBreak)(const uint16_t* s, uint32_t n);      // Unicode::isLineBreak
  bool (*isWhitespace)(const uint16_t* s);                    // Unicode::isWhitespace
};
struct D2Win12410Node { uint32_t str; uint32_t next; };  // retail layout: 8 bytes, 32-bit ptrs (heap addresses < 4 GiB)

static D2Win12410Node* mk(D2Win12410Env& e, const uint16_t* src, int32_t n) {
  auto* nd = (D2Win12410Node*)e.alloc(0x588, 0);
  uint16_t* str = (uint16_t*)e.alloc(0x591, 0); nd->str = (uint32_t)(uintptr_t)str; nd->next = 0;
  memcpy(str, src, (size_t)n * 2);  // Unicode::strncpy (src is NUL-terminated within n in the first case)
  str[n] = 0;
  return nd;
}

// 1.13c D2Win+0x12410
D2Win12410Node* d2win_sub_12410(D2Win12410Env& e, const uint16_t* s, int32_t* count, int32_t maxW) {
  int32_t len = 0; while (s[len]) len++;
  *count = 0;
  if (e.width(s, len) <= maxW) { *count = 1; return mk(e, s, len + 1); }
  D2Win12410Node *head = nullptr, *tail = nullptr;
  for (int32_t st = 0; st <= len;) {
    auto fit = [&] { int32_t x = len; while (st < x && e.width(s + st, x + 1 - st) > maxW) x--; return x; };
    int32_t x = fit();
    if (st < x) while (x != len && !e.isLineBreak(s + st, x - st + 1)) { if (--x <= st) break; }
    if (x == st) x = fit();
    const uint16_t* p = s + st; int32_t n = x - st + 1;
    if (e.isWhitespace(p)) { p++; n--; }
    auto* nd = mk(e, p, n);
    ++*count;
    if (tail) tail->next = (uint32_t)(uintptr_t)nd; else head = nd;
    tail = nd;
    st = x + 1;
  }
  return head;
}
