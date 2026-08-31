#include "api/HttpClient.hpp"

#include <algorithm>
#include <curl/curl.h>

namespace xuitch::api {
namespace {
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    const size_t total = size * nmemb;
    auto* output = static_cast<std::string*>(userdata);
    output->append(ptr, total);
    return total;
}

struct ProbeBuffer {
    std::size_t received{0};
    std::size_t maxBytes{4096};
};

size_t probeWriteCallback(char*, size_t size, size_t nmemb, void* userdata) {
    const size_t total = size * nmemb;
    auto* buffer = static_cast<ProbeBuffer*>(userdata);
    const std::size_t remaining = buffer->maxBytes > buffer->received
        ? buffer->maxBytes - buffer->received : 0;
    const std::size_t accepted = std::min(total, remaining);
    buffer->received += accepted;
    // Deliberately abort after enough bytes. CURLE_WRITE_ERROR is treated as
    // success by probe() when we already received payload data.
    return accepted;
}

curl_slist* makeHeaders(const std::map<std::string, std::string>& headers,
                        bool json = false) {
    curl_slist* list = nullptr;
    if (json) list = curl_slist_append(list, "Content-Type: application/json;charset=utf-8");
    list = curl_slist_append(list, "Cache-Control: no-store");
    for (const auto& [key, value] : headers) {
        const std::string line = key + ": " + value;
        list = curl_slist_append(list, line.c_str());
    }
    return list;
}
} // namespace

HttpClient::HttpClient() { curl_global_init(CURL_GLOBAL_DEFAULT); }
HttpClient::~HttpClient() { curl_global_cleanup(); }

HttpResponse HttpClient::get(const std::string& url,
                             const std::map<std::string, std::string>& headers) const {
    HttpResponse response;
    CURL* curl = curl_easy_init();
    if (!curl) { response.error = "curl_easy_init failed"; return response; }

    char errorBuffer[CURL_ERROR_SIZE] = {0};
    struct curl_slist* headerList = makeHeaders(headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

    const CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) response.error = errorBuffer[0] ? errorBuffer : curl_easy_strerror(code);
    else curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);

    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);
    return response;
}

ProbeResponse HttpClient::probe(const std::string& url,
                                long probeTimeoutSeconds,
                                std::size_t maxBytes) const {
    ProbeResponse response;
    CURL* curl = curl_easy_init();
    if (!curl) { response.error = "curl_easy_init failed"; return response; }

    char errorBuffer[CURL_ERROR_SIZE] = {0};
    ProbeBuffer buffer{0, maxBytes == 0 ? 4096 : maxBytes};
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, probeWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, probeTimeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, probeTimeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

    const CURLcode code = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);
    response.bytesReceived = buffer.received;
    const bool statusOk = response.statusCode >= 200 && response.statusCode < 400;
    response.reachable = statusOk && (code == CURLE_OK || (code == CURLE_WRITE_ERROR && buffer.received > 0));
    if (!response.reachable && code != CURLE_OK) {
        response.error = errorBuffer[0] ? errorBuffer : curl_easy_strerror(code);
    }

    curl_easy_cleanup(curl);
    return response;
}

HttpResponse HttpClient::postJson(const std::string& url,
                                  const std::string& json,
                                  const std::map<std::string, std::string>& headers) const {
    HttpResponse response;
    CURL* curl = curl_easy_init();
    if (!curl) { response.error = "curl_easy_init failed"; return response; }

    char errorBuffer[CURL_ERROR_SIZE] = {0};
    struct curl_slist* headerList = makeHeaders(headers, true);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.data());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(json.size()));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

    const CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        response.error = errorBuffer[0] ? errorBuffer : curl_easy_strerror(code);
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.statusCode);
    }

    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);
    return response;
}

} // namespace xuitch::api
