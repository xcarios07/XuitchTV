# Analysis bundle

This distributable archive keeps the compact, useful outputs of the APK analysis:

- `API_CONTRACT.md`
- `recovered_api_contract.json`
- `PORT_MAP.md`
- recovered activities/endpoints and original small configuration assets

The working directory also generated large intermediate DEX indexes (`dex_annotations.json`,
`dex_call_refs.json`, `dex_class_fields.json`, `dex_method_strings.json`). They are intentionally
excluded from the distributable ZIP because they can be regenerated with the scripts under
`tools/` from the supplied APK and would add tens of megabytes of mostly intermediate data.
