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

//Funcao para desenhar a mesa
void drawTable(GLuint tableProgram, glm::mat4 matrizZoom, glm::mat4 projection, glm::mat4 view)
{
	// Vincula o VAO (Vertex Array Object) para definir o estado do objeto
	glBindVertexArray(VAO);

	// Usa o programa de shader especificado
	glUseProgram(tableProgram);

	// Aplica a rotação ao modelo
	model = glm::rotate(model, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));

	// Define as matrizes de modelo, visualização e projeção no shader
	int modelLoc = glGetUniformLocation(tableProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(tableProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view * matrizZoom));
	int projLoc = glGetUniformLocation(tableProgram, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Define os parâmetros da luz ambiente
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));

	// Define os parâmetros da luz direcional
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

	// Define os parâmetros da luz pontual #1
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 5.0)));
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
	glProgramUniform1f(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
	glProgramUniform1f(tableProgram, glGetProgramResourceLocation(tableProgram, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

	// Renderiza os elementos usando o VAO atual
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Desvincula o programa de shader
	glUseProgram(0);
}

void loadTable() {

	GLfloat vertices[] = {
		// Frente              //Normal
		-0.9f, -0.05f, 0.45f,   0.0f, 0.0f, 1.0f,
		0.9f, -0.05f, 0.45f,    0.0f, 0.0f, 1.0f,
		0.9f, 0.05f, 0.45f,     0.0f, 0.0f, 1.0f,
		-0.9f, 0.05f, 0.45f,    0.0f, 0.0f, 1.0f,

		// Trás
		-0.9f, -0.05f, -0.45f,   0.0f, 0.0f, -1.0f,
		-0.9f, 0.05f, -0.45f,    0.0f, 0.0f, -1.0f,
		0.9f, 0.05f, -0.45f,     0.0f, 0.0f, -1.0f,
		0.9f, -0.05f, -0.45f,    0.0f, 0.0f, -1.0f,

		// Direita
		0.9f, -0.05f, 0.45f,     1.0f, 0.0f, 0.0f,
		0.9f, -0.05f, -0.45f,    1.0f, 0.0f, 0.0f,
		0.9f, 0.05f, -0.45f,     1.0f, 0.0f, 0.0f,
		0.9f, 0.05f, 0.45f,      1.0f, 0.0f, 0.0f,

		// Esquerda
		-0.9f, -0.05f, 0.45f,    -1.0f, 0.0f, 0.0f,
		-0.9f, 0.05f, 0.45f,     -1.0f, 0.0f, 0.0f,
		-0.9f, 0.05f, -0.45f,    -1.0f, 0.0f, 0.0f,
		-0.9f, -0.05f, -0.45f,   -1.0f, 0.0f, 0.0f,

		// Cima											    
		-0.9f, 0.05f, 0.45f,     0.0f, 1.0f, 0.0f,
		0.9f, 0.05f, 0.45f,      0.0f, 1.0f, 0.0f,
		0.9f, 0.05f, -0.45f,     0.0f, 1.0f, 0.0f,
		-0.9f, 0.05f, -0.45f,    0.0f, 1.0f, 0.0f,

		// Baixo
		-0.9f, -0.05f, 0.45f,   	0.0f, -1.0f, 0.0f,
		-0.9f, -0.05f, -0.45f,  	0.0f, -1.0f, 0.0f,
		0.9f, -0.05f, -0.45f,   	0.0f, -1.0f, 0.0f,
		0.9f, -0.05f, 0.45f ,    	0.0f, -1.0f, 0.0f,
	};

	GLuint indices[] = {
		// Face frontal
		0, 1, 2,
		2, 3, 0,

		// Face traseira
		4, 5, 6,
		6, 7, 4,

		// Face direita
		8, 9, 10,
		10, 11, 8,

		// Face esquerda
		12, 13, 14,
		14, 15, 12,

		// Face superior
		16, 17, 18,
		18, 19, 16,

		// Face inferior
		20, 21, 22,
		22, 23, 20
	};



	// Gera um único objeto de VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Gera um único buffer de vértices (VBO) e um único buffer de elementos (EBO)
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Define o VAO atual, vinculando-o
	glBindVertexArray(VAO);

	// Vincula o VBO como um buffer do tipo GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Armazena os vértices no VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Tipo de buffer, tamanho, dados, uso dos dados

	// Vincula o EBO como um buffer do tipo GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Configura o VAO para o OpenGL saber como ler o VBO
	// Especifica o layout do atributo de vértice 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // O 3 especifica o número de componentes por atributo, cada vértice tem 3 posições (x, y, z)

	// Especifica o layout do atributo de vértice 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // O 3 especifica o número de componentes por atributo, cada vértice tem 3 cores (r, g, b)

	// Habilita o atributo de vértice 0
	glEnableVertexAttribArray(0); // Dá-lhe 0 porque é a posição do atributo de vértice
	// Habilita o atributo de vértice 1
	glEnableVertexAttribArray(1); // Dá-lhe 1 porque é a posição do atributo de vértice

	// Não obrigatório, mas para garantir que não alteramos o VAO e o VBO acidentalmente
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Isso deve ser feito após o VAO, porque o EBO está armazenado no VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	// Array de informações dos shaders para o programa da mesa
	ShaderInfo tableshaders[] = {
		{ GL_VERTEX_SHADER,   "PoolTable.vert" },     // Shader de vértice da mesa
		{ GL_FRAGMENT_SHADER, "PoolTable.frag" },     // Shader de fragmento da mesa
		{ GL_NONE, NULL }                            // Marcação de fim do array
	};

	// Carrega os shaders e cria o programa da mesa
	GLuint tableProgram = LoadShaders(tableshaders);

	//Chama a função para carregar os dados da mesa
	loadTable();

	//Habilita o teste de profundidade
	glEnable(GL_DEPTH_TEST);


	//Cria e carrega as bolas

	objLoader::Ball ball1;
	ball1.Load("PoolBalls/Ball1", 1, shaderProgram);
	ball1.Install();

	objLoader::Ball ball2;
	ball2.Load("PoolBalls/Ball2", 2, shaderProgram);
	ball2.Install();

	objLoader::Ball ball3;
	ball3.Load("PoolBalls/Ball3", 3, shaderProgram);
	ball3.Install();

	objLoader::Ball ball4;
	ball4.Load("PoolBalls/Ball4", 4, shaderProgram);
	ball4.Install();

	objLoader::Ball ball5;
	ball5.Load("PoolBalls/Ball5", 5, shaderProgram);
	ball5.Install();

	objLoader::Ball ball6;
	ball6.Load("PoolBalls/Ball6", 6, shaderProgram);
	ball6.Install();

	objLoader::Ball ball7;
	ball7.Load("PoolBalls/Ball7", 7, shaderProgram);
	ball7.Install();

	objLoader::Ball ball8;
	ball8.Load("PoolBalls/Ball8", 8, shaderProgram);
	ball8.Install();

	objLoader::Ball ball9;
	ball9.Load("PoolBalls/Ball9", 9, shaderProgram);
	ball9.Install();

	objLoader::Ball ball10;
	ball10.Load("PoolBalls/Ball10", 10, shaderProgram);
	ball10.Install();

	objLoader::Ball ball11;
	ball11.Load("PoolBalls/Ball11",11, shaderProgram);
	ball11.Install();

	objLoader::Ball ball12;
	ball12.Load("PoolBalls/Ball12", 12, shaderProgram);
	ball12.Install();

	objLoader::Ball ball13;
	ball13.Load("PoolBalls/Ball13", 13, shaderProgram);
	ball13.Install();

	objLoader::Ball ball14;
	ball14.Load("PoolBalls/Ball14", 14, shaderProgram);
	ball14.Install();

	objLoader::Ball ball15;
	ball15.Load("PoolBalls/Ball15", 15, shaderProgram);
	ball15.Install();



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
		ball1.Render(BallPositions[0], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball2.Render(BallPositions[1], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball3.Render(BallPositions[2], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball4.Render(BallPositions[3], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball5.Render(BallPositions[4], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball6.Render(BallPositions[5], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball7.Render(BallPositions[6], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball8.Render(BallPositions[7], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball9.Render(BallPositions[8], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball10.Render(BallPositions[9], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball11.Render(BallPositions[10], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball12.Render(BallPositions[11], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball13.Render(BallPositions[12], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball14.Render(BallPositions[13], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);
		ball15.Render(BallPositions[14], glm::vec3(0.0f, 0.0f, 0.0f), view * matrizZoom, projection, model, scale);

		//Desenhar a mesa
		drawTable(tableProgram, matrizZoom, projection, view);

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