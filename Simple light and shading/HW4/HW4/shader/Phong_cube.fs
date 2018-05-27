#version 330 core
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffStrength;
uniform int Shininess;
void main()
{
    //ambient
    vec3 ambient = ambientStrength * lightColor;
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightPos + FragPos);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diffStrength * diff * lightColor;
    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = specularStrength * spec * lightColor;
    //result
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}