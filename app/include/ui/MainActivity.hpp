#pragma once
#include <borealis.hpp>

namespace xuitch::ui {
class MainActivity : public brls::Activity {
public:
    CONTENT_FROM_XML_RES("activity/main.xml");
    void onContentAvailable() override;
    void onPause() override;
};
}
