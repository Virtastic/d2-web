import test from 'node:test';
import { readFileSync } from 'node:fs';
import assert from 'node:assert/strict';
import {
  MEMORY64_PROBE_BYTES, probeMemory64, probeCapabilities, describeBrowser, unsupportedMessage,
  MIN_CHROME, MIN_FIREFOX, MIN_SAMSUNG,
} from '../play/assets/platform-probe.mjs';

// Pins the exact probe module bytes (docs/client-parity.md B3): a minimal wasm module declaring one
// memory with the memory64 proposal's limits-flags bit (0x04) set. Verified under Node 24 (V8, the
// same engine family as Chrome/Edge 133+): the real WebAssembly implementation, not a stand-in.
test('the Memory64 probe module validates on this engine (Node 24 / V8, same family as Chrome 133+)', () => {
  assert.equal(MEMORY64_PROBE_BYTES.length, 13);
  assert.equal(WebAssembly.validate(MEMORY64_PROBE_BYTES), true);
  assert.equal(probeMemory64(), true);
});

test('the probe is narrow: it reads the memory64 flag bit, not "validate() accepts anything"', () => {
  // Same module, ordinary 32-bit memory (flags=0x00): still legal wasm everywhere, must also validate.
  const plain32 = Uint8Array.from(MEMORY64_PROBE_BYTES); plain32[11] = 0x00;
  assert.equal(WebAssembly.validate(plain32), true, 'a plain 32-bit memory module is valid wasm');
  // Same module, an unused limits-flags pattern: must be rejected.
  const bogus = Uint8Array.from(MEMORY64_PROBE_BYTES); bogus[11] = 0xff;
  assert.equal(WebAssembly.validate(bogus), false, 'an invalid limits-flags byte must not validate');
});

test('probeMemory64 never throws, even with no WebAssembly object', () => {
  assert.equal(probeMemory64(null), false);   // undefined would fall through to the real default param
  assert.equal(probeMemory64({}), false);
  assert.equal(probeMemory64({ validate: () => { throw new Error('boom'); } }), false);
});

test('probeCapabilities reads crossOriginIsolated and Memory64, never throwing in a non-browser global', () => {
  // globalThis in Node has no crossOriginIsolated property (not a ReferenceError -- reads as undefined).
  const caps = probeCapabilities();
  assert.equal(caps.crossOriginIsolated, false);
  assert.equal(caps.memory64, true);
  assert.equal(caps.supported, false);
  assert.equal(caps.secureContext, false);   // Node has no isSecureContext either
  // A fake global standing in for a fully-capable browser served right: HTTPS/localhost + COOP/COEP.
  const full = probeCapabilities({ crossOriginIsolated: true, isSecureContext: true, WebAssembly });
  assert.deepEqual(full, { crossOriginIsolated: true, memory64: true, secureContext: true, supported: true });
  // A capable engine served over plain HTTP from a LAN address: not isolated AND not secure.
  const insecure = probeCapabilities({ crossOriginIsolated: false, isSecureContext: false, WebAssembly });
  assert.equal(insecure.secureContext, false);
  assert.equal(insecure.memory64, true);
  assert.equal(insecure.supported, false);
});

test('describeBrowser names the browser families the support matrix cares about', () => {
  const UA = {
    chrome: 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/133.0.0.0 Safari/537.36',
    firefox: 'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:134.0) Gecko/20100101 Firefox/134.0',
    edge: 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/133.0.0.0 Safari/537.36 Edg/133.0.0.0',
    samsung: 'Mozilla/5.0 (Linux; Android 14) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/29.0 Chrome/122.0.0.0 Mobile Safari/537.36',
    iphone: 'Mozilla/5.0 (iPhone; CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Mobile/15E148 Safari/604.1',
    ipad: 'Mozilla/5.0 (iPad; CPU OS 18_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Mobile/15E148 Safari/604.1',
    macSafari: 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.4 Safari/605.1.15',
  };
  assert.equal(describeBrowser(UA.chrome), 'Chrome');
  assert.equal(describeBrowser(UA.firefox), 'Firefox');
  assert.equal(describeBrowser(UA.edge), 'Edge');
  assert.equal(describeBrowser(UA.samsung), 'Samsung Internet');
  assert.equal(describeBrowser(UA.iphone), 'Safari');
  assert.equal(describeBrowser(UA.ipad), 'Safari');
  assert.equal(describeBrowser(UA.macSafari), 'Safari');
});

