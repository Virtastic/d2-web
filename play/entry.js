// Entry decision, made once, before anything else reads the page. The player's page ignores the URL
// completely: retail data mounted, enhanced (full-screen/widescreen) view. URL params are a TEST path:
// they are honoured only when navigator.webdriver is set (the test runner's headless Chrome / Playwright).
// Kept test params: retail, play, enhanced, view (800|fill), parity, plugy, seed, debug, perf, terrain,
// inventory, character, skills, ma0, unsupported, bootDeadline, bootUrl, noRetail.
// Harness rows with no enhanced/view param still get the 800x600 parity view they compare against retail.
(() => {
  const test = !!navigator.webdriver;
  const search = test ? location.search : '';
  const q = new URLSearchParams(search);
  const retail = q.has('retail') ? q.get('retail') === '1' : true;
  const player = retail && q.get('play') !== '1';
  const fill = test ? q.get('view') !== '800' && q.get('parity') !== '1' && (q.get('enhanced') === '1' || q.get('view') === 'fill') : true;
  globalThis.__d2Entry = { retail, player, fill, parity: !fill, search, test };
  if (typeof document !== 'undefined' && document.documentElement) {
    if (fill) document.documentElement.classList.add('fill');
    if (player) document.documentElement.classList.add('playing');
  }
})();
