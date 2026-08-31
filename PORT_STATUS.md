# XuitchTV — Port status

## Completed foundation

- [x] Native C++17/Borealis application structure.
- [x] XuitchTV identity, NACP name, SD paths and final logo/icon.
- [x] libcurl network layer and persistent configuration.
- [x] Portal Live/VOD/SLB models and route catalogue isolated under `analysis/`.
- [x] Live/VOD quality selection and CDN candidate resolution.
- [x] Independent IPTV section using the configurable Paraguay M3U source.
- [x] M3U metadata parser and category navigation.
- [x] Channel selection -> `PlayerActivity` -> libmpv.
- [x] deko3d `VideoView` for Switch builds.

## v0.5.1 — First Hardware Build readiness

- [x] Player state callback to keep the UI synchronized with libmpv.
- [x] MPV end-of-file errors retained and shown instead of silently becoming `Stopped`.
- [x] Switch playback options: direct rendering, 3 decoder threads and `hwdec=auto` fallback path.
- [x] devkitPro local preflight script.
- [x] Dedicated Docker build script using the official devkitPro A64 image.
- [x] GitHub Actions NRO build workflow.
- [x] SD-card package generator.
- [x] Borealis bootstrap now initializes nested submodules.
- [x] All host-side tests pass with warnings treated as errors for the non-UI core.
- [ ] Cross-compile `XuitchTV.nro` with devkitPro.
- [ ] Launch on a real Nintendo Switch in full-memory homebrew mode.
- [ ] Confirm M3U download from the console.
- [ ] Confirm first channel video + audio.
- [ ] Validate pause/back and error reporting.
- [ ] Test handheld <-> dock framebuffer transition.
- [ ] Tune buffering/hwdec based on hardware results.

## Post-MVP

After the first successful hardware playback:

- channel logos/grid UI;
- search and favorites;
- EPG/XMLTV;
- background playlist cache and health checks;
- portal browsing UI;
- polished full-screen player OSD.

No credential bypass or DRM circumvention is implemented.
