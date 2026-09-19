# Vendored third-party code

## unbikit.js

A Bink (`.bik`) container and video decoder, by David Hart (blennies), version 0.10.7 from npm,
`dist/unbikit.js` byte-for-byte. Upstream: <https://github.com/blennies/unbikit>. License: MIT OR
Apache-2.0, at your option (`unbikit.LICENSE.md` is the package's own LICENSE.md).

Why it is vendored rather than a dependency: this client is plain ES modules served straight out of
`build-wasm/site` (`play/serve.mjs`, `ci/stage.sh`) -- there is no bundler and no `node_modules` in
the page's world, so a library the page imports has to be a file in `play/`.

Attribution: this file and the licence beside it.
