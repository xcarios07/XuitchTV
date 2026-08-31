#include "core/AppConfig.hpp"
#include <utility>
namespace xuitch::core {
AppConfig& AppConfig::instance() { static AppConfig cfg; return cfg; }
Session& AppConfig::session() { return currentSession; }
const Session& AppConfig::session() const { return currentSession; }
void AppConfig::setPortalBaseUrl(std::string value) { currentSession.portalBaseUrl = std::move(value); }
}
