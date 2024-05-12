#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <Windows.h>
#include <gl\GL.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include "stb_image.h"

#include "objLoader.h"

std::vector<Vertex> vertices;
std::vector<TextureCoord> texCoords;
std::vector<Normal> normals;
std::vector<Face> faces;

void loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
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
    }
}
