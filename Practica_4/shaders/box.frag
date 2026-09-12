#version 330 core

uniform vec3 partColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(partColor, 1.0);
}
