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

#include "Sand.h"
#include "Empty.h"
#include <memory>
#include <array>
#include "Fps.h"

#define TILE_SIZE 4
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}


struct ShaderSources {

    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderSources ParseShader(const std::string& filePath) {

    std::fstream stream(filePath);

    enum class ShaderType {

        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {

        if (line.find("#shader") != std::string::npos) {

            if (line.find("vertex") != std::string::npos) {

                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {

                type = ShaderType::FRAGMENT;
            }
        }
        else {

            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, std::string& source) {

    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {

        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char* message = (char*)malloc(sizeof(char) * length);
        if (!message) {

            std::cout << "Failed to allocate memory for message!" << std::endl;
            return 0;
        }

        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);

        return 0;
    }

    return id;
}

static int CreateShader(std::string& vertexShader, std::string& fragmentShader) {

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

const int width = SCREEN_WIDTH / TILE_SIZE;
const int height = SCREEN_HEIGHT / TILE_SIZE;

typedef struct color_t {

    float r;
    float g;
    float b;
    float a;

}color_t;

color_t mat_col_sand{ 194.0f / 255.0f, 178.0f / 255.0f, 128.0f / 255.0f, .2f };

void ChangeQuadColor(int index, float* colors, color_t& color, ShaderStorageBuffer* ssbo) {

    colors[index * 4] = color.r; //r
    colors[index * 4 + 1] = color.g; //g
    colors[index * 4 + 2] = color.b; //b
    colors[index * 4 + 3] = color.a; //a


    ssbo->UpdateColors(index, 4 * sizeof(float), colors);
}

void drawCircle(int x, int y, int radius, float* colors, color_t& color, ShaderStorageBuffer* ssbo) {

    int rSquared = radius * radius;

    //std::cout << "Color: " << color.r << " " << color.g << " " << color.b << std::endl;

    for (int u = x - radius; u < x + radius + 1; u++) {
        for (int v = y - radius; v < y + radius + 1; v++) {
            if ((x - u) * (x - u) + (y - v) * (y - v) < rSquared)
            {
                ChangeQuadColor((width)*v + u, colors, color, ssbo);
            }
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, double* xpos, double* ypos)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //getting cursor position
        glfwGetCursorPos(window, xpos, ypos);

        *ypos = SCREEN_HEIGHT - *ypos;
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

        Cell* cells[4];
        cells[0] = new Sand();
        cells[1] = new Empty();

        Cell* temp = cells[0];
        cells[0] = cells[1];
        cells[1] = temp;

        std::cout << cells[1]->type << std::endl;
        cells[1]->Update();
        std::cout << cells[1]->type << std::endl;

        int* vertices = (int*)malloc(sizeof(int) * (width + 1) * (height + 1) * 2);

        if (vertices == NULL) return -1;

        for (int y = 0, i = 0; y <= height; y++) {

            for (int x = 0; x <= width; x++, i += 2) {

                vertices[i] = x * TILE_SIZE; //x
                vertices[i + 1] = y * TILE_SIZE; //y
            }
        }

        float* colors = (float*)malloc(sizeof(float) * (width) * (height) * 4);

        if (colors == NULL) return -1;

        for (int y = 0, i = 0; y < height; y++) {

            for (int x = 0; x < width; x++, i += 4) {

                colors[i] = randomFloat(); //r
                colors[i + 1] = randomFloat(); //g
                colors[i + 2] = randomFloat(); //b
                colors[i + 3] = 1.f; //b
            }
        }

        unsigned int* indices = (unsigned int*)malloc(sizeof(unsigned int) * width * height * 6);

        if (indices == NULL) return -1;

        for (int ti = 0, vi = 0, y = 0; y < height; y++, vi++) {
            for (int x = 0; x < width; x++, ti += 6, vi++) {

                indices[ti] = vi;
                indices[ti + 1] = vi + 1;
                indices[ti + 2] = vi + width + 2;
                indices[ti + 3] = indices[ti + 2];
                indices[ti + 4] = vi + width + 1;
                indices[ti + 5] = indices[ti];
            }
        }

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexBuffer vb(vertices, (width + 1) * (height + 1) * 2 * sizeof(int));

        ShaderStorageBuffer ssbo(colors, (width) * (height) * 4 * sizeof(float));

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(int) * 2, 0);

        IndexBuffer ib(indices, width * height * 6);

        glm::mat4 projMat = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT, .0f, 1.f);

        ShaderSources source = ParseShader("VertFrag.shader");

        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        glUseProgram(shader);


        /*int location = glGetUniformLocation(shader, "u_Color");
        ASSERT(location != -1);
        glUniform4f(location, 1.f, 0.f, 0.3f, 1.f);*/

        //int colorLocation = glGetUniformLocation(shader, "colors");
        //glUniform3fv(colorLocation, (width) * (height) * 3, colors);

        int uMVPlocation = glGetUniformLocation(shader, "u_MVP");
        GLCall(glUniformMatrix4fv(uMVPlocation, 1, GL_FALSE, &projMat[0][0]));


        /*GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));*/

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        float r = 0.0f;
        float increment = 0.05f;

        /*GLCall(glUseProgram(shader));


        GLCall(glBindVertexArray(vao));


        ib.Bind();*/

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            //Display FPS
            fps.update();

            int fps_num = fps.getFPS();

            auto s_fps = std::to_string(fps_num);

            glfwSetWindowTitle(window, s_fps.c_str());

            //Update
            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            if (state == GLFW_PRESS)
            {
                double xpos, ypos;

                mouse_button_callback(window, GLFW_MOUSE_BUTTON_LEFT, state, &xpos, &ypos);

                int x = (int)xpos / TILE_SIZE;
                int y = (int)ypos / TILE_SIZE;

                //std::cout << "Cursor Position at (" << x << " : " << y << ")" << std::endl;

                int index = (width)*y + x;
                //ChangeQuadColor(index, colors, mat_col_sand);

                drawCircle(x, y, 5, colors, mat_col_sand, &ssbo);
            }

            /*for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {

                    if (y > 0) {

                        colors[((width)*y + x) * 4] = 0.f;
                        colors[((width) * (y - 1) + x) * 4] = 194.0f / 255.0f;

                        glBufferSubData(GL_SHADER_STORAGE_BUFFER, ((width)*y + x) * 4 * sizeof(float), 3 * sizeof(float), &colors[((width)*y + x) * 4]);
                        glBufferSubData(GL_SHADER_STORAGE_BUFFER, ((width)* (y - 1) + x) * 4 * sizeof(float), 3 * sizeof(float), &colors[((width) * (y - 1) + x) * 4]);
                    }
                }
            }*/

            GLCall(glDrawElements(GL_TRIANGLES, (width * height) * 6, GL_UNSIGNED_INT, nullptr));


            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        glDeleteProgram(shader);

        GLCall(glBindVertexArray(0));
        vb.Unbind();
        ib.Unbind();
        ssbo.Unbind();

        free(vertices);
        free(indices);
        free(colors);
    }


    glfwTerminate();
    return 0;
}
