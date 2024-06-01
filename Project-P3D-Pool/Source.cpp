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

GLuint VAO, VBO;

//Variável para controlar o zoom
GLfloat ZOOM = 5.0f;

glm::vec3 BallPositions[] = {
    glm::vec3(-0.5f, 0.15f, 0.2f),
    glm::vec3(-0.4f, 0.15f, 0.1f),
    glm::vec3(-0.2f, 0.15f, -0.3f),
    glm::vec3(-0.1f, 0.15f, 0.4f),
    glm::vec3(-0.8f, 0.15f, 0.3f),
    glm::vec3(-0.7f, 0.15f, -0.1f),
    glm::vec3(-0.6f, 0.15f, 0.35f),
    glm::vec3(0.7f, 0.15f, -0.35f),
    glm::vec3(0.2f, 0.15f, 0.2f),
    glm::vec3(0.1f, 0.15f, 0.0f),
    glm::vec3(0.3f, 0.15f, -0.2f),
    glm::vec3(0.4f, 0.15f, 0.1f),
    glm::vec3(0.5f, 0.15f, -0.15f),
    glm::vec3(0.6f, 0.15f, 0.25f),
    glm::vec3(0.8f, 0.15f, -0.25f)
};



//variaveis para controlar a rotação do objeto
glm::vec2 clickPos;
glm::vec2 prevClickPos;
glm::vec3 rotationAngles(0.0f, 0.0f, 0.0f);

//matrizes de modelação e projeção
glm::mat4 model(1.0f);
glm::mat4 proj(1.0f);