// The iOS browsers that are not Safari: every one of them is WebKit, but the screen names the browser
// the person is holding. These branches were dead code while the generic /iPad|iPhone|iPod/ test ran
// first, so a Chrome-on-iPhone user was told "Safari cannot run d2-web" (docs/client-parity.md B13).
test('describeBrowser names a non-Safari iOS browser as itself, not as Safari', () => {
  const IOS = 'CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) ';
  assert.equal(describeBrowser(`Mozilla/5.0 (iPhone; ${IOS}CriOS/140.0.0.0 Mobile/15E148 Safari/604.1`), 'Chrome for iOS (WebKit)');
  assert.equal(describeBrowser(`Mozilla/5.0 (iPhone; ${IOS}FxiOS/140.0 Mobile/15E148 Safari/605.1.15`), 'Firefox for iOS (WebKit)');
  assert.equal(describeBrowser(`Mozilla/5.0 (iPhone; ${IOS}EdgiOS/140.0 Mobile/15E148 Safari/605.1.15`), 'Edge for iOS (WebKit)');
  // iPad too, and Safari on iOS is still Safari.
  assert.equal(describeBrowser(`Mozilla/5.0 (iPad; CPU OS 18_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/140.0.0.0 Mobile/15E148 Safari/604.1`), 'Chrome for iOS (WebKit)');
  assert.equal(describeBrowser(`Mozilla/5.0 (iPhone; ${IOS}Version/18.0 Mobile/15E148 Safari/604.1`), 'Safari');
});

// The screen's honesty (docs/client-parity.md B13). crossOriginIsolated is a fact about how the page
// was SERVED, not a browser capability: Memory64 is the browser capability, and an engine that has it
// is years newer than COOP/COEP. So Memory64-present + not-isolated means the host is at fault, and
// the screen must not tell a capable browser that IT cannot run the game.
test('a capable browser that was served without COOP/COEP is not told it cannot run the game', () => {
  const chromeUA = 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/153.0.0.0 Safari/537.36';
  const info = unsupportedMessage({ crossOriginIsolated: false, memory64: true, supported: false }, chromeUA);
  assert.equal(info.cause, 'host');
  assert.equal(info.browser, 'Chrome');
  assert.deepEqual(info.missing, ['cross-origin isolation (COOP/COEP)']);
  // The live defect: this exact sentence, on Chrome 153 with memory64 true.
  assert.doesNotMatch(info.text, /Chrome cannot run/);
  assert.match(info.summary, /not cross-origin isolated/i);
  assert.match(info.advice, /Chrome supports what the game needs/);
  assert.match(info.advice, /COOP\/COEP/);
  // Nothing may read as "your browser is not one of the supported ones".
  assert.equal(info.supportedBrowsers, '', 'no browser list when the browser is not the problem');
  assert.doesNotMatch(info.text, /Runs today on/);
});

// docs/client-parity.md B23: the OTHER host fault. A capable browser over plain HTTP from a LAN
// address is a non-secure context -- SharedArrayBuffer is unavailable there whatever the host sends,
// so "add the COOP/COEP headers" is advice that cannot help. Measured live (Chrome 153,
// http://<lan-host> with both headers sent): crossOriginIsolated false, isSecureContext false.
test('a capable browser on a non-secure origin is told the secure context, not the headers', () => {
  const chromeUA = 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/153.0.0.0 Safari/537.36';
  const info = unsupportedMessage({ crossOriginIsolated: false, memory64: true, secureContext: false, supported: false }, chromeUA);
  assert.equal(info.cause, 'host');
  assert.equal(info.browser, 'Chrome');
  assert.deepEqual(info.missing, ['a secure context (HTTPS or localhost)']);
  assert.match(info.summary, /not a secure context/i);
  assert.match(info.advice, /HTTPS or localhost/);
  assert.doesNotMatch(info.text, /Chrome cannot run/);
  assert.doesNotMatch(info.text, /served without the COOP\/COEP headers/, 'does not blame headers that cannot help here');
  assert.equal(info.supportedBrowsers, '');
});

