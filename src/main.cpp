#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "common.hpp"
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    (void)window;
}

int main()
{
    if (!glfwInit())
        die("failed to init glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *win = glfwCreateWindow(1280, 720, "GLFW Window", NULL, NULL);
    if (!win)
        die ("failed to create glfw window");
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        die("failed to init glad");

    printf("Vendor: %s\nVersion: %s\nRenderer: %s\n", glGetString(GL_VENDOR), glGetString(GL_VERSION), glGetString(GL_RENDERER));

    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    float verts[] = {
         0.0f,  1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    const char * vert_shader_src = R"vsh(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aCol;
        out vec3 vCol;
        void main() {
            gl_Position = vec4(aPos, 1.0f);
            vCol = aCol;
        }
    )vsh";

    const char * frag_shader_src = R"fsh(
        #version 330 core
        in vec3 vCol;
        out vec4 fCol;
        void main() {
            fCol = vec4(vCol, 1.0f);
        }
    )fsh";

    u32 vsh = compile_shader(GL_VERTEX_SHADER  , vert_shader_src);
    u32 fsh = compile_shader(GL_FRAGMENT_SHADER, frag_shader_src);

    u32 prog = glCreateProgram();
    glAttachShader(prog, vsh);
    glAttachShader(prog, fsh);
    link_program(prog);

    glClearColor(.18f, .18f, .18f, .18f);
    glUseProgram(prog);
    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    return 0;
}
