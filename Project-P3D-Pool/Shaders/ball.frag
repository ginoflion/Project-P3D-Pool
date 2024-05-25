#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;  // View * Model
uniform sampler2D textSampler;

// Estrutura da fonte de luz ambiente global
struct AmbientLight {
    vec3 ambient;  // Componente de luz ambiente global
};

uniform AmbientLight ambientLight; // Fonte de luz ambiente global

// Estrutura de uma fonte de luz direcional
struct DirectionalLight {
    vec3 direction;  // Dire��o da luz, espa�o do mundo

    vec3 ambient;    // Componente de luz ambiente
    vec3 diffuse;    // Componente de luz difusa
    vec3 specular;   // Componente de luz especular
};

uniform DirectionalLight directionalLight; // Fonte de luz direcional

// Estrutura de uma fonte de luz pontual
struct PointLight {
    vec3 position;  // Posi��o do ponto de luz, espa�o do mundo

    vec3 ambient;   // Componente de luz ambiente
    vec3 diffuse;   // Componente de luz difusa
    vec3 specular;  // Componente de luz especular

    float constant;  // Coeficiente de atenua��o constante
    float linear;    // Coeficiente de atenua��o linear
    float quadratic; // Coeficiente de atenua��o quadr�tica
};

uniform PointLight pointLight[2]; // Duas fontes de luz pontual

// Estrutura de uma fonte de luz c�nica
struct SpotLight {
    vec3 position;  // Posi��o do foco de luz, espa�o do mundo

    vec3 ambient;   // Componente de luz ambiente
    vec3 diffuse;   // Componente de luz difusa
    vec3 specular;  // Componente de luz especular

    float constant; // Coeficiente de atenua��o constante
    float linear;   // Coeficiente de atenua��o linear
    float quadratic; // Coeficiente de atenua��o quadr�tica

    float spotCutoff, spotExponent;
    vec3 spotDirection;
};

uniform SpotLight spotLight; // Fonte de luz c�nica

uniform bool ambientLightEnabled;
uniform bool directionalLightEnabled;
uniform bool pointLightEnabled[2]; // Duas fontes de luz pontual
uniform bool spotLightEnabled;

struct Material {
    vec3 emissive;
    vec3 ambient;   // Ka
    vec3 diffuse;   // kd
    vec3 specular;  // ke
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
vec4 calcSpotLight(SpotLight light);

void main()
{
    // C�lculo da componente emissiva do material.
    vec4 emissive = vec4(material.emissive, 1.0);

    // Com cor de fragmento
    fColor = emissive;

    // Se a luz ambiente estiver ativada, adiciona sua contribui��o
    if (ambientLightEnabled) {
        fColor += calcAmbientLight(ambientLight);
    }

    // Adiciona a contribui��o da luz direcional, se estiver ativada
    if (directionalLightEnabled) {
        fColor += calcDirectionalLight(directionalLight);
    }

    // Adiciona a contribui��o das luzes pontuais, se estiverem ativadas
    for (int i = 0; i < 2; ++i) {
        if (pointLightEnabled[i]) {
            fColor += calcPointLight(pointLight[i]);
        }
    }

    // Adiciona a contribui��o da luz c�nica, se estiver ativada
    if (spotLightEnabled) {
        fColor += calcSpotLight(spotLight);
    }

    fColor *= texture(textSampler, textureVector);
}

vec4 calcAmbientLight(AmbientLight light) {
    // C�lculo da contribui��o da fonte de luz ambiente global, para a cor do objeto.
    vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
    return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light) {
    // C�lculo da reflex�o da componente da luz ambiente.
    vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

    // C�lculo da reflex�o da componente da luz difusa.
    vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
    vec3 L = normalize(-lightDirectionEyeSpace); // Dire��o inversa � da dire��o luz.
    vec3 N = normalize(vNormalEyeSpace);
    float NdotL = max(dot(N, L), 0.0);
    vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

    // C�lculo da reflex�o da componente da luz especular.
    vec3 V = normalize(-vPositionEyeSpace);
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

    // C�lculo da contribui��o da fonte de luz direcional para a cor final do fragmento.
    return (ambient + diffuse + specular);
}

vec4 calcPointLight(PointLight light) {
    // C�lculo da reflex�o da componente da luz ambiente.
    vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

    // C�lculo da reflex�o da componente da luz difusa.
    vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
    vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
    vec3 N = normalize(vNormalEyeSpace);
    float NdotL = max(dot(N, L), 0.0);
    vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

    // C�lculo da reflex�o da componente da luz especular.
    vec3 V = normalize(-vPositionEyeSpace);
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

    // Atenua��o
    float dist = length(mat3(View) * light.position - vPositionEyeSpace);  // C�lculo da dist�ncia entre o ponto de luz e o v�rtice
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // C�lculo da contribui��o da fonte de luz pontual para a cor final do fragmento.
    return (attenuation * (ambient + diffuse + specular));
}

float calcAngleBetweenVectors(vec3 N, vec3 L) {
    // Calcula o produto escalar
    float dotProduct = dot(N, L);
    // Clampa o valor para o intervalo [-1, 1] para evitar valores fora do dom�nio da fun��o arccos devido a imprecis�es num�ricas
    dotProduct = clamp(dotProduct, -1.0, 1.0);
    // Calcula o �ngulo em radianos usando arccos
    float angle = acos(dotProduct);
    return angle; // O �ngulo est� em radianos
}

vec4 calcSpotLight(SpotLight light) {
    // C�lculo da reflex�o da componente da luz ambiente.
    vec4 ambient = vec4(light.ambient, 1.0);

    // C�lculo da reflex�o da componente da luz difusa.
    vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
    vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
    vec3 N = normalize(vNormalEyeSpace);
    float NdotL = max(dot(N, L), 0.0);
    vec4 diffuse = vec4(light.diffuse, 1.0) * NdotL;

    // C�lculo da reflex�o da componente da luz especular.
    vec3 V = normalize(-vPositionEyeSpace);
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    vec4 specular = vec4(light.specular, 1.0) * pow(RdotV, 32.0); // Valor fixo para o expoente especular

    // Atenua��o
    float dist = length(mat3(View) * light.position - vPositionEyeSpace); // C�lculo da dist�ncia entre o ponto de luz e o v�rtice
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // C�lculo do �ngulo entre o vetor dire��o do foco e o vetor L
    float spotEffect = dot(normalize(light.spotDirection), -L);

    // Verifica se est� dentro do cone do foco de luz
    if (spotEffect > cos(light.spotCutoff)) {
        float spotFactor = pow(spotEffect, light.spotExponent);
        return (attenuation * (ambient + diffuse + specular)) * spotFactor;
    } else {
        return vec4(0, 0, 0, 1.0);
    }
}
