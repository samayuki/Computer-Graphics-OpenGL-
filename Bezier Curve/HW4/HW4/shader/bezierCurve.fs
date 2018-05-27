#version 330 core
out vec4 FragColor;

uniform vec3 curveColor;
void main()
{
    FragColor = vec4(curveColor, 1.0f);
} 