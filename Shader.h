#pragma once

#include <glad/glad.h>	
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader {
public:
	unsigned int program;

	Shader(const char* vertexShader, const char* fragmentShader);
	void use();
	void setInt(const std::string& name, int v1);
	//void setFloat(float value);
	void setVec3(const std::string& name, float v1, float v2, float v3);
	void setMat4(const std::string& name, glm::mat4 value);
};
