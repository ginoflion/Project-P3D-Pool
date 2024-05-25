#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView; // View * Model
uniform sampler2D textSampler;

// Estrutura da fonte de luz ambiente global
struct AmbientLight {
    vec3 ambient; // Componente de luz ambiente global
};

uniform AmbientLight ambientLight; // Fonte de luz ambiente global

// Estrutura de uma fonte de luz direcional
struct DirectionalLight {
    vec3 direction; // Direção da luz, espaço do mundo
    vec3 ambient;   // Componente de luz ambiente
    vec3 diffuse;   // Componente de luz difusa
    vec3 specular;  // Componente de luz especular
};

uniform DirectionalLight directionalLight; // Fonte de luz direcional

// Estrutura de uma fonte de luz pontual
struct PointLight {
    vec3 position;  // Posição do ponto de luz, espaço do mundo
    vec3 ambient;   // Componente de luz ambiente
    vec3 diffuse;   // Componente de luz difusa
    vec3 specular;  // Componente de luz especular
    float constant; // Coeficiente de atenuação constante
    float linear;   // Coeficiente de atenuação linear
    float quadratic;// Coeficiente de atenuação quadrática
};

uniform PointLight pointLight[2]; // Duas fontes de luz pontual

// Estrutura de uma fonte de luz cônica
struct SpotLight {
    vec3 position;  // Posição do foco de luz, espaço do mundo
    vec3 ambient;   // Componente de luz ambiente
    vec3 diffuse;   // Componente de luz difusa
    vec3 specular;  // Componente de luz especular
    float constant; // Coeficiente de atenuação constante
    float linear;   // Coeficiente de atenuação linear
    float quadratic;// Coeficiente de atenuação quadrática
    float spotCutoff, spotExponent;
    vec3 spotDirection;
};

uniform SpotLight spotLight; // Fonte de luz cônica

struct Material {
    vec3 emissive;
    vec3 ambient;  // Ka
    vec3 diffuse;  // Kd
    vec3 specular; // Ks
    float shininess;
};

uniform Material material;

in vec3 vPositionEyeSpace;
in vec3 vNormalEyeSpace;
in vec2 textureVector;

layout (location = 0) out vec4 fColor; // Cor final do fragmento

vec4 calcAmbientLight(AmbientLight light);
vec4 calcDirectionalLight(DirectionalLight light);
vec4 calcPointLight(PointLight light);
//vec4 calcSpotLight(SpotLight light);

void main() {
    // Cálculo da componente emissiva do material.
    vec4 emissive = vec4(material.emissive, 1.0);

    // Cálculo da cor final do fragmento
    vec4 ambient = calcAmbientLight(ambientLight);
    vec4 directional = calcDirectionalLight(directionalLight);
    vec4 point = vec4(0.0);

    for (int i = 0; i < 2; ++i) {
        point += calcPointLight(pointLight[i]);
    }

    fColor = (ambient + directional + point + emissive) * texture(textSampler, textureVector);
}

vec4 calcAmbientLight(AmbientLight light) {
    // Cálculo da contribuição da fonte de luz ambiente global, para a cor do objeto.
    return vec4(material.ambient * light.ambient, 1.0);
}

vec4 calcDirectionalLight(DirectionalLight light) {
    // Cálculo da reflexão da componente da luz ambiente.
    vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

    // Cálculo da reflexão da componente da luz difusa.
    vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
    vec3 L = normalize(-lightDirectionEyeSpace); // Direção inversa à da direção luz.
    vec3 N = normalize(vNormalEyeSpace);
    float NdotL = max(dot(N, L), 0.0);
    vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

    // Cálculo da reflexão da componente da luz especular.
    vec3 V = normalize(-vPositionEyeSpace);
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

    // Cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
    return (ambient + diffuse + specular);
}

vec4 calcPointLight(PointLight light) {
    // Cálculo da reflexão da componente da luz ambiente.
    vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

    // Cálculo da reflexão da componente da luz difusa.
    vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
    vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
    vec3 N = normalize(vNormalEyeSpace);
    float NdotL = max(dot(N, L), 0.0);
    vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

    // Cálculo da reflexão da componente da luz especular.
    vec3 V = normalize(-vPositionEyeSpace);
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

    // Atenuação
    float dist = length(lightPositionEyeSpace - vPositionEyeSpace); // Cálculo da distância entre o ponto de luz e o vértice
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // Cálculo da contribuição da fonte de luz pontual para a cor final do fragmento.
    return attenuation * (ambient + diffuse + specular);
}

//vec4 calcSpotLight(SpotLight light) {} // Função não utilizada, pode ser implementada se necessário
