#pragma once
#include <string>

struct ShaderSources {

	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {

private:
	unsigned int m_rendererID;
	std::string m_filepath;

public:

	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	int uMVPlocation;

private:

	ShaderSources ParseShader(const std::string& filePath);
	unsigned int CompileShader(unsigned int type, std::string& source);
	int CreateShader(std::string& vertexShader, std::string& fragmentShader);

};