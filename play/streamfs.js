// streamfs.js — copied near-verbatim from openmw-web/play/streamfs.js (GPL-3.0-or-later,
// Copyright (C) 2025-2026 Virtastic). Divergences are marked 'd2-web divergence'. It assumes nothing but `FS` and
// crossOriginIsolated, and it is how user-supplied MPQs get read without hosting them.
//
// streamfs.js — synchronous-read streaming files for the openmw-web build.
//
// Mounts a byte source as a read-only file in the emscripten FS whose bytes are fetched ON
// DEMAND in chunks by a helper Web Worker. The engine's synchronous main-thread read()
// spin-waits on a SharedArrayBuffer flag while the worker does the async read — the standard
// emscripten sync-over-async pattern (requires crossOriginIsolated, which server.py's
// COOP/COEP headers provide). Fetched chunks are LRU-cached in JS memory.
//
// Two byte sources are supported by the same machinery:
//   - a URL, read via HTTP Range requests (server-hosted mwdata) — StreamFS.mount()
//   - a local FileSystemFileHandle, read via getFile().slice() (user-picked Data Files) —
//     StreamFS.mountLocal()
//
// Why: modern Chrome forbids synchronous binary XHR on the main thread (so
// FS.createLazyFile aborts), and OpenMW reads BSAs synchronously on the main thread.
//
// Usage (from index.html preRun, after FS exists):
//   StreamFS.init();                                       // once
//   StreamFS.mount('/mwdata/Morrowind.bsa', 'mwdata/Morrowind.bsa', sizeBytes);
//   StreamFS.mountLocal('/mwdata/Morrowind.bsa', fileHandle, sizeBytes);
(function () {
  'use strict';
  // Chunk size and cache depth. MEASURED on a Balmora boot with retail data (see
  // window.__streamfsStats): at 4MB x 32 slots the cache THRASHED -- 27 evictions against 32 slots,
  // 59 misses, 1269ms of main-thread stall, for 229MB read. A BSA read is a mesh or a texture:
  // small, and scattered all over the archive. Pulling 4MB to serve a 20KB texture meant a handful
  // of live regions could not stay resident at once.
  //
  // Same ~128MB budget, four times as many distinct regions. Smaller chunks also make each miss
  // cheaper, and a miss BLOCKS the main thread (fetchChunkSync spins -- Atomics.wait is banned on
  // the main thread), so miss cost is frame time, not just bandwidth.
  //
  // Tunable at runtime for A/B without a rebuild: ?chunk=<KB>&lru=<slots>.
  // The page's query only through entry.js (the player page has none); a context without it keeps its own.
  const _q = globalThis.__d2Entry ? globalThis.__d2Entry.search : '';
  const _n = (re, dflt) => { const m = re.exec(_q); return m ? (parseInt(m[1], 10) || dflt) : dflt; };
  // CHUNK AND LRU MUST BE TUNED TOGETHER -- measured 2026-08-28, Balmora boot, at a constant
  // ~384MB cache budget so the comparison is memory-neutral:
  //
  //     1MB x 384 slots : 259 cold misses, 2253ms stall, 270MB fetched, NIF load 2457ms
  //     2MB x 192 slots : 150 cold misses, 1380ms stall, 307MB fetched, NIF load 1568ms  <-- best
  //     4MB x  96 slots :  89 cold misses, 1346ms stall, 337MB fetched, NIF load 1613ms
  //
  // With the LRU large enough that evictions are 0, every remaining miss is a COLD miss, and the
  // only way to cut those is to cover more bytes per fetch. 2MB folds neighbouring cold misses
  // into one round trip (-42% misses, -39% stall) and takes 36% off total mesh load time, since
  // ~59% of that is I/O (see components/resource/nifstats.hpp). 4MB keeps cutting misses but each
  // fetch costs more and pulls more unused bytes, so stall stops improving and boot gets worse.
  //
  // This supersedes the original F4 note below, which found 1MB beat 4MB -- that was measured at a
  // 32-to-128 slot LRU, where 4MB chunks thrashed. The chunk size was never the whole story.
  const CHUNK = _n(/[?&]chunk=(\d+)/, 2048) * 1024;   // 2MB default
  // Slot count is TIER-DEPENDENT and resolved lazily in init(), not here: index.html picks the
  // tier from navigator.deviceMemory and publishes window.__omwLruDefault, and this file is parsed
  // before that runs. 0 means "not decided yet"; an explicit ?lru= always wins.
  //
  // MEASURED 2026-08-28 (Balmora, matched workload -- hits within 0.1%): 128 slots was thrashing.
  //     lru=128 -> 215 evictions, 343 misses, 2684ms main-thread stall, 356MB fetched
  //     lru=384 ->   0 evictions, 259 misses, 1923ms main-thread stall, 268MB fetched
  // -28% stall and -25% bytes. The working set settles at 259 chunks, so 384 is headroom rather
  // than a guess. A miss BLOCKS the main thread (fetchChunkSync spins), so this is frame time.
  let LRU_MAX = _n(/[?&]lru=(\d+)/, 0);

  // cache is a Map used as an LRU: insertion order IS the recency order (delete+set moves to end),
  // so eviction pops the first (oldest) key. No separate order array → O(1) hit path, no linear scan.
  const S = { worker: null, ctrl: null, data: null, cache: new Map(), nextId: 1, urlSrcs: new Map(), mounts: new Map(), warming: new Set(), waiters: [] };

  // d2-web divergence: MEASURED 2026-09-25 -- in Firefox 156 the worker+Atomics round trip below
  // never completes. The Range fetch's own headers resolve in milliseconds (confirmed by a
  // Firefox-side timestamp taken right after `await fetch`), but `await r.arrayBuffer()` inside the
  // worker then stalls for the full 30s timeout, and the body is delivered the instant the spinning
  // main-thread loop stops (the `dbgStore` completion message and the timeout's throw land within a
  // few ms of each other every time). A worker's own thread is independent of the main thread's JS,
  // but its fetch body still has to cross back through the content process's networking/IPC path,
  // and that path only advances when the main thread returns to the event loop -- which the
  // Atomics-poll loop below never does. Chrome's network stack is fully out-of-process and does not
  // have this dependency, so the same code boots there in milliseconds; a synthetic repro (a bare
  // Range fetch from a Worker, no main-thread spin) also returns instantly in Firefox, isolating the
  // main-thread spin as the cause, not the server or the Range request itself.
  //
  // Firefox (unlike Chrome, matching this file's own "modern Chrome forbids synchronous binary XHR"
  // note above) still runs a synchronous XMLHttpRequest to completion on the main thread when its
  // responseType is left at the default ('text' -- setting 'arraybuffer' throws for a sync request in
  // BOTH browsers, that part of the comment above was never Chrome-specific). Reading the bytes back
  // through the classic `overrideMimeType('text/plain; charset=x-user-defined')` trick (each char
  // code is one raw byte, 0-255) sidesteps the worker/Atomics path -- and therefore this stall --
  // entirely. Used only for URL-sourced (server-hosted) reads on Firefox: it needs no worker round
  // trip at all, so it also skips the persistent Cache-API chunk store the worker keeps for the other
  // browsers (a speed loss noted here, not a correctness one -- ).
  //
  // d2-web divergence: WebKit hits the identical stall, MEASURED 2026-09-28
  // (`tools/webkit-boot-check.mjs`) -- capability probe passes (`crossOriginIsolated:true,
  // memory64:true`), the worker mounts every MPQ, and the first read then times out at the same
  // ~30s worker/Atomics deadline issue 9 diagnosed for Firefox, because WebKit's process model has the
  // same property: a worker's fetch body delivery depends on the page's main thread returning to
  // its event loop, which the `fetchChunkSync` spin below never does (Chrome's out-of-process
  // network stack is the odd one out, not the rule). issue 9's fix -- skip the worker/Atomics path for
  // URL-sourced reads and use a synchronous XHR on the main thread instead -- is not
  // Firefox-specific in its reasoning, so it is extended to WebKit rather than re-derived: same
  // `fetchRangeSyncXHR`, same `overrideMimeType('text/plain; charset=x-user-defined')` byte
  // trick. UA sniffing is otherwise avoided in this codebase (platform-probe.mjs feature-detects),
  // but no feature flag distinguishes "does this browser's worker fetch need the main thread free"
  // -- the same trade issue 9 already made. The desktop-Safari/WebKit signature
  // (`Version/<n> Safari/<n>` with no `Chrome/` token) is the one platform-probe.mjs's own
  // `describeBrowser` already uses to name Safari, so it is reused here rather than invented.
  const IS_FIREFOX = typeof navigator !== 'undefined' && /Firefox\//.test(navigator.userAgent || '');
  const IS_WEBKIT = typeof navigator !== 'undefined' &&
    /Version\/.*Safari\//.test(navigator.userAgent || '') && !/Chrome\//.test(navigator.userAgent || '');
  const NEEDS_SYNC_XHR = IS_FIREFOX || IS_WEBKIT;
  // d2-web divergence: this read has NO deadline of its own, and none can be given to
  // it. A window-context synchronous XHR may not set `xhr.timeout` (the spec throws
  // InvalidAccessError -- the whole reason the request is synchronous is that a caller needs the bytes
  // in this tick), so a host that accepts the Range request and then never answers blocks the tab for
  // as long as the browser's own network stack allows. MEASURED 2026-09-26, Firefox 156, against such
  // a host: the page was unresponsive for 37.6 s and then threw NetworkError (the whole probe run took
  // 44 s, browser teardown included). The two bounded-looking alternatives were measured too, and
  // BOTH fail on Firefox, so neither is available: (a) a worker's own sync XHR -- `xhr.timeout` IS
  // legal inside a worker -- never completes while this thread spins (8 s cap, no completion, the same
  // deadlock as the fetch path above: a worker's read needs the content process's main thread and the
  // spin never yields it); (b) pumping the loop with a sync XHR to a healthy URL does not deliver a
  // worker's fetch either (1913 pumps in 5 s, body never arrived). Chrome is unaffected: its reads go
  // through the worker path, whose spin below carries its own 30 s deadline.
  //
  // What is left is to report the wait instead of hiding it. A failed read used to surface as a bare
  // NetworkError with nothing to say how long it took or which host; the tab's own stall counters did
  // not move either, because the miss is only counted after the read returns. Both now carry the read's
  // real cost, and the error names the host, the wait, and where to read about it
  //. `play/index.html` keeps this path from being reached for the common
  // case -- a host that never answers -- by bounding the boot's own requests.
  function fetchRangeSyncXHR(src, url, start, end) {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, false);
    xhr.overrideMimeType('text/plain; charset=x-user-defined');
    xhr.setRequestHeader('Range', 'bytes=' + start + '-' + (end - 1));
    const t0 = performance.now();
    try {
      xhr.send(null);
    } catch (err) {
      // A read that never lands is the most expensive miss there is: count it as one, with its cost,
      // before re-throwing -- window.__streamfsStats is where a stall is supposed to be visible.
      const ms = Math.round(performance.now() - t0);
      ST.misses++; ST.stallMs += ms; ST.lastReadMs = ms; ST.lastReadUrl = url;
      throw new Error('streamfs: ' + url + ' did not answer within ' + ms + 'ms -- the host accepted ' +
        'the request and then stalled. This browser reads URL sources synchronously, and a synchronous XHR ' +
        'cannot take a timeout, so the browser\'s own network timeout is the only bound.', { cause: err });
    }
    ST.lastReadMs = Math.round(performance.now() - t0); ST.lastReadUrl = url;
    if (xhr.status !== 206 && xhr.status !== 200) throw new Error('streamfs: HTTP ' + xhr.status + ' ' + url);
    // A 200 is a server that ignored Range and sent the whole file: take this chunk's slice of it.
    const text = xhr.status === 200 ? xhr.responseText.slice(start, end) : xhr.responseText;
    const chunk = new Uint8Array(text.length);
    for (let i = 0; i < text.length; ++i) chunk[i] = text.charCodeAt(i) & 0xff;
    return chunk;
  }

  // Streaming cost counters, exposed as window.__streamfsStats. Chunk misses block the main thread
  // (see fetchChunkSync), so this is the only place the stall is observable. Two adds per read —
  // cheap enough to leave always-on, unlike the flag-gated ?glcount/?perfstats probes.
  // lastReadMs/lastReadUrl: the most recent Firefox synchronous read and what it cost. A read
  // that stalls is the one fact a report of a frozen tab needs, and it is gone by the time the error
  // is caught unless it is kept here.
  const ST = { hits: 0, misses: 0, stallMs: 0, evictions: 0, bytes: 0, lastReadMs: 0, lastReadUrl: '', warmed: 0, recent: [] };
  // The last 64 misses as [startMs, ms, url@pos]: a long frame the shell records lists the ones inside it.
  function noteMiss(t0, at) { ST.recent.push([Math.round(t0), Math.round(performance.now() - t0), at]); if (ST.recent.length > 64) ST.recent.shift(); }

  function workerSource() {
    return `
      let ctrl, data;
      const handles = new Map();   // id -> FileSystemFileHandle
      const files = new Map();     // id -> File (cached getFile() result)
      let prefetch = null;         // {ckey, done}: the one outstanding sequential prefetch (backlog 389)
      // url#start -> Promise<Uint8Array|null>: a WARM in flight, kept until its cache put lands. A
      // synchronous read of the same chunk awaits it instead of fetching the 2MB a second time while
      // the main thread spins (a hero outrunning area-prefetch.mjs's warm was paying both fetches).
      const warming = new Map();
      // d2-web divergence: the chunk store opened once, not per read: every sync miss paid an open
      // while the main thread spun. A Cache API that is missing stays a rejection, as before.
      let chunkStore = null;
      const chunkCache = () => (chunkStore ??= Promise.resolve().then(() => caches.open('mwdata-chunks-v1')));
      // d2-web divergence: Chrome can reject a local range fetch transiently. Retry only
      // transport errors, with a bound; missing files and exhausted reads must still fail.
      async function readRange(url, start, end) {
        for (let attempt = 0; ; ++attempt) {
          try {
            const r = await fetch(url, { headers: { Range: 'bytes=' + start + '-' + (end - 1) } });
            if (!r.ok && r.status !== 206) throw new Error('HTTP ' + r.status);
            return new Uint8Array(await r.arrayBuffer());
          } catch (err) {
            if (!(err instanceof TypeError) || attempt === 2) throw err;
            await new Promise((resolve) => setTimeout(resolve, 25 * (attempt + 1)));
          }
        }
      }
      onmessage = async (e) => {
        const m = e.data;
        if (m.init) { ctrl = new Int32Array(m.ctrl); data = new Uint8Array(m.data); postMessage({ ready: 1 }); return; }
        if (m.mountLocal) { handles.set(m.mountLocal.id, m.mountLocal.handle); return; }
        // A Blob (BSA from the Cache API) reads exactly like a File (slice + arrayBuffer), so it
        // goes straight into the files map and the m.id read path below slices it locally.
        if (m.mountBlob) { files.set(m.mountBlob.id, m.mountBlob.blob); return; }
        // WARM (s-perf-streaming): fetch a chunk with no one waiting and hand it to the main thread's
        // in-memory cache, so the later synchronous read is a hit instead of a stalled round trip.
        if (m.warm) {
          // d2-web divergence: a Blob or file-handle mount (a bring-your-own-files host) warms by slicing
          // it here, exactly as the synchronous read below does; it was URL-only, so those hosts read
          // every byte as a main-thread miss.
          if (m.id !== undefined) {
            let buf = null;
            try {
              let f = files.get(m.id);
              if (!f) { f = await handles.get(m.id).getFile(); files.set(m.id, f); }
              buf = new Uint8Array(await f.slice(m.start, m.end).arrayBuffer());
            } catch (e) {}
            postMessage({ warmed: m.key, buf });
            return;
          }
          const wkey = m.url + '#' + m.start;
          let put = null;
          const p = (async () => {
            const ckey = m.pkey ? 'https://sfs.chunk/' + encodeURIComponent(m.pkey) + '/' + m.start : null;
            if (ckey) { try { const hit = await (await chunkCache()).match(ckey); if (hit) return new Uint8Array(await hit.arrayBuffer()); } catch (e) {} }
            const buf = await readRange(m.url, m.start, m.end);
            if (ckey) put = chunkCache().then((c) => c.put(ckey, new Response(buf.slice())));
            return buf;
          })().catch(() => null);
          warming.set(wkey, p);
          const buf = await p;
          Promise.resolve(put).catch(() => {}).finally(() => { if (warming.get(wkey) === p) warming.delete(wkey); });
          // A copy, not a transfer: a synchronous read awaiting p may still be reading buf.
          postMessage({ warmed: m.key, buf });
          return;
        }
        // m: {url|id, start, end, gen}
        try {
          let buf;
          if (m.id !== undefined) {
            let f = files.get(m.id);
            if (!f) { f = await handles.get(m.id).getFile(); files.set(m.id, f); }
            buf = new Uint8Array(await f.slice(m.start, m.end).arrayBuffer());
          } else {
            // Persistent chunk cache. The in-memory LRU dies with the page, so without this
            // every boot re-downloaded every byte the engine touched (~300MB measured). Keyed
            // by m.pkey — the ORIGINAL mount URL plus file size, so a renewed presigned URL
            // still hits, and a re-uploaded file of a new size misses. pkey is null for URLs
            // that should not persist (cross-origin or query-carrying, i.e. presigned S3).
            const ckey = m.pkey ? 'https://sfs.chunk/' + encodeURIComponent(m.pkey) + '/' + m.start : null;
            const w = warming.get(m.url + '#' + m.start);
            if (w) buf = await w;
            if (!buf && ckey) {
              try {
                // A prefetch of exactly this chunk may be in flight (backlog 389): wait for it
                // rather than fetching the same bytes twice.
                if (prefetch && prefetch.ckey === ckey) await prefetch.done;
                const hit = await (await chunkCache()).match(ckey);
                if (hit) buf = new Uint8Array(await hit.arrayBuffer());
              } catch (e) { /* Cache API unavailable: fall through to the network */ }
            }
            if (!buf) {
              buf = await readRange(m.url, m.start, m.end);
              if (ckey) {
                // Not awaited: the copy into the shared buffer below reads buf, the put keeps
                // its own reference, and nothing mutates buf afterwards.
                chunkCache().then((c) => c.put(ckey, new Response(buf))).catch(() => {});
              }
            }
          }
          data.set(buf.subarray(0, Math.min(buf.length, data.length)), 0);
          ctrl[1] = buf.length;         // bytes delivered
          Atomics.store(ctrl, 0, m.gen);  // completion flag = generation
          Atomics.notify(ctrl, 0);
          // SEQUENTIAL PREFETCH (backlog 389): the main thread says this miss followed the
          // previous chunk of the same file (m.next = the chunk after it), which is what a
          // first TR boot looks like -- ~500 serial range requests, each a full round trip
          // of main-thread stall. Pull the next chunk into the persistent cache now, off the
          // critical path, so the next miss is a Cache API hit. One outstanding at a time,
          // and only for persistable (same-origin, unsigned) URLs: the cache key is the
          // same formula the read path uses, so nothing else changes.
          if (m.next && m.pkey && !prefetch) {
            const nkey = 'https://sfs.chunk/' + encodeURIComponent(m.pkey) + '/' + m.next.start;
            prefetch = { ckey: nkey, done: (async () => {
              try {
                const c = await chunkCache();
                if (await c.match(nkey)) return;
                const r = await fetch(m.url, { headers: { Range: 'bytes=' + m.next.start + '-' + (m.next.end - 1) } });
                if (r.ok || r.status === 206) await c.put(nkey, new Response(await r.arrayBuffer()));
              } catch (e) { /* best effort: the read path fetches it itself */ }
              finally { prefetch = null; }
            })() };
          }
        } catch (err) {
          // d2-web: preserve the worker's cause when a synchronous engine read fails.
          console.error('streamfs worker read failed:', String(err));
          ctrl[1] = -1;
          Atomics.store(ctrl, 0, m.gen);
          Atomics.notify(ctrl, 0);
        }
      };`;
  }

  let generation = 0;
  // src: {url} for a range-fetched URL, or {id} for a local file handle. cacheKey uniquely
  // identifies the (source, offset) chunk across both modes.
  function fetchChunkSync(src, cacheKey, start, end, size) {
    const hit = S.cache.get(cacheKey);
    if (hit) {
      S.cache.delete(cacheKey); S.cache.set(cacheKey, hit); // move-to-end (most-recently-used)
      ST.hits++;
      return hit;
    }
    // d2-web divergence: Firefox's and WebKit's worker+Atomics round trip stalls (see
    // NEEDS_SYNC_XHR above, issue 9/issue 20) -- read URL sources with a direct synchronous XHR instead, no
    // worker or spin needed.
    if (NEEDS_SYNC_XHR && src.url) {
      const t0f = performance.now();
      const chunk = fetchRangeSyncXHR(src, src.url, start, end);
      ST.misses++; ST.stallMs += performance.now() - t0f; ST.bytes += chunk.length; noteMiss(t0f, src.url + '@' + start);
      S.cache.set(cacheKey, chunk);
      if (S.cache.size > LRU_MAX) { S.cache.delete(S.cache.keys().next().value); ST.evictions++; }
      return chunk;
    }
    const gen = ++generation;
    // Backlog 389: a miss right after the previous chunk of the same file is a sequential
    // walk; tell the worker which chunk comes next so it can prefetch it (URL sources only).
    const seq = S.lastMiss && S.lastMiss.key === cacheKey.slice(0, cacheKey.lastIndexOf(':')) && S.lastMiss.end === start;
    S.lastMiss = { key: cacheKey.slice(0, cacheKey.lastIndexOf(':')), end };
    const next = seq && src.url && end - start === CHUNK && end < size ? { start: end, end: Math.min(end + CHUNK, size) } : null;
    S.worker.postMessage(Object.assign({ start, end, gen, next }, src));
    // Spin until the worker signals completion. The worker thread runs independently, so
    // this terminates; local reads complete in ~1-5ms. (Atomics.wait is disallowed on
    // the main thread, so poll.)
    //
    // NB this BLOCKS the main thread for the whole worker round-trip. For a local file handle
    // that is a disk read; for an HTTP source it is a network round-trip, so a miss here stalls
    // the frame. ST.stallMs is what makes that cost visible (window.__streamfsStats) — an
    // eviction-thrashing working set shows up as misses climbing without bytes growing.
    const t0 = performance.now();
    while (Atomics.load(S.ctrl, 0) !== gen) {
      if (performance.now() - t0 > 30000) throw new Error('streamfs: read timeout ' + cacheKey + '@' + start);
    }
    ST.misses++; ST.stallMs += performance.now() - t0;
    ST.lastReadMs = Math.round(performance.now() - t0); ST.lastReadUrl = (src.url || '') + '@' + start; noteMiss(t0, ST.lastReadUrl);
    const n = S.ctrl[1];
    if (n < 0) throw new Error('streamfs: read failed ' + cacheKey + '@' + start);
    const chunk = new Uint8Array(n);
    chunk.set(S.data.subarray(0, n));
    S.cache.set(cacheKey, chunk);
    ST.bytes += n;
    if (S.cache.size > LRU_MAX) { S.cache.delete(S.cache.keys().next().value); ST.evictions++; } // evict oldest
    return chunk;
  }

  function readSync(src, keyPrefix, size, buffer, offset, length, position) {
    let done = 0;
    while (done < length && position + done < size) {
      const pos = position + done;
      const cs = Math.floor(pos / CHUNK) * CHUNK;
      const ce = Math.min(cs + CHUNK, size);
      const chunk = fetchChunkSync(src, keyPrefix + ':' + cs, cs, ce, size);
      const within = pos - cs;
      const n = Math.min(length - done, chunk.length - within);
      if (n <= 0) break;
      buffer.set(chunk.subarray(within, within + n), offset + done);
      done += n;
    }
    return done;
  }

  // Shared lazy read-only FS node: reports `size` for stat/seek, forces the read() path
  // (no mmap), and routes reads through `doRead(buffer, offset, length, position)`.
  function makeNode(path, size, doRead) {
    const name = path.substring(path.lastIndexOf('/') + 1);
    const dir = path.substring(0, path.lastIndexOf('/')) || '/';
    const node = FS.createFile(dir, name, {}, /*canRead*/ true, /*canWrite*/ false);
    node.usedBytes = size; // some FS paths consult this
    const getattr = node.node_ops.getattr;
    node.node_ops = Object.assign({}, node.node_ops, {
      getattr(n) { const a = getattr(n); a.size = size; return a; },
    });
    node.stream_ops = Object.assign({}, node.stream_ops, {
      llseek(stream, off, whence) {
        let p = off;
        if (whence === 1) p += stream.position;
        else if (whence === 2) p += size;
        if (p < 0) throw new FS.ErrnoError(28 /*EINVAL*/);
        return p;
      },
      read(stream, buffer, offset, length, position) {
        return doRead(buffer, offset, length, position);
      },
      write() { throw new FS.ErrnoError(63 /*EROFS*/); },
      mmap() { throw new FS.ErrnoError(52 /*ENOSYS: force read() path*/); },
    });
    return node;
  }

  // Background warm queue: at most WARM_INFLIGHT chunk fetches ride behind the worker at once so a
  // synchronous read never queues behind a long prefetch backlog. Newest requests go first.
  const WARM_INFLIGHT = 2;
  let warmQ = [];
  function pumpWarm() {
    while (S.warming.size < WARM_INFLIGHT && warmQ.length) {
      const w = warmQ.pop();
      if (S.cache.has(w.key) || S.warming.has(w.key)) continue;
      S.warming.add(w.key);
      S.worker.postMessage(w);
    }
  }

  window.StreamFS = {
    // Live streaming cost: misses each blocked the main thread for a worker round-trip.
    // High misses + high evictions = the working set exceeds LRU_MAX and is thrashing.
    stats() { return Object.assign({ cached: S.cache.size, lruMax: LRU_MAX }, ST); },

    init() {
      if (S.worker) return;
      // Resolve the tier default now that index.html has run. Slots are ~1MB each, so this is
      // also the JS-side memory ceiling: 384MB high / 192MB mid / 96MB low, on top of the wasm heap.
      if (!LRU_MAX) LRU_MAX = (typeof window !== 'undefined' && window.__omwLruDefault | 0) || 128;
      if (!self.crossOriginIsolated) throw new Error('streamfs needs crossOriginIsolated (COOP/COEP)');
      const ctrlBuf = new SharedArrayBuffer(8);
      const dataBuf = new SharedArrayBuffer(CHUNK);
      S.ctrl = new Int32Array(ctrlBuf);
      S.data = new Uint8Array(dataBuf);
      S.worker = new Worker(URL.createObjectURL(new Blob([workerSource()], { type: 'text/javascript' })));
      // d2-web divergence: the worker's blob script is fetched on the PARENT thread in Chrome, so
      // a sync read issued before it has loaded spins until the 30 s timeout with the worker
      // never starting (M0a: StormLib's first read came ~50 ms after init()). openmw-web never
      // hit this because its engine takes seconds to reach its first read. Callers await
      // StreamFS.ready before the first synchronous read.
      S.ready = new Promise((res) => { S.worker.onmessage = (e) => {
        if (e.data && e.data.ready) res();
        else if (e.data && e.data.warmed) {
          const k = e.data.warmed; S.warming.delete(k);
          if (e.data.buf && !S.cache.has(k)) {
            S.cache.set(k, e.data.buf); ST.bytes += e.data.buf.length; ST.warmed++;
            if (S.cache.size > LRU_MAX) { S.cache.delete(S.cache.keys().next().value); ST.evictions++; }
          }
          pumpWarm();
          S.waiters = S.waiters.filter((w) => w.pending() || (w.res(), false));
        }
      }; });
      S.worker.postMessage({ init: 1, ctrl: ctrlBuf, data: dataBuf });
      try { Object.defineProperty(window, '__streamfsStats', { get: () => window.StreamFS.stats() }); } catch (e) {}
    },

    // Ask for the chunks covering [pos, pos+len) of the file mounted at `path` to be fetched in the
    // background into the in-memory cache. Fire and forget; a read that beats it just misses as before.
    warm(path, pos, len) {
      const m = S.mounts.get(path); if (!m || !S.worker || (m.src.url ? NEEDS_SYNC_XHR : m.src.id === undefined)) return;   // Firefox: a worker fetch stalls behind the spin
      for (let cs = Math.floor(pos / CHUNK) * CHUNK; cs < pos + len && cs < m.size; cs += CHUNK) {
        const key = m.abs + ':' + cs;
        if (S.cache.has(key) || S.warming.has(key) || warmQ.some((w) => w.key === key)) continue;
        warmQ.push(Object.assign({ warm: 1, key, start: cs, end: Math.min(cs + CHUNK, m.size) }, m.src));
      }
      pumpWarm();
    },

    // d2-web divergence: warm() and resolve once those chunks are cached or their background fetch ended, so a
    // synchronous read after it does not spin on the network. Resolves at once where warm() cannot run.
    fetched(path, pos, len) {
      this.warm(path, pos, len);
      const m = S.mounts.get(path), keys = [];
      if (m) for (let cs = Math.floor(pos / CHUNK) * CHUNK; cs < pos + len && cs < m.size; cs += CHUNK) keys.push(m.abs + ':' + cs);
      const pending = () => keys.some((k) => !S.cache.has(k) && (S.warming.has(k) || warmQ.some((w) => w.key === k)));
      return pending() ? new Promise((res) => S.waiters.push({ pending, res })) : Promise.resolve();
    },

    // Synchronous read of [pos, pos+len) of a mounted file (through the same cache the engine uses).
    readAt(path, pos, len) {
      const m = S.mounts.get(path); const out = new Uint8Array(len);
      readSync(m.src, m.abs, m.size, out, 0, len, pos); return out;
    },

    // Resolves once the worker is running. Await it before the first synchronous read.
    ready() { return S.ready; },

    // Mount `url` (absolute-ized against the page) at FS path `path` with known byte size.
    // `mtime` (ms, from the manifest's `m`) joins the persistent cache key when given: a
    // re-installed mod file of identical size otherwise served the old chunks forever.
    mount(path, url, size, mtime) {
      const { src, abs } = this.register(path, url, size, mtime);
      return makeNode(path, size, function (buffer, offset, length, position) {
        return readSync(src, abs, size, buffer, offset, length, position);
      });
    },

    // d2-web divergence: the source alone, with no FS node, so warm()/fetched() can run on it before the
    // engine (and its FS) exists. mount() registers again with the same keys.
    register(path, url, size, mtime) {
      const abs = new URL(url, location.href).href;
      const u = new URL(abs);
      // Persist chunks only for plain same-origin files (server-hosted mwdata). A URL with a
      // query is presigned and expiring — its bytes are cached by the locker's own layer, and
      // keying on a signature would never hit twice.
      // CHUNK is part of the key: chunks are cached at fetch granularity, and a ?chunk= retune
      // would otherwise hit a cached chunk shorter than the read loop expects — a short read
      // the engine treats as EOF mid-file.
      const pkey = u.origin === location.origin && !u.search
        ? u.pathname + '@' + size + (mtime ? '@' + mtime : '') + '@' + CHUNK : null;
      const src = { url: abs, pkey };
      // Registered so the URL can be refreshed later (presigned locker URLs expire): the cache
      // key stays `abs` (stable across renewals — the bytes are the same file), only src.url
      // changes, so a re-signed URL is used for future Range fetches without dropping the cache.
      const m = { src, abs, size };
      S.urlSrcs.set(path, src); S.mounts.set(path, m);
      return m;
    },

    // Swap in a freshly-signed URL for an already-mounted path. The read path is synchronous
    // (it blocks on the worker), so it cannot re-sign on a 403; instead the app renews each
    // locker URL on a timer, before expiry, by calling this.
    setUrl(path, url) {
      const src = S.urlSrcs.get(path);
      if (src) src.url = new URL(url, location.href).href;
    },

    // Mount a local `FileSystemFileHandle` at FS path `path` with known byte size. The handle
    // is posted to the worker once (keyed by a per-file id); reads slice bytes from disk on
    // demand — nothing is copied into browser memory up front.
    mountLocal(path, handle, size) {
      const id = S.nextId++;
      S.worker.postMessage({ mountLocal: { id, handle } });
      const src = { id };
      S.mounts.set(path, { src, abs: 'id' + id, size });   // d2-web divergence: warm()/fetched()/readAt() reach it
      return makeNode(path, size, function (buffer, offset, length, position) {
        return readSync(src, 'id' + id, size, buffer, offset, length, position);
      });
    },

    // Mount a Blob (e.g. a BSA held in the browser Cache API) at FS path `path`. Reads slice
    // bytes from the Blob on demand — a LOCAL read (disk-backed Cache blob), so no network and
    // no main-thread stall, unlike streaming from S3. The Blob is posted to the worker once.
    mountBlob(path, blob, size) {
      const { src, abs } = this.registerBlob(path, blob, size);
      return makeNode(path, size, function (buffer, offset, length, position) {
        return readSync(src, abs, size, buffer, offset, length, position);
      });
    },

    // d2-web divergence: register()'s Blob twin. The Blob is posted to the worker once and the source is
    // warmable before the engine (and its FS) exists; mountBlob() of the same Blob reuses it, so chunks
    // warmed before the mount are the ones the engine's reads hit.
    registerBlob(path, blob, size) {
      const had = S.mounts.get(path);
      if (had && had.blob === blob) return had;
      const id = S.nextId++;
      S.worker.postMessage({ mountBlob: { id, blob } });
      const m = { src: { id }, abs: 'id' + id, size, blob };
      S.mounts.set(path, m);
      return m;
    },
  };
})();
