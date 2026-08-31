#!/usr/bin/env python3
from __future__ import annotations
import json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
anns=json.loads((ROOT/'analysis/dex_annotations.json').read_text())
fields=json.loads((ROOT/'analysis/dex_class_fields.json').read_text())
field_map={c['class']:[{'name':f['name'],'type':f['type']} for f in c['fields'] if f['kind']=='instance'] for c in fields}
ann_http={'Lmf/o;':'POST','Lmf/f;':'GET'}
param_kind={'Lmf/s;':'path','Lmf/t;':'query','Lmf/a;':'body','Lmf/y;':'url'}
rows=[]
for r in anns:
    endpoint=None; http=None; headers=[]; signature=[]
    for a in r['annotations']:
        t=a['type']; e=a.get('elements',{})
        if t in ann_http:
            http=ann_http[t]; endpoint=e.get('value')
        elif t=='Lmf/k;': headers=e.get('value',[]) if isinstance(e.get('value',[]),list) else [e.get('value')]
        elif t=='Ldalvik/annotation/Signature;': signature=e.get('value',[])
    if not isinstance(endpoint,str) or '/api/portalCore/' not in endpoint: continue
    path=re.sub(r'^\{agreement\}://\{ip\}','',endpoint)
    params=[]
    for pset in r.get('parameter_annotations',[]):
        entry={'kind':'unknown'}
        for a in pset:
            if a['type'] in param_kind:
                entry={'kind':param_kind[a['type']]}
                if 'value' in a.get('elements',{}): entry['name']=a['elements']['value']
                break
        params.append(entry)
    rows.append({'path':path,'http_method':http or 'UNKNOWN','headers':headers,'retrofit_class':r['class'],'retrofit_method':r['method'],'signature':''.join(signature),'parameters':params})
rows.sort(key=lambda x:x['path'])
models={}
for cls in ['Lcom/request/bean/LoginBean;','Lcom/request/bean/GetHomeBean;','Lcom/request/bean/GetLiveDataBean;','Lcom/request/bean/StartPlayLiveBean;','Lcom/request/bean/StartPlayVODBean;',
            'Lcom/request/result/LoginResultData;','Lcom/request/result/StartPlayLiveResultData;','Lcom/request/result/LiveAddress;','Lcom/request/result/StartPlayVODResultData;','Lcom/request/result/StartPlayVODResultDataItem;','Lcom/request/result/MovieList;','Lcom/request/result/GetSlbInfoBeanResultData;','Lcom/request/result/CdnListBeanResult;','Lcom/request/result/UrlListBeanResult;']:
    if cls in field_map: models[cls]=field_map[cls]
known={'login':'Lcom/request/bean/LoginBean;','home':'Lcom/request/bean/GetHomeBean;','live_data':'Lcom/request/bean/GetLiveDataBean;','start_live':'Lcom/request/bean/StartPlayLiveBean;','start_vod':'Lcom/request/bean/StartPlayVODBean;'}
payload={'source':'Recovered from DEX annotations/field tables in supplied APK','transport_notes':{'Lmf/o':'Retrofit POST (inferred from usage)','Lmf/f':'Retrofit GET (inferred from usage)','Lmf/s':'@Path','Lmf/t':'@Query','Lmf/a':'@Body','body_encoding':'Core request wrappers call Gson.toJson(bean) before Retrofit; no extra encryption step observed in those wrappers.'},'endpoints':rows,'known_models':known,'models':models}
(ROOT/'analysis/recovered_api_contract.json').write_text(json.dumps(payload,ensure_ascii=False,indent=2))

md=['# Recovered API contract (Phase 3)','',f'Recovered **{len(rows)}** portal endpoints from DEX annotations.','', '## Confirmed transport pattern','', '- Core endpoints are declared as dynamic `{agreement}://{ip}/...` Retrofit routes.', '- Core request wrappers call `Gson.toJson(bean)` and pass that JSON string as `@Body`.', '- `agreement` and `ip` are dynamic path components; the port models them as one configurable `portalBaseUrl`.', '- Several endpoints add `ProcessResult:false`; the port preserves this for login, home and live-data requests.', '', '## Core request models','']
for key,cls in known.items():
    md.append(f'### {key} — `{cls}`')
    for f in models.get(cls,[]): md.append(f'- `{f["name"]}` — `{f["type"]}`')
    md.append('')
md += ['## Playback chain recovered','', '1. `/v4/startPlayLive` returns `StartPlayLiveResultData.liveAddressList`.', '2. Each `LiveAddress` contains `playCode`, `license`, `cdnType`, `quality`/`tag` metadata.', '3. `/v10/startPlayVOD` returns episode/movie descriptors including `contentId` and license metadata.', '4. `/v14/getSlbInfo` and `/v15/getSlbInfo` return CDN data containing `cdn_list`, `url_list[].url`, tokens and `play_params`.', '5. The exact final URL-composition logic is not yet fully reconstructed, so Phase 3 intentionally does not guess it.', '', '## Endpoint summary','', '| Method | Path | Retrofit result |', '|---|---|---|']
for r in rows:
    sig=r['signature']; ret=sig.split(')')[-1] if ')' in sig else sig
    md.append(f'| {r["http_method"]} | `{r["path"]}` | `{ret}` |')
(ROOT/'analysis/API_CONTRACT.md').write_text('\n'.join(md))
print('wrote',len(rows),'endpoints')
