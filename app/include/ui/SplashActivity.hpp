#pragma once

#include <borealis.hpp>

namespace xuitch::ui {

class SplashActivity : public brls::Activity {
public:
    CONTENT_FROM_XML_RES("activity/splash.xml");
};

} // namespace xuitch::ui
