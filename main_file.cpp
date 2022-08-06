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
#include <iostream>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "Model.h"

Model Car("Models/Formula.fbx");
Model LowPolyCar("Models/Formula.fbx");

glm::mat4 V;

float speed_x=0;
float speed_y=0;
float aspectRatio=1;


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
	std::cout << "xxx" << std::endl;
    /*if (action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key == GLFW_KEY_RIGHT) speed_x=PI/2;
		if (key == GLFW_KEY_UP) Car.linear_speed += 1;
		else if (key == GLFW_KEY_DOWN) Car.linear_speed -= 1;
    }*/
    /*if (action==GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) speed_x=0;
        if (key == GLFW_KEY_RIGHT) speed_x=0;
        if (key == GLFW_KEY_UP) Car.linear_speed += 0;
        if (key == GLFW_KEY_DOWN) Car.linear_speed += 0;
    }*/
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	Car.readTexture("Textures/Formula.png");
	LowPolyCar.readTexture("Textures/Car1.png");
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

	V = glm::lookAt(
		glm::vec3(
			Car.x + (750.0f * glm::cos(Car.angular_displacement)),
			Car.y + 250.0f,
			Car.z - (750.0f * glm::sin(Car.angular_displacement))
		), // camera position (eye)
		glm::vec3(Car.x, Car.y, Car.z), // camera lookat point (center)
		glm::vec3(0.0f, 1.0f, 0.0f) // where is up
	);

	/*std::cout << Car.linear_speed << Car.angular_speed << std::endl;*/
	
    glm::mat4 P = glm::perspective(
		(50.0f*PI) / 180.0f, // FoV
		aspectRatio, // window (width/height)
		0.2f, // near clipping plane
		50000.0f // far clipping plane
	);

	LowPolyCar.drawModel(P, V);
	Car.drawModel(P, V);

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void) {
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);// init error callback
	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(500, 500, "3D Racing game", NULL, NULL);  // create window
	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora
	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window); //Operacje inicjujące
	glfwMaximizeWindow(window);


	//Główna pętla
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) { //Tak długo jak okno nie powinno zostać zamknięte
		Car.performMovement(window);
		glfwSetTime(0); //Zeruj timer
		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
