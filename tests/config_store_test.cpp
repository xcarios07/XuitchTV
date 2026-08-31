#include "core/ConfigStore.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
int main() {
    const char* path = "/tmp/xuitchtv-config-test.json";
    xuitch::core::Session a; a.portalBaseUrl="https://example.test"; a.portalCode="p"; a.deviceId="d";
    std::string error;
    assert(xuitch::core::ConfigStore::save(path,a,&error));
    xuitch::core::Session b;
    assert(xuitch::core::ConfigStore::load(path,b,&error));
    assert(b.portalBaseUrl==a.portalBaseUrl && b.portalCode==a.portalCode && b.deviceId==a.deviceId);
    std::remove(path);
    std::cout << "config store test: OK\n";
}
