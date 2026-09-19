#!/bin/sh
# Content-version the built engine so a CDN can NEVER serve a mismatched mix of two builds.
# Ported near-verbatim from openmw-web/wasm-build/version-engine.sh; the reasoning there holds
# unchanged: d2.js (loader) + d2.wasm (code) are one set with stable names, and across redeploys
# a browser could get the loader from build A and the wasm from build B -> "null function" at
# boot. So: hash EVERY file that gets the immutable cache header, move them into
# <play>/e/<hash>/, stamp the hash into index.html. Every build lives at a unique path.
#
# streamfs.js is in the hash for the same reason as in openmw-web: it is served immutable from
# the same dir, so a streamfs-only change must produce a new hash or returning visitors are
# pinned to the stale copy for up to a year.
#
# Run AFTER link + make_br. Idempotent on a fresh build tree. Usage: PLAY=play sh version-engine.sh
set -eu
PLAY="${PLAY:-${1:-play}}"
cd "$PLAY"
[ -f d2.wasm ] || { echo "version-engine: d2.wasm not found in $PLAY" >&2; exit 1; }

VER=$( { cat d2.wasm d2.js streamfs.js 2>/dev/null || true; } | { sha256sum 2>/dev/null || shasum -a 256; } | cut -c1-12 )
[ -n "$VER" ] || { echo "version-engine: could not hash engine files" >&2; exit 1; }

DIR="e/$VER"
mkdir -p "$DIR"
for f in d2.js d2.wasm d2.js.br d2.wasm.br streamfs.js; do
  [ -f "$f" ] && mv -f "$f" "$DIR/$f" || true
done

sed -e "s/__ENGINE_VERSION__/$VER/g" -e "s|src=\"streamfs.js\"|src=\"$DIR/streamfs.js\"|g" \
  index.html > index.html.tmp && mv index.html.tmp index.html
printf "%s" "$VER" > engine-version.txt
echo "version-engine: engine -> $PLAY/$DIR ; index.html stamped ($VER)"
