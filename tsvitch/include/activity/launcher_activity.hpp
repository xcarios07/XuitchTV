#pragma once

#include <borealis/core/activity.hpp>

class LauncherActivity : public brls::Activity {
public:
    CONTENT_FROM_XML_RES("activity/launcher.xml");

    void onContentAvailable() override;
};
