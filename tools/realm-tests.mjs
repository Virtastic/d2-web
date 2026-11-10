// node wrapper: node tools/realm-tests.mjs  (runs the realm host tests, exits non-zero on failure)
import { spawnSync } from 'node:child_process';
const r = spawnSync('bash', [new URL('./realm-tests.sh', import.meta.url).pathname, ...process.argv.slice(2)], { stdio: 'inherit' });
process.exit(r.status ?? 1);
