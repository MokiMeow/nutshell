# Milestone 7: Polish (portfolio pass) ✅ (done)

**Goal:** turn a working kernel into a repo that impresses on sight. The code is
done; this is about proof, presentation, and repeatability.

## Tasks

### Proof it works
- [x] Add a QEMU **isa-debug-exit** test build: a `tests/` entry that boots,
      runs assertions (pmm/heap self-tests, a scripted shell command), and exits
      QEMU with code 0 on success via port `0xF4`.
- [x] Extend CI to run that test and assert the exit code, in addition to the
      serial banner check.
- [x] Have each `*_init` print `[ok] <subsystem>`; assert the full set appears
      in the serial log in CI.

### Presentation
- [x] Record the **boot-to-shell GIF** (power on → banner → `help` →
      `echo` → `mem` → `uptime` → `clear`) and embed it at the top of the
      README. See [docs/01 §4](../01-getting-started.md).
- [x] Add a short "How it boots" diagram/section to the README (link to
      [docs/03](../03-boot-process.md)).
- [x] Screenshot of a deliberate `panic` showing the fault report.

### Hygiene
- [x] Ensure every doc's status tables are accurate; all milestone DoDs ticked.
- [x] Update `CHANGELOG.md`; move items from Unreleased to `1.0.0`.
- [x] Confirm the build is warning-free and CI is green on `main`.
- [x] Tag the release: `git tag v1.0.0`.

## Definition of Done

- [x] CI runs build + smoke-boot + kernel self-test, all green on `main`.
- [x] README opens with the GIF and a crisp description.
- [x] `v1.0.0` tagged. The repo reads as finished, not abandoned.

## After this

The stretch goals in [the roadmap](../04-roadmap.md) (history, tab-completion,
in-RAM FS, higher-half mapping, cross-compiler CI) are all optional follow-ups.
Ship `v1.0.0` first.
