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


ShaderProgram* sp;

class Model {
protected:
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;
	GLuint tex = 0;


public:
	float x, y, z;
	float size_x, size_y, size_z;

	Model() {}

	Model(std::string filename, float x, float y, float z, float size_x, float size_y, float size_z, unsigned int repeat_factor = 1) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->size_x = size_x;
		this->size_y = size_y;
		this->size_z = size_z;
		readModel(filename, repeat_factor);
	}

	Model(std::string filename, unsigned int repeat_factor = 1) {
		readModel(filename, repeat_factor);
	}

	inline void readModel(std::string filename, unsigned int repeat_factor) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

		if (strlen(importer.GetErrorString()) > 0) { // print errors if there are any
			std::cout << importer.GetErrorString() << std::endl;
		}

		aiMesh* mesh = scene->mMeshes[0];
		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D vertex = mesh->mVertices[i];
			verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

			aiVector3D normal = mesh->mNormals[i];
			norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

			aiVector3D texCoord = mesh->mTextureCoords[0][i];
			if (repeat_factor != 1) {
				texCoord.x *= repeat_factor;
				texCoord.y *= repeat_factor;
			}
			texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
		}

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];

			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
	}

	void readTexture(std::string filename) {
		glActiveTexture(GL_TEXTURE0);

		std::vector<unsigned char> image;   // allocate image vector
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, filename); // load image

		// import to graph card memory
		glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
		glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
		//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	void drawModel(glm::mat4 P, glm::mat4 V, glm::mat4 M = glm::mat4(1.0f), float reflectMin = 0.0f, float reflectMax = 1.0f) {
		glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
		glUniform1f(sp->u("reflectMin"), reflectMin);
		glUniform1f(sp->u("reflectMax"), reflectMax);

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

		glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
		glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0
	}
};