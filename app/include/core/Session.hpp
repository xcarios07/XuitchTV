#pragma once
#include <string>

namespace xuitch::core {
struct Session {
    std::string portalBaseUrl;
    std::string portalCode;
    std::string accessToken;   // LoginResultData.token
    std::string userToken;     // LoginResultData.userToken
    std::string userId;
    std::string deviceId;
    std::string accountType;
    std::string areaCode;
    std::string heartbeatInterval;

    // Independent IPTV source. XuitchTV ships with the public Paraguay
    // playlist URL as a user-replaceable default; no credentials are embedded.
    std::string iptvPlaylistUrl{"https://iptv-org.github.io/iptv/countries/py.m3u"};
    std::string iptvEpgUrl;
    bool iptvEnabled{true};
    bool iptvHideUnavailable{false};
    bool authenticated{false};
};
} // namespace xuitch::core
