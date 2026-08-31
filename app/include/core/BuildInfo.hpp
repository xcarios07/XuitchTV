#pragma once

#include <string>

#ifndef APP_VERSION
#define APP_VERSION "0.5.1"
#endif

namespace xuitch::core {
inline constexpr const char* kAppName = "XuitchTV";
inline constexpr const char* kAppVersion = APP_VERSION;
inline std::string userAgent() {
    return std::string(kAppName) + "/" + kAppVersion;
}
} // namespace xuitch::core
