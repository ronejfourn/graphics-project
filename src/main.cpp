#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "common.hpp"
#include "shader.hpp"
#include "math.hpp"

Mat4 projection;

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    /*
     * NOTE(rijan):
     *    for perspective projection,
     *    projection[0][0] = 1 / (tan(half_FOV) * aspect_ratio)
     *    if FOV = 90 degrees, tan(half_FOV) = 1 so,
     *    projection[0][0] = 1 / aspect_ratio
     */

    projection[0][0] = (f32)height / (f32)width;
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

    glfwSetFramebufferSizeCallback(win, framebufferSizeCallback);

    f32 verts[] = {
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(f32), (void *)(3 * sizeof(f32)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    const char * vertShaderSrc = R"vsh(
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

    const char * fragShaderSrc = R"fsh(
        #version 330 core
        in vec3 vCol;
        out vec4 fCol;
        void main() {
            fCol = vec4(vCol, 1.0f);
        }
    )fsh";

    u32 vsh = compileShader(GL_VERTEX_SHADER  , vertShaderSrc);
    u32 fsh = compileShader(GL_FRAGMENT_SHADER, fragShaderSrc);

    u32 prog = glCreateProgram();
    glAttachShader(prog, vsh);
    glAttachShader(prog, fsh);
    linkProgram(prog);

    glClearColor(.18f, .18f, .18f, .18f);
    glUseProgram(prog);
    glBindVertexArray(vao);
    glEnable(GL_DEPTH_TEST);

    GLuint uMVP = glGetUniformLocation(prog, "mvp");
    f32 a = 0;

    projection = mat4Perspective(0.01f, 1000, 90, 1);

    glfwSwapInterval(1); // vsync on for now

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        f32 c = cosf(a);
        f32 s = sinf(a);
        a += 0.01f;
        if (a > 2 * PI)
            a = 0;

        Mat4 rotX = mat4RotationX(c, s);
        Mat4 rotY = mat4RotationY(c, s);
        Mat4 rotZ = mat4RotationZ(c, s);
        Mat4 trns = mat4Translation(0, 0, 3);

        Mat4 view; // TODO

        Mat4 model = trns * rotZ * rotY * rotX;
        Mat4 mvp = projection * view * model;
        glUniformMatrix4fv(uMVP, 1, GL_TRUE, &mvp[0][0]);
        glDrawElements(GL_TRIANGLES, sizeof(indxs) / sizeof(indxs[0]), GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    return 0;
}
