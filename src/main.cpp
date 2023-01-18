#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "common.hpp"
#include "shader.hpp"
#include "math.hpp"

mat4 projection;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    /*
     * NOTE(rijan):
     *    for perspective projection,
     *    projection[0][0] = 1 / (tan(half_FOV) * aspect_ratio)
     *    if FOV = 90 degrees, tan(half_FOV) = 1 so,
     *    projection[0][0] = 1 / aspect_ratio
     */

    projection[0][0] = (float)height / (float)width;
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

    GLFWwindow *win = glfwCreateWindow(800, 800, "GLFW Window", NULL, NULL);
    if (!win)
        die ("failed to create glfw window");
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        die("failed to init glad");

    printf("Vendor: %s\nVersion: %s\nRenderer: %s\n", glGetString(GL_VENDOR), glGetString(GL_VERSION), glGetString(GL_RENDERER));

    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    float verts[] = {
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, //0
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, //1
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, //2
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, //3

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, //4
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, //5
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f, //6
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, //7
    };

    GLuint indxs[] = {
        0, 1, 3,  0, 2, 3,
        4, 5, 7,  4, 6, 7,

        0, 1, 5,  0, 4, 5,
        2, 3, 7,  2, 6, 7,

        0, 4, 6,  0, 2, 6,
        1, 5, 7,  1, 3, 7,
    };

    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    u32 ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indxs), indxs, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    const char * vert_shader_src = R"vsh(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aCol;
        out vec3 vCol;
        uniform mat4 mvp;
        void main() {
            gl_Position = mvp * vec4(aPos, 1.0f);;
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
    glEnable(GL_DEPTH_TEST);

    GLuint u_mvp = glGetUniformLocation(prog, "mvp");
    float a = 0;

    projection = mat4_perspective(0.01f, 1000, 90, 1);

    glfwSwapInterval(1); // vsync on for now

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float c = cosf(a);
        float s = sinf(a);
        a += 0.01f;
        if (a > 2 * PI)
            a = 0;

        mat4 rotX = mat4_rotation_x(c, s);
        mat4 rotY = mat4_rotation_y(c, s);
        mat4 rotZ = mat4_rotation_z(c, s);
        mat4 trns = mat4_translation(0, 0, 3);

        mat4 view; // TODO

        mat4 model = trns * rotZ * rotY * rotX;
        mat4 mvp = projection * view * model;
        glUniformMatrix4fv(u_mvp , 1, GL_TRUE, &mvp[0][0]);
        glDrawElements(GL_TRIANGLES, sizeof(indxs) / sizeof(indxs[0]), GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    return 0;
}
