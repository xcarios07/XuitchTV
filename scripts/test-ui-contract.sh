#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
python3 - "$ROOT" <<'PY'
from pathlib import Path
import sys
root = Path(sys.argv[1])
required_files = [
    'app/include/ui/IptvActivity.hpp',
    'app/source/ui/IptvActivity.cpp',
    'app/include/ui/PlayerActivity.hpp',
    'app/source/ui/PlayerActivity.cpp',
    'app/include/ui/VideoView.hpp',
    'app/source/ui/VideoView.cpp',
    'resources/xml/activity/main.xml',
    'resources/xml/activity/iptv.xml',
    'resources/xml/activity/player.xml',
]
for rel in required_files:
    p = root / rel
    assert p.exists() and p.stat().st_size > 0, f'missing {rel}'

checks = {
    'resources/xml/activity/main.xml': ['main/iptv/button', 'main/portal/button'],
    'resources/xml/activity/iptv.xml': ['iptv/refresh', 'iptv/categories', 'iptv/channels', 'iptv/status', 'iptv/count'],
    'resources/xml/activity/player.xml': ['player/video/host', 'player/title', 'player/status', 'player/pause', 'player/stop'],
}
for rel, needles in checks.items():
    text = (root / rel).read_text(encoding='utf-8')
    for needle in needles:
        assert needle in text, f'{needle} missing from {rel}'

video = (root / 'app/source/ui/VideoView.cpp').read_text(encoding='utf-8')
assert 'MPV_RENDER_API_TYPE_DEKO3D' in video
assert 'MPV_RENDER_PARAM_DEKO3D_FBO' in video
assert 'getFramebuffer' in video
assert 'mpv_render_context_render' in video

main = (root / 'app/source/ui/MainActivity.cpp').read_text(encoding='utf-8')
assert 'new IptvActivity()' in main
assert 'XuitchTV v0.5.5 - IPTV Shell Diagnostic' in main
for checkpoint in ('[01]', '[02]', '[03]', '[04]', '[05]'):
    assert checkpoint in main, f'{checkpoint} missing from MainActivity diagnostic log'

iptv_header = (root / 'app/include/ui/IptvActivity.hpp').read_text(encoding='utf-8')
for forbidden in ('HttpClient', 'IptvService', 'IptvNavigator', 'IptvPlaylist'):
    assert forbidden not in iptv_header, f'{forbidden} must not be constructed by diagnostic shell'

iptv = (root / 'app/source/ui/IptvActivity.cpp').read_text(encoding='utf-8')
for checkpoint in ('[11]', '[12]', '[13]', '[14]', '[15]', '[16]', '[18]'):
    assert checkpoint in iptv, f'{checkpoint} missing from IptvActivity diagnostic log'
for forbidden in ('AppConfig::', 'refreshPlaylist(', 'new PlayerActivity', 'service.refresh'):
    assert forbidden not in iptv, f'{forbidden} must remain disabled in v0.5.5 diagnostic shell'
print('v0.5.5 IPTV diagnostic shell contract test: OK')
PY

