import { test } from 'node:test';
import assert from 'node:assert/strict';

// input-controller.mjs installs its poller when the DOM is ready. Keep the DOM callback dormant so
// these tests exercise only the pure target mapping and navigation helpers.
globalThis.document ??= { readyState: 'loading', addEventListener() {} };
const { panelPoints, snapMove } = await import('../play/assets/input-controller.mjs');

test('character creation class navigation uses the shell-provided hit-test points', () => {
  const targets = {
    create: {
      classes: { amazon: [100, 300], assassin: [230, 300], barbarian: [400, 300] },
      ok: [400, 500],
      exit: [20, 20],
    },
  };

  const points = panelPoints(targets);
  assert.deepEqual(points, [
    { key: 'create:amazon', x: 100, y: 300 },
    { key: 'create:assassin', x: 230, y: 300 },
    { key: 'create:barbarian', x: 400, y: 300 },
    { key: 'create:exit', x: 20, y: 20 },
  ]);
  assert.equal(points[1].x, targets.create.classes.assassin[0]);
  assert.equal(points[1].y, targets.create.classes.assassin[1]);
});

test('the d-pad moves between class hit points in the pressed direction', () => {
  const points = panelPoints({ create: { classes: { amazon: [100, 300], assassin: [230, 300], barbarian: [400, 300] } } });
  const index = snapMove(points, 0, 1, 0);

  assert.equal(points[index].key, 'create:assassin');
});
