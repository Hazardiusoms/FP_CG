#pragma once
#include "SceneState.hpp"
#include <array>
#include <cstddef>

namespace sw {
enum class Key { Left, Right, A, D, W, S, C, R, B, T, Escape, Count };
class Input {
public:
    void set(Key key, bool down) {
        const auto i=static_cast<std::size_t>(key);
        if (down && !held_[i]) pressed_[i]=true;
        held_[i]=down;
    }
    void clear() { held_.fill(false); pressed_.fill(false); }
    bool down(Key key) const { return held_[static_cast<std::size_t>(key)]; }
    InputState consume() {
        InputState s;
        s.steering=static_cast<float>(down(Key::D)||down(Key::Right))-
                   static_cast<float>(down(Key::A)||down(Key::Left));
        s.speedAdjustment=static_cast<float>(down(Key::W))-static_cast<float>(down(Key::S));
        s.changeCamera=pressed_[static_cast<std::size_t>(Key::C)];
        s.reset=pressed_[static_cast<std::size_t>(Key::R)];
        s.toggleBloom=pressed_[static_cast<std::size_t>(Key::B)];
        s.toggleCrt=pressed_[static_cast<std::size_t>(Key::T)];
        pressed_.fill(false);
        return s;
    }
private:
    std::array<bool,static_cast<std::size_t>(Key::Count)> held_{};
    std::array<bool,static_cast<std::size_t>(Key::Count)> pressed_{};
};
} // namespace sw
