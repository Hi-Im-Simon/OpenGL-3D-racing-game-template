/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <stdlib.h>
#include <map>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mciapi.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "constants.h"
#include "shaderprogram.h"
#include "Model.h"
#include "Car.h"
#pragma comment(lib, "winmm.lib")

Car Player("Models/Formula.fbx");
//std::vector<glm::vec3> path{
//	glm::vec3(1000, 0, 1000),
//	glm::vec3(1000, 0, 0),
//	glm::vec3(0, 0, 0),
//	glm::vec3(0, 0, 1000)
//	};
//Car Ai0("Models/Formula.fbx", path);
Model Sphere("Models/Sphere.fbx");
Model Grass("Models/Grass.fbx", 200);
Model Track("Models/Track.fbx", 50);

std::vector<Model> Trees = {
	Model("Models/Plant.fbx", 5),
};

std::map<std::string, Model> Bands = {
	{ "x0", Model("Models/Grass.fbx", 0.0f, 0.0f, -7500.0f, 36000.0f, 150.0f, 5.0f, 500) },
	{ "x1", Model("Models/Grass.fbx", 0.0f, 0.0f, 7500.0f, 36000.0f, 150.0f, 5.0f, 500) },
	{ "z0", Model("Models/Grass.fbx", -18000.0f, 0.0f, 0.0f, 5.0f, 150.0f, 15000.0f, 208) },
	{ "z1", Model("Models/Grass.fbx", 18000.0f, 0.0f, 0.0f, 5.0f, 150.0f, 15000.0f, 208) }
};

glm::mat4 M_Skybox = glm::mat4(1.0f);
glm::mat4 M_Grass = glm::mat4(1.0f);
glm::mat4 M_Track = glm::mat4(1.0f);



std::map<std::string, glm::mat4> Ms_Bands = {
	{ "x0", glm::mat4(1.0f) },
	{ "x1", glm::mat4(1.0f) },
	{ "z0", glm::mat4(1.0f) },
	{ "z1", glm::mat4(1.0f) }
};

float aspect_ratio = 1;
int camera_control = 1;


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_DOWN) camera_control = -1;
    }
    if (action==GLFW_RELEASE) {
        if (key == GLFW_KEY_DOWN) camera_control = 1;
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspect_ratio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	Player.readTexture("Textures/Formula.png");
	//Ai0.readTexture("Textures/Formula.png");
	Sphere.readTexture("Textures/Sphere.png");
	Grass.readTexture("Textures/Grass.png");
	Track.readTexture("Textures/Track.png");

	for (auto it = Trees.begin(); it != Trees.end(); ++it) {
		it->readTexture("Textures/Grass.png");
	}

	for (auto it = Bands.begin(); it != Bands.end(); ++it) {
		(it->second).readTexture("Textures/Band.png");
		Ms_Bands[it->first] = glm::translate(Ms_Bands[it->first], glm::vec3(it->second.x, it->second.y, it->second.z));
		Ms_Bands[it->first] = glm::scale(Ms_Bands[it->first], glm::vec3(it->second.size_x * 0.5, it->second.size_y, it->second.size_z * 0.5));
	}

	M_Skybox = glm::scale(M_Skybox, glm::vec3(1000.0f, 1000.0f, 1000.0f));
	M_Grass = glm::scale(M_Grass, glm::vec3(25000.0f, 1.0f, 25000.0f));
	M_Track = glm::scale(M_Track, glm::vec3(100.0f, 1.0f, 100.0f));
	M_Track = glm::translate(M_Track, glm::vec3(0.0f, 0.0f, -25.0f));
	M_Track = glm::rotate(M_Track, -PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
	

	mciSendString(TEXT("open \"Sounds/Music.mp3\" type mpegvideo alias mp3"), NULL, 0, NULL);
	mciSendString(TEXT("play mp3 repeat"), NULL, 0, NULL);
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

    delete sp;
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffer

	glm::mat4 V = glm::lookAt(
		glm::vec3(
			Player.x + (450.0f * glm::cos(Player.angular_displacement) * camera_control),
			Player.y + 150.0f,
			Player.z - (450.0f * glm::sin(Player.angular_displacement) * camera_control)
		), // camera position (eye)
		glm::vec3(
			Player.x,
			Player.y + 100.0f,
			Player.z
		), // camera lookat point (center)
		glm::vec3(0.0f, 1.0f, 0.0f) // where is up
	);

	/*std::cout << Car.linear_speed << Car.angular_speed << std::endl;*/
	
    glm::mat4 P = glm::perspective(
		(50.0f*PI) / 180.0f, // FoV
		aspect_ratio, // window (width/height)
		100.0f, // near clipping plane
		500000.0f // far clipping plane
	);

	sp->use();//Aktywacja programu cieniującego

	Player.drawModel(P, V);

	float reflectPow = 0.4;
	
	Sphere.drawModel(P, V, M_Skybox, 1.0, 1.0);
	Grass.drawModel(P, V, M_Grass, reflectPow, reflectPow);
	Track.drawModel(P, V, M_Track, reflectPow, reflectPow);

	for (auto it = Trees.begin(); it != Trees.end(); ++it) {
		it->drawModel(P, V, glm::mat4(1.0f), 0.0, 0.0);
	}
	

	for (auto it = Bands.begin(); it != Bands.end(); ++it) {
		(it->second).drawModel(P, V, Ms_Bands[it->first], reflectPow, reflectPow);
	}

    glfwSwapBuffers(window); // swap back buffer to front
}


int main(void) {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback); // init error callback
	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(500, 500, "3D Racing game", NULL, NULL);  // create window
	if (!window) {
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Can't init GLEW.\n");
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window);
	glfwMaximizeWindow(window);

	glfwSetTime(0); // timer reset
	while (!glfwWindowShouldClose(window)) {
		Player.readInput(window);
		Player.checkCollision(Bands);

		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents(); // Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
