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
assert 'XuitchTV v0.5.7 - IPTV Playlist Test' in main
assert 'brls::TransitionAnimation::NONE' in main
assert 'brls::TransitionAnimation::SLIDE_LEFT' not in main
for checkpoint in ('[01]', '[02]', '[03]', '[04]', '[05]'):
    assert checkpoint in main, f'{checkpoint} missing from MainActivity diagnostic log'

iptv_header = (root / 'app/include/ui/IptvActivity.hpp').read_text(encoding='utf-8')
for forbidden in ('HttpClient', 'IptvService', 'PlayerActivity'):
    assert forbidden not in iptv_header, f'{forbidden} must remain out of the activity lifetime'
assert 'IptvNavigator' in iptv_header
assert 'IptvPlaylist' in iptv_header

iptv = (root / 'app/source/ui/IptvActivity.cpp').read_text(encoding='utf-8')
for checkpoint in ('[11]', '[12]', '[13]', '[14]', '[15]', '[16]', '[18]', '[20]', '[21]', '[22]', '[23]', '[30]', '[31]', '[32]', '[33]', '[34]', '[35]', '[36]', '[37]', '[38]', '[39]', '[40]', '[41]'):
    assert checkpoint in iptv, f'{checkpoint} missing from IptvActivity diagnostic log'
for required in ('api::HttpClient http', 'iptv::IptvService service(http)', 'service.refresh(', 'renderCategories()', 'renderChannels()'):
    assert required in iptv, f'{required} missing from v0.5.7 playlist test'
for forbidden in ('AppConfig::', 'new PlayerActivity', 'SLIDE_LEFT'):
    assert forbidden not in iptv, f'{forbidden} must remain disabled in v0.5.7 playlist test'
print('v0.5.7 IPTV playlist contract test: OK')
PY
