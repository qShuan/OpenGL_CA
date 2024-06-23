#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ErrorHandling.h"


Shader::Shader(const std::string& filepath)
	: m_filepath(filepath), m_rendererID(0)
{

    ShaderSources source = ParseShader(filepath);
    m_rendererID = CreateShader(source.VertexSource, source.FragmentSource);

    uMVPlocation = glGetUniformLocation(m_rendererID, "u_MVP");
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_rendererID));
}

ShaderSources Shader::ParseShader(const std::string& filePath) {

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

unsigned int Shader::CompileShader(unsigned int type, std::string& source) {

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

int Shader::CreateShader(std::string& vertexShader, std::string& fragmentShader) {

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

void Shader::Bind() const
{
    GLCall(glUseProgram(m_rendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}
