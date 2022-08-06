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
#include "shaderprogram.h"


ShaderProgram* sp;

class Model {
public:
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;
	glm::mat4 M = glm::mat4(1.0f);
	GLuint tex;
	float angular_speed = 0.0f;
	float linear_speed = 0.0f;
	float x, y, z;
	float angular_displacement;

	Model() {}
	Model(std::string filename) {
		loadModel(filename);
	}

	inline void loadModel(std::string filename) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
		Model model;

		std::cout << importer.GetErrorString() << std::endl;

		aiMesh* mesh = scene->mMeshes[0];
		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D vertex = mesh->mVertices[i];
			verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

			aiVector3D normal = mesh->mNormals[i];
			norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

			aiVector3D texCoord = mesh->mTextureCoords[0][i];
			texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));

		}

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];

			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
	}

	void performMovement(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) linear_speed += 0.02;
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) linear_speed -= 0.02;
		else linear_speed *= 0.99;

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) angular_speed = 0.01 * (linear_speed / 10);
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) angular_speed = -0.01 * (linear_speed / 10);
		else angular_speed = 0;

		/*std::cout << linear_speed << std::endl;*/
	}

	void readTexture(std::string filename) {
		glActiveTexture(GL_TEXTURE0);

		//Wczytanie do pamięci komputera
		std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
		unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
		//Wczytaj obrazek
		unsigned error = lodepng::decode(image, width, height, filename);

		//Import do pamięci karty graficznej
		glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
		glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
		//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void drawModel(glm::mat4 P, glm::mat4 V) {
		sp->use();//Aktywacja programu cieniującego
		M = glm::mat4(1.0f);

		angular_displacement += angular_speed;
		x += -1 * linear_speed * glm::cos(angular_displacement);
		z += linear_speed * glm::sin(angular_displacement);

		M = glm::translate(M, glm::vec3(x, y, z));
		M = glm::rotate(M, angular_displacement, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

		glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data()); //Wskaż tablicę z danymi dla atrybutu vertex

		glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords.data()); //Wskaż tablicę z danymi dla atrybutu texCoord

		glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, norms.data()); //Wskaż tablicę z danymi dla atrybutu normal

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data()); //Narysuj obiekt

		//double mv[16];
		//glGetDoublev(GL_MODELVIEW_MATRIX, mv);
		//x = mv[13];
		//y = mv[14];
		//z = mv[15];

		glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
		glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0
	}
};