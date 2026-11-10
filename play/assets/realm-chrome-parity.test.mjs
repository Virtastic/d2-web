// A game joined from the title screen must be drawn and heard with everything a local game has: the shell builds both
// through the same functions. These read the shell's source, like the other chrome tests: the shell itself needs a page.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
const fn = (head) => { const a = shell.indexOf(head); assert.ok(a >= 0, head); return shell.slice(a, shell.indexOf('\n}\n', a)); };
// Every `name = createX(` or `makeX(` (also behind `??=`, a second name or an audioVolume.attach) in a piece of source.
const built = (source) => [...new Set([...source.matchAll(/(?<![.\w])(\w+) (?:\?\?)?= (?:\w+ \?\?= )?(?:audioVolume\.attach\([^,]+, )?(?:create|make)\w+\(/g)].map((m) => m[1]))];
const calls = (source) => [...new Set([...source.matchAll(/\b(buildChrome\w+|buildStoragePanels)\b/g)].map((m) => m[1]))];

const local = fn('async function bootstrapSession('), joined = fn('function ensureRealmChrome()');
// What a local game is known to build; a new renderer added inline to the local start fails the first test instead.
const LOCAL = ['hoverFont', 'automapFont', 'gameCursor', 'pointerLayer', 'playerRenderer', 'missileRenderer', 'skillFx', 'stateFx', 'unitOverlayFx',
  'overlayRenderer', 'weatherRenderer', 'corpseRenderer', 'monsterRenderer', 'npcSpeechAlertRenderer', 'areaNameRenderer', 'entryBannerRenderer',
  'inventoryPanelRenderer', 'characterPanelRenderer', 'hirelingPanelRenderer', 'skillPanelRenderer', 'questLogRenderer', 'messageLogRenderer',
  'partyRenderer', 'bubbleRenderer', 'stashPanelRenderer', 'cubePanelRenderer', 'stashGoldRenderer', 'repairConfirmRenderer', 'waypointPanelRenderer',
  'beltRenderer', 'beltTooltip', 'controlPanelRenderer', 'hudHelp', 'helpRenderer', 'automapOverlay', 'skillSelectRenderer', 'actMusic',
  'uiClickSounds', 'unitSounds', 'combatSounds', 'eventSounds', 'missileSounds', 'ambient'];

test('a local game constructs no renderer of its own beside the object, item and automap ones', () => {
  // The automap names its data source where it is built; the object and item renderers are built beside it.
  assert.deepEqual(built(local).filter((name) => name !== 'assetReader').sort(), ['automapOverlay', 'itemRenderer', 'objectRenderer']);
});

test('a joined game calls every builder a local game calls', () => {
  const want = calls(local);
  assert.ok(want.length >= 10, `the local game builds through the shared builders (${want.length})`);
  for (const name of want) assert.ok(calls(joined).includes(name), `the joined game calls ${name}`);
});

test('a joined game has every renderer and sound player a local game has', () => {
  const have = new Set(built(joined));
  for (const name of calls(joined)) for (const made of built(fn(`function ${name}(`))) have.add(made);
  assert.deepEqual(LOCAL.filter((name) => !have.has(name)), []);
  const listed = [...fn('const CHROME_RENDERERS = ').matchAll(/(\w+): \(\) => \1\b/g)].map((m) => m[1]);
  for (const name of LOCAL) assert.ok(listed.includes(name), `the shell's own list names ${name}`);
});

test('a joined game asks only for what is missing and one failing builder leaves the join standing', () => {
  for (const name of calls(joined).filter((n) => n !== 'buildStoragePanels' && n !== 'buildChromeMusic'))
    assert.match(joined, new RegExp(`build\\('[^']+', \\(\\) => ${name}\\(false, assetReader\\)\\)`), `${name} is guarded and not fresh`);
  for (const name of calls(local).filter((n) => n !== 'buildStoragePanels' && n !== 'buildChromeMusic'))
    assert.match(local, new RegExp(`${name}\\(true, assetReader\\)`), `a local game builds ${name} fresh`);
  const scene = fn('async function syncRealmScene()');
  assert.equal(built(scene).length, 0, 'the scene sync builds nothing beside the chrome');
});

test('a joined game reaches the skill speed bar, the area name, the entry banner and its own stats', () => {
  assert.match(shell, /if \(\(session \|\| realmSceneActive\) && skillSelectRenderer\) skillSelectRenderer\.open = /);
  assert.match(shell, /if \(\(engine \|\| realmWorld\) && !clearScreen && automapOverlay\?\.isOpen\)/);
  assert.match(shell, /if \(\(engine \|\| realmWorld\) && entryBannerRenderer && entryBannerLevel > 0\)/);
  const view = fn('const realmClientView = {');
  assert.match(view, /playerStat: \(stat\) => .*_d2_realm_client_player_stat/);
  assert.doesNotMatch(view, /playerStat: \(\) => 0/);
});
