import test from 'node:test';
import assert from 'node:assert/strict';
import { createDeleteConfirmFlow, DELETE_REFUSED_STRING } from './delete-confirm-flow.mjs';

const make = (up) => { const log = []; const flow = createDeleteConfirmFlow(async (n) => { log.push(n); return { status: 'deleted' }; }, () => up.v); return { flow, log }; };

test('realm down + named row: popup 5211, nothing deleted, OK closes', async () => {
  const up = { v: false }, { flow, log } = make(up);
  assert.equal(flow.arm(0), true);
  assert.equal(await flow.confirm('Ghost'), false);
  assert.deepEqual(log, []);
  assert.equal(flow.state, 'notice'); assert.equal(flow.notice, 5211); assert.equal(DELETE_REFUSED_STRING, 5211);
  assert.equal(flow.armed, -1); assert.equal(flow.blocked, true);
  assert.equal(flow.dismissNotice(), true);
  assert.equal(flow.state, 'idle'); assert.equal(flow.notice, 0); assert.equal(flow.blocked, false);
  assert.equal(flow.dismissNotice(), false);
});
test('realm up: normal delete', async () => {
  const { flow, log } = make({ v: true });
  flow.arm(1);
  assert.equal(await flow.confirm('Ghost'), true);
  assert.deepEqual(log, ['Ghost']); assert.equal(flow.state, 'idle');
});
test('empty name deletes without the connection check', async () => {
  const { flow, log } = make({ v: false });
  flow.arm(0);
  assert.equal(await flow.confirm(''), true);
  assert.deepEqual(log, ['']); assert.equal(flow.state, 'idle');
});
test('offline (no connection check) deletes as before', async () => {
  const log = []; const flow = createDeleteConfirmFlow(async (n) => { log.push(n); return { status: 'deleted' }; });
  flow.arm(0); assert.equal(await flow.confirm('A'), true); assert.deepEqual(log, ['A']);
});
