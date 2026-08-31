#include "core/ConfigStore.hpp"
#include "util/JsonLite.hpp"

#include <fstream>
#include <sstream>

namespace xuitch::core {
namespace {
std::string getString(const util::JsonValue& root, const char* key) {
    const auto* v = root.get(key);
    return v && v->isString() ? v->asString() : std::string{};
}
bool getBool(const util::JsonValue& root, const char* key, bool fallback) {
    const auto* v = root.get(key);
    return v && v->isBool() ? v->asBool() : fallback;
}
}

std::string ConfigStore::defaultPath() {
#ifdef __SWITCH__
    return "sdmc:/switch/XuitchTV/config.json";
#else
    return "config.json";
#endif
}

bool ConfigStore::load(const std::string& path, Session& session, std::string* error) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        if (error) *error = "Config file not found: " + path;
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    util::JsonValue root;
    if (!util::JsonValue::parse(buffer.str(), root, error) || !root.isObject()) return false;
    session.portalBaseUrl = getString(root, "portalBaseUrl");
    session.portalCode = getString(root, "portalCode");
    session.deviceId = getString(root, "deviceId");
    if (const auto* iptv = root.get("iptv"); iptv && iptv->isObject()) {
        const std::string playlist = getString(*iptv, "playlistUrl");
        if (!playlist.empty()) session.iptvPlaylistUrl = playlist;
        session.iptvEpgUrl = getString(*iptv, "epgUrl");
        session.iptvEnabled = getBool(*iptv, "enabled", session.iptvEnabled);
        session.iptvHideUnavailable = getBool(*iptv, "hideUnavailable", session.iptvHideUnavailable);
    }
    // A portal is optional now: XuitchTV can run in IPTV-only mode.
    return !session.portalBaseUrl.empty() || (session.iptvEnabled && !session.iptvPlaylistUrl.empty());
}

bool ConfigStore::save(const std::string& path, const Session& session, std::string* error) {
    util::JsonValue::Object root;
    root["portalBaseUrl"] = util::JsonValue(session.portalBaseUrl);
    root["portalCode"] = util::JsonValue(session.portalCode);
    root["deviceId"] = util::JsonValue(session.deviceId);
    util::JsonValue::Object iptv;
    iptv["enabled"] = util::JsonValue(session.iptvEnabled);
    iptv["playlistUrl"] = util::JsonValue(session.iptvPlaylistUrl);
    iptv["epgUrl"] = util::JsonValue(session.iptvEpgUrl);
    iptv["hideUnavailable"] = util::JsonValue(session.iptvHideUnavailable);
    root["iptv"] = util::JsonValue(std::move(iptv));
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        if (error) *error = "Unable to write config file: " + path;
        return false;
    }
    file << util::JsonValue(std::move(root)).stringify() << '\n';
    return static_cast<bool>(file);
}

} // namespace xuitch::core
