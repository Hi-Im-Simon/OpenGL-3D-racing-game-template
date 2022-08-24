#pragma once
#define GLM_FORCE_CTOR_INIT
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

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
	float max_speed;
	float angular_displacement;
	bool is_player;
	/*std::vector<glm::vec3> path;
	unsigned int path_progression;*/

	Car(std::string filename, float max_speed = 40.0) {
		this->is_player = true;
		this->max_speed = max_speed;
		Model::readModel(filename, 1);
	}

	/*Car(std::string filename, const std::vector<glm::vec3>& path) {
		this->is_player = false;
		this->path = path;
		this->x = path[0].x;
		this->y = path[0].y;
		this->z = path[0].z;
		Model::readModel(filename, 1);
	}*/

	void readInput(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (linear_speed >= 0) {
				linear_speed = std::max(linear_speed - 0.1, 0.0);
			}
			else {
				linear_speed = std::min(linear_speed + 0.1, 0.0);
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			if (linear_speed >= 0) {
				if (linear_speed == 0) linear_speed = 0.02f;
				linear_speed = std::min((linear_speed + std::min((float)pow(linear_speed / max_speed, 1.05), 0.02f)), max_speed);
			}
			else {
				linear_speed = std::min(linear_speed + 0.08, 0.0);
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			if (linear_speed <= 0) {
				if (linear_speed == 0) linear_speed = -0.02f;
				linear_speed = std::max((linear_speed - std::min((float)pow(linear_speed / -max_speed, 1.05), 0.02f)), -max_speed / 8);
			}
			else {
				linear_speed = std::max(linear_speed - 0.08, 0.0);
			}
		else linear_speed *= 0.9995;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			linear_speed *= 0.9993;
			if (linear_speed > 2.0) {
				angular_speed = std::min(0.2 / linear_speed, 0.005);
			}
			else {
				angular_speed = linear_speed * 0.005 / 2.0;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			linear_speed *= 0.9993;
			if (linear_speed > 2.0) {
				angular_speed = -std::min(0.2 / linear_speed, 0.005);
			}
			else {
				angular_speed = -linear_speed * 0.005 / 2.0;
			}
		}
		else angular_speed = 0;

		//std::cout << linear_speed << std::endl;
	}

	void checkCollision(std::map<std::string, Model> Bands) {
		std::cout << x << " " << z << std::endl;
		if ((z < Bands["x0"].z + 100 || z > Bands["x1"].z - 100 || x < Bands["z0"].x + 100 || x > Bands["z1"].x - 100)) {
			x = std::clamp(x, Bands["z0"].x + 100, Bands["z1"].x - 100);
			z = std::clamp(z, Bands["x0"].z + 100, Bands["x1"].z - 100);
			linear_speed *= -0.1;
		}
		
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