// Phase 1 worker split: runs d2_session_tick on one pthread
// worker, opt-in from play/gameplay-shell.mjs (?worker=). Nothing in src/core calls into this file. No static
// data, no string literals, no thread_local, no EM_ASM, every function no_instrument_function: the file adds
// code and nothing else to the module's layout.
#include <emscripten.h>
#include <emscripten/threading.h>
#include <emscripten/stack.h>
#include <emscripten/heap.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#define D2W extern "C" EMSCRIPTEN_KEEPALIVE __attribute__((no_instrument_function))
#define D2W_LOCAL static __attribute__((no_instrument_function))

extern "C" int d2_session_tick(int frames);

// One per worker, calloc'd. posted/done count batches; k < 0 is selftest kind -k.
struct Ctl { int32_t posted, done, k, ready, last, stop, wantKb; const char* arg; double busyMs; };

D2W_LOCAL Ctl* ctl(double h) { return (Ctl*)(uintptr_t)h; }
D2W_LOCAL int32_t ld(int32_t* p) { return __atomic_load_n(p, __ATOMIC_SEQ_CST); }
D2W_LOCAL void st(int32_t* p, int32_t v) { __atomic_store_n(p, v, __ATOMIC_SEQ_CST); }
D2W_LOCAL int stack_kb() { return (int)((emscripten_stack_get_base() - emscripten_stack_get_end()) / 1024); }

D2W_LOCAL int32_t checksum(const char* path) {
  if (!path) return -1;
  int fd = open(path, O_RDONLY);
  if (fd < 0) return -1;
  uint8_t buf[4096];
  uint32_t sum = 0;
  ssize_t n;
  while ((n = read(fd, buf, sizeof buf)) > 0)
    for (ssize_t i = 0; i < n; i++) sum = sum * 31 + buf[i];
  close(fd);
  return (int32_t)(sum & INT_MAX);
}

// Runs on the worker. 1: grow memory and write a sentinel above the old end; 2: checksum c->arg through
// the FS (proxied to main, which must keep servicing queued calls); 3: this thread's stack in KB.
D2W_LOCAL int32_t worker_selftest(Ctl* c, int kind) {
  if (kind == 1) {
    size_t old = __builtin_wasm_memory_size(0) * 65536;
    if (!emscripten_resize_heap(old + 65536)) return 0;
    uint8_t* p = (uint8_t*)old;
    for (int i = 0; i < 16; i++) p[i] = (uint8_t)(i * 37 + 11);
    return 1;
  }
  if (kind == 2) return checksum(c->arg);
  if (kind == 3) return stack_kb();
  return 0;
}

D2W_LOCAL void* d2w_main(void* p) {
  Ctl* c = (Ctl*)p;
  int32_t seen = 0;
  st(&c->ready, 1);
  for (;;) {
    emscripten_futex_wait(&c->posted, (uint32_t)seen, INFINITY);
    int32_t now = ld(&c->posted);
    if (now == seen) continue;
    seen = now;
    if (ld(&c->stop)) {
      st(&c->done, seen);
      emscripten_futex_wake(&c->done, INT_MAX);
      return 0;
    }
    double t0 = emscripten_get_now();
    int k = ld(&c->k);
    // One call per batch: d2_session_tick resets and publishes the packet stream per call, so k calls of
    // tick(1) are not tick(k).
    int32_t r = k >= 0 ? d2_session_tick(k) : worker_selftest(c, -k);
    c->busyMs = emscripten_get_now() - t0;
    st(&c->last, r);
    st(&c->done, seen);
    emscripten_futex_wake(&c->done, INT_MAX);
  }
}

// Main thread: wait for the batch in flight, servicing calls the worker proxies here (FS, JS imports).
D2W_LOCAL int wait_until(Ctl* c, double deadline) {
  for (;;) {
    int32_t d = ld(&c->done);
    if (d == ld(&c->posted)) return 1;
    if (emscripten_get_now() >= deadline) return 0;
    emscripten_current_thread_process_queued_calls();
    emscripten_futex_wait(&c->done, (uint32_t)d, 1.0);
  }
}

D2W_LOCAL int post(Ctl* c, int k) {
  if (!c || ld(&c->stop)) return 0;
  wait_until(c, INFINITY);
  st(&c->k, k);
  __atomic_add_fetch(&c->posted, 1, __ATOMIC_SEQ_CST);
  emscripten_futex_wake(&c->posted, 1);
  return 1;
}

D2W double d2_web_worker_start(int stack_kb_req) {
  int kb = stack_kb_req > 1024 ? stack_kb_req : 1024;
  int mk = stack_kb();
  if (mk > kb) kb = mk;
  Ctl* c = (Ctl*)calloc(1, sizeof(Ctl));
  if (!c) return 0;
  c->wantKb = kb;
  pthread_attr_t a;
  pthread_attr_init(&a);
  pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize(&a, (size_t)kb * 1024);
  pthread_t t;
  int e = pthread_create(&t, &a, d2w_main, c);
  pthread_attr_destroy(&a);
  if (e) { free(c); return 0; }
  return (double)(uintptr_t)c;
}

D2W int d2_web_worker_ready(double h) { Ctl* c = ctl(h); return c && ld(&c->ready); }
D2W void d2_web_worker_post(double h, int k) { post(ctl(h), k); }
D2W int d2_web_worker_done(double h) { Ctl* c = ctl(h); return c && ld(&c->done) == ld(&c->posted); }

D2W int d2_web_worker_wait(double h) {
  Ctl* c = ctl(h);
  if (!c) return 0;
  wait_until(c, INFINITY);
  return ld(&c->last);
}

D2W int d2_web_worker_tick_sync(double h, int n) { return post(ctl(h), n) ? d2_web_worker_wait(h) : 0; }

D2W int d2_web_worker_park(double h) {
  Ctl* c = ctl(h);
  if (!c) return 0;
  if (ld(&c->stop)) return 1;
  wait_until(c, INFINITY);
  st(&c->stop, 1);
  __atomic_add_fetch(&c->posted, 1, __ATOMIC_SEQ_CST);
  emscripten_futex_wake(&c->posted, 1);
  return 1;
}

D2W double d2_web_worker_heap_bytes(void) { return __builtin_wasm_memory_size(0) * 65536.0; }
D2W double d2_web_worker_busy_ms(double h) { Ctl* c = ctl(h); return c ? c->busyMs : 0; }

// Kind 0 does nothing (JS calls it with a string argument so ccall refreshes its heap views); 1-3 run on the
// worker, bounded at 5 s: -2 is a timeout, which section 6.2 treats as a STOP.
D2W int d2_web_worker_selftest(double h, int kind, const char* arg) {
  if (kind == 0) return 1;
  Ctl* c = ctl(h);
  if (!c || kind < 1 || kind > 3) return 0;
  int32_t mine = kind == 2 ? checksum(arg) : 0;
  c->arg = arg;
  if (!post(c, -kind)) return 0;
  if (!wait_until(c, emscripten_get_now() + 5000)) return -2;
  int32_t r = ld(&c->last);
  if (kind == 1) return r;
  if (kind == 2) return mine >= 0 && r == mine;
  return r >= c->wantKb && r >= stack_kb();
}
