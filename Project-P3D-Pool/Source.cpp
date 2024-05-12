#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>

#include <Windows.h>
#include <gl\GL.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include "objLoader.h"

void init(void);

#define WIDTH 800
#define HEIGHT 600

GLfloat ZOOM = 10.0f;
GLfloat ANGLE = 0.0f;
double lastX = WIDTH / 2.0;
double lastY = HEIGHT / 2.0;
bool firstMouse = true;

// Variáveis para armazenar o ângulo de rotação
float yaw = -90.0f; // yaw é inicializado para -90 graus para que a primeira pessoa olhe para o centro
float pitch = 0.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

// Função de callback para o movimento do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	// Se faz zoom in
	if (yoffset == 1) {
		// Incremento no zoom, varia com a dist�ncia da c�mara
		ZOOM += fabs(ZOOM) * 0.1f;
	}
	// Sen�o, se faz zoom out
	else if (yoffset == -1) {
		// Incremento no zoom, varia com a dist�ncia da c�mara
		ZOOM -= fabs(ZOOM) * 0.1f;
	}
	std::cout << "ZOOM = " << ZOOM << std::endl;
}

std::vector<glm::vec3> Load3DModel(void) {
	// 6 faces * 4 v�rtices por face
	float length = 7.0f; // Comprimento do paralelepípedo
	float width = 4.0f;  // Largura do paralelepípedo
	float height = 1.0f; // Altura do paralelepípedo


	glm::vec3 point[6 * 4] = {
		// Frente
		glm::vec3(-length / 2.0f, -width / 2.0f,  height / 2.0f),
		glm::vec3(length / 2.0f, -width / 2.0f,  height / 2.0f),
		glm::vec3(length / 2.0f,  width / 2.0f,  height / 2.0f),
		glm::vec3(-length / 2.0f,  width / 2.0f,  height / 2.0f),
		// Trás
		glm::vec3(-length / 2.0f, -width / 2.0f, -height / 2.0f),
		glm::vec3(-length / 2.0f,  width / 2.0f, -height / 2.0f),
		glm::vec3(length / 2.0f,  width / 2.0f, -height / 2.0f),
		glm::vec3(length / 2.0f, -width / 2.0f, -height / 2.0f),
		// Direita
		glm::vec3(length / 2.0f, -width / 2.0f,  height / 2.0f),
		glm::vec3(length / 2.0f, -width / 2.0f, -height / 2.0f),
		glm::vec3(length / 2.0f,  width / 2.0f, -height / 2.0f),
		glm::vec3(length / 2.0f,  width / 2.0f,  height / 2.0f),
		// Esquerda
		glm::vec3(-length / 2.0f, -width / 2.0f,  height / 2.0f),
		glm::vec3(-length / 2.0f,  width / 2.0f,  height / 2.0f),
		glm::vec3(-length / 2.0f,  width / 2.0f, -height / 2.0f),
		glm::vec3(-length / 2.0f, -width / 2.0f, -height / 2.0f),
		// Cima
		glm::vec3(-length / 2.0f,  width / 2.0f,  height / 2.0f),
		glm::vec3(length / 2.0f,  width / 2.0f,  height / 2.0f),
		glm::vec3(length / 2.0f,  width / 2.0f, -height / 2.0f),
		glm::vec3(-length / 2.0f,  width / 2.0f, -height / 2.0f),
		// Baixo
		glm::vec3(-length / 2.0f, -width / 2.0f,  height / 2.0f),
		glm::vec3(-length / 2.0f, -width / 2.0f, -height / 2.0f),
		glm::vec3(length / 2.0f, -width / 2.0f, -height / 2.0f),
		glm::vec3(length / 2.0f, -width / 2.0f,  height / 2.0f)
	};

	std::vector<glm::vec3> ret;
	for (auto i : point)
		ret.push_back(i);

	return ret;
}

void display(std::vector<glm::vec3> obj, glm::mat4 mvp) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float* vertex_stream = static_cast<float*>(glm::value_ptr(obj.front()));

	std::vector<glm::vec3> colors{
		glm::vec3(1.0f, 0.0f, 0.0f), // Red
		glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
		glm::vec3(0.0f, 1.0f, 0.0f), // Green
		glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
		glm::vec3(0.0f, 0.0f, 1.0f), // Blue
		glm::vec3(1.0f, 0.0f, 1.0f)  // Magenta
	};

	// Desenha quad em modo imediato
	glBegin(GL_QUADS);
	/* obj.size() * (obj.front().length()) � o mesmo que (6*4)*3 */
	/* 6 faces * 4 v�rtices por face * 3 coordenadas por v�rtice */
	for (int nv = 0; nv < 6 * 4 * 3; nv += 3) {
		// Uma cor por face
		glColor3f(colors[nv / (4 * 3)].r, colors[nv / (4 * 3)].g, colors[nv / (4 * 3)].b);
		glm::vec4 vertex = glm::vec4(vertex_stream[nv], vertex_stream[nv + 1], vertex_stream[nv + 2], 1.0f);
		// C�lculo das coordenadas de recorte
		glm::vec4 transformed_vertex = mvp * vertex;
		// Divis�o de Perspetiva
		glm::vec4 normalized_vertex = transformed_vertex / transformed_vertex.w;
		// Desenho do v�rtice
		glVertex3f(normalized_vertex.x, normalized_vertex.y, normalized_vertex.z);
	}
	glEnd();
}

int main(void) {
	std::vector<glm::vec3> obj = Load3DModel();

	GLFWwindow* window;

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "P3D - Trabalho Pratico 1 (Part #1)", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}






	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	init();

	glfwSetScrollCallback(window, scrollCallback);

	glfwSetCursorPosCallback(window, mouse_callback);

	// Projection
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	while (!glfwWindowShouldClose(window)) {
		// View
		glm::vec3 cameraPos;
		cameraPos.x = ZOOM * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraPos.y = ZOOM * sin(glm::radians(pitch));
		cameraPos.z = ZOOM * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		// View
		//  // Vector vertical
		glm::vec3 up = glm::vec3(0.0f, cos(glm::radians(pitch)) > 0 ? 1.0f : -1.0f, 0.0f);

		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), up);


		// Model
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = projection * view * model;


		display(obj, mvp);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
}