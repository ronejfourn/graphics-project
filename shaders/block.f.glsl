#version 330 core
out vec4 fCol;

in vec3 uvw;
uniform sampler2DArray arr;

void main() {
    fCol = texture(arr, uvw);
}
