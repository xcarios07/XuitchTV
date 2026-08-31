#include "player/Player.hpp"

#include "core/BuildInfo.hpp"

#ifndef XUITCHTV_HAS_MPV
#define XUITCHTV_HAS_MPV 0
#endif

#if XUITCHTV_HAS_MPV
extern "C" {
#include <mpv/client.h>
}
#endif

namespace xuitch::player {

struct Player::Impl {
#if XUITCHTV_HAS_MPV
    mpv_handle* handle{nullptr};
#endif
};

Player::Player() : impl(std::make_unique<Impl>()) {}

Player::~Player() {
#if XUITCHTV_HAS_MPV
    if (impl && impl->handle) {
        mpv_terminate_destroy(impl->handle);
        impl->handle = nullptr;
    }
#endif
}

void Player::setState(PlayerState value) {
    if (currentState == value) return;
    currentState = value;
    if (stateCallback) stateCallback(currentState);
}

bool Player::backendAvailable() const {
#if XUITCHTV_HAS_MPV
    return true;
#else
    return false;
#endif
}

void* Player::nativeHandle() const {
#if XUITCHTV_HAS_MPV
    return impl ? static_cast<void*>(impl->handle) : nullptr;
#else
    return nullptr;
#endif
}

bool Player::initialize() {
    errorText.clear();
#if XUITCHTV_HAS_MPV
    if (impl->handle) return true;

    impl->handle = mpv_create();
    if (!impl->handle) {
        errorText = "mpv_create failed";
        setState(PlayerState::Error);
        return false;
    }

    // XuitchTV owns the UI/input layer. libmpv only handles media playback.
    mpv_set_option_string(impl->handle, "config", "no");
    mpv_set_option_string(impl->handle, "terminal", "no");
    mpv_set_option_string(impl->handle, "input-default-bindings", "no");
    mpv_set_option_string(impl->handle, "input-vo-keyboard", "no");
    mpv_set_option_string(impl->handle, "keep-open", "no");
    mpv_set_option_string(impl->handle, "vo", "libmpv");
    mpv_set_option_string(impl->handle, "osd-level", "0");
    mpv_set_option_string(impl->handle, "ytdl", "no");
    mpv_set_option_string(impl->handle, "audio-channels", "auto-safe");
    mpv_set_option_string(impl->handle, "video-timing-offset", "0");
    mpv_set_option_string(impl->handle, "user-agent", core::userAgent().c_str());

#if defined(__SWITCH__)
    // These settings mirror the proven Switch/libmpv path used by current
    // native media clients. hwdec=auto falls back to software decode if the
    // stream/codec cannot use the hardware backend.
    mpv_set_option_string(impl->handle, "vd-lavc-dr", "yes");
    mpv_set_option_string(impl->handle, "vd-lavc-threads", "3");
    mpv_set_option_string(impl->handle, "hwdec", "auto");
    mpv_set_option_string(impl->handle, "opengl-glfinish", "yes");
#endif

    const int rc = mpv_initialize(impl->handle);
    if (rc < 0) {
        errorText = mpv_error_string(rc);
        setState(PlayerState::Error);
        mpv_terminate_destroy(impl->handle);
        impl->handle = nullptr;
        return false;
    }
    return true;
#else
    errorText = "MPV backend was not available at build time";
    setState(PlayerState::Error);
    return false;
#endif
}

bool Player::open(const std::string& url) {
    errorText.clear();
    if (url.empty()) {
        errorText = "Empty playback URL";
        setState(PlayerState::Error);
        return false;
    }
    if (!initialize()) return false;

#if XUITCHTV_HAS_MPV
    openedUrl = url;
    setState(PlayerState::Opening);
    const char* command[] = {"loadfile", openedUrl.c_str(), "replace", nullptr};
    const int rc = mpv_command_async(impl->handle, 0, command);
    if (rc < 0) {
        errorText = mpv_error_string(rc);
        setState(PlayerState::Error);
        return false;
    }
    return true;
#else
    (void)url;
    return false;
#endif
}

bool Player::setPaused(bool paused) {
#if XUITCHTV_HAS_MPV
    if (!impl->handle) {
        errorText = "MPV is not initialized";
        return false;
    }
    int flag = paused ? 1 : 0;
    const int rc = mpv_set_property(impl->handle, "pause", MPV_FORMAT_FLAG, &flag);
    if (rc < 0) {
        errorText = mpv_error_string(rc);
        return false;
    }
    setState(paused ? PlayerState::Paused : PlayerState::Playing);
    return true;
#else
    (void)paused;
    errorText = "MPV backend was not available at build time";
    return false;
#endif
}

void Player::stop() {
#if XUITCHTV_HAS_MPV
    if (impl->handle) {
        const char* command[] = {"stop", nullptr};
        mpv_command_async(impl->handle, 0, command);
    }
#endif
    openedUrl.clear();
    setState(PlayerState::Stopped);
}

void Player::pollEvents() {
#if XUITCHTV_HAS_MPV
    if (!impl->handle) return;
    while (true) {
        mpv_event* event = mpv_wait_event(impl->handle, 0.0);
        if (!event || event->event_id == MPV_EVENT_NONE) break;
        switch (event->event_id) {
            case MPV_EVENT_START_FILE:
                setState(PlayerState::Opening);
                break;
            case MPV_EVENT_FILE_LOADED:
            case MPV_EVENT_PLAYBACK_RESTART:
                setState(PlayerState::Playing);
                break;
            case MPV_EVENT_END_FILE: {
                const auto* end = static_cast<mpv_event_end_file*>(event->data);
                if (end && end->reason == MPV_END_FILE_REASON_ERROR && end->error < 0) {
                    errorText = mpv_error_string(end->error);
                    setState(PlayerState::Error);
                } else {
                    setState(PlayerState::Stopped);
                }
                break;
            }
            case MPV_EVENT_SHUTDOWN:
                setState(PlayerState::Stopped);
                break;
            default:
                break;
        }
    }
#endif
}

} // namespace xuitch::player
