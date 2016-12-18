# version 440 core

in vec4 vertex;
out vec4 fragPos;

uniform mat4 model;
uniform mat4 ortho;

void main() {
    fragPos = model * vertex;
    gl_Position = ortho * model * vertex;
}
