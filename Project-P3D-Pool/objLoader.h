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
    class Object {
        struct Vertex {
            float x, y, z;
        };

        struct TextureCoord {
            float u, v;
        };

        struct Normal {
            float x, y, z;
        };

    private: 

        GLuint ShaderProgram;
        GLuint textureIndex;
        GLuint VAO, VBOvertices, VBOtexCoords, VBOnormals;
        
    public:

        std::vector<Vertex> vertices;
        std::vector<TextureCoord> texCoords;
        std::vector<Normal> normals;

        void Load(const std::string& filename, GLuint textureIndex, GLuint shaderprogram);
        void Install();
        void ReadMTL(const std::string& filename);
        void LoadTexture(const std::string& filename);
        void Render(glm::vec3 position, glm::vec3 orientation, glm::mat4 view, glm::mat4 projection, glm::mat4 model, glm::vec3 scale);
    };
}

