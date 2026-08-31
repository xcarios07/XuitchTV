#!/usr/bin/env python3
from __future__ import annotations
import argparse,json,struct
from pathlib import Path

def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def uleb(b,o):
 v=0;s=0;p=o
 while 1:
  c=b[p];p+=1;v|=(c&0x7f)<<s
  if not c&0x80:return v,p
  s+=7

def readstr(b,o):
 _,p=uleb(b,o);e=b.find(b'\0',p);return b[p:e].decode('utf-8','replace')
def parse(path):
 b=Path(path).read_bytes(); ss,so=u32(b,56),u32(b,60); ts,to=u32(b,64),u32(b,68); ms,mo=u32(b,88),u32(b,92); cs,co=u32(b,96),u32(b,100)
 strings=[readstr(b,u32(b,so+i*4)) for i in range(ss)]; types=[strings[u32(b,to+i*4)] for i in range(ts)]
 methods=[]
 for i in range(ms):
  o=mo+i*8;c=u16(b,o);p=u16(b,o+2);n=u32(b,o+4);methods.append({'class':types[c],'name':strings[n],'proto':p})
 enc=[]
 for ci in range(cs):
  o=co+ci*32; cidx=u32(b,o); data=u32(b,o+24)
  if not data:continue
  p=data;sf,p=uleb(b,p);inf,p=uleb(b,p);dm,p=uleb(b,p);vm,p=uleb(b,p)
  idx=0
  for _ in range(sf+inf):d,p=uleb(b,p);a,p=uleb(b,p);idx+=d
  for kind,cnt in [('direct',dm),('virtual',vm)]:
   midx=0
   for _ in range(cnt):
    d,p=uleb(b,p);a,p=uleb(b,p);code,p=uleb(b,p);midx+=d
    if code:enc.append((midx,code))
 out=[]
 for midx,code in enc:
  if code+16>len(b):continue
  n=u32(b,code+12); off=code+16; units=[u16(b,off+2*j) for j in range(n) if off+2*j+2<=len(b)]
  calls=[]; strs=[]
  for j,w in enumerate(units):
   op=w&0xff
   if op==0x1a and j+1<len(units):
    si=units[j+1]
    if si<len(strings):strs.append(strings[si])
   elif op==0x1b and j+2<len(units):
    si=units[j+1]|units[j+2]<<16
    if si<len(strings):strs.append(strings[si])
   elif (0x6e<=op<=0x72 or 0x74<=op<=0x78 or op in (0xfa,0xfb)) and j+1<len(units):
    mi=units[j+1]
    if mi<len(methods):calls.append(mi)
  if calls:
   m=methods[midx]
   out.append({'dex':Path(path).name,'caller_idx':midx,'caller_class':m['class'],'caller_method':m['name'],'calls':list(dict.fromkeys(calls)),'strings':list(dict.fromkeys(strs))})
 return methods,out

def main():
 ap=argparse.ArgumentParser();ap.add_argument('dex',nargs='+');ap.add_argument('-o','--out',required=True);a=ap.parse_args(); payload=[]
 for f in a.dex:
  methods,refs=parse(f); payload.append({'dex':Path(f).name,'methods':methods,'callers':refs})
 Path(a.out).write_text(json.dumps(payload,ensure_ascii=False),encoding='utf-8');print('ok')
if __name__=='__main__':main()
