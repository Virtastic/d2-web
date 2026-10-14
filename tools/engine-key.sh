#!/usr/bin/env bash
# The engine's SOURCE KEY: the build INPUTS play/d2.wasm is a function of -- the committed source
# (src/, link-d2.sh, third_party, ci/build.sh), the build CONFIGURATION (D2_CXXFLAGS), and the
# uncommitted source content (digest plus "-dirty"). Used by tools/engine-cache.sh / engine-sync.sh / ci/stage.sh
# ("we do not want to keep hitting the same issues" -- stale wasm seeded into fresh
# worktrees, mtime false positives, hand-copied builds, D2_STAGE_ALLOW_STALE).
#
# THIS IS NOT AN ENGINE IDENTITY. The old first line here ("Same key = same engine") was FALSE and
# cost a methodology scare on 2026-09-30. Two ways it is false, both measured:
#   1. It is computed from HEAD, not from play/d2.wasm. A worktree whose source moved on, or that was
#      seeded/left with another build, reports a key that does NOT describe the engine it serves --
#      22 of this box's worktrees did (`rate`: HEAD-src 2cd2494ebc23 vs the wasm built from
#      360ccbe6b7d6). Two runs can therefore "share a key" and still be two different engines.
#   2. It does not cover D2_CXXFLAGS (below), so ci/sanitize.sh's build shares the release build's key.
# The identity of the engine a RUN measured is play/engine-version.txt -- a content hash of
# d2.wasm + d2.js + streamfs.js -- which the test runner prints as `engine:` and tools/run-compare.sh
# refuses a comparison on. Decide WHETHER TO REBUILD with this key; decide WHAT RAN with that stamp.
set -euo pipefail
cd "${1:-.}"
k=$(git rev-parse HEAD:src HEAD:wasm-build/link-d2.sh HEAD:third_party HEAD:ci/build.sh 2>/dev/null | sha256sum | cut -c1-12)
# The source key covers WHAT was compiled; D2_CXXFLAGS covers HOW, and link-d2.sh ties both the
# objects (OBJDIR) and the link line to it -- so ci/sanitize.sh's "-g -O1 -sASSERTIONS=2 -sSAFE_HEAP=1"
# build is a DIFFERENT engine. Without this term it took the release build's cache slot, and
# `engine-cache.sh put` could then hand the sanitizer wasm to every worktree under the release key
# (measured 2026-09-30: two worktrees each served a 4 MB sanitizer build under a clean release
# key). Empty (the default release build) leaves the key unchanged, so cached release engines stay a
# hit. Known remaining gap: Dockerfile.builder64 (the toolchain) is not in the key either; adding it
# now would move every existing key and force a rebuild of every worktree, so it is left for a change
# that lands with a full rebuild.
[ -z "${D2_CXXFLAGS:-}" ] || k="$k-cxx$(printf '%s' "$D2_CXXFLAGS" | sha256sum | cut -c1-8)"
inputs=(src wasm-build/link-d2.sh third_party ci/build.sh)
if [ -n "$(git status --porcelain -- "${inputs[@]}")" ]; then
  edits=$(
    {
      git diff --no-ext-diff --no-textconv --binary --submodule=diff HEAD -- "${inputs[@]}"
      while IFS= read -r -d '' file; do
        printf '%s\0' "$file"
        git hash-object -- "$file"
      done < <(git ls-files --others --exclude-standard -z -- "${inputs[@]}")
    } | sha256sum | cut -c1-12
  )
  # Keep the suffix: engine-cache.sh deliberately refuses to reuse dirty builds.
  k="$k-$edits-dirty"
fi
echo "$k"
