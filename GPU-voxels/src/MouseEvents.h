#pragma once

#include <math/Vector.h>
#include <vector>
#include <utility>
#include <unordered_map>
#include <functional>

namespace pg {
/**
 * @class MouseEvents
 * @file MouseEvents.h
 * @brief Bind commands to mouse events.
 * Internally uses SDL. Mouse button ID's are denoted by integers. Get the corresponding value
 * by doing SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT. Get the correct mask by
 * writing SDL_BUTTON( SDL_BUTTON_LEFT ), etc.
 */
class MouseEvents{
    using Command = std::function<void()>;
    public:
        MouseEvents()   = default;
        ~MouseEvents()  = default;
        /*
        * @brief set a callback command for when a mouse button is pressed.
        * @param button The SDL button id defined in SDL_mouse.h
        * @param command The command 
        **/
        void setPressCallback( int button, const Command& command );
        /*
        * @brief set a callback command for when a mouse button is released.
        * @param button The SDL button id defined in SDL_mouse.h
        * @param command The command 
        **/
        void setReleaseCallback( int button, const Command& command );
        void update();

        Vec2i coords() const;

    private:
        uint32_t    previousState_{0u}; // the mouse state mask
        Vec2i       previousCoords_{};
        Vec2i       currentCoords_{};
        std::unordered_map<int, Command>  pressed_{};
        std::unordered_map<int, Command>  released_{};
};

}