bool isMousePressed = false;

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //especificar que opengl profile queremos usar , é como se fosse um package de funções , core profile dá-nos as funções modernas

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
        { GL_VERTEX_SHADER,   "shaders/triangles.vert" },
        { GL_FRAGMENT_SHADER, "shaders/triangles.frag" },
        { GL_NONE, NULL }
    };

    // Carrega os shaders e cria o programa das bolas
    GLuint shaderProgram = LoadShaders(shaders);
    if (!shaderProgram) exit(EXIT_FAILURE);

    // Usa o programa das bolas para a renderização
    glUseProgram(shaderProgram);


    //Habilita o teste de profundidade
    glEnable(GL_DEPTH_TEST);

    //Cria e carrega as bolas
    objLoader::Object ball1;
    ball1.SetShader(1, shaderProgram);
    ball1.Load("PoolBalls/Ball1");
    ball1.Install();
    objLoader::Object ball2;
    ball2.SetShader(2, shaderProgram);
    ball2.Load("PoolBalls/Ball2");
    ball2.Install();
    objLoader::Object ball3;
    ball3.SetShader(3, shaderProgram);
    ball3.Load("PoolBalls/Ball3");
    ball3.Install();
    objLoader::Object ball4;
    ball4.SetShader(4, shaderProgram);
    ball4.Load("PoolBalls/Ball4");
    ball4.Install();
    objLoader::Object ball5;
    ball5.SetShader(5, shaderProgram);
    ball5.Load("PoolBalls/Ball5");
    ball5.Install();
    objLoader::Object ball6;
    ball6.SetShader(6, shaderProgram);
    ball6.Load("PoolBalls/Ball6");
    ball6.Install();
    objLoader::Object ball7;
    ball7.SetShader(7, shaderProgram);
    ball7.Load("PoolBalls/Ball7");
    ball7.Install();
    objLoader::Object ball8;
    ball8.SetShader(8, shaderProgram);
    ball8.Load("PoolBalls/Ball8");
    ball8.Install();
    objLoader::Object ball9;
    ball9.SetShader(9, shaderProgram);
    ball9.Load("PoolBalls/Ball9");
    ball9.Install();
    objLoader::Object ball10;
    ball10.SetShader(10, shaderProgram);
    ball10.Load("PoolBalls/Ball10");
    ball10.Install();
    objLoader::Object ball11;
    ball11.SetShader(11, shaderProgram);
    ball11.Load("PoolBalls/Ball11");
    ball11.Install();
    objLoader::Object ball12;
    ball12.SetShader(12, shaderProgram);
    ball12.Load("PoolBalls/Ball12");
    ball12.Install();
    objLoader::Object ball13;
    ball13.SetShader(13, shaderProgram);
    ball13.Load("PoolBalls/Ball13");
    ball13.Install();
    objLoader::Object ball14;
    ball14.SetShader(14, shaderProgram);
    ball14.Load("PoolBalls/Ball14");
    ball14.Install();
    objLoader::Object ball15;
    ball15.SetShader(15, shaderProgram);
    ball15.Load("PoolBalls/Ball15");
    ball15.Install();
    objLoader::Object table;
    table.SetShader(16, shaderProgram);
    table.Load("PoolBalls/Table");
    table.Install();



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



    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(5.0, 5.0, 5.0)));

    // Fonte de luz direcional
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0.0, 1.0, 1.0)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

    // Fonte de luz pontual #1
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 5.0)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    glProgramUniform1f(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
    glProgramUniform1f(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
    glProgramUniform1f(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

    // Fonte de luz pontual #2
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(-2.0, 2.0, 5.0)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    glProgramUniform3fv(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
    glProgramUniform1f(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].constant"), 1.0f);
    glProgramUniform1f(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].linear"), 0.06f);
    glProgramUniform1f(shaderProgram, glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "pointLight[1].quadratic"), 0.02f);



    while (!glfwWindowShouldClose(window)) {
        // Define a cor de fundo para o framebuffer
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Limpar buffer de cor e profundidade
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Matriz de zoom
        glm::mat4 matrizZoom = glm::scale(glm::mat4(1.0f), glm::vec3(ZOOM));




        //Posição da camera
        glm::vec3 position(0.0f, 10.0f, 20.0f);

        //Definicao das matrizes
        view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));

        // Dizer que VAO usar
        glBindVertexArray(VAO);

        // Dizer que programa usar (usamos o programa das bolas)
        glUseProgram(shaderProgram);

        // Multiplicar a matriz view pela matriz de zoom
        view = view * matrizZoom;

        // Definir a escala das bolas
        glm::vec3 scale = glm::vec3(0.05f, 0.05f, 0.05f);
        // Definir a escala da mesa
        glm::vec3 tabbleScale(0.3f, 0.15f, 0.15f); // Reduz o tamanho da mesa

        model = glm::rotate(model, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
        // Definir as matrizes de model, view e projection para a mesa
        table.SetMatrices(view, projection, model, tabbleScale);


        //Definir as matrizes de model, view e projection para as bolas
        ball1.SetMatrices(view, projection, model, scale);
        ball2.SetMatrices(view, projection, model, scale);
        ball3.SetMatrices(view, projection, model, scale);
        ball4.SetMatrices(view, projection, model, scale);
        ball5.SetMatrices(view, projection, model, scale);
        ball6.SetMatrices(view, projection, model, scale);
        ball7.SetMatrices(view, projection, model, scale);
        ball8.SetMatrices(view, projection, model, scale);
        ball9.SetMatrices(view, projection, model, scale);
        ball10.SetMatrices(view, projection, model, scale);
        ball11.SetMatrices(view, projection, model, scale);
        ball12.SetMatrices(view, projection, model, scale);
        ball13.SetMatrices(view, projection, model, scale);
        ball14.SetMatrices(view, projection, model, scale);
        ball15.SetMatrices(view, projection, model, scale);

        // Desenhar as bolas com a escala definida
        ball1.Render(BallPositions[0], glm::vec3(0.0f, 0.0f, 0.0f));
        ball2.Render(BallPositions[1], glm::vec3(0.0f, 0.0f, 0.0f));
        ball3.Render(BallPositions[2], glm::vec3(0.0f, 0.0f, 0.0f));
        ball4.Render(BallPositions[3], glm::vec3(0.0f, 0.0f, 0.0f));
        ball5.Render(BallPositions[4], glm::vec3(0.0f, 0.0f, 0.0f));
        ball6.Render(BallPositions[5], glm::vec3(0.0f, 0.0f, 0.0f));
        ball7.Render(BallPositions[6], glm::vec3(0.0f, 0.0f, 0.0f));
        ball8.Render(BallPositions[7], glm::vec3(0.0f, 0.0f, 0.0f));
        ball9.Render(BallPositions[8], glm::vec3(0.0f, 0.0f, 0.0f));
        ball10.Render(BallPositions[9], glm::vec3(0.0f, 0.0f, 0.0f));
        ball11.Render(BallPositions[10], glm::vec3(0.0f, 0.0f, 0.0f));
        ball12.Render(BallPositions[11], glm::vec3(0.0f, 0.0f, 0.0f));
        ball13.Render(BallPositions[12], glm::vec3(0.0f, 0.0f, 0.0f));
        ball14.Render(BallPositions[13], glm::vec3(0.0f, 0.0f, 0.0f));
        ball15.Render(BallPositions[14], glm::vec3(0.0f, 0.0f, 0.0f));

        table.Render(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        glfwSwapBuffers(window);
        // Dizer ao glfw para procesar todos os eventos como a janela aparecer , mudar de tamanho , input etc , senão a janela fica num estado sem resposta
        glfwPollEvents();
    }

    //Apaga o VAO,VBO,EBO e o programa shader
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    //Destruir a janela
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
