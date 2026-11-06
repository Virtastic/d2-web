# Contributing

Thanks for helping. A few rules keep the project legal and reviewable:

- **Never commit Blizzard content**: no MPQs, DLLs, EXEs, extracted art, sound, tables or
  screenshots of game assets. `.gitignore` blocks the common extensions; check your diff anyway.
- **Never paste decompiler output.** Describe what retail does in your own words and write the code
  yourself. Cite the retail function with a `// 1.13c <DLL>+0x<address>` comment.
- **Retail 1.13c is the reference.** When behaviour is unclear, match what the original game does,
  and say how you checked in the pull request.
- Keep changes small and focused: one fix or feature per pull request.
- Run the tests before opening a pull request: `node --test test/*.test.mjs`, and `bash ci/build.sh`
  if you touched `src/`.

Bug reports: say what you did, what you expected, what happened, your browser, and the URL flags.

## New here? Quick start

You never need write access. Every change comes in through a pull request and is reviewed before it is merged.

1. **Fork** the repository on GitHub and clone your fork.
2. Create a branch: `git checkout -b my-change`.
3. Make one small, focused change and test it as described above.
4. Push the branch to your fork and open a **pull request** against `main`.
5. A maintainer reviews it. Requests for changes are normal; they are not a rejection.

**Finding something to work on:** look for issues labelled
[`good first issue`](https://github.com/Virtastic/d2-web/labels/good%20first%20issue) or
[`help wanted`](https://github.com/Virtastic/d2-web/labels/help%20wanted), and comment on the issue to claim it.
For anything bigger than a small fix, open an issue first so we can agree on the approach.

**Questions?** Ask in the [Discord](https://discord.gg/PzFfDkbSue). Security issues go through [SECURITY.md](SECURITY.md), not a public issue.
