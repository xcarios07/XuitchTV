# XuitchTV-Next

XuitchTV-Next is the new Nintendo Switch media-client foundation for XuitchTV.
It is based on the proven TsVitch v0.3.2 playback stack and keeps the original
GPL-3.0 license and attribution.

## Current milestone: 0.1.0

- Nintendo Switch Borealis/libmpv player inherited from TsVitch.
- User-configurable M3U/M3U8 playlist URL.
- User-configurable Xtream server, username and password.
- Channel categories, search, favorites, history and downloads.
- Direct playback without the original TsVitch registration/advertising server.
- Analytics disabled.
- XuitchTV application name, NRO path and icon.
- Default public source: iptv-org Paraguay playlist.

## Configure a source

Open **Settings → Tools** inside the application. Choose either:

- **M3U8 Playlist** and enter an authorized M3U/M3U8 URL; or
- **Xtream Codes** and enter the server URL and credentials supplied by an
  authorized provider.

The local configuration is stored under `/config/XuitchTV` on Nintendo Switch.
Credentials and private playlist URLs are never committed to this repository.

## Nintendo Switch build

The GitHub Actions workflow produces both `XuitchTV.nro` and an SD-ready ZIP.
For a local devkitPro build:

```bash
docker run --platform linux/amd64 --rm \
  -e M3U8_URL="https://raw.githubusercontent.com/iptv-org/iptv/master/streams/py.m3u" \
  -v "$(pwd):/data" \
  devkitpro/devkita64:20251117 \
  bash -c "/data/scripts/build_switch.sh"
```

Install to:

```text
/switch/XuitchTV/XuitchTV.nro
```

## Content and licensing

XuitchTV-Next does not host or ship television channels, movies, series,
accounts or DRM-bypass mechanisms. Use only sources and content you are
authorized to access.

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and [LICENSE](LICENSE).
