#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <Windows.h>

#include "stb_image.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <gl/GL.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <glm/ext.hpp>

#include "objLoader.h"

namespace objLoader {
    std::string mtlFilename;

    GLuint VAO, VBOvertices, VBOtexCoords, VBOnormals;

    void Ball::Load(const std::string& filename, GLuint textureIndex, GLuint shaderprogram) {
        this->ShaderProgram = shaderprogram;
        this->textureIndex = textureIndex;

        std::ifstream file(filename + ".obj");
        if (!file) {
            std::cerr << "Cannot open OBJ file: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string line;
        std::vector<Vertex> temp_vertices;
        std::vector<TextureCoord> temp_texCoords;
        std::vector<Normal> temp_normals;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if (prefix == "v") {
                Vertex vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);
            }
            else if (prefix == "vt") {
                TextureCoord texCoord;
                iss >> texCoord.u >> texCoord.v;
                temp_texCoords.push_back(texCoord);
            }
            else if (prefix == "vn") {
                Normal normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (prefix == "f") {
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                char slash;

                for (int i = 0; i < 3; ++i) {
                    iss >> vertexIndex[i] >> slash >> uvIndex[i] >> slash >> normalIndex[i];
                    vertices.push_back(temp_vertices.at(vertexIndex[i] - 1));
                    texCoords.push_back(temp_texCoords.at(uvIndex[i] - 1));
                    normals.push_back(temp_normals.at(normalIndex[i] - 1));
                }
            }
            else if (prefix == "mtllib") {
                std::string mtlFilename;
                iss >> mtlFilename;
                ReadMTL("PoolBalls/" + mtlFilename);
            }
        }
    }

    void Ball::Install() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBOvertices);
        glBindBuffer(GL_ARRAY_BUFFER, VBOvertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &VBOtexCoords);
        glBindBuffer(GL_ARRAY_BUFFER, VBOtexCoords);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(TextureCoord), texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // Certifique-se de que location = 2
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &VBOnormals);
        glBindBuffer(GL_ARRAY_BUFFER, VBOnormals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Normal), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void Ball::ReadMTL(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        if (!file) {
            std::cerr << "Cannot open MTL file: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "Ka") {
                glm::vec3 ambientColor;
                iss >> ambientColor.r >> ambientColor.g >> ambientColor.b;
            }
            else if (prefix == "Kd") {
                glm::vec3 diffuseColor;
                iss >> diffuseColor.r >> diffuseColor.g >> diffuseColor.b;
            }
            else if (prefix == "Ks") {
                glm::vec3 specularColor;
                iss >> specularColor.r >> specularColor.g >> specularColor.b;
            }
            else if (prefix == "Ns") {
                float shininess;
                iss >> shininess;
            }
            else if (prefix == "map_Kd") {
                std::string textureFilename;
                iss >> textureFilename;
                LoadTexture("PoolBalls/" + textureFilename);
            }
        }
    }

    void Ball::LoadTexture(const std::string& filename) {
        glGenTextures(1, &textureIndex);
        glBindTexture(GL_TEXTURE_2D, textureIndex);
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        unsigned char* data = stbi_load((filename).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        // Configurações de filtro e wrap da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void Ball::Render(glm::vec3 position, glm::vec3 orientation, glm::mat4 view, glm::mat4 projection, glm::mat4 model, glm::vec3 scale) {
        glBindVertexArray(VAO);

        glm::mat4 Model = model;
        Model = glm::translate(Model, position);
        Model = glm::rotate(Model, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        Model = glm::scale(Model, scale); // Aplica a transformação de escala

        GLint viewId = glGetUniformLocation(ShaderProgram, "View");
        glUniformMatrix4fv(viewId, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionId = glGetUniformLocation(ShaderProgram, "Projection");
        glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection));

        GLint modelId = glGetUniformLocation(ShaderProgram, "Model");
        glUniformMatrix4fv(modelId, 1, GL_FALSE, glm::value_ptr(Model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIndex);
        glUniform1i(glGetUniformLocation(ShaderProgram, "TexSampler"), 0);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glBindVertexArray(0);
    }

}