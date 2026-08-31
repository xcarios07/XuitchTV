#!/usr/bin/env python3
from pathlib import Path
import re, subprocess, sys, json
apk_dir=Path(sys.argv[1] if len(sys.argv)>1 else '.')
paths=set(); classes=set()
for dex in apk_dir.glob('classes*.dex'):
    try:
        out=subprocess.check_output(['strings','-a',str(dex)], text=True, errors='ignore')
    except Exception:
        continue
    for s in out.splitlines():
        for m in re.findall(r'/api/portalCore/[A-Za-z0-9_./{}?-]+', s): paths.add(m.rstrip(');,\"\''))
        if 'Activity' in s and ('com.' in s or '/' in s):
            for m in re.findall(r'Lcom/[A-Za-z0-9_/$]+Activity;', s):
                classes.add(m[1:-1].replace('/','.'))
print(json.dumps({'endpoint_candidates':sorted(paths),'activity_candidates':sorted(classes)}, indent=2))
