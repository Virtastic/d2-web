// Every mouse hit test under play/ has a finger aim that covers it, or is named here with why it does not. The scan
// reads each module's own hit-test definitions (hitAt, hit, skillAt, itemAt, ...), so a new hit test in a new file
// fails this test until the manifest says which aim reaches it. The manifest names the aim functions; the test
// checks that each one is really defined, so a rename cannot leave a stale entry behind.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readdirSync, readFileSync, statSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const ROOT = fileURLToPath(new URL('../play/', import.meta.url));
const TOUCH = readFileSync(new URL('../play/assets/input-touch.mjs', import.meta.url), 'utf8');
const SHELL = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');

// A hit-test definition: a method, a function or a property that answers "is this point on a control".
const DEF = /^\s*(?:export\s+)?(?:function\s+)?(hitAt|hitClose|hitScroll|hit|skillAt|itemAt|cellAt|slotAt|entryAt|rowAt|buttonAt|tabAt|weaponTabProbe|optionsPageRowAt|beltSlotAt|orbAt|fingerHit)\s*(?:\(.*\)\s*\{|:\s*\(|\s*\(.*\)\s*\{)/;

function modules(dir = ROOT, out = []) {
  for (const name of readdirSync(dir)) {
    const p = `${dir}${name}`;
    if (statSync(p).isDirectory()) modules(`${p}/`, out);
    else if (name.endsWith('.mjs') && !name.endsWith('.test.mjs')) out.push(p);
  }
  return out;
}
const rel = (p) => `play/${p.slice(ROOT.length)}`;
const hitTested = () => modules().filter((p) => readFileSync(p, 'utf8').split('\n').some((l) => DEF.test(l))).map(rel).sort();

// File -> the aim(s) or shell probes that reach its controls. The aim names are checked against the source below.
const COVERED = {
  'play/assets/belt-renderer.mjs': 'beltAim',
  'play/assets/belt-tooltip.mjs': 'beltAim (beltSlotAt: the hover plan reads the slot the belt aim reaches)',
  'play/assets/byo-door.mjs': 'door-finger.mjs (fingerHit: the door\'s own touch snap, hit(p, touch))',
  'play/assets/door-finger.mjs': 'byo-door.mjs (fingerHit, the front-door snap; not input-touch.mjs)',
  'play/assets/character-panel.mjs': 'statAim',
  'play/assets/configure-controls.mjs': 'modalAim (touchControlAt: controlsPanel)',
  'play/assets/game-menu.mjs': 'modalAim (touchControlAt: gameMenu)',
  'play/assets/help-overlay.mjs': 'modalAim (touchControlAt: helpVisible)',
  'play/assets/inventory-panel.mjs': 'panelAim, tabAim, inspect (touchItemAt)',
  'play/assets/npc-menu.mjs': 'modalAim (touchControlAt: npcMenu)',
  'play/assets/options-pages.mjs': 'modalAim (touchControlAt: gameMenu, its sub-page)',
  'play/assets/orb-readout.mjs': 'orbAim (orbAt, through orbAimPoint)',
  'play/assets/quest-log.mjs': 'modalAim (touchControlAt: questVisible)',
  'play/assets/skill-panel.mjs': 'modalAim (touchControlAt: skillsVisible), inspect (touchSkillAt)',
  'play/assets/stash-gold-dialog.mjs': 'goldAim, modalAim (touchControlAt: stash gold dialog)',
  'play/assets/stash-panel.mjs': 'panelAim, inspect (touchItemAt)',
  'play/assets/vendor-repair-confirm.mjs': 'modalAim (touchControlAt: repairConfirm)',
  'play/gameplay-shell.mjs': 'the touch probes (touchControlAt, touchGoldAt, touchSkillAt, touchItemAt, touchWeaponTabAt)',
};
// Owned by another platform row (vendor, NPC, waypoints): listed so the scan accepts them; their coverage is that row's to prove.
const OWNED = {
  'play/assets/npc-panel.mjs': 'vendor/NPC row',
  'play/assets/vendor-panel.mjs': 'vendor row',
  'play/assets/waypoint-panel.mjs': 'waypoint row',
};
// Not a control: a game effect that shares the hit-test name (the 0x0C combat hit animation).
const EXEMPT = {
  'play/assets/state-fx.mjs': 'the 0x0C combat hit effect, not a clickable control',
};
// Controls no aim reaches yet. Each is a known gap with the reason; a new one fails the test.
const KNOWN_GAPS = {};

test('every hit-tested module under play/ is in the manifest (covered, owned by another row, or a known gap)', () => {
  const unlisted = hitTested().filter((f) => !(f in COVERED || f in OWNED || f in KNOWN_GAPS || f in EXEMPT));
  assert.deepEqual(unlisted, [], 'hit-tested files with no aim: add an aim, or name the gap in KNOWN_GAPS with its reason');
});

test('the manifest is current: every listed file still defines a hit test, and every aim it names still exists', () => {
  const found = new Set(hitTested());
  const stale = [...Object.keys(COVERED), ...Object.keys(OWNED), ...Object.keys(KNOWN_GAPS), ...Object.keys(EXEMPT)].filter((f) => !found.has(f));
  assert.deepEqual(stale, [], 'manifest entries that no longer define a hit test');
  for (const [file, how] of Object.entries(COVERED)) {
    for (const aim of how.match(/\b\w+Aim\b/g) || []) assert.match(TOUCH, new RegExp(`function ${aim}\\(`), `${file} names ${aim}, which input-touch.mjs does not define`);
    for (const probe of how.match(/\btouch[A-Z]\w*/g) || []) assert.match(SHELL, new RegExp(`\\b${probe}\\(`), `${file} names ${probe}, which gameplay-shell.mjs does not define`);
  }
});
