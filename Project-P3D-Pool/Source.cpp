#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW/glfw3.h>

#include "objLoader.h"
#include "shaderLoader.h"


//Variável para controlar o zoom
GLfloat ZOOM = 10.0f;

//Variável para controlar a rotação da bola durante a animação
float currentBallRotation = 0.0f;

//Variaveis para criação de VAO,VBO,EBO
GLuint VAO, VBO, EBO;

//Posições das bolas de bilhar
glm::vec3 BallPositions[] = {
	glm::vec3(-0.5f, 0.1f, 0.2f),
	glm::vec3(-0.4f, 0.1f, 0.1f),
	glm::vec3(-0.2f, 0.1f, -0.3f),
	glm::vec3(-0.1f, 0.1f, 0.4f),
	glm::vec3(-0.8f, 0.1f, 0.3f),
	glm::vec3(-0.7f, 0.1f, -0.1f),
	glm::vec3(-0.6f, 0.1f, 0.35f),
	glm::vec3(0.7f, 0.1f, -0.35f),
	glm::vec3(0.2f, 0.1f, 0.2f),
	glm::vec3(0.1f, 0.1f, 0.0f),
	glm::vec3(0.3f, 0.1f, -0.2f),
	glm::vec3(0.4f, 0.1f, 0.1f),
	glm::vec3(0.5f, 0.1f, -0.15f),
	glm::vec3(0.6f, 0.1f, 0.25f),
	glm::vec3(0.8f, 0.1f, -0.25f)
};

//variaveis para controlar a rotação do objeto
glm::vec2 clickPos;
glm::vec2 prevClickPos;
glm::vec3 rotationAngles(0.0f, 0.0f, 0.0f);

//matrizes de modelação e projeção
glm::mat4 model(1.0f);
glm::mat4 proj(1.0f);


// Função de callback para clique do rato
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Verifica se o botão esquerdo do mouse foi pressionado
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// Obtém as coordenadas do cursor do rato
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		clickPos = glm::vec2(xpos, ypos);
		prevClickPos = clickPos;
	}
	// Verifica se o botão esquerdo do mouse foi libertado
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// Zera os ângulos de rotação
		rotationAngles.x = 0.0f;
		rotationAngles.y = 0.0f;
	}

	// Aplica a rotação no modelo
	model = glm::rotate(model, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
}


//Funcao de callback para lidar com o movimento do rato
void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		prevClickPos = clickPos;
		clickPos = glm::vec2(xpos, ypos);

		// Calcula a diferença entre as posições atuais do clique e as posições anteriores do clique
		glm::vec2 clickDelta = clickPos - prevClickPos;

		// Sensibilidade de rotação (quanto cada pixel de movimento do mouse afeta a rotação)
		const float sensitivity = 0.004f;

		// Atualiza o ângulo de rotação ao longo do eixo Y com base no movimento horizontal do mouse
		rotationAngles.y += clickDelta.x * sensitivity;
	}
}


//Função callback para o zoom do rato
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

	// Se faz zoom in
	if (yoffset == 1) {

		// Incremento no zoom, varia com a distância da câmara
		ZOOM += fabs(ZOOM) * 0.1f;
	}

	// Senão, se faz zoom out
	else if (yoffset == -1) {

		// Incremento no zoom, varia com a distância da câmara
		ZOOM -= fabs(ZOOM) * 0.1f;
	}

}



