#version 460 core
layout(location=0) out vec4 fragment;
layout(location=4) in vec3 Normal;
layout(location=5) in vec3 FragPos;
layout(location=2) in vec2 TexCoords;



uniform vec3 viewPos;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;  

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
  
uniform Material material;



void main() {



    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance)); 

    float specularStrength = 0.7;

    vec3 lightDir = normalize(light.position - FragPos); 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  

    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    vec3 result = (ambient* attenuation + diffuse* attenuation + specular* attenuation);

    fragment = vec4(result, 1.0);
}
