#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <Windows.h>
#include <gl\GL.h>
#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include "stb_image.h"

struct Vertex {
    float x, y, z;
};

struct TextureCoord {
    float u, v;
};

struct Normal {
    float x, y, z;
};

struct Face {
    int vertexIndex[3];
    int textureIndex[3];
    int normalIndex[3];
};


void loadOBJ(const std::string& filename);