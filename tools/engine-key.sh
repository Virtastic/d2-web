#!/usr/bin/env bash
# The engine's SOURCE KEY: what play/d2.wasm is a pure function of. Same key = same engine, whatever else changed
# (JS, docs, vectors, scenarios do not move it). Uncommitted engine-source edits add "-dirty" (forces a build).
# Used by tools/engine-cache.sh / engine-sync.sh / ci/stage.sh ("we do not want to keep hitting the
# same issues" -- stale wasm seeded into fresh worktrees, mtime false positives, hand-copied builds, D2_STAGE_ALLOW_STALE).
set -euo pipefail
cd "${1:-.}"
k=$(git rev-parse HEAD:src HEAD:wasm-build/link-d2.sh HEAD:third_party HEAD:ci/build.sh 2>/dev/null | sha256sum | cut -c1-12)
[ -z "$(git status --porcelain -- src wasm-build/link-d2.sh 2>/dev/null)" ] || k="$k-dirty"
echo "$k"
