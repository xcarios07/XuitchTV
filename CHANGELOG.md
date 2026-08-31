# XuitchTV changelog

## 0.5.1 — First Hardware Preview

- Locked the exact user-approved final 1:1 XuitchTV logo as the project master asset.
- Regenerated the NRO icon from that exact master image.
- Centralized runtime version/User-Agent through `BuildInfo.hpp`.
- Added a Switch Applet Mode warning recommending full-RAM/title-takeover execution for video.
- Added `opengl-glfinish=yes`, matching a stability workaround used by current Switch libmpv clients.
- Pinned the reproducible Docker builder to `devkitpro/devkita64:20251117`.
- Added build/preflight validation for `mpv/render_dk3d.h`.
- Added `deploy-nxlink.sh`, `release-switch.sh`, and the first-hardware-test checklist.

# Changelog

## 0.5.0 — First Hardware Build

- Added reproducible Nintendo Switch build paths for local devkitPro, Docker and GitHub Actions.
- Added preflight checks for toolchain/portlibs and an SD-card package generator.
- Improved Borealis dependency bootstrap to initialize nested submodules.
- Added player state callbacks so UI status follows MPV events.
- Preserve MPV end-of-file errors for real stream diagnostics.
- Added Switch-specific playback tuning (`vd-lavc-dr`, 3 decoder threads, `hwdec=auto`).
- Added host tests for player state/error propagation.
- Bumped runtime/User-Agent/NACP version to 0.5.0.

## 0.4.1 — Interactive IPTV + Switch VideoView

- Added interactive IPTV navigation model and tests.
- Added IPTV Paraguay activity with dynamic categories and channel buttons.
- Added player activity wired directly to IPTV channel URLs.
- Added pause/stop/back playback controls.
- Added `VideoView` with a deko3d/libmpv render-context implementation for Switch builds.
- Exposed the native libmpv handle from the playback core for the renderer layer.
- Configured MPV for `vo=libmpv` before initialization.
- Added UI contract tests and consolidated them into `test-all.sh`.
- Integrated the user-approved XuitchTV logo and regenerated the 256x256 NRO icon.

## 0.4.0 — IPTV Paraguay

- Added independent IPTV section and dynamic M3U/M3U8 playlist loading.
- Added default Paraguay playlist: https://iptv-org.github.io/iptv/countries/py.m3u
- Added extended M3U parser (tvg-id, tvg-name, tvg-logo, group-title).
- Added category grouping and stream health state.
- Added bounded libcurl stream probe so unavailable channels can be hidden without deleting them.
- Portal configuration is now optional for IPTV-only mode.

## 0.3.0 — XuitchTV identity + playback pipeline

- Renamed the application/runtime project to XuitchTV.
- Added original XuitchTV icon and wordmark assets.
- Moved historical source-app branding/provenance out of runtime resources.
- Added Live/VOD stream selection by quality and tag.
- Added SLB/CDN candidate ranking and deduplication.
- Preserved opaque token/play parameters without guessing protocol composition.
- Added libmpv playback core scaffold (`loadfile`, pause, stop, events).
- Added Switch pkg-config integration for `switch-libmpv`.
