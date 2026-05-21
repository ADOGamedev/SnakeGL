#include <glad/glad.h>	
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#include <random>

#include "Shader.h"
#include "stb_image.h"
#include "Player.h"
#include "Apple.h"

// Initial values
#define WIDTH 800
#define HEIGHT 800

const glm::vec3 cameraPos = glm::vec3(8.0f, 17.0f, 23.0f);
const float cameraFov = 60.0f;

// Later will use to divide the plane in a grid 16x16
const unsigned int tilesAmount = 17;

unsigned int currentWidth = WIDTH;
unsigned int currentHeight = HEIGHT;

const glm::vec3 FRONT_VEC = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 RIGHT_VEC = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 LEFT_VEC = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::vec3 BACK_VEC = glm::vec3(0.0f, 0.0f, 1.0f);


GLFWwindow* createWindow() {
	// Sets version of OpenGL to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Sets profile to core OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SnakeGL", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwShowWindow(window);

	return window;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void processMovementInput(GLFWwindow* window, Player &player) {
	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) and (player.frontVec != BACK_VEC)) {
		player.currentFrontVec = FRONT_VEC;
	}
	if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) and (player.frontVec != RIGHT_VEC)) {
		player.currentFrontVec = LEFT_VEC;
	}
	if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) and (player.frontVec != FRONT_VEC)) {
		player.currentFrontVec = BACK_VEC;
	}
	if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) and (player.frontVec != LEFT_VEC)) {
		player.currentFrontVec = RIGHT_VEC;
	}
}

void resize_callback(GLFWwindow* window, int width, int height) {
	currentWidth = width;
	currentHeight = height;
	glViewport(0, 0, width, height);
}

void setUpWorldPlane(unsigned int &planeVAO, unsigned int &planeVBO, unsigned int &planeEBO, unsigned int indices[], size_t indexCount) {
	float vertices[] = {
		 0.0f, 0.0f,  0.0f,    0.0f, 0.0f,
		 0.0f, 0.0f,  1.0f,    0.0f, 1.0f,
		 1.0f, 0.0f,  1.0f,    1.0f, 1.0f,
		 1.0f, 0.0f,  0.0f,     1.0f, 0.0f
	};
	// Vertex Array Object
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	//Vertex Buffer Object
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Element Buffer Object
	glGenBuffers(1, &planeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// Defines how to interpretate the data in vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Defines how to interpretate the data in vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

unsigned int createTexture(const char* path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Tells openGL that the resolution of the image inst a power of two
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return texture;
}

void checkSegmentsCollision(GLFWwindow* window, Player& player, Apple &apple) {
	for (int i = 1; i < player.segments.size(); i++) {
		if (player.pos == player.segments[i].pos) {
			glfwSetWindowShouldClose(window, true);
		}
		if (apple.pos - glm::vec3(8.0f, 0.0f, 8.0f) == player.segments[i].pos) {
			apple.moveApple();
		}
	}
}

int main() {
	glfwInit();

	GLFWwindow* window = createWindow();

	// Inits GLAD
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetWindowSizeCallback(window, resize_callback);

	Shader shader("vertexShader.vertex", "fragmentShader.fragment");

	glViewport(0, 0, WIDTH, HEIGHT);


	unsigned int indices[] = {
		1, 0, 2,
		0, 2, 3
	};
	unsigned int planeVAO, planeVBO, planeEBO;
	setUpWorldPlane(planeVAO, planeVBO, planeEBO, indices, sizeof(indices) / sizeof(unsigned int));


	Player player;
	unsigned int playerVAO, playerVBO;
	player.createPlayer(playerVAO, playerVBO);

	Apple apple;
	unsigned int appleVAO, appleVBO;
	apple.createApple(appleVAO, appleVBO);
	apple.moveApple();

	unsigned int planeTexture = createTexture("Assets\\worldPlane.jpg");
	unsigned int playerTexture = createTexture("Assets\\player.jpg");
	unsigned int appleTexture = createTexture("Assets\\apple.jpg");
	unsigned int segmentTexture = createTexture("Assets\\segment.jpg");


	shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, playerTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, appleTexture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, segmentTexture);


	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		processMovementInput(window, player);

		glClearColor(0.3f, 0.35f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Apple's 0, 0 is in the upper left corner while Player's 0, 0 is in the center
		if (player.pos == (apple.pos - glm::vec3(8.0f, 0.0f, 8.0f))) {
			apple.moveApple();

			Segment new_segment;
			new_segment.pos = player.segments[player.segments.size() - 1].pos;

			player.segments.push_back(new_segment);
			player.points++;
			player.updateSpeed();
		}

		checkSegmentsCollision(window, player, apple);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(cameraFov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, glm::vec3((float)(tilesAmount / 2), 0.0f, (float)(tilesAmount / 2)), glm::vec3(0.0f, 1.0f, 0.0));

		glm::mat4 planeModel = glm::mat4(1.0f);

		// 4. Set it back to where the camera is centered
		planeModel = glm::translate(planeModel, glm::vec3(0.5f, 0.0f, 0.5f) * (float)tilesAmount);

		// 3. Scales it by the inverse of aspect ratio to not distor the image
		planeModel = glm::scale(planeModel, glm::vec3((float)currentHeight / (float)currentWidth, 1.0f, 1.0f));

		// 2. The plain scales from 1x1 to 16x16
		planeModel = glm::scale(planeModel, glm::vec3((float)tilesAmount));

		// 1. Sets the plane to 0, 0
		planeModel = glm::translate(planeModel, glm::vec3(-0.5f, 0.0f, -0.5f));


		glm::mat4 objectScaling = glm::mat4(1.0f);

		objectScaling = glm::translate(objectScaling, glm::vec3(0.5f, 0.0f, 0.5f) * (float)tilesAmount);
		objectScaling = glm::scale(objectScaling, glm::vec3((float)currentHeight / (float)currentWidth, 1.0f, 1.0f));
		objectScaling = glm::translate(objectScaling, glm::vec3(-0.5f, 0.0f, -0.5f));


		glm::mat4 playerModel = glm::mat4(1.0f);

		player.moveForward();

		playerModel = glm::translate(playerModel, player.pos);


		glm::mat4 appleModel = glm::mat4(1.0f);

		appleModel = glm::translate(appleModel, apple.pos - glm::vec3(8.0f, 0.0f, 8.0f));


		
		glBindVertexArray(planeVAO);
		shader.setMat4("transform", projection * view * planeModel);
		shader.setInt("myTexture", 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(playerVAO);
		shader.setMat4("transform", projection * view * objectScaling * playerModel);
		shader.setInt("myTexture", 1);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(appleVAO);
		shader.setMat4("transform", projection * view * objectScaling * appleModel);
		shader.setInt("myTexture", 2);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		for (Segment segment : player.segments) {
			unsigned int segmentVAO, segmentVBO;
			segment.createSegment(segmentVAO, segmentVBO);

			glBindVertexArray(segmentVAO);

			glm::mat4 segmentModel = glm::mat4(1.0f);
			segmentModel = glm::translate(segmentModel, segment.pos);

			shader.setMat4("transform", projection * view * objectScaling * segmentModel);
			shader.setInt("myTexture", 3);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}