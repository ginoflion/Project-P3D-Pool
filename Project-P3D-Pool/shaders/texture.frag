#version 440 core

layout (location = 0) in vec3 color;			// Cor do vértice
layout (location = 1) in vec2 textureCoord;		// Coordenada de textura do vértice

uniform sampler2D TexSampler;					// Sampler de textura

layout (location = 0) out vec4 fColor;			// Cor final do fragmento

void main()
{
	fColor = texture(TexSampler, textureCoord);
}