test('a browser that genuinely lacks Memory64 is still named as the cause', () => {
  const safari = unsupportedMessage({ crossOriginIsolated: true, memory64: false, supported: false },
    'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Safari/605.1.15');
  assert.equal(safari.cause, 'browser');
  assert.match(safari.text, /Safari cannot run d2-web/);
  assert.match(safari.text, /WebAssembly Memory64/);
  assert.match(safari.text, /not Safari/);
  // Both missing at once: the browser is still the cause (it lacks the engine's build).
  const both = unsupportedMessage({ crossOriginIsolated: false, memory64: false, supported: false }, 'unknown/1.0');
  assert.equal(both.cause, 'browser');
  assert.equal(both.supportedBrowsers.length > 0, true);
  assert.deepEqual(both.missing, ['cross-origin isolation (COOP/COEP)', 'WebAssembly Memory64']);
});

// The browser-fault screen printed the same sentence twice: `advice` was "Runs today on <list>" and the
// page's own `runs-today` line printed "Runs today on <list>" again under it. Found by looking at the
// frame (harness-out/s328-browser-unsupported-honest/unsupported-apple.png, 2026-09-26), not by any
// assertion here -- every text assertion passed while the screen said it twice. So the count is pinned.
test('the browser list is printed once, and the advice says why', () => {
  const safariUA = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Safari/605.1.15';
  for (const caps of [
    { crossOriginIsolated: true, memory64: false, supported: false },   // a real Mac: served right, no Memory64
    { crossOriginIsolated: false, memory64: false, supported: false },  // both missing
  ]) {
    const info = unsupportedMessage(caps, safariUA);
    assert.equal(info.text.split('Runs today on').length - 1, 1, `the browser list prints once: ${info.text}`);
    assert.doesNotMatch(info.advice, /Runs today on/, 'the advice line does not repeat the list below it');
    assert.match(info.advice, /no 64-bit WebAssembly memory yet/, 'the advice says what the engine needs, in plain words');
    // And the list is still exactly one line, rendered from supportedBrowsers by the page.
    assert.match(info.supportedBrowsers, /Chrome or Edge/);
    assert.match(info.text, /not Safari/);
  }
  // Only the capability the browser lacks is explained: a Mac that was served right is not told COOP/COEP
  // is missing (row B2's whole distinction from the both-missing switch).
  const apple = unsupportedMessage({ crossOriginIsolated: true, memory64: false, supported: false }, safariUA);
  assert.doesNotMatch(apple.advice, /cross-origin isolation/);
  const both = unsupportedMessage({ crossOriginIsolated: false, memory64: false, supported: false }, safariUA);
  assert.match(both.advice, /cross-origin isolated page/);
});

test('unsupportedMessage names the browser, what is missing, and every browser that runs the game', () => {
  const info = unsupportedMessage({ crossOriginIsolated: true, memory64: false },
    'Mozilla/5.0 (iPhone; CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 Version/18.0 Safari/604.1');
  assert.equal(info.browser, 'Safari');
  assert.deepEqual(info.missing, ['WebAssembly Memory64']);
  assert.match(info.text, /Safari/);
  assert.match(info.text, /Memory64/);
  assert.match(info.text, new RegExp(`Chrome.*${MIN_CHROME}`));
  assert.match(info.text, new RegExp(`Firefox.*${MIN_FIREFOX}`));
  assert.match(info.text, new RegExp(`Samsung Internet.*${MIN_SAMSUNG}`));
  assert.match(info.text, /not Safari/);
});

