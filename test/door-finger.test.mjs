import { test } from 'node:test';
import assert from 'node:assert/strict';
import { FINGER_REACH_CSS, distanceTo, fingerHit, fingerReachGame, pressCommands } from '../play/assets/door-finger.mjs';

// The front door's four wide buttons, in its 800x600 frame (byo-door.mjs BTN), 272x35 each.
const BUTTONS = [
  { key: 'dir', x: 264, y: 418, w: 272, h: 35 },
  { key: 'files', x: 264, y: 460, w: 272, h: 35 },
  { key: 'enter', x: 264, y: 502, w: 272, h: 35 },
  { key: 'discord', x: 264, y: 544, w: 272, h: 35 },
];

// A phone at the floor size the touch notes measure (915x412 CSS, dpr 2.625): the frame scales to
// fit the height, so 1 game px is 412/600 CSS px. canvasPerCss is the canvas's own px per CSS px.
const PHONE = { canvasPerCss: 2.625, frameScale: 2.625 * 412 / 600 };   // canvas px per game px
const reachOnPhone = fingerReachGame(PHONE.canvasPerCss, PHONE.frameScale);

test('the reach is 16 CSS px, converted through the canvas and the frame', () => {
  assert.equal(FINGER_REACH_CSS, 16);
  // A desktop canvas at 1 canvas px per CSS px and a frame at 1 px per game px: 16 game px.
  assert.equal(fingerReachGame(1, 1), 16);
  // Doubling the canvas's px per CSS px doubles the reach in CSS-equivalent terms, not in game px.
  assert.equal(fingerReachGame(2, 1), 32);
  // A frame at 0.5 canvas px per game px: the same 16 CSS px of glass is 32 game px.
  assert.equal(fingerReachGame(1, 0.5), 32);
  // Unmeasurable box or scale: no snap at all.
  assert.equal(fingerReachGame(0, 1), 0);
  assert.equal(fingerReachGame(1, 0), 0);
});

test('a finger 10 CSS px beside ENTER SANCTUARY presses it (left of the button)', () => {
  // 10 CSS px is 10 / (412/600) = 14.56 game px left of the button's left edge.
  const p = { x: 264 - 10 / (412 / 600), y: 502 + 17 };
  assert.equal(fingerHit(p, BUTTONS, reachOnPhone), 'enter');
});

test('the same point is no hit for a mouse (no reach), as before', () => {
  const p = { x: 264 - 10 / (412 / 600), y: 519 };
  assert.equal(fingerHit(p, BUTTONS, 0), null);
});

test('a finger 17 CSS px away presses nothing', () => {
  const p = { x: 264 - 17 / (412 / 600), y: 519 };
  assert.equal(fingerHit(p, BUTTONS, reachOnPhone), null);
});

test('an exact hit wins over a closer-looking neighbour snap', () => {
  // Inside DIR, a few px above FILES: it must stay DIR, not the nearer-centre FILES.
  assert.equal(fingerHit({ x: 300, y: 452 }, BUTTONS, reachOnPhone), 'dir');
  assert.equal(fingerHit({ x: 300, y: 461 }, BUTTONS, reachOnPhone), 'files');
});

test('a finger in the 7-game-px gap between two buttons snaps to the nearer edge', () => {
  // DIR ends at y 453, FILES starts at y 460: y 455 is 2 px from DIR, 5 px from FILES.
  assert.equal(fingerHit({ x: 300, y: 455 }, BUTTONS, reachOnPhone), 'dir');
  // y 458 is 5 px from DIR and 2 px from FILES.
  assert.equal(fingerHit({ x: 300, y: 458 }, BUTTONS, reachOnPhone), 'files');
});

test('a point equidistant from two buttons goes to the one whose centre is nearer', () => {
  const a = { key: 'a', x: 0, y: 0, w: 10, h: 10 };
  const b = { key: 'b', x: 0, y: 30, w: 10, h: 40 };
  // The gap is y 10..30; (5,20) is 10 from each box's edge, but a's centre (5,5) is nearer than b's (5,50).
  assert.equal(fingerHit({ x: 5, y: 20 }, [b, a], 12), 'a');
  assert.equal(distanceTo({ x: 5, y: 20 }, a), distanceTo({ x: 5, y: 20 }, b));
});

test('the discord button and the "again" link are snap targets too, when they are on screen', () => {
  const again = { key: 'again', x: 320, y: 401, w: 160, h: 14 };
  assert.equal(fingerHit({ x: 320 + 80, y: 401 - 5 }, [...BUTTONS, again], reachOnPhone), 'again');
  assert.equal(fingerHit({ x: 300, y: 556 }, BUTTONS, reachOnPhone), 'discord');
  // Without DISCORD, a point 38 game px below ENTER is past the reach.
  assert.equal(fingerHit({ x: 300, y: 575 }, BUTTONS.slice(0, 3), reachOnPhone), null);
});

test('a touch sequence sends exactly the press-and-lift commands the door acts on', () => {
  const sample = (x, y) => fingerHit({ x, y }, BUTTONS, reachOnPhone);
  const L = (dx) => 264 - dx / (412 / 600);   // a point dx CSS px left of the buttons' left edge
  const events = [
    // A thumb lands 9 CSS px beside ENTER and lifts exactly on it: the press counts.
    { type: 'down', key: sample(L(9), 519) }, { type: 'up', key: sample(300, 519) },
    // A press on DIR that lifts on FILES (a drag off the button): no command.
    { type: 'down', key: sample(300, 430) }, { type: 'up', key: sample(300, 470) },
    // A press 20 CSS px out from every button: nothing down, nothing acted.
    { type: 'down', key: sample(L(20), 519) }, { type: 'up', key: sample(L(20), 519) },
    // DISCORD pressed a few CSS px below its bottom edge and lifted on it.
    { type: 'down', key: sample(300, 582) }, { type: 'up', key: sample(300, 580) },
  ];
  assert.deepEqual(pressCommands(events), ['act:enter', 'act:discord']);
});
