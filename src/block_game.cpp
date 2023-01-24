#include <glad/glad.h>
#include <stdio.h>
#include "platform.hpp"
#include "common.hpp"
#include "shader.hpp"
#include "math.hpp"
#include "events.hpp"
#include "texture.hpp"
#include "camera.hpp"

// TODO better error messages

int blockGame(const Events &events)
{
    if (!plInitialize()) {
        plTerminate();
        die ("initialization failed");
    }

    if (!gladLoadGL())
        die("failed to init glad");

    printf("Vendor: %s\nVersion: %s\nRenderer: %s\n", glGetString(GL_VENDOR), glGetString(GL_VERSION), glGetString(GL_RENDERER));

    f32 verts[] = {
        // coordinates          /        colors           /     texturelocation
         0.5f,  0.5f,  0.5f, /*****/ 0.0f, 0.0f, 0.0f, /*****/ 1.0f, 0.0f, // 0
         0.5f,  0.5f, -0.5f, /*****/ 0.0f, 0.0f, 1.0f, /*****/ 0.0f, 0.0f, // 1
         0.5f, -0.5f,  0.5f, /*****/ 0.0f, 1.0f, 0.0f, /*****/ 0.0f, 0.0f, // 2
         0.5f, -0.5f, -0.5f, /*****/ 0.0f, 1.0f, 1.0f, /*****/ 1.0f, 0.0f, // 3

        -0.5f,  0.5f,  0.5f, /*****/ 1.0f, 0.0f, 0.0f, /*****/ 1.0f, 1.0f, // 4
        -0.5f,  0.5f, -0.5f, /*****/ 1.0f, 0.0f, 1.0f, /*****/ 0.0f, 1.0f, // 5
        -0.5f, -0.5f,  0.5f, /*****/ 1.0f, 1.0f, 0.0f, /*****/ 0.0f, 1.0f, // 6
        -0.5f, -0.5f, -0.5f, /*****/ 1.0f, 1.0f, 1.0f, /*****/ 1.0f, 1.0f, // 7
    };

    GLuint indxs[] = {
        0, 1, 3,   0, 2, 3,
        4, 5, 7,   4, 6, 7,

        0, 1, 5,   0, 4, 5,
        2, 3, 7,   2, 6, 7,

        0, 4, 6,   0, 2, 6,
        1, 5, 7,   1, 3, 7,
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    const char *vertShaderSrc = R"vsh(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aCol;
        layout (location = 2) in vec2 aTexCoord;

        out vec3 vCol;
        out vec2 TexCoord;
        uniform mat4 mvp;
        void main() {
            gl_Position = mvp * vec4(aPos, 1.0f);;
            vCol = aCol;
            TexCoord=aTexCoord;
        }
    )vsh";

    const char *fragShaderSrc = R"fsh(
        #version 330 core
        in vec3 vCol;
        in vec2 TexCoord;
        out vec4 fCol;

        uniform int mx;
        uniform int my;
        uniform sampler2D ourTexture;
        void main() {
            fCol=texture(ourTexture,TexCoord);
            float dx = gl_FragCoord.x - mx;
            float dy = gl_FragCoord.y - my;
            if (dx * dx + dy * dy < 4000)
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

    GLuint umx  = glGetUniformLocation(prog, "mx");
    GLuint umy  = glGetUniformLocation(prog, "my");
    GLuint uMVP = glGetUniformLocation(prog, "mvp");
    f32 a = 0;

    plSwapInterval(1); // vsync on for now

    ////////////////////////////////////////////////////////////////////////////////

    // Texture [Risav]

    Texture2D mytexture;
    if (!mytexture.setImageFromFile("../resources/minecraft.png"))
        die("failed to set texture image");

    ////////////////////////////////////////////////////////////////////////////////

    Camera cam;

    while (!events.shouldClose) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (events.keyPressed(KEY_RETURN))
            printf("pressed enter\n");

        if (events.keyReleased(KEY_RETURN))
            printf("released enter\n");

        u32 direction = 0;
        if (events.keyHeld(KEY_W))
            direction |= FORWARD;
        if (events.keyHeld(KEY_A))
            direction |= LEFT;
        if (events.keyHeld(KEY_S))
            direction |= BACKWARD;
        if (events.keyHeld(KEY_D))
            direction |= RIGHT;
        cam.processKeyboard((CameraMovement)direction, 0.02f);

        if (events.keyHeld(KEY_H))
            cam.processMouseMovement(-0.5,  0);
        if (events.keyHeld(KEY_J))
            cam.processMouseMovement( 0, -0.5);
        if (events.keyHeld(KEY_K))
            cam.processMouseMovement( 0,  0.5);
        if (events.keyHeld(KEY_L))
            cam.processMouseMovement( 0.5,  0);

        if (events.window.resized) {
            cam.setAspectRatio((float)events.window.w / (float)events.window.h);
            glViewport(0, 0, events.window.w, events.window.h);
        }

        f32 c = cosf(a);
        f32 s = sinf(a);
        a += 0.01f;
        if (a > 2 * PI)
            a = 0;

        Mat4 rotX = mat4RotationX(c, s);
        Mat4 rotY = mat4RotationY(c, s);
        Mat4 rotZ = mat4RotationZ(c, s);
        Mat4 trns = mat4Translation(0, 0, 0);

        Mat4 proj = cam.getProjectionMatrix();
        Mat4 view = cam.getViewMatrix();

        Mat4 model = trns;
        Mat4 mvp = proj * view * model;

        glUniformMatrix4fv(uMVP, 1, GL_TRUE, &mvp[0][0]);
        glUniform1i(umx, events.cursor.x);
        glUniform1i(umy, events.cursor.y);
        mytexture.bind();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indxs) / sizeof(indxs[0]), GL_UNSIGNED_INT, (void *)0);

        plSwapBuffers();
        plPollEvents();
    }

    plTerminate();
    return 0;
}
