#include "api/PortalModels.hpp"
#include "util/JsonLite.hpp"

#include <cassert>
#include <iostream>

int main() {
    using namespace xuitch;
    api::LoginRequest login;
    login.userName = "demo";
    login.password = "p\"ass";
    const auto body = login.toJson();
    util::JsonValue parsed;
    std::string error;
    assert(util::JsonValue::parse(body, parsed, &error));
    assert(parsed.get("userName") && parsed.get("userName")->asString() == "demo");
    assert(parsed.get("password") && parsed.get("password")->asString() == "p\"ass");

    api::PortalResponseInfo info;
    api::LoginResponseData data;
    const std::string response = R"({"returnCode":"0","data":{"userId":"42","userToken":"ut","token":"t","accountType":"normal","areaCode":"595","heartBeatTime":"60"}})";
    assert(api::parseLoginResponse(response, info, data, &error));
    assert(data.userId == "42");
    assert(data.userToken == "ut");
    assert(data.token == "t");

    api::StartPlayVodRequest vod;
    vod.contentId = "movie1";
    vod.episodeNumberList = {1,2};
    util::JsonValue vodJson;
    assert(util::JsonValue::parse(vod.toJson(), vodJson, &error));
    assert(vodJson.get("episodeNumberList") && vodJson.get("episodeNumberList")->asArray().size() == 2);

    api::GetSlbInfoRequest slbRequest;
    slbRequest.appVer = "1.0";
    slbRequest.portalCode = "portal";
    slbRequest.liveCodeList = {"live-1", "live-2"};
    util::JsonValue slbJson;
    assert(util::JsonValue::parse(slbRequest.toJson(), slbJson, &error));
    assert(slbJson.get("liveCodeList") && slbJson.get("liveCodeList")->asArray().size() == 2);

    api::StartPlayLiveResponseData liveData;
    const std::string liveResponse = R"({"returnCode":"0","data":{"name":"Channel","liveAddressList":[{"AVFormat":"HLS","cdnType":"1","license":"","playCode":"pc1","quality":"HD","tag":"main"}]}})";
    assert(api::parseStartPlayLiveResponse(liveResponse, info, liveData, &error));
    assert(liveData.liveAddressList.size() == 1);
    assert(liveData.liveAddressList[0].playCode == "pc1");

    api::StartPlayVodResponseData vodData;
    const std::string vodResponse = R"({"returnCode":"0","data":{"name":"Movie","seriesFlag":"0","vodFreeCount":1,"vodFreeFlag":"1","episodeList":[{"episodeNumber":1,"programContentId":"program-1","totalMovieList":[{"contentId":"movie-1","quality":"4K","videoFormat":"H265"}]}]}})";
    assert(api::parseStartPlayVodResponse(vodResponse, info, vodData, &error));
    assert(vodData.episodes.size() == 1);
    assert(vodData.episodes[0].movies.size() == 1);
    assert(vodData.episodes[0].movies[0].contentId == "movie-1");

    api::SlbInfoResponseData slbData;
    const std::string slbResponse = R"({"returnCode":"0","data":{"error_code":0,"play_params":"opaque-params","cdn_list":[{"cdn_id":"cdn-1","cdn_type":"main","token":"opaque-token","url_list":[{"tag":"primary","url":"https://media.example/stream.m3u8"}]}]}})";
    assert(api::parseSlbInfoResponse(slbResponse, info, slbData, &error));
    assert(slbData.cdnList.size() == 1);
    assert(slbData.playParams == "opaque-params");
    const auto candidates = api::collectSlbCandidateUrls(slbData);
    assert(candidates.size() == 1);
    assert(candidates.front() == "https://media.example/stream.m3u8");

    std::cout << "phase3 core tests: OK\n";
    return 0;
}
