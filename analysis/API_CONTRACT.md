# Recovered API contract (Phase 2)

Recovered **88** portal endpoints from DEX annotations.

## Confirmed transport pattern

- Core endpoints are declared as dynamic `{agreement}://{ip}/...` Retrofit routes.
- Core request wrappers call `Gson.toJson(bean)` and pass that JSON string as `@Body`.
- `agreement` and `ip` are dynamic path components; the port models them as one configurable `portalBaseUrl`.
- Several endpoints add `ProcessResult:false`; the port preserves this for login, home and live-data requests.

## Core request models

### login — `Lcom/request/bean/LoginBean;`
- `accountType` — `Ljava/lang/String;`
- `areaCode` — `Ljava/lang/String;`
- `channel` — `Ljava/lang/String;`
- `macAddr` — `Ljava/lang/String;`
- `matadata` — `Ljava/lang/String;`
- `password` — `Ljava/lang/String;`
- `signdata` — `Ljava/lang/String;`
- `type` — `Ljava/lang/String;`
- `userName` — `Ljava/lang/String;`
- `userToken` — `Ljava/lang/String;`
- `verificationCode` — `Ljava/lang/String;`
- `verificationToken` — `Ljava/lang/String;`

### home — `Lcom/request/bean/GetHomeBean;`
- `homePageCode` — `Ljava/lang/String;`
- `portalCode` — `Ljava/lang/String;`
- `userId` — `Ljava/lang/String;`
- `userToken` — `Ljava/lang/String;`
- `version` — `Ljava/lang/String;`

### live_data — `Lcom/request/bean/GetLiveDataBean;`
- `columnId` — `I`
- `dataVersion` — `Ljava/lang/String;`
- `expireTimeStr` — `Ljava/lang/String;`
- `pageNum` — `Ljava/lang/Integer;`
- `pageSize` — `Ljava/lang/Integer;`
- `portalCode` — `Ljava/lang/String;`
- `userId` — `Ljava/lang/String;`
- `userToken` — `Ljava/lang/String;`

### start_live — `Lcom/request/bean/StartPlayLiveBean;`
- `channelCode` — `Ljava/lang/String;`
- `columnId` — `Ljava/lang/Integer;`
- `portalCode` — `Ljava/lang/String;`
- `type` — `Ljava/lang/String;`
- `userId` — `Ljava/lang/String;`
- `userToken` — `Ljava/lang/String;`

### start_vod — `Lcom/request/bean/StartPlayVODBean;`
- `authType` — `Ljava/lang/String;`
- `columnId` — `Ljava/lang/Integer;`
- `contentId` — `Ljava/lang/String;`
- `episodeNumberList` — `[I`
- `portalCode` — `Ljava/lang/String;`
- `seriesContentId` — `Ljava/lang/String;`
- `startTime` — `I`
- `type` — `Ljava/lang/String;`
- `userId` — `Ljava/lang/String;`
- `userToken` — `Ljava/lang/String;`

## Playback chain recovered

1. `/v4/startPlayLive` returns `StartPlayLiveResultData.liveAddressList`.
2. Each `LiveAddress` contains `playCode`, `license`, `cdnType`, `quality`/`tag` metadata.
3. `/v10/startPlayVOD` returns episode/movie descriptors including `contentId` and license metadata.
4. `/v14/getSlbInfo` and `/v15/getSlbInfo` return CDN data containing `cdn_list`, `url_list[].url`, tokens and `play_params`.
5. The exact final URL-composition logic is not yet fully reconstructed, so Phase 2 intentionally does not guess it.

## Endpoint summary

