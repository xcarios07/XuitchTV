# XuitchTV

XuitchTV is an experimental native media client for Nintendo Switch homebrew, implemented in C++17 with Borealis, libcurl and libmpv.

## Current version: 0.5.6 — IPTV Navigation Diagnostic

The main menu is stable on Nintendo Switch hardware. This diagnostic build
opens an offline IPTV shell with networking, playlist refresh and MPV disabled.
Activity transitions are disabled while navigation is verified with checkpoints
in `sdmc:/switch/XuitchTV/iptv.log`.

The MVP path is now wired end-to-end in source code:

```text
Inicio
  -> IPTV Paraguay
  -> Descargar M3U
  -> Categorias
  -> Canales
  -> Player
  -> libmpv
  -> deko3d
  -> Nintendo Switch framebuffer
```

The default IPTV playlist is configurable and currently points to the public Paraguay country playlist from iptv-org:

```text
https://iptv-org.github.io/iptv/countries/py.m3u
```

Channels are loaded dynamically; they are not baked into the `.nro`.

## Implemented

- XuitchTV standalone identity and final logo/icon resources.
- Persistent config at `sdmc:/switch/XuitchTV/config.json`.
- Public M3U/M3U8 download with libcurl.
- Extended M3U metadata parsing and category grouping.
- Interactive IPTV category/channel navigation.
- Optional stream reachability checks without permanently deleting channels.
- libmpv playback core with error/state propagation to the UI.
- Switch playback tuning and hardware decode auto-selection when supported.
- deko3d `mpv_render_context` renderer targeting the Borealis Switch framebuffer.
- Reproducible build paths for local devkitPro, Docker and GitHub Actions.
- SD-card packaging script.
- Portal/API analysis isolated under `analysis/` for authorized services/content.

## Host tests

```bash
./scripts/test-all.sh
```

## Build the first NRO

See `HARDWARE_BUILD.md`.

Typical Docker path:

```bash
./scripts/bootstrap-deps.sh
./scripts/build-switch-docker.sh
./scripts/package-sd.sh
```

Expected artifacts:

```text
build_switch/XuitchTV.nro
dist/XuitchTV-SD.zip
```

No DRM or authentication bypass is implemented. Use the portal path only with services and content you are authorized to access.
