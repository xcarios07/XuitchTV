#include "api/ApiClient.hpp"
#include <cassert>
#include <iostream>
int main() {
    xuitch::core::Session s;
    s.portalBaseUrl = "https://example.test/";
    xuitch::api::ApiClient api(s);
    assert(api.makeUrl("/api/portalCore/v8/login") == "https://example.test/api/portalCore/v8/login");
    std::cout << "phase3 http compile test: OK\n";
}
