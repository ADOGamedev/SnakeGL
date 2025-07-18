#pragma once

#include <glad/glad.h>	
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Segment.h"

const glm::vec3 INITIAL_POS = glm::vec3(-4.5f, 0.5f, 0.5f);

class Player {
public:
	glm::vec3 pos = INITIAL_POS;
	glm::vec3 frontVec = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 currentFrontVec = glm::vec3(1.0f, 0.0f, 0.0f);

	std::vector<Segment> segments = {};
	float time = 0.0f;

	float lastFrame = 0.0f;
	float deltaTime = 0.0f;

	float movementSpeed = 0.2f;

	int points = 0;


	Player();
	void createPlayer(unsigned int& VAO, unsigned int& VBO);
	void moveForward();
	void updateSpeed();
};
