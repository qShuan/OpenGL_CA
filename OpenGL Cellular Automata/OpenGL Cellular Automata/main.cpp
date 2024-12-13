#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ErrorHandling.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ShaderStorageBuffer.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Sandbox.h"
#include "FPS.h"

#define TARGET_FPS 100

void mouse_button_callback(GLFWwindow* window, int button, int action, double* xpos, double* ypos)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //getting cursor position
        glfwGetCursorPos(window, xpos, ypos);

        *ypos = SCREEN_HEIGHT - *ypos;
    }
}

void CheckCellType(GLFWwindow* window, Sandbox& sandbox) {

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        sandbox.currentType = EMPTY;
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        sandbox.FillScreen();
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        sandbox.currentType = SAND;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        sandbox.currentType = WATER;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        sandbox.currentType = WOOD;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        sandbox.currentType = STONE;
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        sandbox.currentType = LAVA;
    }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        sandbox.currentType = FIRE;
    }
}

int main(void) {

    srand(time(NULL));

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {

        std::cout << "GlewInit ERROR" << std::endl;
    }


    //Print OpenGL version
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    {
        FPS fps;

        Sandbox sandbox;

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexBuffer vb(sandbox.vertices, (sandbox.width + 1) * (sandbox.height + 1) * 2 * sizeof(int));

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(int) * 2, 0);

        IndexBuffer ib(sandbox.indices, sandbox.width * sandbox.height * 6);

        glm::mat4 projMat = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT, .0f, 1.f);

        Shader shader("VertFrag.shader");
        shader.Bind();

        GLCall(glUniformMatrix4fv(shader.uMVPlocation, 1, GL_FALSE, &projMat[0][0]));
        
        GLCall(glBindVertexArray(0));
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        float r = 0.0f;
        float increment = 0.05f;

        /*GLCall(glUseProgram(shader));


        GLCall(glBindVertexArray(vao));


        ib.Bind();*/  
        double lasttime = glfwGetTime();
        double lastUpdateTime = 0;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            GLCall(glBindVertexArray(vao));
            shader.Bind();
            vb.Bind();
            ib.Bind();

            double now = glfwGetTime();
            double deltaTime = now - lastUpdateTime;

            //Fps Limit
            while (glfwGetTime() < lasttime + 1.0 / TARGET_FPS) {}

            sandbox.UpdateDeltaTime(deltaTime);

                //Display FPS
                fps.update();
                int fps_num = fps.getFPS();
                auto s_fps = std::to_string(fps_num);
                glfwSetWindowTitle(window, s_fps.c_str());


                int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                if (state == GLFW_PRESS)
                {
                    double xpos, ypos;

                    mouse_button_callback(window, GLFW_MOUSE_BUTTON_LEFT, state, &xpos, &ypos);

                    int x = (int)xpos / TILE_SIZE;
                    int y = (int)ypos / TILE_SIZE;

                    int index = (sandbox.width) * y + x;

                    sandbox.DrawCircle(x, y, 5);
                }

                CheckCellType(window, sandbox);

                sandbox.Update();

                sandbox.Draw();


                /* Swap front and back buffers */
                glfwSwapBuffers(window);

                /* Poll for and process events */
                glfwPollEvents();
            

            lasttime += 1.0 / TARGET_FPS;
            lastUpdateTime = now;
        }


        GLCall(glBindVertexArray(0));
        vb.Unbind();
        ib.Unbind();
    }


    glfwTerminate();
    return 0;
}