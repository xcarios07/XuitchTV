#pragma once

#include <string>
#include "api/HttpClient.hpp"
#include "api/PortalModels.hpp"
#include "core/Session.hpp"

namespace xuitch::api {

class ApiClient {
public:
    explicit ApiClient(core::Session& session) : session(session) {}

    std::string makeUrl(const std::string& path) const;
    bool isConfigured() const;

    HttpResponse login(const LoginRequest& request);
    HttpResponse getHome(const GetHomeRequest& request);
    HttpResponse getLiveData(const GetLiveDataRequest& request);
    HttpResponse startPlayLive(const StartPlayLiveRequest& request);
    HttpResponse startPlayVod(const StartPlayVodRequest& request);
    HttpResponse getSlbInfoV14(const GetSlbInfoRequest& request);
    HttpResponse getSlbInfoV15(const GetSlbInfoRequest& request);

    bool applyLoginResponse(const HttpResponse& response, std::string* error = nullptr);

private:
    HttpResponse post(const std::string& path, const std::string& body, bool processResultFalse = false);

    core::Session& session;
    HttpClient http;
};

} // namespace xuitch::api