| Method | Path | Retrofit result |
|---|---|---|
| POST | `/api/portalCore/addSubscribe` | `Lio/reactivex/Observable<Lcom/request/result/AddSubscribeResult;>;` |
| POST | `/api/portalCore/apkQueryCoupon` | `Lio/reactivex/Observable<Lcom/request/result/ApkQueryCouponResult;>;` |
| POST | `/api/portalCore/apkReceiveCoupon` | `Lio/reactivex/Observable<Lcom/request/result/ApkReceiveCouponResult;>;` |
| POST | `/api/portalCore/bindEmailGiftDays` | `Lio/reactivex/Observable<Lcom/request/result/BindEmailGiftDaysResult;>;` |
| POST | `/api/portalCore/bindPhone` | `Lio/reactivex/Observable<Lcom/request/result/BindPhoneResult;>;` |
| POST | `/api/portalCore/blSearchByContent` | `Lio/reactivex/Observable<Lcom/request/result/SearchByContentResult;>;` |
| POST | `/api/portalCore/blSearchByName` | `Lio/reactivex/Observable<Lcom/request/result/SearchByNameResult;>;` |
| POST | `/api/portalCore/changeBindPhone` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/checkForceBind` | `Lio/reactivex/Observable<Lcom/request/result/CheckForceBindResult;>;` |
| POST | `/api/portalCore/checkVerifiCode` | `Lio/reactivex/Observable<Lcom/request/result/CheckVerifiCodeResult;>;` |
| POST | `/api/portalCore/config/get` | `Lio/reactivex/Observable<Lcom/request/result/ConfigResult;>;` |
| POST | `/api/portalCore/delFavorite` | `Lio/reactivex/Observable<Lcom/request/result/DelFavoriteResult;>;` |
| POST | `/api/portalCore/delSubscribe` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/device-management/deleteDevice` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/device-management/getDevice` | `Lio/reactivex/Observable<Lcom/request/result/GetDeviceResult;>;` |
| POST | `/api/portalCore/emailResetPassNotice` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| GET | `/api/portalCore/epg/v2/getAllMatch` | `Lio/reactivex/Observable<Lcom/request/result/FootballMatchResult;>;` |
| GET | `/api/portalCore/epg/v2/getLineUps` | `Lio/reactivex/Observable<Lcom/request/result/LineupResult;>;` |
| GET | `/api/portalCore/epg/v2/getMatchDetail` | `Lio/reactivex/Observable<Lcom/request/result/MatchStatResult;>;` |
| GET | `/api/portalCore/epg/v2/getScoreboard` | `Lio/reactivex/Observable<Lcom/request/result/MatchScoreBoardResult;>;` |
| GET | `/api/portalCore/epg/v2/getShelveMatch` | `Lio/reactivex/Observable<Lcom/request/result/ShelveMatchResult;>;` |
| GET | `/api/portalCore/epg/v2/getTeamEvent` | `Lio/reactivex/Observable<Lcom/request/result/MatchEventResult;>;` |
| GET | `/api/portalCore/epg/v3/getFootballMatch` | `Lio/reactivex/Observable<Lcom/request/result/FootballMatchResult;>;` |
| GET | `/api/portalCore/epg/v4/getNearestMatch` | `Lio/reactivex/Observable<Lcom/request/result/FootballMatchResult;>;` |
| GET | `/api/portalCore/epg/v5/getNearestMatch` | `Lio/reactivex/Observable<Lcom/request/result/FootballMatchResult;>;` |
| POST | `/api/portalCore/feedback/getCustomerService` | `Lio/reactivex/Observable<Lcom/request/result/FeedBackContactResult;>;` |
| POST | `/api/portalCore/feedback/userFeedBack` | `Lio/reactivex/Observable<Lcom/request/result/UserFeedBackResult;>;` |
| POST | `/api/portalCore/getAllbackProgram` | `Lio/reactivex/Observable<Lcom/request/result/GetAllbackProgramResult;>;` |
| POST | `/api/portalCore/getAreaCode` | `Lio/reactivex/Observable<Lcom/request/result/GetAreaCodeResult;>;` |
| POST | `/api/portalCore/getBaseTime` | `Lio/reactivex/Observable<Lcom/request/result/BaseTimeResult;>;` |
| POST | `/api/portalCore/getEmailSuffix` | `Lio/reactivex/Observable<Lcom/request/result/GetEmailSuffixResult;>;` |
| POST | `/api/portalCore/getExchangeOrderInfo` | `Lio/reactivex/Observable<Lcom/request/result/GetExchangeOrderInfoResult;>;` |
| POST | `/api/portalCore/getFavorite` | `Lio/reactivex/Observable<Lcom/request/result/GetFavoritesResult;>;` |
| POST | `/api/portalCore/getHome` | `Lio/reactivex/Observable<Lcom/request/result/GetHomeResult;>;` |
| POST | `/api/portalCore/getItemByPerson` | `Lio/reactivex/Observable<Lcom/request/result/PersonWorksResult;>;` |
| POST | `/api/portalCore/getNextColumns` | `Lio/reactivex/Observable<Lcom/request/result/GetNextColumnResult;>;` |
| POST | `/api/portalCore/getPerson` | `Lio/reactivex/Observable<Lcom/request/result/PersonResult;>;` |
| POST | `/api/portalCore/getPropertiesInfo` | `Lio/reactivex/Observable<Lcom/request/result/PropertiesInfoResult;>;` |
| POST | `/api/portalCore/getRecommendColumnContents` | `Lio/reactivex/Observable<Lcom/request/result/RecommendColumnContentResult;>;` |
| POST | `/api/portalCore/getSubscribe` | `Lio/reactivex/Observable<Lcom/request/result/GetSubscribeResult;>;` |
| POST | `/api/portalCore/getVerifiCode` | `Lio/reactivex/Observable<Lcom/request/result/GetVerifiCodeResult;>;` |
| POST | `/api/portalCore/liveSearchByName` | `Lio/reactivex/Observable<Lcom/request/result/GetLiveDataResult;>;` |
| POST | `/api/portalCore/package/getOrderInfo` | `Lio/reactivex/Observable<Lcom/request/result/GetOrderInfoResult;>;` |
| POST | `/api/portalCore/package/getPackageCustomization` | `Lio/reactivex/Observable<Lcom/request/result/GetPackageResult;>;` |
| POST | `/api/portalCore/pwdCheck` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/qr/allowLogin` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/qr/token` | `Lio/reactivex/Observable<Lcom/request/result/GetQrTokenResult;>;` |
| POST | `/api/portalCore/register` | `Lio/reactivex/Observable<Lcom/request/result/RegisterResult;>;` |
| POST | `/api/portalCore/searchResourceOrPerson` | `Lio/reactivex/Observable<Lcom/request/result/SearchROrPResult;>;` |
| POST | `/api/portalCore/setPassword` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/terminalAuth` | `Lio/reactivex/Observable<Lcom/request/result/TerminalAuthResult;>;` |
| POST | `/api/portalCore/unBindEmail` | `Lio/reactivex/Observable<Lcom/request/result/UnBindEmailResult;>;` |
| POST | `/api/portalCore/updateBindEmailOrPwd` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v10/startPlayVOD` | `Lretrofit2/Call<Lcom/request/result/StartPlayVODResult;>;` |
| POST | `/api/portalCore/v10/startPlayVOD` | `Lio/reactivex/Observable<Lcom/request/result/StartPlayVODResult;>;` |
| POST | `/api/portalCore/v14/getSlbInfo` | `Lio/reactivex/Observable<Lcom/request/result/GetSlbInfoBeanResult;>;` |
| POST | `/api/portalCore/v15/getSlbInfo` | `Lio/reactivex/Observable<Lcom/request/result/GetSlbInfoBeanResult;>;` |
| POST | `/api/portalCore/v2/addFavorite` | `Lio/reactivex/Observable<Lcom/request/result/AddFavoriteResult;>;` |
| POST | `/api/portalCore/v2/bindEmail` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v2/changeBindEmail` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v2/getFree` | `Lio/reactivex/Observable<Lcom/request/result/FreeResult;>;` |
| POST | `/api/portalCore/v2/getInviteCode` | `Lio/reactivex/Observable<Lcom/request/result/GetInviteCodeResult;>;` |
| POST | `/api/portalCore/v2/sendEmailVerifyCode` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v2/unBind` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v2/updateRestrictedStatus` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v2/validateVerifyCode` | `Lio/reactivex/Observable<Lcom/request/result/VerifyEmailCodeResult;>;` |
| POST | `/api/portalCore/v3/filterByContent` | `Lio/reactivex/Observable<Lcom/request/result/FilterByContentResult;>;` |
| POST | `/api/portalCore/v3/filterGenre` | `Lio/reactivex/Observable<Lcom/request/result/FilterGenreResult;>;` |
| POST | `/api/portalCore/v3/getColumnContents` | `Lio/reactivex/Observable<Lcom/request/result/GetColumnContentsResult;>;` |
| POST | `/api/portalCore/v3/getColumnRecommendResource` | `Lio/reactivex/Observable<Lcom/request/result/GetColumnRecommendsResult;>;` |
| POST | `/api/portalCore/v3/getProgram` | `Lio/reactivex/Observable<Lcom/request/result/EpgResult;>;` |
| POST | `/api/portalCore/v3/getRecommends` | `Lio/reactivex/Observable<Lcom/request/result/GetRecommendsResult;>;` |
| POST | `/api/portalCore/v3/getShelveData` | `Lio/reactivex/Observable<Lcom/request/result/GetShelveResult;>;` |
| POST | `/api/portalCore/v3/searchByContent` | `Lio/reactivex/Observable<Lcom/request/result/SearchByContentResult;>;` |
| POST | `/api/portalCore/v3/searchByName` | `Lio/reactivex/Observable<Lcom/request/result/SearchByNameResult;>;` |
| POST | `/api/portalCore/v3/snToken` | `Lio/reactivex/Observable<Lcom/request/result/SnTokenResult;>;` |
| POST | `/api/portalCore/v4/getItemData` | `Lio/reactivex/Observable<Lcom/request/result/GetItemDataResult;>;` |
| POST | `/api/portalCore/v4/resetPwd` | `Lio/reactivex/Observable<Lcom/request/result/BaseResult;>;` |
| POST | `/api/portalCore/v4/startPlayLive` | `Lio/reactivex/Observable<Lcom/request/result/StartPlayLiveResult;>;` |
| POST | `/api/portalCore/v5/exchange` | `Lio/reactivex/Observable<Lcom/request/result/ExchangeResult;>;` |
| POST | `/api/portalCore/v5/heartbeat` | `Lio/reactivex/Observable<Lcom/request/result/HeartBeatResult;>;` |
| POST | `/api/portalCore/v5/loginOut` | `Lio/reactivex/Observable<Lcom/request/result/LoginResult;>;` |
| POST | `/api/portalCore/v5/qr/getResult` | `Lio/reactivex/Observable<Lcom/request/result/GetQrResult;>;` |
| POST | `/api/portalCore/v6/getLiveData` | `Lio/reactivex/Observable<Lcom/request/result/GetLiveDataResult;>;` |
| POST | `/api/portalCore/v6/startPlayBTV` | `Lio/reactivex/Observable<Lcom/request/result/StartPlayBTVResult;>;` |
| POST | `/api/portalCore/v8/active` | `Lio/reactivex/Observable<Lcom/request/result/ActiveResult;>;` |
| POST | `/api/portalCore/v8/login` | `Lio/reactivex/Observable<Lcom/request/result/LoginResult;>;` |
| POST | `/api/portalCore/v9/getAuthInfo` | `Lio/reactivex/Observable<Lcom/request/result/GetAuthInfoResult;>;` |