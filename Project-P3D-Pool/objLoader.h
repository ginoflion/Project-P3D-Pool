#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <Windows.h>
#include <gl/GL.h>
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

namespace objLoader {
    class Ball {
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


        
    public:

        std::vector<Vertex> vertices;
        std::vector<TextureCoord> texCoords;
        std::vector<Normal> normals;
        std::vector<Face> faces;

        void loadOBJ(const std::string& filename, GLuint textureIndex, GLuint shaderprogram);
        void loadVertexGPU();
        void loadMTL(const std::string& filename);
        void loadTexture(const std::string& filename);
        void Draw(glm::vec3 position, glm::vec3 orientation, glm::mat4 view, glm::mat4 projection, glm::mat4 model);

    };
}

