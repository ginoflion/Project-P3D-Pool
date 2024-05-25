
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

    GLuint VAO, VBOvertices, VBOtexCoords, VBOnormals, VBOfaces;

    

    void Ball:: Read(const std::string& filename, GLuint textureIndex, GLuint shaderprogram) {
        this->ShaderProgram = shaderprogram;
        this->textureIndex = textureIndex;


        std::ifstream file(filename + ".obj");
        if (!file) {
            std::cerr << "Cannot open OBJ file: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if (prefix == "v") {
                Vertex vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                vertices.push_back(vertex);
            }
            else if (prefix == "vt") {
                TextureCoord texCoord;
                iss >> texCoord.u >> texCoord.v;
                texCoords.push_back(texCoord);
            }
            else if (prefix == "vn") {
                Normal normal;
                iss >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            }
            else if (prefix == "f") {
                Face face;
                for (int i = 0; i < 3; ++i) {
                    std::string vertexData;
                    iss >> vertexData;
                    std::istringstream viss(vertexData);
                    viss >> face.vertexIndex[i];
                    if (viss.peek() == '/') {
                        viss.ignore(); // skip '/'
                        if (viss.peek() != '/') {
                            viss >> face.textureIndex[i];
                        }
                        if (viss.peek() == '/') {
                            viss.ignore(); // skip '/'
                            viss >> face.normalIndex[i];
                        }
                    }
                }
                faces.push_back(face);
            }
            else if (prefix == "mtllib") {
                iss >> mtlFilename;
                ReadMTL("PoolBalls/" + mtlFilename);
            }
        }
        std::vector<Vertex> orderedVertices;
        for (const auto& face : faces) {
            for (int i = 0; i < 3; ++i) {
                int vertexIndex = face.vertexIndex[i] - 1; // Os índices em .obj começam de 1
                orderedVertices.push_back(vertices[vertexIndex]);
            }
        }


    }

    void Ball::Send() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBOvertices);
    glBindBuffer(GL_ARRAY_BUFFER, VBOvertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBOtexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBOtexCoords);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW); // Change glm::vec3 to glm::vec2
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // Change 3 to 2
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &VBOnormals);
    glBindBuffer(GL_ARRAY_BUFFER, VBOnormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &VBOfaces);
    glBindBuffer(GL_ARRAY_BUFFER, VBOfaces);
    glBufferData(GL_ARRAY_BUFFER, faces.size() * sizeof(glm::vec3), faces.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);

    GLint textura = glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "textSampler");
    glProgramUniform1i(ShaderProgram, textura, 0);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << std::endl;
    }

    glBindVertexArray(0);
}


    void Ball:: ReadMTL(const std::string& filename) {
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
                LoadTexture(textureFilename);
            }
        }
    }


    void Ball:: LoadTexture(const std::string& filename) {
        glGenTextures(1, &textureIndex);
        glBindTexture(GL_TEXTURE_2D, textureIndex);
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        std::cout << filename;
        unsigned char* data = stbi_load(("PoolBalls/"+filename).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        // Configurações de filtro e wrap da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }


    void Ball::Draw(glm::vec3 position, glm::vec3 orientation, glm::mat4 view, glm::mat4 projection, glm::mat4 model) {

        //Vincula o VAO
        glBindVertexArray(VAO);

        glm::mat4 Model = model;

        //Posicao da Bola
        Model = glm::translate(Model, position);

        //Orientação da bola
        Model = glm::rotate(Model, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));


        GLint viewId = glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "View");
        glProgramUniformMatrix4fv(ShaderProgram, viewId, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionId = glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "Projection");
        glProgramUniformMatrix4fv(ShaderProgram, projectionId, 1, GL_FALSE, glm::value_ptr(projection));

        GLint modelId = glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "Model");
        glProgramUniformMatrix4fv(ShaderProgram, modelId, 1, GL_FALSE, glm::value_ptr(Model));

        glm::mat4 modelView = view * Model;

        GLint modelViewId = glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "ModelView");
        glProgramUniformMatrix4fv(ShaderProgram, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelView));

        GLint normalMatrixId = glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "NormalMatrix");
        glProgramUniformMatrix4fv(ShaderProgram, normalMatrixId, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "OpenGL Error: " << error << std::endl;
        }

        glProgramUniform3fv(ShaderProgram, glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "material.emissive"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
        glProgramUniform3fv(ShaderProgram, glGetProgramResourceLocation(ShaderProgram, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(1.5, 1.5, 1.5)));

        glBindTexture(GL_TEXTURE_2D, textureIndex);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }


  
}


