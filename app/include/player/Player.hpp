#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace xuitch::player {

enum class PlayerState { Idle, Opening, Playing, Paused, Stopped, Error };

class Player {
public:
    using StateCallback = std::function<void(PlayerState)>;

    Player();
    ~Player();
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    bool initialize();
    bool open(const std::string& url);
    bool setPaused(bool paused);
    void stop();
    void pollEvents();

    PlayerState state() const { return currentState; }
    const std::string& lastError() const { return errorText; }
    const std::string& currentUrl() const { return openedUrl; }
    bool backendAvailable() const;
    void* nativeHandle() const;

    void setStateCallback(StateCallback callback) { stateCallback = std::move(callback); }

private:
    void setState(PlayerState value);

    struct Impl;
    std::unique_ptr<Impl> impl;
    PlayerState currentState{PlayerState::Idle};
    StateCallback stateCallback;
    std::string errorText;
    std::string openedUrl;
};

} // namespace xuitch::player
