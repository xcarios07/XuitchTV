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

#if XUITCHTV_HAS_MPV && defined(BOREALIS_USE_OPENGL)
extern "C" {
#include <mpv/render_gl.h>
}
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define NANOVG_GL3 1
#include <borealis/extern/nanovg/nanovg_gl.h>
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
#elif XUITCHTV_HAS_MPV && defined(BOREALIS_USE_OPENGL)
    mpv_render_context* context{nullptr};
    GLuint framebuffer{0};
    GLuint texture{0};
    int nvgImage{0};
    NVGcontext* nvgContext{nullptr};
    int framebufferWidth{0};
    int framebufferHeight{0};

    static void* getProcAddress(void*, const char* name) {
        return reinterpret_cast<void*>(glfwGetProcAddress(name));
    }

    bool attach(void* handlePtr) {
        detach();
        if (!handlePtr) return false;

        auto* handle = static_cast<mpv_handle*>(handlePtr);
        mpv_opengl_init_params glInit{getProcAddress, nullptr};
        int advancedControl = 1;
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &glInit},
            {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advancedControl},
            {MPV_RENDER_PARAM_INVALID, nullptr},
        };

        if (mpv_render_context_create(&context, handle, params) < 0) {
            context = nullptr;
            return false;
        }
        return true;
    }

    void releaseFramebuffer() {
        if (nvgImage && nvgContext) nvgDeleteImage(nvgContext, nvgImage);
        if (framebuffer) glDeleteFramebuffers(1, &framebuffer);
        if (texture) glDeleteTextures(1, &texture);
        framebuffer = 0;
        texture = 0;
        nvgImage = 0;
        nvgContext = nullptr;
        framebufferWidth = 0;
        framebufferHeight = 0;
    }

    void detach() {
        if (context) {
            mpv_render_context_free(context);
            context = nullptr;
        }
        releaseFramebuffer();
    }

    bool available() const { return context != nullptr; }

    void render(NVGcontext* vg, float x, float y, float width, float height) {
        if (!available() || !vg || width <= 0 || height <= 0) return;

        const int targetWidth = std::max(1,
            static_cast<int>(std::round(width * brls::Application::windowScale)));
        const int targetHeight = std::max(1,
            static_cast<int>(std::round(height * brls::Application::windowScale)));

        if (!framebuffer || targetWidth != framebufferWidth
            || targetHeight != framebufferHeight) {
            releaseFramebuffer();

            GLint previousFramebuffer = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, targetWidth, targetHeight,
                0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, texture, 0);
            const bool complete =
                glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
            glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
            if (!complete) {
                releaseFramebuffer();
                return;
            }

            nvgContext = vg;
            nvgImage = nvglCreateImageFromHandleGL3(vg, texture,
                targetWidth, targetHeight,
                NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED | NVG_IMAGE_NODELETE);
            if (!nvgImage) {
                releaseFramebuffer();
                return;
            }
            framebufferWidth = targetWidth;
            framebufferHeight = targetHeight;
        }

        GLint previousFramebuffer = 0;
        GLint previousViewport[4] = {0, 0, 0, 0};
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
        glGetIntegerv(GL_VIEWPORT, previousViewport);

        mpv_opengl_fbo target{
            static_cast<int>(framebuffer), targetWidth, targetHeight, 0};
        int flipY = 1;
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_OPENGL_FBO, &target},
            {MPV_RENDER_PARAM_FLIP_Y, &flipY},
            {MPV_RENDER_PARAM_INVALID, nullptr},
        };
        mpv_render_context_render(context, params);
        mpv_render_context_report_swap(context);

        glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
        glViewport(previousViewport[0], previousViewport[1],
            previousViewport[2], previousViewport[3]);

        nvgBeginPath(vg);
        nvgRect(vg, x, y, width, height);
        nvgFillPaint(vg, nvgImagePattern(vg, x, y, width, height, 0,
            nvgImage, 1.0f));
        nvgFill(vg);
    }
#else
    bool attach(void*) { return false; }
    void detach() {}
    bool available() const { return false; }
    void render(NVGcontext*, float, float, float, float) {}
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
    if (renderer && renderer->available()) {
#if XUITCHTV_HAS_MPV && defined(BOREALIS_USE_DEKO3D)
        renderer->render();
#else
        renderer->render(vg, x, y, width, height);
#endif
    }
}

} // namespace xuitch::ui
