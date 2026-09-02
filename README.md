# XuitchTV

XuitchTV is an experimental native media client for Nintendo Switch homebrew, implemented in C++17 with Borealis, libcurl and libmpv.

## Current version: 0.7.0 — Portal Foundation

The main menu and IPTV navigation are stable on Nintendo Switch hardware. This
preview adds an original XuitchTV splash screen, a redesigned media-center menu,
a light two-column channel catalog with embedded logos and an OpenGL/libmpv video
surface. Selecting a channel first opens a safe player screen; MPV starts after
pressing **Reproducir**. Diagnostic checkpoints are written to
`sdmc:/switch/XuitchTV/iptv.log` and `sdmc:/switch/XuitchTV/player.log`.
Playlist-provided HTTP referrer and user-agent metadata are preserved and sent
to libmpv for streams that reject requests without their required headers.

Portal, Movies, Series and Sports now open a configuration/status screen instead
of remaining disabled. It loads `sdmc:/switch/XuitchTV/config.json` and can test
whether an explicitly configured authorized portal host is reachable. Login and
catalog browsing remain intentionally disabled until valid service parameters
and a verified response schema are available.

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
https://raw.githubusercontent.com/iptv-org/iptv/master/streams/py.m3u
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
- Portal configuration status and connectivity diagnostic on Nintendo Switch.

## Portal configuration

The SD package includes `config.example.json` but never overwrites
`config.json` during an update. To prepare an authorized portal, copy the
example to `sdmc:/switch/XuitchTV/config.json`, then set `portalBaseUrl` and
`portalCode`. Credentials are not included in the repository or example file.

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
