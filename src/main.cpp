#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdarg>
#include <cstdint>

typedef int8_t   i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    (void)window;
}

void die(const char *fmt, ...) {
    // TODO report err before death
    fprintf(stderr, fmt);
    exit(1);
}

u32 create_and_compile_shader(GLenum type, const char * const *src) {
    u32 sh = glCreateShader(type);
    glShaderSource(sh, 1, src, NULL);
    glCompileShader(sh);
    int su = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &su);
    if (!su) {
        char log[1025];
        log [1024] = 0;
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        // TODO better err msg
        fprintf(stderr, "%s\n", log);
        die("failed to compile shader\n");
    }
    return sh;
}

int main()
{
    if (!glfwInit())
        die("failed to init glfw\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *win = glfwCreateWindow(1280, 720, "GLFW Window", NULL, NULL);
    if (!win)
        die ("failed to create glfw window");
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        die("failed to init glad\n");

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

    const char * vert_shader_src = " \
        #version 330 core \n\
        layout (location = 0) in vec3 aPos; \n\
        layout (location = 1) in vec3 aCol; \n\
        out vec3 vCol; \n\
        void main() { \n\
            gl_Position = vec4(aPos, 1.0f); \n\
            vCol = aCol; \n\
        } \
    ";

    const char * frag_shader_src = " \
        #version 330 core \n\
        in vec3 vCol; \n\
        out vec4 fCol; \n\
        void main() { \n\
            fCol = vec4(vCol, 1.0f); \n\
        } \
    ";

    u32 vsh = create_and_compile_shader(GL_VERTEX_SHADER  , &vert_shader_src);
    u32 fsh = create_and_compile_shader(GL_FRAGMENT_SHADER, &frag_shader_src);

    u32 prog = glCreateProgram();
    glAttachShader(prog, vsh); // TODO move these into fxn + check err
    glAttachShader(prog, fsh);
    glLinkProgram(prog);

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
