#version 440 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 2 ) in vec2 vTextureCoords;

out vec2 textureCoord;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
    
    gl_Position = Projection * View * Model * vec4(vPosition, 1.0f);
    textureCoord = vTextureCoords;
}