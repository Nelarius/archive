#version 440 core

uniform mat4 camera;
uniform mat4 model;

in vec4 vertex;

void main() {
    gl_Position = camera * model * vertex;
}
