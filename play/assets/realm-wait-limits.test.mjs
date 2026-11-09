import test from 'node:test';
import assert from 'node:assert/strict';
import { BNCS_REPLY_MS, MCP_STARTUP_MS } from './realm-client.mjs';
import { CHARACTER_CREATE_CHECKBOXES } from './charcreate-checkboxes.mjs';

test('reply waits are the client libraries own: 45 s per Battle.net reply, 60 s for realm startup', () => {
  assert.equal(BNCS_REPLY_MS, 45000);
  assert.equal(MCP_STARTUP_MS, 60000);
});

test('the Ladder box starts checked, as the class pick leaves it, beside Expansion', () => {
  const initial = Object.fromEntries(CHARACTER_CREATE_CHECKBOXES.entries.map((e) => [e.key, e.initial]));
  assert.deepEqual(initial, { expansion: true, hardcore: false, ladder: true });
});
