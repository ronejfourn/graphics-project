#version 330 core
in vec3 vCol;
in vec2 TexCoord;
out vec4 fCol;

uniform int mx;
uniform int my;
uniform sampler2D ourTexture;
void main() {
    fCol = texture(ourTexture, TexCoord);
    float dx = gl_FragCoord.x - mx;
    float dy = gl_FragCoord.y - my;
    if (dx * dx + dy * dy < 4000)
        fCol = vec4(vCol, 1.0f);
}
