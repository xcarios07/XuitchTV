# XuitchTV build notes — v0.5.1

## Host validation

```bash
./scripts/test-all.sh
```

The suite validates portal models, HTTP compilation, config persistence, playback selection/resolution, player state/error propagation, M3U parsing/catalogs, IPTV navigation and UI wiring contracts.

## Nintendo Switch dependencies

The current Switchfin development instructions use the same core package set XuitchTV targets:

```bash
sudo dkp-pacman -S switch-dev switch-glfw switch-libwebp switch-curl switch-libmpv
```

Then:

```bash
./scripts/preflight-switch.sh
./scripts/bootstrap-deps.sh
./scripts/build-switch.sh
```

Expected artifact:

```text
build_switch/XuitchTV.nro
```

## Docker build

```bash
./scripts/bootstrap-deps.sh
./scripts/build-switch-docker.sh
```

The script installs the required Switch portlibs inside `devkitpro/devkita64:latest`, sources `switchvars.sh`, cross-compiles and requests the `XuitchTV.nro` target.

## GitHub Actions

`.github/workflows/build-switch.yml` performs the same Docker build and uploads:

```text
XuitchTV-nro
XuitchTV-SD
```

## SD package

After a successful build:

```bash
./scripts/package-sd.sh
```

produces:

```text
dist/XuitchTV-SD.zip
  switch/XuitchTV/XuitchTV.nro
  switch/XuitchTV/config.json
```

## Renderer

`VideoView` creates a libmpv deko3d render context when these are present:

- `XUITCHTV_HAS_MPV=1`
- `BOREALIS_USE_DEKO3D`
- Borealis Switch video context
- `switch-libmpv` with `render_dk3d.h`

It resolves the current Borealis framebuffer, signals the ready fence, renders with `MPV_RENDER_PARAM_DEKO3D_FBO`, waits for the done fence and reports the swap. This mirrors the active deko3d/libmpv approach used by current native Switch media clients.

The current ChatGPT execution environment has neither Docker nor devkitPro, so the ARM64/NRO link cannot be executed here. The remaining validation is the real cross-compile and hardware test.
