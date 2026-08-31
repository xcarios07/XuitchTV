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
        if not c & 0x80: return v,p
        s+=7

def read_mutf8(b,o):
    _,p=uleb(b,o); e=b.find(b'\0',p)
    return b[p:e if e>=0 else len(b)].decode('utf-8','replace')

def signed(raw: bytes): return int.from_bytes(raw,'little',signed=True)
def unsigned(raw: bytes): return int.from_bytes(raw,'little',signed=False)

def parse(path: Path):
    b=path.read_bytes()
    string_ids_size,string_ids_off=u32(b,56),u32(b,60)
    type_ids_size,type_ids_off=u32(b,64),u32(b,68)
    method_ids_size,method_ids_off=u32(b,88),u32(b,92)
    class_defs_size,class_defs_off=u32(b,96),u32(b,100)
    strings=[read_mutf8(b,u32(b,string_ids_off+i*4)) for i in range(string_ids_size)]
    types=[strings[u32(b,type_ids_off+i*4)] for i in range(type_ids_size)]
    methods=[]
    for i in range(method_ids_size):
        o=method_ids_off+i*8
        cls=u16(b,o); proto=u16(b,o+2); name=u32(b,o+4)
        methods.append({'class':types[cls], 'name':strings[name], 'proto_idx':proto})

    def value(p):
        head=b[p]; p+=1; typ=head&0x1f; arg=head>>5; n=arg+1
        if typ in (0x00,0x02,0x04,0x06):
            raw=b[p:p+n]; p+=n; return signed(raw),p
        if typ in (0x03,0x10,0x11,0x15,0x16,0x18,0x19,0x1a,0x1b):
            raw=b[p:p+n]; p+=n; idx=unsigned(raw)
            if typ==0x18: return {'type': types[idx] if idx<len(types) else idx},p
            if typ==0x1a: return {'method': methods[idx] if idx<len(methods) else idx},p
            return idx,p
        if typ==0x17:
            raw=b[p:p+n]; p+=n; idx=unsigned(raw); return strings[idx] if idx<len(strings) else idx,p
        if typ==0x1c:
            sz,p=uleb(b,p); arr=[]
            for _ in range(sz):
                v,p=value(p); arr.append(v)
            return arr,p
        if typ==0x1d:
            a,p=annotation(p); return a,p
        if typ==0x1e: return None,p
        if typ==0x1f: return bool(arg),p
        raw=b[p:p+n]; p+=n; return {'unknown_type':typ,'raw':raw.hex()},p

    def annotation(p):
        tidx,p=uleb(b,p); sz,p=uleb(b,p); els={}
        for _ in range(sz):
            nidx,p=uleb(b,p); v,p=value(p); els[strings[nidx] if nidx<len(strings) else str(nidx)]=v
        return {'type':types[tidx] if tidx<len(types) else tidx,'elements':els},p

    def annotation_item(off):
        if not off: return None
        vis=b[off]; a,_=annotation(off+1); a['visibility']=vis; return a

    def annotation_set(off):
        if not off: return []
        sz=u32(b,off); return [annotation_item(u32(b,off+4+i*4)) for i in range(sz)]

    rows=[]
    for ci in range(class_defs_size):
        co=class_defs_off+ci*32
        class_idx=u32(b,co); ann_dir_off=u32(b,co+20)
        if not ann_dir_off: continue
        class_name=types[class_idx] if class_idx<len(types) else str(class_idx)
        p=ann_dir_off
        class_ann_off=u32(b,p); fsz=u32(b,p+4); msz=u32(b,p+8); psz=u32(b,p+12); p+=16
        p += fsz*8
        for _ in range(msz):
            midx=u32(b,p); aset=u32(b,p+4); p+=8
            anns=annotation_set(aset)
            if midx<len(methods): rows.append({'dex':path.name,'class':class_name,'method_idx':midx,'method':methods[midx]['name'],'annotations':anns,'parameter_annotations':[]})
        param_entries=[]
        for _ in range(psz):
            midx=u32(b,p); aref=u32(b,p+4); p+=8
            # annotation_set_ref_list
            if aref:
                cnt=u32(b,aref); sets=[]
                for j in range(cnt): sets.append(annotation_set(u32(b,aref+4+j*4)))
            else: sets=[]
            param_entries.append((midx,sets))
        # attach or create
        byidx={r['method_idx']:r for r in rows if r['dex']==path.name and r['class']==class_name}
        for midx,sets in param_entries:
            if midx in byidx: byidx[midx]['parameter_annotations']=sets
            elif midx<len(methods): rows.append({'dex':path.name,'class':class_name,'method_idx':midx,'method':methods[midx]['name'],'annotations':[],'parameter_annotations':sets})
    return rows

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('dex',nargs='+'); ap.add_argument('-o','--out',required=True); a=ap.parse_args()
    rows=[]
    for f in a.dex:
        try: rows += parse(Path(f))
        except Exception as e: print('ERR',f,repr(e))
    Path(a.out).write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
    print('annotated methods',len(rows))
if __name__=='__main__': main()
