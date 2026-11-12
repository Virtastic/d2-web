import test from 'node:test';
import assert from 'node:assert/strict';
import { makeSkillMapper, PAD } from './pad-skill-mapper.mjs';

const sample = (down = [], { lt = 0, rt = 0, axes = [0, 0, 0, 0] } = {}) => {
  const buttons = Array.from({ length: 17 }, (_, i) => ({ pressed: down.includes(i), value: down.includes(i) ? 1 : 0 }));
  buttons[PAD.LT] = { pressed: lt > 0, value: lt };
  buttons[PAD.RT] = { pressed: rt > 0, value: rt };
  return { buttons, axes };
};
const F = { field: true, seconds: 0.05 };

test('an LT tap swaps weapons on its release', () => {
  const m = makeSkillMapper();
  assert.equal(m(sample([], { lt: 1 }), F).swapTap, false);
  assert.equal(m(sample([], { lt: 1 }), F).modifierHeld, true);
  assert.equal(m(sample(), F).swapTap, true);
  assert.equal(m(sample(), F).swapTap, false);
});

test('an LT hold past the tap time does not swap', () => {
  const m = makeSkillMapper();
  for (let i = 0; i < 6; i++) m(sample([], { lt: 1 }), F);
  assert.equal(m(sample(), F).swapTap, false);
});

test('an LT chord never swaps, whichever button is let go first', () => {
  for (const ltFirst of [true, false]) {
    const m = makeSkillMapper(), seen = [];
    seen.push(m(sample([], { lt: 1 }), F));
    const press = m(sample([PAD.A], { lt: 1 }), F);
    assert.equal(press.slot, 0);
    assert.equal(press.modifierHeld, true);
    seen.push(m(ltFirst ? sample([PAD.A]) : sample([], { lt: 1 }), F));
    seen.push(m(sample(), F));
    assert.ok(seen.every((o) => !o.swapTap), `ltFirst=${ltFirst}`);
  }
});

test('the triggers press at 0.5 and let go at 0.3', () => {
  const m = makeSkillMapper();
  assert.equal(m(sample([], { rt: 0.45 }), F).mainDown, false);
  const p = m(sample([], { rt: 0.5 }), F);
  assert.deepEqual([p.main, p.mainDown], [true, true]);
  assert.deepEqual([m(sample([], { rt: 0.35 }), F).mainDown, m(sample([], { rt: 0.3 }), F).mainDown], [true, false]);
});

test('the press that closes the wheel never reaches the field', () => {
  const m = makeSkillMapper();
  m(sample(), { ui: 'wheel' });
  const closing = m(sample([PAD.A], { rt: 1 }), { ui: 'wheel' });
  assert.equal(closing.slot, 0);
  const after = m(sample([PAD.A], { rt: 1 }), F);
  assert.deepEqual([after.slot, after.mainDown, after.slotDown[0]], [-1, false, false]);
  m(sample(), F);
  assert.equal(m(sample([PAD.X]), F).slot, 2);
});

test('the wheel pages with the d-pad, repeating while held, and aims with the right stick', () => {
  const m = makeSkillMapper(), W = { ui: 'wheel', seconds: 0.1 };
  m(sample(), W);
  const steps = [];
  for (let i = 0; i < 6; i++) steps.push(m(sample([PAD.DOWN]), W).wheelStep);
  // the press, then nothing until 0.35 s held (0.4 here), then every 0.12 s (0.5)
  assert.deepEqual(steps, [1, 0, 0, 0, 1, 1]);
  assert.equal(m(sample([PAD.RIGHT]), W).wheelTab, 1);
  assert.equal(m(sample([], { axes: [0, 0, 0.2, 0.1] }), W).aim, null);
  assert.deepEqual(m(sample([], { axes: [0, 0, 0, -0.9] }), W).aim, [0, -1]);
  assert.equal(m(sample([PAD.BACK]), W).cancel, true);
});

test('the d-pad does nothing in the field, and the list confirms with A and closes with B', () => {
  const m = makeSkillMapper();
  assert.equal(m(sample([PAD.DOWN]), F).wheelStep, 0);
  const S = { ui: 'strip', seconds: 0.05 };
  m(sample(), S);
  assert.equal(m(sample([PAD.DOWN]), S).stripStep, 1);
  assert.equal(m(sample([PAD.A]), S).confirm, true);
  assert.equal(m(sample([PAD.B]), S).cancel, true);
});
