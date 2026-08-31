#pragma once
#include <string>
#include "core/Session.hpp"
namespace xuitch::core {
class AppConfig {
public:
    static AppConfig& instance();
    Session& session();
    const Session& session() const;
    void setPortalBaseUrl(std::string value);
private:
    AppConfig() = default;
    Session currentSession;
};
}
