#pragma once
#include <string_view>

namespace xuitch::api::endpoint {
inline constexpr std::string_view Login          = "/api/portalCore/v8/login";
inline constexpr std::string_view Activate       = "/api/portalCore/v8/active";
inline constexpr std::string_view Logout         = "/api/portalCore/v5/loginOut";
inline constexpr std::string_view Heartbeat      = "/api/portalCore/v5/heartbeat";
inline constexpr std::string_view AuthInfo       = "/api/portalCore/v9/getAuthInfo";
inline constexpr std::string_view Home           = "/api/portalCore/getHome";
inline constexpr std::string_view LiveData       = "/api/portalCore/v6/getLiveData";
inline constexpr std::string_view StartLive      = "/api/portalCore/v4/startPlayLive";
inline constexpr std::string_view StartBtv       = "/api/portalCore/v6/startPlayBTV";
inline constexpr std::string_view StartVod       = "/api/portalCore/v10/startPlayVOD";
inline constexpr std::string_view SlbInfoV14     = "/api/portalCore/v14/getSlbInfo";
inline constexpr std::string_view SlbInfoV15     = "/api/portalCore/v15/getSlbInfo";
inline constexpr std::string_view ItemData       = "/api/portalCore/v4/getItemData";
inline constexpr std::string_view Program        = "/api/portalCore/v3/getProgram";
inline constexpr std::string_view Recommends     = "/api/portalCore/v3/getRecommends";
inline constexpr std::string_view Shelves        = "/api/portalCore/v3/getShelveData";
inline constexpr std::string_view ColumnContents = "/api/portalCore/v3/getColumnContents";
inline constexpr std::string_view NextColumns    = "/api/portalCore/getNextColumns";
inline constexpr std::string_view Search         = "/api/portalCore/searchResourceOrPerson";
inline constexpr std::string_view Favorites      = "/api/portalCore/getFavorite";
inline constexpr std::string_view AddFavorite    = "/api/portalCore/v2/addFavorite";
inline constexpr std::string_view DeleteFavorite = "/api/portalCore/delFavorite";
inline constexpr std::string_view QrToken        = "/api/portalCore/qr/token";
inline constexpr std::string_view QrResult       = "/api/portalCore/v5/qr/getResult";
inline constexpr std::string_view Devices        = "/api/portalCore/device-management/getDevice";
}
