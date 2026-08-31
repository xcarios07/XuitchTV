#include "iptv/IptvNavigator.hpp"

#include <algorithm>
#include <cctype>
#include <set>

namespace xuitch::iptv {

namespace {
std::string normalizedCategory(const IptvChannel& channel) {
    return channel.groupTitle.empty() ? std::string("Paraguay") : channel.groupTitle;
}
}

void IptvNavigator::setPlaylist(const IptvPlaylist& playlist, bool hide) {
    source = &playlist;
    hideUnavailable = hide;
    rebuildCategories();
}

void IptvNavigator::setHideUnavailable(bool value) {
    hideUnavailable = value;
    rebuildCategories();
}

void IptvNavigator::setSearch(std::string value) {
    searchText = std::move(value);
}

bool IptvNavigator::selectCategory(const std::string& category) {
    const auto it = std::find(categoryNames.begin(), categoryNames.end(), category);
    if (it == categoryNames.end()) return false;
    currentCategory = *it;
    return true;
}

bool IptvNavigator::selectCategory(std::size_t index) {
    if (index >= categoryNames.size()) return false;
    currentCategory = categoryNames[index];
    return true;
}

std::vector<const IptvChannel*> IptvNavigator::visibleChannels() const {
    std::vector<const IptvChannel*> result;
    if (!source) return result;
    result.reserve(source->channels.size());
    for (const auto& channel : source->channels) {
        if (includeChannel(channel)) result.push_back(&channel);
    }
    return result;
}

const IptvChannel* IptvNavigator::channelAt(std::size_t visibleIndex) const {
    if (!source) return nullptr;
    std::size_t index = 0;
    for (const auto& channel : source->channels) {
        if (!includeChannel(channel)) continue;
        if (index == visibleIndex) return &channel;
        ++index;
    }
    return nullptr;
}

std::size_t IptvNavigator::visibleCount() const {
    if (!source) return 0;
    std::size_t count = 0;
    for (const auto& channel : source->channels) {
        if (includeChannel(channel)) ++count;
    }
    return count;
}

void IptvNavigator::rebuildCategories() {
    categoryNames.clear();
    categoryNames.emplace_back("Todos");
    if (!source) {
        currentCategory = "Todos";
        return;
    }

    std::set<std::string> unique;
    for (const auto& channel : source->channels) {
        if (hideUnavailable && channel.health == StreamHealth::Unreachable) continue;
        unique.insert(normalizedCategory(channel));
    }
    categoryNames.insert(categoryNames.end(), unique.begin(), unique.end());

    if (std::find(categoryNames.begin(), categoryNames.end(), currentCategory) == categoryNames.end()) {
        currentCategory = "Todos";
    }
}

bool IptvNavigator::includeChannel(const IptvChannel& channel) const {
    if (hideUnavailable && channel.health == StreamHealth::Unreachable) return false;
    if (currentCategory != "Todos" && normalizedCategory(channel) != currentCategory) return false;
    if (searchText.empty()) return true;
    return containsNoCase(channel.name, searchText)
        || containsNoCase(channel.tvgName, searchText)
        || containsNoCase(channel.tvgId, searchText)
        || containsNoCase(channel.groupTitle, searchText);
}

bool IptvNavigator::containsNoCase(const std::string& value, const std::string& needle) {
    if (needle.empty()) return true;
    auto lower = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };
    std::string lhs(value.size(), '\0');
    std::string rhs(needle.size(), '\0');
    std::transform(value.begin(), value.end(), lhs.begin(), lower);
    std::transform(needle.begin(), needle.end(), rhs.begin(), lower);
    return lhs.find(rhs) != std::string::npos;
}

} // namespace xuitch::iptv
