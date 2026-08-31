#include "ui/VideoView.hpp"

#ifndef XUITCHTV_HAS_MPV
#define XUITCHTV_HAS_MPV 0
#endif

#if XUITCHTV_HAS_MPV
extern "C" {
#include <mpv/client.h>
#include <mpv/render.h>
}
#endif

#if XUITCHTV_HAS_MPV && defined(BOREALIS_USE_DEKO3D)
extern "C" {
#include <mpv/render_dk3d.h>
}
#include <borealis/platforms/switch/switch_video.hpp>
#endif

namespace xuitch::ui {

struct VideoView::Renderer {
#if XUITCHTV_HAS_MPV && defined(BOREALIS_USE_DEKO3D)
    mpv_render_context* context{nullptr};
    brls::SwitchVideoContext* videoContext{nullptr};
    DkFence readyFence{};
    DkFence doneFence{};
    mpv_deko3d_fbo fbo{};

    bool attach(void* handlePtr) {
        detach();
        if (!handlePtr) return false;

        auto* handle = static_cast<mpv_handle*>(handlePtr);
        videoContext = dynamic_cast<brls::SwitchVideoContext*>(
            brls::Application::getPlatform()->getVideoContext());
        if (!videoContext) return false;

        mpv_deko3d_init_params initParams{};
        initParams.device = videoContext->getDeko3dDevice();

        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>(MPV_RENDER_API_TYPE_DEKO3D)},
            {MPV_RENDER_PARAM_DEKO3D_INIT_PARAMS, &initParams},
            {MPV_RENDER_PARAM_INVALID, nullptr},
        };

        if (mpv_render_context_create(&context, handle, params) < 0) {
            context = nullptr;
            videoContext = nullptr;
            return false;
        }

        fbo.tex = nullptr;
        fbo.ready_fence = &readyFence;
        fbo.done_fence = &doneFence;
        fbo.w = 1280;
        fbo.h = 720;
        fbo.format = DkImageFormat_RGBA8_Unorm;
        return true;
    }

    void detach() {
        if (context) {
            mpv_render_context_free(context);
            context = nullptr;
        }
        videoContext = nullptr;
        fbo.tex = nullptr;
    }

    bool available() const { return context != nullptr && videoContext != nullptr; }

    void render() {
        if (!available()) return;

        fbo.tex = videoContext->getFramebuffer();
        fbo.w = static_cast<int>(brls::Application::windowWidth);
        fbo.h = static_cast<int>(brls::Application::windowHeight);

        videoContext->queueSignalFence(&readyFence);
        videoContext->queueFlush();

        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_DEKO3D_FBO, &fbo},
            {MPV_RENDER_PARAM_INVALID, nullptr},
        };
        mpv_render_context_render(context, params);
        videoContext->queueWaitFence(&doneFence);
        mpv_render_context_report_swap(context);
    }
#else
    bool attach(void*) { return false; }
    void detach() {}
    bool available() const { return false; }
    void render() {}
#endif

    ~Renderer() { detach(); }
};

VideoView::VideoView()
    : brls::Box(brls::Axis::COLUMN), renderer(std::make_unique<Renderer>()) {
    setFocusable(true);
    setHideHighlightBackground(true);
}

VideoView::~VideoView() {
    detachPlayer();
}

bool VideoView::attachPlayer(player::Player* value) {
    detachPlayer();
    attachedPlayer = value;
    if (!attachedPlayer || !attachedPlayer->backendAvailable()) return false;
    return renderer->attach(attachedPlayer->nativeHandle());
}

void VideoView::detachPlayer() {
    if (renderer) renderer->detach();
    attachedPlayer = nullptr;
}

bool VideoView::rendererAvailable() const {
    return renderer && renderer->available();
}

void VideoView::draw(NVGcontext* vg, float x, float y, float width, float height,
                     brls::Style, brls::FrameContext*) {
    // Always clear the video surface. This avoids stale frames during channel
    // switches and gives a deterministic fallback on non-Switch builds.
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgFillColor(vg, nvgRGB(0, 0, 0));
    nvgFill(vg);

    if (attachedPlayer) attachedPlayer->pollEvents();
    if (renderer && renderer->available()) renderer->render();
}

} // namespace xuitch::ui
