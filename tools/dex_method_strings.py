#!/usr/bin/env python3
from __future__ import annotations
import argparse, json, struct
from pathlib import Path


def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def uleb(b,o):
    v=0; s=0; p=o
    while True:
        c=b[p]; p+=1; v |= (c & 0x7f) << s
        if not (c & 0x80): return v,p
        s += 7

def read_mutf8(b,o):
    _,p=uleb(b,o)
    e=b.find(b'\0',p)
    raw=b[p:e if e>=0 else len(b)]
    return raw.decode('utf-8','replace')

def parse_dex(path: Path):
    b=path.read_bytes()
    if not b.startswith(b'dex\n'): raise ValueError(f'not dex: {path}')
    string_ids_size,string_ids_off=u32(b,56),u32(b,60)
    type_ids_size,type_ids_off=u32(b,64),u32(b,68)
    proto_ids_size,proto_ids_off=u32(b,72),u32(b,76)
    method_ids_size,method_ids_off=u32(b,88),u32(b,92)
    class_defs_size,class_defs_off=u32(b,96),u32(b,100)
    strings=[read_mutf8(b,u32(b,string_ids_off+i*4)) for i in range(string_ids_size)]
    type_string_idx=[u32(b,type_ids_off+i*4) for i in range(type_ids_size)]
    types=[strings[i] if i < len(strings) else '?' for i in type_string_idx]
    methods=[]
    for i in range(method_ids_size):
        o=method_ids_off+i*8
        cls_idx=u16(b,o); proto_idx=u16(b,o+2); name_idx=u32(b,o+4)
        methods.append({'class': types[cls_idx] if cls_idx<len(types) else '?','name':strings[name_idx] if name_idx<len(strings) else '?','proto_idx':proto_idx})

    class_methods=[]
    for ci in range(class_defs_size):
        o=class_defs_off+ci*32
        class_idx=u32(b,o); class_data_off=u32(b,o+24)
        if class_data_off==0: continue
        p=class_data_off
        sf,p=uleb(b,p); inf,p=uleb(b,p); dm,p=uleb(b,p); vm,p=uleb(b,p)
        # skip fields
        idx=0
        for _ in range(sf+inf):
            d,p=uleb(b,p); _,p=uleb(b,p); idx+=d
        for kind,count in [('direct',dm),('virtual',vm)]:
            midx=0
            for _ in range(count):
                d,p=uleb(b,p); access,p=uleb(b,p); code_off,p=uleb(b,p); midx+=d
                if midx < len(methods):
                    class_methods.append((midx,code_off,kind))
    out=[]
    for midx,code_off,kind in class_methods:
        if not code_off or code_off+16>len(b): continue
        insns_size=u32(b,code_off+12)
        insns_off=code_off+16
        units=[]
        for j in range(insns_size):
            oo=insns_off+j*2
            if oo+2>len(b): break
            units.append(u16(b,oo))
        sidxs=[]
        # scan for const-string and jumbo. Some false positives possible but useful for correlation.
        for j,w in enumerate(units):
            op=w & 0xff
            if op==0x1a and j+1<len(units):
                si=units[j+1]
                if si<len(strings): sidxs.append(si)
            elif op==0x1b and j+2<len(units):
                si=units[j+1] | (units[j+2]<<16)
                if si<len(strings): sidxs.append(si)
        if sidxs:
            uniq=[]; seen=set()
            for si in sidxs:
                if si not in seen:
                    seen.add(si); uniq.append(strings[si])
            m=methods[midx]
            out.append({'dex':path.name,'method_idx':midx,'class':m['class'],'method':m['name'],'kind':kind,'strings':uniq})
    return out

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('dex',nargs='+'); ap.add_argument('-o','--out',required=True)
    a=ap.parse_args(); allm=[]
    for d in a.dex:
        try: allm.extend(parse_dex(Path(d)))
        except Exception as e: print('ERR',d,e)
    Path(a.out).write_text(json.dumps(allm,ensure_ascii=False,indent=2),encoding='utf-8')
    print('methods_with_strings',len(allm))

if __name__=='__main__': main()
