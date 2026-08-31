#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "iptv/IptvModels.hpp"

namespace xuitch::iptv {

// Pure navigation/filter model used by the Borealis UI and covered by host tests.
class IptvNavigator {
public:
    void setPlaylist(const IptvPlaylist& playlist, bool hideUnavailable = false);
    void setHideUnavailable(bool value);
    void setSearch(std::string value);

    const std::vector<std::string>& categories() const { return categoryNames; }
    const std::string& selectedCategory() const { return currentCategory; }
    const std::string& search() const { return searchText; }

    bool selectCategory(const std::string& category);
    bool selectCategory(std::size_t index);

    std::vector<const IptvChannel*> visibleChannels() const;
    const IptvChannel* channelAt(std::size_t visibleIndex) const;
    std::size_t visibleCount() const;

private:
    void rebuildCategories();
    bool includeChannel(const IptvChannel& channel) const;
    static bool containsNoCase(const std::string& value, const std::string& needle);

    const IptvPlaylist* source{nullptr};
    bool hideUnavailable{false};
    std::vector<std::string> categoryNames;
    std::string currentCategory{"Todos"};
    std::string searchText;
};

} // namespace xuitch::iptv
