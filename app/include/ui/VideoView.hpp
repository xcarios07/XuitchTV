#pragma once

#include <memory>
#include <borealis.hpp>

#include "player/Player.hpp"

namespace xuitch::ui {

// Video surface used by XuitchTV. On Nintendo Switch builds using the
// Borealis deko3d backend, it creates a libmpv deko3d render context and
// renders directly into the current framebuffer. Other builds keep the
// surface as a safe black preview while the playback core remains usable.
class VideoView : public brls::Box {
public:
    VideoView();
    ~VideoView() override;

    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style style, brls::FrameContext* ctx) override;

    bool attachPlayer(player::Player* player);
    void detachPlayer();
    bool rendererAvailable() const;

private:
    struct Renderer;
    std::unique_ptr<Renderer> renderer;
    player::Player* attachedPlayer{nullptr};
};

} // namespace xuitch::ui
