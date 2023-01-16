#version 330 core
out vec4 fCol;
in  vec3 vCol;

void main() {
    fCol = vec4(vCol, 1.0f);
}
