#include "api/PortalModels.hpp"
#include "util/JsonLite.hpp"

#include <utility>

namespace xuitch::api {
namespace {
using util::JsonValue;

void put(JsonValue::Object& o, const char* key, const std::string& value) { o[key] = JsonValue(value); }
void put(JsonValue::Object& o, const char* key, int value) { o[key] = JsonValue(value); }

std::string stringAt(const JsonValue* object, const char* key) {
    if (!object) return {};
    const JsonValue* v = object->get(key);
    if (!v) return {};
    if (v->isString()) return v->asString();
    if (v->isNumber()) return std::to_string(static_cast<long long>(v->asNumber()));
    if (v->isBool()) return v->asBool() ? "true" : "false";
    return {};
}

int intAt(const JsonValue* object, const char* key) {
    if (!object) return 0;
    const JsonValue* v = object->get(key);
    if (!v) return 0;
    if (v->isNumber()) return static_cast<int>(v->asNumber());
    if (v->isString()) {
        try { return std::stoi(v->asString()); }
        catch (...) { return 0; }
    }
    return 0;
}

bool parseEnvelope(const std::string& json, PortalResponseInfo& info, JsonValue& root,
                   const JsonValue*& data, std::string* error) {
    if (error) error->clear();
    if (!JsonValue::parse(json, root, error) || !root.isObject()) {
        if (error && error->empty()) *error = "Portal response is not a JSON object";
        return false;
    }
    info.returnCode = stringAt(&root, "returnCode");
    info.errorMessage = stringAt(&root, "errorMessage");
    data = root.get("data");
    if (!data || !data->isObject()) {
        if (error && error->empty()) *error = "Portal response does not contain an object named data";
        return false;
    }
    return true;
}

const JsonValue::Array* arrayAt(const JsonValue* object, const char* key) {
    if (!object) return nullptr;
    const JsonValue* value = object->get(key);
    if (!value || !value->isArray()) return nullptr;
    return &value->asArray();
}
} // namespace

std::string LoginRequest::toJson() const {
    JsonValue::Object o;
    put(o,"accountType",accountType); put(o,"areaCode",areaCode); put(o,"channel",channel);
    put(o,"macAddr",macAddr); put(o,"matadata",matadata); put(o,"password",password);
    put(o,"signdata",signdata); put(o,"type",type); put(o,"userName",userName);
    put(o,"userToken",userToken); put(o,"verificationCode",verificationCode); put(o,"verificationToken",verificationToken);
    return JsonValue(std::move(o)).stringify();
}

std::string GetHomeRequest::toJson() const {
    JsonValue::Object o;
    put(o,"homePageCode",homePageCode); put(o,"portalCode",portalCode); put(o,"userId",userId);
    put(o,"userToken",userToken); put(o,"version",version);
    return JsonValue(std::move(o)).stringify();
}

std::string GetLiveDataRequest::toJson() const {
    JsonValue::Object o;
    put(o,"columnId",columnId); put(o,"dataVersion",dataVersion); put(o,"expireTimeStr",expireTimeStr);
    put(o,"pageNum",pageNum); put(o,"pageSize",pageSize); put(o,"portalCode",portalCode);
    put(o,"userId",userId); put(o,"userToken",userToken);
    return JsonValue(std::move(o)).stringify();
}

std::string StartPlayLiveRequest::toJson() const {
    JsonValue::Object o;
    put(o,"channelCode",channelCode); put(o,"columnId",columnId); put(o,"portalCode",portalCode);
    put(o,"type",type); put(o,"userId",userId); put(o,"userToken",userToken);
    return JsonValue(std::move(o)).stringify();
}

std::string StartPlayVodRequest::toJson() const {
    JsonValue::Object o;
    put(o,"authType",authType); put(o,"columnId",columnId); put(o,"contentId",contentId);
    JsonValue::Array episodes; for (int episode : episodeNumberList) episodes.emplace_back(episode);
    o["episodeNumberList"] = JsonValue(std::move(episodes));
    put(o,"portalCode",portalCode); put(o,"seriesContentId",seriesContentId); put(o,"startTime",startTime);
    put(o,"type",type); put(o,"userId",userId); put(o,"userToken",userToken);
    return JsonValue(std::move(o)).stringify();
}

std::string GetSlbInfoRequest::toJson() const {
    JsonValue::Object o;
    put(o,"appParams",appParams); put(o,"appVer",appVer); put(o,"encMediaSupported",encMediaSupported);
    put(o,"hasPay",hasPay); put(o,"lang",lang);
    JsonValue::Array liveCodes; for (const auto& code : liveCodeList) liveCodes.emplace_back(code);
    o["liveCodeList"] = JsonValue(std::move(liveCodes));
    put(o,"pipFlag",pipFlag); put(o,"portalCode",portalCode); put(o,"reserve1",reserve1);
    put(o,"type",type); put(o,"userId",userId); put(o,"userIdentity",userIdentity); put(o,"userToken",userToken);
    return JsonValue(std::move(o)).stringify();
}

bool parseLoginResponse(const std::string& json, PortalResponseInfo& info, LoginResponseData& data, std::string* error) {
    JsonValue root;
    const JsonValue* d = nullptr;
    if (!parseEnvelope(json, info, root, d, error)) return false;
    data.userId = stringAt(d, "userId");
    data.userToken = stringAt(d, "userToken");
    data.token = stringAt(d, "token");
    data.accountType = stringAt(d, "accountType");
    data.areaCode = stringAt(d, "areaCode");
    data.heartBeatTime = stringAt(d, "heartBeatTime");
    return true;
}

bool parseStartPlayLiveResponse(const std::string& json, PortalResponseInfo& info,
                                StartPlayLiveResponseData& data, std::string* error) {
    JsonValue root;
    const JsonValue* d = nullptr;
    if (!parseEnvelope(json, info, root, d, error)) return false;
    data = {};
    data.name = stringAt(d, "name");
    if (const auto* list = arrayAt(d, "liveAddressList")) {
        for (const auto& item : *list) {
            if (!item.isObject()) continue;
            LiveAddress address;
            address.avFormat = stringAt(&item, "AVFormat");
            address.cdnType = stringAt(&item, "cdnType");
            address.license = stringAt(&item, "license");
            address.playCode = stringAt(&item, "playCode");
            address.quality = stringAt(&item, "quality");
            address.tag = stringAt(&item, "tag");
            data.liveAddressList.push_back(std::move(address));
        }
    }
    return true;
}

bool parseStartPlayVodResponse(const std::string& json, PortalResponseInfo& info,
                               StartPlayVodResponseData& data, std::string* error) {
    JsonValue root;
    const JsonValue* d = nullptr;
    if (!parseEnvelope(json, info, root, d, error)) return false;
    data = {};
    data.name = stringAt(d, "name");
    data.seriesFlag = stringAt(d, "seriesFlag");
    data.vodFreeCount = intAt(d, "vodFreeCount");
    data.vodFreeFlag = stringAt(d, "vodFreeFlag");
    if (const auto* episodes = arrayAt(d, "episodeList")) {
        for (const auto& episodeValue : *episodes) {
            if (!episodeValue.isObject()) continue;
            VodEpisode episode;
            episode.episodeNumber = intAt(&episodeValue, "episodeNumber");
            episode.programContentId = stringAt(&episodeValue, "programContentId");
            if (const auto* movies = arrayAt(&episodeValue, "totalMovieList")) {
                for (const auto& movieValue : *movies) {
                    if (!movieValue.isObject()) continue;
                    VodMovie movie;
                    movie.audioInfo = stringAt(&movieValue, "audioInfo");
                    movie.audioType = stringAt(&movieValue, "audioType");
                    movie.bitRateType = stringAt(&movieValue, "bitRateType");
                    movie.contentId = stringAt(&movieValue, "contentId");
                    movie.encodeFormat = stringAt(&movieValue, "encodeFormat");
                    movie.quality = stringAt(&movieValue, "quality");
                    movie.screenFormat = stringAt(&movieValue, "screenFormat");
                    movie.terminalType = stringAt(&movieValue, "terminalType");
                    movie.type = stringAt(&movieValue, "type");
                    movie.videoFormat = stringAt(&movieValue, "videoFormat");
                    movie.videoType = stringAt(&movieValue, "videoType");
                    episode.movies.push_back(std::move(movie));
                }
            }
            data.episodes.push_back(std::move(episode));
        }
    }
    return true;
}

bool parseSlbInfoResponse(const std::string& json, PortalResponseInfo& info,
                          SlbInfoResponseData& data, std::string* error) {
    JsonValue root;
    const JsonValue* d = nullptr;
    if (!parseEnvelope(json, info, root, d, error)) return false;
    data = {};
    data.errorCode = intAt(d, "error_code");
    data.invalidTime = stringAt(d, "invalidTime");
    data.mergeRstStatus = intAt(d, "merge_rst_status");
    data.nowTime = stringAt(d, "nowTime");
    data.playParams = stringAt(d, "play_params");
    data.reserveA = stringAt(d, "reserve_a");
    data.reserveB = stringAt(d, "reserve_b");
    data.rstStatus = intAt(d, "rst_status");
    data.switchLiveSourceTime = intAt(d, "switchLiveSourceTime");
    data.switchLiveSourceTimeV2 = stringAt(d, "switchLiveSourceTimeV2");
    data.switchVodSourceTime = intAt(d, "switchVodSourceTime");
    data.switchVodSourceTimeV2 = stringAt(d, "switchVodSourceTimeV2");

    if (const auto* cdns = arrayAt(d, "cdn_list")) {
        for (const auto& cdnValue : *cdns) {
            if (!cdnValue.isObject()) continue;
            SlbCdnEntry cdn;
            cdn.cdnId = stringAt(&cdnValue, "cdn_id");
            cdn.cdnIdMark = stringAt(&cdnValue, "cdn_id_mark");
            cdn.cdnType = stringAt(&cdnValue, "cdn_type");
            cdn.groupIdMark = stringAt(&cdnValue, "group_id_mark");
            cdn.gslbParams = stringAt(&cdnValue, "gslb_params");
            cdn.mainAddr = stringAt(&cdnValue, "main_addr");
            cdn.mainAddrMark = stringAt(&cdnValue, "main_addr_mark");
            cdn.ruleIdMark = stringAt(&cdnValue, "rule_id_mark");
            cdn.serialNumber = intAt(&cdnValue, "serial_number");
            cdn.sparedAddr = stringAt(&cdnValue, "spared_addr");
            cdn.sparedAddrMark = stringAt(&cdnValue, "spared_addr_mark");
            cdn.tag = stringAt(&cdnValue, "tag");
            cdn.token = stringAt(&cdnValue, "token");
            cdn.weight = intAt(&cdnValue, "weight");
            if (const auto* urls = arrayAt(&cdnValue, "url_list")) {
                for (const auto& urlValue : *urls) {
                    if (!urlValue.isObject()) continue;
                    SlbUrlEntry url;
                    url.tag = stringAt(&urlValue, "tag");
                    url.url = stringAt(&urlValue, "url");
                    cdn.urls.push_back(std::move(url));
                }
            }
            data.cdnList.push_back(std::move(cdn));
        }
    }
    return true;
}

std::vector<std::string> collectSlbCandidateUrls(const SlbInfoResponseData& data) {
    std::vector<std::string> result;
    for (const auto& cdn : data.cdnList) {
        for (const auto& item : cdn.urls) {
            if (!item.url.empty()) result.push_back(item.url);
        }
    }
    return result;
}

} // namespace xuitch::api
