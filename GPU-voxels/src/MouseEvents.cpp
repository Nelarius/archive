#include "MouseEvents.h"
#include "utils/Log.h"
#include <SDL_mouse.h>

namespace pg {

void MouseEvents::setPressCallback( int button, const Command& command ) {
    pressed_.insert( std::make_pair( button, command ) );
}

void MouseEvents::setReleaseCallback( int button, const Command& command ) {
    released_.insert( std::make_pair( button, command ) );
}

Vec2i MouseEvents::coords() const {
    return currentCoords_;
}

void MouseEvents::update() {
    previousCoords_ = currentCoords_;
    uint32_t current = SDL_GetMouseState( &currentCoords_.x, &currentCoords_.y );
    // see which buttons have changed state
    uint32_t delta = current ^ previousState_;
    previousState_ = current;
    
    if ( current & delta & SDL_BUTTON( SDL_BUTTON_LEFT ) ) {
        auto it = pressed_.find( SDL_BUTTON_LEFT );
        if ( it != pressed_.end() ) {
            it->second();
        }
    }

    if ( ~current & delta & SDL_BUTTON( SDL_BUTTON_LEFT ) ) {
        auto it = released_.find( SDL_BUTTON_LEFT );
        if ( it != released_.end() ) {
            it->second();
        }
    }

    if ( current & delta & SDL_BUTTON( SDL_BUTTON_MIDDLE ) ) {
        auto it = pressed_.find( SDL_BUTTON_MIDDLE );
        if ( it != pressed_.end() ){
            it->second();
        }
    }

    if ( ~current & delta & SDL_BUTTON( SDL_BUTTON_MIDDLE ) ) {
        auto it = released_.find( SDL_BUTTON_MIDDLE );
        if ( it != released_.end() ){
            it->second();
        }
    }

    if ( current & delta & SDL_BUTTON( SDL_BUTTON_RIGHT ) ) {
        auto it = pressed_.find( SDL_BUTTON_RIGHT );
        if ( it != pressed_.end() ) {
            it->second();
        }
    }

    if ( ~current & delta & SDL_BUTTON( SDL_BUTTON_RIGHT ) ) {
        auto it = released_.find( SDL_BUTTON_RIGHT );
        if ( it != released_.end() ) {
            it->second();
        }
    }
}

}
