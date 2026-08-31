#pragma once

#include "core/BuildInfo.hpp"

#include <map>
#include <string>

namespace xuitch::api {

struct HttpResponse {
    long statusCode{0};
    std::string body;
    std::string error;
    bool ok() const { return error.empty() && statusCode >= 200 && statusCode < 300; }
};

struct ProbeResponse {
    long statusCode{0};
    std::size_t bytesReceived{0};
    bool reachable{false};
    std::string error;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;

    void setTimeoutSeconds(long value) { timeoutSeconds = value; }
    void setUserAgent(std::string value) { userAgent = std::move(value); }

    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& headers = {}) const;

    ProbeResponse probe(const std::string& url,
                        long timeoutSeconds = 6,
                        std::size_t maxBytes = 4096) const;

    HttpResponse postJson(const std::string& url,
                          const std::string& json,
                          const std::map<std::string, std::string>& headers = {}) const;

private:
    long timeoutSeconds{20};
    std::string userAgent{xuitch::core::userAgent()};
};

} // namespace xuitch::api