// Phones take the SAME gate as desktop (docs/platform.md, the phone row): no device-class check. A
// fake navigator carries the phone's UA, touch points and a phone-sized window; the gate reads only
// crossOriginIsolated, isSecureContext and Memory64, so the phone facts must change nothing.
const PHONE_UA = {
  android: 'Mozilla/5.0 (Linux; Android 14; Pixel 8) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/133.0.0.0 Mobile Safari/537.36',
  ios: 'Mozilla/5.0 (iPhone; CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/133.0.0.0 Mobile/15E148 Safari/604.1',
};
function fakePhone({ memory64, isolated = true, secure = true, ua }) {
  return {
    crossOriginIsolated: isolated,
    isSecureContext: secure,
    // A WebAssembly stand-in: Memory64 answered by the real engine when present, refused when not.
    WebAssembly: { validate: (bytes) => (memory64 ? WebAssembly.validate(bytes) : false) },
    navigator: { userAgent: ua, maxTouchPoints: 5 },
    innerWidth: 390, innerHeight: 844,
    matchMedia: () => ({ matches: true }),
  };
}

test('a fake phone with Memory64 and cross-origin isolation passes the gate, like desktop', () => {
  const caps = probeCapabilities(fakePhone({ memory64: true, ua: PHONE_UA.android }));
  assert.deepEqual(caps, { crossOriginIsolated: true, memory64: true, secureContext: true, supported: true });
  // The same capabilities on a desktop global give the same answer: the device does not gate.
  const desk = probeCapabilities({ crossOriginIsolated: true, isSecureContext: true, WebAssembly });
  assert.deepEqual(caps, desk);
});

test('a fake phone without Memory64 gets the unsupported screen, with the plain wording', () => {
  const caps = probeCapabilities(fakePhone({ memory64: false, ua: PHONE_UA.android }));
  assert.equal(caps.supported, false);
  const info = unsupportedMessage(caps, PHONE_UA.android);
  assert.equal(info.cause, 'browser');
  assert.equal(info.browser, 'Chrome');
  assert.deepEqual(info.missing, ['WebAssembly Memory64']);
  assert.match(info.advice, /This browser has no 64-bit WebAssembly memory yet, and the game needs it\./);
  assert.doesNotMatch(info.advice, /WebKit/, 'Android Chrome is not WebKit: no iOS sentence');
  assert.doesNotMatch(info.text, /implement yet|wasm64/, 'the old engine-jargon wording is gone');
  assert.match(info.text, /Runs today on Chrome or Edge 133\+/);
});

test('an iPhone browser without Memory64 is told iOS is waiting on WebKit', () => {
  const caps = probeCapabilities(fakePhone({ memory64: false, ua: PHONE_UA.ios }));
  assert.equal(caps.supported, false);
  const info = unsupportedMessage(caps, PHONE_UA.ios);
  assert.equal(info.browser, 'Chrome for iOS (WebKit)');
  assert.match(info.advice, /no 64-bit WebAssembly memory yet/);
  assert.match(info.advice, /Every browser on iPhone and iPad is WebKit/);
  assert.match(info.advice, /iOS is waiting on WebKit for 64-bit WebAssembly/);
  assert.match(info.supportedBrowsers, /iOS is waiting on WebKit for 64-bit WebAssembly/);
  assert.match(info.supportedBrowsers, /not Safari or any iPhone\/iPad browser yet/);
});

test('a phone served without COOP/COEP is named as the host fault, not the browser', () => {
  const caps = probeCapabilities(fakePhone({ memory64: true, isolated: false, ua: PHONE_UA.android }));
  const info = unsupportedMessage(caps, PHONE_UA.android);
  assert.equal(info.cause, 'host');
  assert.doesNotMatch(info.text, /cannot run/);
});

test('the phone block is gone from the probe module and the page: no isPhone, no PHONE_MESSAGE, no phone screen', async () => {
  const probe = await import('../play/assets/platform-probe.mjs');
  assert.equal('isPhone' in probe, false);
  assert.equal('PHONE_MESSAGE' in probe, false);
  const html = readFileSync(new URL('../play/index.html', import.meta.url), 'utf8');
  assert.doesNotMatch(html, /isPhone|PHONE_MESSAGE|showPhone|unsupported=phone|Not on mobile browsers yet|body\.phone/);
  const src = readFileSync(new URL('../play/assets/platform-probe.mjs', import.meta.url), 'utf8');
  assert.doesNotMatch(src, /isPhone|PHONE_MESSAGE|Not on mobile/);
});
