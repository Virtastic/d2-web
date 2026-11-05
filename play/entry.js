// Entry decision, made once, before anything else reads the page. Retail data mounted, player page; the
// enhanced (full-screen/widescreen) view, except under the test runner (navigator.webdriver), which gets the
// 800x600 parity view the harness compares against retail. Public visits never read the URL: `search` is
// empty for them. The test runner keeps its URL options (play=1, seed, debug...) that every scenario uses.
(() => {
  // as=player (test runner only): the player's page -- enhanced view, rAF loop, every !navigator.webdriver
  // path -- booted with the runner's URL options, so ?play=1 skips the title screen and the frame loop a
  // player gets can be measured (tools/perf-entry.mjs). A public visit never reads the URL, so it never
  // reaches this: the gate is navigator.webdriver, which only automation sets.
  const asPlayer = !!navigator.webdriver && /[?&]as=player(?:&|$)/.test(location.search);
  if (asPlayer && typeof Navigator !== 'undefined') Object.defineProperty(Navigator.prototype, 'webdriver', { get: () => false, configurable: true });
  const test = !!navigator.webdriver;
  // fill=1 lets a test run the full-screen view too: the screen-size rows measure its sizing on emulated devices.
  const fill = !test || /[?&]fill=1(?:&|$)/.test(location.search);
  const search = test || asPlayer ? location.search : '';
  globalThis.__d2Entry = { retail: true, player: true, fill, parity: !fill, search, test, assets: '', ...(asPlayer ? { asPlayer } : {}) };
  if (typeof document !== 'undefined' && document.documentElement) {
    if (fill) document.documentElement.classList.add('fill');
    document.documentElement.classList.add('playing');
  }
})();
// Every 2D canvas is CPU-backed. The canvases the client draws INTO are read back every frame (blends,
// the display ramp), so they are willReadFrequently; Chrome then reads a GPU-backed SOURCE canvas back
// to the CPU on every drawImage -- one sync GPU round-trip per glyph, cel and tile (char select and
// Act 1 measured ~100 ms drawMs; 400 sprites 10.6 -> 4.2 ms when the sources are CPU too). Pixels are
// unchanged: the test runner has always run every canvas on the CPU (--disable-gpu).
if (typeof HTMLCanvasElement !== 'undefined') {
  const getContext = HTMLCanvasElement.prototype.getContext;
  HTMLCanvasElement.prototype.getContext = function (type, opts) {
    return getContext.call(this, type, type === '2d' ? { willReadFrequently: true, ...opts } : opts);
  };
}
