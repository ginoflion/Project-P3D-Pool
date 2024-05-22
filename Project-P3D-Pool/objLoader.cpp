
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


#define GLEW_STATIC
#include <GL\glew.h>

#include <gl\GL.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include "objLoader.h"

std::vector<Vertex> vertices;
std::vector<TextureCoord> texCoords;
std::vector<Normal> normals;
std::vector<Face> faces;
std::string mtlFilename;

GLuint VAO, VBOvertices, VBOtexCoords, VBOnormals,VBOfaces;

void loadVertexGPU() {
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &VBOvertices);
    glBindBuffer(GL_ARRAY_BUFFER, VBOvertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3, GL_FLOAT,GL_FALSE, 3* sizeof(float), (void*)0);

    glGenBuffers(1, &VBOtexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, VBOtexCoords);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec3), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glGenBuffers(1, &VBOnormals);
    glBindBuffer(GL_ARRAY_BUFFER, VBOnormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &VBOfaces);
    glBindBuffer(GL_ARRAY_BUFFER, VBOfaces);
    glBufferData(GL_ARRAY_BUFFER, faces.size() * sizeof(glm::vec3), faces.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glBindVertexArray(0);
}

void loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Cannot open MTL file: " << filename << std::endl;
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
            loadMTL(mtlFilename);
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

void loadMTL(const std::string& filename) {
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
            loadTexture(textureFilename);
        }
    }
}

GLuint textureID; // Variável global para armazenar o ID da textura

void loadTexture(const std::string& filename) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
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


