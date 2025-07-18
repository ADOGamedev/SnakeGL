#pragma once

#include <glad/glad.h>	
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Segment {
public:
	glm::vec3 pos = glm::vec3(0.0f);

	void createSegment(unsigned int& VAO, unsigned int& VBO);
};