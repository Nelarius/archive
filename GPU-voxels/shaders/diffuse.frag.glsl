#version 440 core

uniform vec3 surfColor;

out vec4 color;

void main() {
    color = vec4( surfColor, 1.0 );
}

