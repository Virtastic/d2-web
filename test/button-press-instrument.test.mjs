// Instrument: every momentary button draws retail's down frame (same DC6, up+1) only while armed and held over,
// and acts only on a release over the same button. Fixed event sequences, frame records compared to the rule.
import test from 'node:test';
import assert from 'node:assert/strict';
import { BUTTON_PRESS_RULES as R, simulateButton, pressedFrameOf } from '../play/assets/button-press-rules.mjs';
import { HUD_LAYOUT, levelButtonFrame } from '../play/assets/control-panel.mjs';
import { STAT_PLUS_ART, STAT_PLUS_FRAME, STAT_PLUS_PRESSED_FRAME, STAT_PLUS_DISABLED_FRAME } from '../play/assets/character-panel.mjs';
import { panelCloseFrame } from '../play/assets/panel-close.mjs';
import { characterPanelCloseFrame } from '../play/assets/character-panel-close.mjs';
import { createWaypointMenuInput } from '../play/assets/waypoint-menu-input.mjs';

const SEQ = [ { type: 'move', over: 'b' }, { type: 'down', over: 'b' }, { type: 'move', over: 'b' }, { type: 'move', over: null },
  { type: 'move', over: 'b' }, { type: 'up', over: 'b' }, { type: 'move', over: 'b' } ];
const OFF = [ { type: 'down', over: 'b' }, { type: 'move', over: null }, { type: 'up', over: null } ];

for (const [id, rule] of Object.entries(R)) test(`${id}: ${rule.cel} up ${rule.up} -> down ${rule.down}`, () => {
  assert.equal(rule.down, rule.up + 1 === rule.down ? rule.down : -1, 'down is the frame after up');
  const r = simulateButton(rule, 'b', SEQ);
  assert.deepEqual(r.frames, [rule.up, rule.down, rule.down, rule.up, rule.down, rule.up, rule.up]);
  assert.deepEqual(r.activated, [5]);
  const off = simulateButton(rule, 'b', OFF);
  assert.deepEqual(off.frames, [rule.down, rule.up, rule.up]);
  assert.deepEqual(off.activated, [], 'release off the button does not activate');
  assert.equal(pressedFrameOf(rule, null, 'b'), rule.up);
  assert.equal(pressedFrameOf(rule, 'other', 'b'), rule.up);
});

test('rules agree with the renderers that draw them', () => {
  assert.equal(R['stat-plus'].cel.toLowerCase(), STAT_PLUS_ART.toLowerCase());
  assert.deepEqual([R['stat-plus'].up, R['stat-plus'].down, R['stat-plus'].disabled], [STAT_PLUS_FRAME, STAT_PLUS_PRESSED_FRAME, STAT_PLUS_DISABLED_FRAME]);
  const lv = HUD_LAYOUT.levelButtons;
  assert.equal(lv.path.toLowerCase(), R['level-hud'].cel.toLowerCase());
  assert.deepEqual([levelButtonFrame(0, -1), levelButtonFrame(0, 0), lv.disabledFrame], [R['level-hud'].up, R['level-hud'].down, R['level-hud'].disabled]);
  assert.equal(HUD_LAYOUT.runButton.frame, R['run-walk'].up);
  assert.equal(HUD_LAYOUT.runButton.runFrame, R['run-run'].up);
  assert.equal(HUD_LAYOUT.menuButton.openFrame, R['menu-open'].up);
  assert.equal(HUD_LAYOUT.menuButton.closedFrame, R['menu-closed'].up);
  assert.equal(HUD_LAYOUT.miniButtons.path.toLowerCase(), 'data\\global\\ui\\panel\\minipanelbtn.dc6');
  assert.deepEqual([panelCloseFrame(false), panelCloseFrame(true)], [R['panel-close'].up, R['panel-close'].down]);
  assert.deepEqual([characterPanelCloseFrame(false), characterPanelCloseFrame(true)], [R['panel-close'].up, R['panel-close'].down]);
});

test('waypoint close: only the 36x34 box is momentary; its edges are retail\'s', () => {
  const w = createWaypointMenuInput();
  for (const [x, y, inside] of [[353, 447, true], [388, 480, true], [353, 446, false], [352, 460, false], [389, 460, false], [370, 481, false]])
    assert.equal(w.inCloseRect(x, y, 800, 600), inside, `${x},${y}`);
});
