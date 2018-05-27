#version 330 core
in vec3 resultColor;
out vec4 FragColor;
  
uniform vec3 objectColor;
void main()
{
    //result
    vec3 result = resultColor * objectColor;
    FragColor = vec4(result, 1.0);
}