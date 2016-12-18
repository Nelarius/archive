#pragma once

#include <cstdint>

namespace pg {

class Window;
class Program;

/*
 * @brief Inherit from this class in order to render a user interface using the ImGui library
 * Note that the implementation of this class has a global state.
 **/
class ImGuiRenderer {
    public:
        ImGuiRenderer() = delete;
        ImGuiRenderer( Window*, Program* );
        virtual ~ImGuiRenderer();

        /*
        * @brief Callback for when a mouse button is pressed.
        * @param button The SDL button id, defined in SDL_mouse.h
        **/
        void mouseButtonPressed( int button );
        /*
         * @brief Callback for when a mouse button is released.
         * @param button the SDL button id, defined in SDL_mouse.h
         **/
        void mouseButtonReleased( int button );

        void newFrame( float dt, int x, int y );
        void render();

        // public function callbacks need to be set here
        // the idea would be to have ImGuiRenderer do something with a mouse click
        // I would bind the callback to a real time mouse input handler

    protected:
        void initialize_();
        void createDeviceObjects_();
        
        Window*         window_;
        Program*        panelShader_;
        static uint32_t refCount_;
        
};

}   // pg
