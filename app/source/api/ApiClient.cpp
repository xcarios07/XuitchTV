#include "api/ApiClient.hpp"
#include "api/EndpointCatalog.hpp"

#include <map>

namespace xuitch::api {

std::string ApiClient::makeUrl(const std::string& path) const {
    if (session.portalBaseUrl.empty()) return {};
    if (session.portalBaseUrl.back() == '/' && !path.empty() && path.front() == '/')
        return session.portalBaseUrl.substr(0, session.portalBaseUrl.size() - 1) + path;
    if (session.portalBaseUrl.back() != '/' && !path.empty() && path.front() != '/')
        return session.portalBaseUrl + "/" + path;
    return session.portalBaseUrl + path;
}

bool ApiClient::isConfigured() const { return !session.portalBaseUrl.empty(); }

HttpResponse ApiClient::post(const std::string& path, const std::string& body, bool processResultFalse) {
    if (!isConfigured()) return HttpResponse{0, {}, "Portal base URL is not configured"};
    std::map<std::string, std::string> headers;
    if (processResultFalse) headers["ProcessResult"] = "false";
    return http.postJson(makeUrl(path), body, headers);
}

HttpResponse ApiClient::login(const LoginRequest& request) {
    return post(std::string(endpoint::Login), request.toJson(), true);
}
HttpResponse ApiClient::getHome(const GetHomeRequest& request) {
    return post(std::string(endpoint::Home), request.toJson(), true);
}
HttpResponse ApiClient::getLiveData(const GetLiveDataRequest& request) {
    return post(std::string(endpoint::LiveData), request.toJson(), true);
}
HttpResponse ApiClient::startPlayLive(const StartPlayLiveRequest& request) {
    return post(std::string(endpoint::StartLive), request.toJson(), false);
}
HttpResponse ApiClient::startPlayVod(const StartPlayVodRequest& request) {
    return post(std::string(endpoint::StartVod), request.toJson(), false);
}
HttpResponse ApiClient::getSlbInfoV14(const GetSlbInfoRequest& request) {
    return post(std::string(endpoint::SlbInfoV14), request.toJson(), false);
}
HttpResponse ApiClient::getSlbInfoV15(const GetSlbInfoRequest& request) {
    return post(std::string(endpoint::SlbInfoV15), request.toJson(), false);
}

bool ApiClient::applyLoginResponse(const HttpResponse& response, std::string* error) {
    if (!response.ok()) {
        if (error) *error = response.error.empty() ? ("HTTP " + std::to_string(response.statusCode)) : response.error;
        return false;
    }
    PortalResponseInfo info;
    LoginResponseData data;
    std::string parseError;
    if (!parseLoginResponse(response.body, info, data, &parseError)) {
        if (error) *error = parseError;
        return false;
    }
    session.userId = data.userId;
    session.userToken = data.userToken;
    session.accessToken = data.token;
    session.accountType = data.accountType;
    session.areaCode = data.areaCode;
    session.heartbeatInterval = data.heartBeatTime;
    session.authenticated = !session.userId.empty() && !session.userToken.empty();
    if (!session.authenticated && error) {
        *error = !info.errorMessage.empty() ? info.errorMessage : "Login response did not contain userId/userToken";
    }
    return session.authenticated;
}

} // namespace xuitch::api
