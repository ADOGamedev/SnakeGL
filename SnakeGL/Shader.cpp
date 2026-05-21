#include <glad/glad.h>	
#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	std::stringstream vertexStream;
	std::stringstream fragmentStream;

	std::ifstream vertexFile;
	std::ifstream fragmentFile;

	vertexFile.open(vertexPath);
	fragmentFile.open(fragmentPath);

	// Reads the stream of the file
	vertexStream << vertexFile.rdbuf();
	fragmentStream << fragmentFile.rdbuf();

	vertexFile.close();
	fragmentFile.close();

	std::string vertexString, fragmentString;

	// Reads the string
	vertexString = vertexStream.str();
	fragmentString = fragmentStream.str();

	// Reads the const char*
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();
	
	// Creates and compiles shaders
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexCode, NULL);
	glShaderSource(fragmentShader, 1, &fragmentCode, NULL);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Creates and links program
	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() {
	glUseProgram(program);
}

void Shader::setInt(const std::string& name,int v1) {
	glUniform1i(glGetUniformLocation(program, name.c_str()), v1);
}

void Shader::setVec3(const std::string& name, float v1, float v2, float v3) {
	glUniform3f(glGetUniformLocation(program, name.c_str()), v1, v2, v3);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}