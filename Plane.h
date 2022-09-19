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

class Plane : public Model {
public:
	float rotation = 0.0f;
	float circle_radius, height, speed;
	std::string texture_filename;

	Plane(std::string model_filename, float circle_radius, float height, float speed) {
		this->circle_radius = circle_radius;
		this->height = height;
		this->speed = speed;
		Model::readModel(model_filename, 1);
	}

	void drawModel(glm::mat4 P, glm::mat4 V, glm::mat4 M = glm::mat4(1.0f), float reflectMin = 0.0f, float reflectMax = 1.0f) {
		M = glm::translate(M, glm::vec3(circle_radius * cos(rotation), height, circle_radius * sin(rotation)));
		M = glm::scale(M, glm::vec3(200.0f, 200.0f, 200.0f));
		M = glm::rotate(M, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::rotate(M, (2 * PI / 6.283f) * rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		
		if (speed < 0) M = glm::rotate(M, PI, glm::vec3(0.0f, 0.0f, 1.0f));

		rotation += speed;
		if (rotation >= 360.0f) rotation -= 360.0f;
		if (rotation < 0) rotation += 360.0f;

		Model::drawModel(P, V, M, reflectMin, reflectMax);
	}
};