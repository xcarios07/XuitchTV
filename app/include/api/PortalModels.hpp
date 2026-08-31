#pragma once

#include <string>
#include <vector>

namespace xuitch::api {

struct LoginRequest {
    std::string accountType;
    std::string areaCode;
    std::string channel;
    std::string macAddr;
    std::string matadata;
    std::string password;
    std::string signdata;
    std::string type;
    std::string userName;
    std::string userToken;
    std::string verificationCode;
    std::string verificationToken;
    std::string toJson() const;
};

struct GetHomeRequest {
    std::string homePageCode;
    std::string portalCode;
    std::string userId;
    std::string userToken;
    std::string version;
    std::string toJson() const;
};

struct GetLiveDataRequest {
    int columnId{0};
    std::string dataVersion;
    std::string expireTimeStr;
    int pageNum{1};
    int pageSize{100};
    std::string portalCode;
    std::string userId;
    std::string userToken;
    std::string toJson() const;
};

struct StartPlayLiveRequest {
    std::string channelCode;
    int columnId{0};
    std::string portalCode;
    std::string type;
    std::string userId;
    std::string userToken;
    std::string toJson() const;
};

struct StartPlayVodRequest {
    std::string authType;
    int columnId{0};
    std::string contentId;
    std::vector<int> episodeNumberList;
    std::string portalCode;
    std::string seriesContentId;
    int startTime{0};
    std::string type;
    std::string userId;
    std::string userToken;
    std::string toJson() const;
};

// Fields recovered from com.request.bean.GetSlbInfoBean.
struct GetSlbInfoRequest {
    std::string appParams;
    std::string appVer;
    int encMediaSupported{0};
    std::string hasPay;
    std::string lang;
    std::vector<std::string> liveCodeList;
    std::string pipFlag;
    std::string portalCode;
    std::string reserve1;
    std::string type;
    std::string userId;
    std::string userIdentity;
    std::string userToken;
    std::string toJson() const;
};

struct LoginResponseData {
    std::string userId;
    std::string userToken;
    std::string token;
    std::string accountType;
    std::string areaCode;
    std::string heartBeatTime;
};

struct PortalResponseInfo {
    std::string returnCode;
    std::string errorMessage;
};

struct LiveAddress {
    std::string avFormat;
    std::string cdnType;
    std::string license;
    std::string playCode;
    std::string quality;
    std::string tag;
};

struct StartPlayLiveResponseData {
    std::string name;
    std::vector<LiveAddress> liveAddressList;
};

struct VodMovie {
    std::string audioInfo;
    std::string audioType;
    std::string bitRateType;
    std::string contentId;
    std::string encodeFormat;
    std::string quality;
    std::string screenFormat;
    std::string terminalType;
    std::string type;
    std::string videoFormat;
    std::string videoType;
};

struct VodEpisode {
    int episodeNumber{0};
    std::string programContentId;
    std::vector<VodMovie> movies;
};

struct StartPlayVodResponseData {
    std::string name;
    std::string seriesFlag;
    int vodFreeCount{0};
    std::string vodFreeFlag;
    std::vector<VodEpisode> episodes;
};

struct SlbUrlEntry {
    std::string tag;
    std::string url;
};

struct SlbCdnEntry {
    std::string cdnId;
    std::string cdnIdMark;
    std::string cdnType;
    std::string groupIdMark;
    std::string gslbParams;
    std::string mainAddr;
    std::string mainAddrMark;
    std::string ruleIdMark;
    int serialNumber{0};
    std::string sparedAddr;
    std::string sparedAddrMark;
    std::string tag;
    std::string token;
    int weight{0};
    std::vector<SlbUrlEntry> urls;
};

struct SlbInfoResponseData {
    int errorCode{0};
    std::string invalidTime;
    int mergeRstStatus{0};
    std::string nowTime;
    std::string playParams;
    std::string reserveA;
    std::string reserveB;
    int rstStatus{0};
    int switchLiveSourceTime{0};
    std::string switchLiveSourceTimeV2;
    int switchVodSourceTime{0};
    std::string switchVodSourceTimeV2;
    std::vector<SlbCdnEntry> cdnList;
};

bool parseLoginResponse(const std::string& json, PortalResponseInfo& info, LoginResponseData& data, std::string* error = nullptr);
bool parseStartPlayLiveResponse(const std::string& json, PortalResponseInfo& info, StartPlayLiveResponseData& data, std::string* error = nullptr);
bool parseStartPlayVodResponse(const std::string& json, PortalResponseInfo& info, StartPlayVodResponseData& data, std::string* error = nullptr);
bool parseSlbInfoResponse(const std::string& json, PortalResponseInfo& info, SlbInfoResponseData& data, std::string* error = nullptr);

// Returns the URL fields exposed by SLB/CDN data. They are candidates only: the
// Android APK can still combine them with play_params/tokens before playback.
std::vector<std::string> collectSlbCandidateUrls(const SlbInfoResponseData& data);

} // namespace xuitch::api