int main(void) {

	//Inicializar glfw para criar uma janela
	glfwInit();

	//Glfw nao sabe que versao do opengl estamos a usar , temos de dizer isso atraves de hints

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //especificamos que vamos usar a versao do opengl3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);//Major = 3 , Minor = 3 , porque é versao 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//especificar que opengl profile queremos usar , é como se fosse um package de funções , core profile dá-nos as funções modernas


	//Width,height,name of the window , full screnn ou não
	GLFWwindow* window = glfwCreateWindow(800, 800, "PoolTable", NULL, NULL);

	if (window == NULL) {

		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	//Usar a janela
	glfwMakeContextCurrent(window);

	// Inicia o gestor de extensões GLEW
	glewExperimental = GL_TRUE;
	glewInit();


	//chamar as funções callback
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseClickCallback);
	glfwSetCursorPosCallback(window, mouseMovementCallback);

	//Dizer ao Opengl para limpar a cor do buffer e dar-lhe outra
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	//Especificamos que queremos usar o comando no buffer de cor
	glClear(GL_COLOR_BUFFER_BIT);

	//Temos um back buffer com a cor que queremos e um front buffer com a cor default , temos de trocar
	glfwSwapBuffers(window);

	//Dizer que area da janela queremos que o OpenGl faça a renderização
	glViewport(0, 0, 800, 800);


	// Array de informações dos shaders para o programa das bolas
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "triangles.vert" },     // Shader de vértice
		{ GL_FRAGMENT_SHADER, "triangles.frag" },     // Shader de fragmento
		{ GL_NONE, NULL }                            // Marcação de fim do array
	};

	// Carrega os shaders e cria o programa das bolas
	GLuint shaderProgram = LoadShaders(shaders);
	if (!shaderProgram)
		exit(EXIT_FAILURE);

	// Usa o programa das bolas para a renderização
	glUseProgram(shaderProgram);

	
	
	//Habilita o teste de profundidade
	glEnable(GL_DEPTH_TEST);

	//Cria e carrega as bolas

	objLoader::Ball ball1;
	ball1.Read("PoolBalls/Ball1", 1, shaderProgram);
	ball1.Send();

	objLoader::Ball ball2;
	ball2.Read("PoolBalls/Ball2", 2, shaderProgram);
	ball2.Send();

	objLoader::Ball ball3;
	ball3.Read("PoolBalls/Ball3", 3, shaderProgram);
	ball3.Send();

	objLoader::Ball ball4;
	ball4.Read("PoolBalls/Ball4", 4, shaderProgram);
	ball4.Send();

	objLoader::Ball ball5;
	ball5.Read("PoolBalls/Ball5", 5, shaderProgram);
	ball5.Send();

	objLoader::Ball ball6;
	ball6.Read("PoolBalls/Ball6", 6, shaderProgram);
	ball6.Send();

	objLoader::Ball ball7;
	ball7.Read("PoolBalls/Ball7", 7, shaderProgram);
	ball7.Send();

	objLoader::Ball ball8;
	ball8.Read("PoolBalls/Ball8", 8, shaderProgram);
	ball8.Send();

	objLoader::Ball ball9;
	ball9.Read("PoolBalls/Ball9", 9, shaderProgram);
	ball9.Send();

	objLoader::Ball ball10;
	ball10.Read("PoolBalls/Ball10", 10, shaderProgram);
	ball10.Send();

	objLoader::Ball ball11;
	ball11.Read("PoolBalls/Ball11",11, shaderProgram);
	ball11.Send();

	objLoader::Ball ball12;
	ball12.Read("PoolBalls/Ball12", 12, shaderProgram);
	ball12.Send();

	objLoader::Ball ball13;
	ball13.Read("PoolBalls/Ball13", 13, shaderProgram);
	ball13.Send();

	objLoader::Ball ball14;
	ball14.Read("PoolBalls/Ball14", 14, shaderProgram);
	ball14.Send();

	objLoader::Ball ball15;
	ball15.Read("PoolBalls/Ball15", 15, shaderProgram);
	ball15.Send();



	//Matriz projeção
	glm::mat4 projection = glm::mat4(1.0f);

	//Matriz visualização
	glm::mat4 view = glm::mat4(1.0f);

	//Posição da camera
	glm::vec3 position(glm::vec3(0.0f, 10.0f, 20.0f));

	//Target da camera
	glm::vec3 target(glm::vec3(0.0f));

	//Calculos para a camara
	glm::vec3 camFront = position - target;
	glm::vec3 camRight = glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 up = -glm::cross(camFront, camRight);

	//Definicao das matrizes
	projection = glm::perspective(glm::radians(45.0f), (float)800 / 800, 0.1f, 100.0f);
	view = glm::lookAt(position, target, up);


	// Define a escala que você quer aplicar às bolas
	glm::vec3 scale(0.05f, 0.05f, 0.05f); // Reduz o tamanho das bolas para 10% do tamanho original

	while (!glfwWindowShouldClose(window)) {

		// Define a cor de fundo para o framebuffer,
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		// Limpar buffer de cor e profundidade
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Matriz ZOOM
		glm::mat4 matrizZoom = glm::scale(glm::mat4(1.0f), glm::vec3(ZOOM));

		// Bind the vao so OPengl knows to use it
		glBindVertexArray(VAO);

		// Dizer que programa usar (usamos o programa das bolas)
		glUseProgram(shaderProgram);

		// Desenhar as bolas com a escala definida
		ball1.Draw(BallPositions[0], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball2.Draw(BallPositions[1], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball3.Draw(BallPositions[2], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball4.Draw(BallPositions[3], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball5.Draw(BallPositions[4], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball6.Draw(BallPositions[5], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball7.Draw(BallPositions[6], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball8.Draw(BallPositions[7], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball9.Draw(BallPositions[8], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball10.Draw(BallPositions[9], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball11.Draw(BallPositions[10], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball12.Draw(BallPositions[11], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball13.Draw(BallPositions[12], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball14.Draw(BallPositions[13], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball15.Draw(BallPositions[14], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);

		glfwSwapBuffers(window);

		// Dizer ao glfw para procesar todos os eventos como a janela aparecer , mudar de tamanho , input etc , senão a janela fica num estado sem resposta
		glfwPollEvents();
	}


	//Apaga o VAO,VBO,EBO e o programa shader
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	//Destruir a janela
	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}