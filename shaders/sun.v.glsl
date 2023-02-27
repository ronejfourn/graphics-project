#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelviewproj;

void main()
{
    gl_Position = modelviewproj * vec4(aPos, 1.0f);
}
