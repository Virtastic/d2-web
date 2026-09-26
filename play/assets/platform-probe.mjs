// The capability probe. Run before
// the engine boots (play/index.html), so a browser that cannot run it sees a readable screen
// instead of an exception in the debug log.
//
// Feature detection, never UA-string version sniffing: the two facts that gate the engine are
// `crossOriginIsolated` (COOP/COEP -> SharedArrayBuffer -> the streamfs worker, play/serve.mjs) and
// Memory64 (the engine's build -- is a locked decision; no build
// exists without it). Memory64 support is asked of the engine directly, through
// `WebAssembly.validate()` on the smallest legal module that declares a 64-bit memory, rather than
// inferred from a browser/version table -- so the probe stays correct as engines change, and the
// browser list below is informational only, never the gate itself.
//
// The module bytes, byte by byte:
//   00 61 73 6d   magic "\0asm"
//   01 00 00 00   version 1
//   05            section id 5 (memory section)
//   03            section size: 3 bytes follow
//   01            memory count: 1
//   04            limits flags: bit 2 (0x04) set = "index64" -- the memory64 proposal's own
//                 encoding of the limits flags byte. An engine that has not implemented the
//                 proposal treats 0x04 as a reserved/invalid flag combination and rejects the
//                 module outright, which is exactly the signal wanted.
//   01            min pages: 1
//
// Verified under Node 24.19.0 (`~/.local/bin/node`, V8 13.x -- the engine class that also backs
// Chrome/Edge 133+, this repo's minimum): `WebAssembly.validate(MEMORY64_PROBE_BYTES)` is `true`.
// Two checks that this is a narrow, faithful probe and not `validate()` merely accepting anything:
// the same bytes with the flags byte zeroed (flags=0x00, a plain 32-bit memory -- still legal wasm
// on every engine) also validate `true`; with the flags byte set to an unused pattern (0xFF) they
// validate `false`. `test/platform-probe.test.mjs` pins all three.

export const MEMORY64_PROBE_BYTES = Uint8Array.from([
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00,
  0x05, 0x03, 0x01, 0x04, 0x01,
]);

export const MIN_CHROME = 133, MIN_FIREFOX = 134, MIN_SAMSUNG = 29;
export const SUPPORTED_BROWSERS =
  `Chrome or Edge ${MIN_CHROME}+, Firefox ${MIN_FIREFOX}+, Android Chrome or Samsung Internet ${MIN_SAMSUNG}+` +
  ` -- not Safari or any iPhone/iPad browser yet`;

/**
 * Feature-detects Memory64 without instantiating a module (a real 64-bit memory reserves address
 * space up front). Never throws: an engine with no WebAssembly object at all is just unsupported.
 */
export function probeMemory64(wasm = globalThis.WebAssembly) {
  try { return !!wasm && typeof wasm.validate === 'function' && wasm.validate(MEMORY64_PROBE_BYTES) === true; }
  catch { return false; }
}

/**
 * The two facts that gate the engine. Pure and side-effect free -- `g` defaults to the real global
 * so the browser needs no argument, but a unit test can pin it without a DOM (`globalThis` in Node
 * has no `crossOriginIsolated` property, which reads as `undefined`, i.e. false, not a throw).
 */
export function probeCapabilities(g = globalThis) {
  const crossOriginIsolated = g.crossOriginIsolated === true;
  const memory64 = probeMemory64(g.WebAssembly);
  return { crossOriginIsolated, memory64, supported: crossOriginIsolated && memory64 };
}

/** Best-effort browser name for the unsupported screen -- informational only, never the gate. */
export function describeBrowser(ua) {
  ua = String(ua || '');
  if (/iPad|iPhone|iPod/.test(ua)) return 'Safari';               // every iOS browser is WebKit
  if (/EdgA?\//.test(ua)) return 'Edge';
  if (/OPR\//.test(ua)) return 'Opera';
  if (/SamsungBrowser\//.test(ua)) return 'Samsung Internet';
  if (/CriOS\//.test(ua)) return 'Chrome for iOS (WebKit)';
  if (/FxiOS\//.test(ua)) return 'Firefox for iOS (WebKit)';
  if (/Firefox\//.test(ua)) return 'Firefox';
  if (/Chrome\//.test(ua)) return 'Chrome';
  if (/Version\/.*Safari\//.test(ua)) return 'Safari';
  return 'This browser';
}

/** The text of the unsupported screen: names the browser, what is missing, what runs the game. */
export function unsupportedMessage(caps, ua = (typeof navigator !== 'undefined' ? navigator.userAgent : '')) {
  const browser = describeBrowser(ua);
  const missing = [];
  if (!caps.crossOriginIsolated) missing.push('cross-origin isolation (COOP/COEP)');
  if (!caps.memory64) missing.push('WebAssembly Memory64');
  return {
    browser, missing, supportedBrowsers: SUPPORTED_BROWSERS,
    text: `${browser} cannot run d2-web: missing ${missing.join(' and ')}. `
      + `Runs today on ${SUPPORTED_BROWSERS}.`,
  };
}
