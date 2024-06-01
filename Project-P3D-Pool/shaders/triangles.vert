#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;
out vec2 textureCoord;

void main()
{ 
	// Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(aPos, 1.0)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * aNormal);

	// Coordenada de textura para o CubeMap
	textureCoord = aTexCoord;

	// Posição final do vértice (em coordenadas de clip)
	gl_Position = Projection * ModelView * vec4(aPos, 1.0f);
}
