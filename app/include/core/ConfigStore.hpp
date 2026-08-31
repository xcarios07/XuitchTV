#pragma once

#include <string>
#include "core/Session.hpp"

namespace xuitch::core {
class ConfigStore {
public:
    static std::string defaultPath();
    static bool load(const std::string& path, Session& session, std::string* error = nullptr);
    static bool save(const std::string& path, const Session& session, std::string* error = nullptr);
};
} // namespace xuitch::core
