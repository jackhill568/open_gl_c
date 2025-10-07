#version 460 core
layout(location=0) out vec4 fragment;
layout(location=0) in vec3 FragPos;
layout(location=1) in vec3 Normal;
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

struct LightCaster {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

uniform Light light;  

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;
  sampler2D texture_diffuse3;
  sampler2D texture_specular1;
  sampler2D texture_specular2;
}; 
  
uniform Material material;

vec3 CalcCasterLight(LightCaster light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient1  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 ambient2  = light.ambient  * vec3(texture(material.texture_diffuse2, TexCoords));
    vec3 ambient3  = light.ambient  * vec3(texture(material.texture_diffuse3, TexCoords));

    vec3 diffuse1  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse2  = light.diffuse  * diff * vec3(texture(material.texture_diffuse2, TexCoords));
    vec3 diffuse3  = light.diffuse  * diff * vec3(texture(material.texture_diffuse3, TexCoords));

    vec3 specular1 = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    vec3 specular2 = light.specular * spec * vec3(texture(material.texture_specular2, TexCoords));
    return (ambient1 + ambient2 + ambient3 + diffuse1 +diffuse2 + diffuse3 +  specular1+specular2);
}; 

void main() {
    
    vec3 diffuse1 = vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse2 = vec3(texture(material.texture_diffuse2, TexCoords));
    vec3 diffuse3 = vec3(texture(material.texture_diffuse3, TexCoords));
    vec3 specular1 = vec3(texture(material.texture_specular1, TexCoords)); 
    vec3 specular2 = vec3(texture(material.texture_specular2, TexCoords)); 


    vec3 sumDiff =  (diffuse1 + diffuse2 + diffuse3);
    vec3 sumSpec = (specular1 + specular2);
    
  

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance)); 
    attenuation = 0.8;
    float specularStrength = 0.7;

    vec3 lightDir = normalize(light.position - FragPos); 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient  = light.ambient * sumDiff; 
    vec3 diffuse  = light.diffuse * diff * sumDiff;  

    vec3 specular = light.specular * spec * sumSpec;

    vec3 result = (ambient* attenuation + diffuse* attenuation + specular* attenuation);

    fragment = vec4(result, 1.0);
}
