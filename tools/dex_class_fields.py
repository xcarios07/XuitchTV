#!/usr/bin/env python3
import argparse, json, struct
from pathlib import Path

def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def uleb(b,o):
    v=0; s=0; p=o
    while True:
        c=b[p]; p+=1; v|=(c&0x7f)<<s
        if not c&0x80: return v,p
        s+=7

def read_str(b,o):
    _,p=uleb(b,o); e=b.find(b'\0',p); return b[p:e].decode('utf-8','replace')

def parse(path):
    b=Path(path).read_bytes()
    ss,so=u32(b,56),u32(b,60); ts,to=u32(b,64),u32(b,68); fs,fo=u32(b,80),u32(b,84); cs,co=u32(b,96),u32(b,100)
    strings=[read_str(b,u32(b,so+i*4)) for i in range(ss)]
    types=[strings[u32(b,to+i*4)] for i in range(ts)]
    fields=[]
    for i in range(fs):
        o=fo+i*8; c=u16(b,o); t=u16(b,o+2); n=u32(b,o+4)
        fields.append({'class':types[c], 'type':types[t], 'name':strings[n]})
    out=[]
    for ci in range(cs):
        o=co+ci*32; cidx=u32(b,o); cdata=u32(b,o+24)
        if not cdata: continue
        p=cdata; sf,p=uleb(b,p); inf,p=uleb(b,p); dm,p=uleb(b,p); vm,p=uleb(b,p)
        rows=[]
        idx=0
        for kind,count in [('static',sf),('instance',inf)]:
            idx=0
            for _ in range(count):
                d,p=uleb(b,p); acc,p=uleb(b,p); idx+=d
                if idx<len(fields): rows.append({**fields[idx],'kind':kind,'access':acc})
        if rows: out.append({'dex':Path(path).name,'class':types[cidx], 'fields':rows})
    return out

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('dex',nargs='+'); ap.add_argument('-o','--out',required=True); a=ap.parse_args(); rows=[]
    for f in a.dex: rows += parse(f)
    Path(a.out).write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8'); print(len(rows))
if __name__=='__main__': main()
