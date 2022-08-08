#pragma once
#define GLM_FORCE_CTOR_INIT
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "lodepng.h"
#include "Model.h"

class Car : public Model {
public:
	float angular_speed = 0.0f;
	float linear_speed = 0.0f;
	float x, y, z;
	float angular_displacement;
	bool is_player;

	Car(std::string filename, bool is_player = false, unsigned int repeat_factor = 1) {
		this->is_player = is_player;
		Model::readModel(filename, repeat_factor);
	}

	void readInput(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) linear_speed += 0.02;
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) linear_speed -= 0.02;
		else linear_speed *= 0.99;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) angular_speed = 0.01 * (linear_speed / 10);
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) angular_speed = -0.01 * (linear_speed / 10);
		else angular_speed = 0;
	}

	void drawModel(glm::mat4 P, glm::mat4 V, glm::mat4 M = glm::mat4(1.0f), float reflectMin = 0.0f, float reflectMax = 1.0f) {
		angular_displacement += angular_speed;
		x += -1 * linear_speed * glm::cos(angular_displacement);
		z += linear_speed * glm::sin(angular_displacement);

		M = glm::translate(M, glm::vec3(x, y, z));
		M = glm::rotate(M, angular_displacement, glm::vec3(0.0f, 1.0f, 0.0f));

		Model::drawModel(P, V, M, reflectMin, reflectMax);
	}
};